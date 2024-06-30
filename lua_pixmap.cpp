// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#include "lua_pixmap.h"

struct PixmapPainter : public QPainter,
                       public std::enable_shared_from_this<PixmapPainter> {
  Pixmap m_ref;

  PixmapPainter(const Pixmap &ref) : QPainter(ref.imagePtr()), m_ref(ref) {}
};

Pixmap::Pixmap(int width, int height) : m_data(new Data(width, height)) {}

Pixmap::~Pixmap() = default;

Pixmap Pixmap::copy() const {
  Pixmap result(NoInit{});
  result.m_data.reset(new Data(m_data->m_pixmap.copy()));
  return result;
}

Pixmap Pixmap::copyPart(int x, int y, int w, int h) const {
  Pixmap result(NoInit{});
  result.m_data.reset(new Data(m_data->m_pixmap.copy(x, y, w, h)));
  return result;
}

int Pixmap::width() const { return m_data->m_pixmap.width(); }

int Pixmap::height() const { return m_data->m_pixmap.height(); }

void Pixmap::registerSol(sol::state &lua) {
  auto pixmap_type =
      lua.new_usertype<Pixmap>("Pixmap", sol::constructors<Pixmap(int, int)>());

  pixmap_type["copy"] = &Pixmap::copy;
  pixmap_type["copyPart"] = &Pixmap::copyPart;
  pixmap_type["width"] = sol::readonly_property(&Pixmap::width);
  pixmap_type["height"] = sol::readonly_property(&Pixmap::height);
  pixmap_type["beginPainting"] = &Pixmap::makePainter;
  pixmap_type["save"] = [](const Pixmap &p, const std::string &fileName,
                           const std::string &format) {
    return p.m_data->m_pixmap.save(QString::fromStdString(fileName),
                                   format.c_str());
  };
}

std::shared_ptr<QPainter> Pixmap::makePainter() {
  return std::make_shared<PixmapPainter>(*this);
}
