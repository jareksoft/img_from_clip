#ifndef RSVG_HELPER_H
#define RSVG_HELPER_H

#include "config.h"
#include <QImage>

#ifdef USE_LIBRSVG_RENDERER
struct RsvgRender {
  static QImage renderSvg(const QString &svg);
};
#endif

#endif // RSVG_HELPER_H
