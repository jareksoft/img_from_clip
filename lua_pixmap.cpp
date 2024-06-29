#include "lua_pixmap.h"

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
}
