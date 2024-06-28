// SPDX-License-Identifier: GPL-3.0-or-later

#include "config.h"

#ifdef USE_LIBRSVG_RENDERER
// Must be first
#include <cairo.h>
#include <librsvg/rsvg.h>

// Don't move up
#include "rsvg_helper.h"
#include <QImage>

namespace {

struct GObjectCleanup {
  void operator()(gpointer obj) const { g_object_unref(obj); }
};

struct CairoSurfaceCleanup {
  void operator()(cairo_surface_t *obj) const { cairo_surface_destroy(obj); }
};

struct CairoCleanup {
  void operator()(cairo_t *obj) const { cairo_destroy(obj); }
};

} // namespace

QImage RsvgRender::renderSvg(const QString &svg, double scale) {
  QByteArray utf = svg.toUtf8();
  gdouble width = 0;
  gdouble height = 0;

  auto handle = rsvg_handle_new_from_data((const guint8 *)utf.constData(),
                                          (gsize)utf.size(), nullptr);
  if (!handle)
    return {};

  std::unique_ptr<RsvgHandle, GObjectCleanup> handleCleanup(handle);
  rsvg_handle_set_dpi(handle, 96 * scale);
  rsvg_handle_get_intrinsic_size_in_pixels(handle, &width, &height);

  auto surface = cairo_image_surface_create(
      CAIRO_FORMAT_ARGB32, (int)(width * scale), (int)(height * scale));
  std::unique_ptr<cairo_surface_t, CairoSurfaceCleanup> surfaceCleanup(surface);

  auto cairo = cairo_create(surface);
  std::unique_ptr<cairo_t, CairoCleanup> cairoCleanup(cairo);

  cairo_scale(cairo, scale, scale);

  RsvgRectangle rect;
  rect.x = 0;
  rect.y = 0;
  rect.width = width;
  rect.height = height;

  if (rsvg_handle_render_document(handle, cairo, &rect, nullptr)) {
    return QImage(cairo_image_surface_get_data(surface),
                  cairo_image_surface_get_width(surface),
                  cairo_image_surface_get_height(surface),
                  cairo_image_surface_get_stride(surface),
                  QImage::Format_ARGB32)
        .copy();
  }

  return QImage();
}
#endif
