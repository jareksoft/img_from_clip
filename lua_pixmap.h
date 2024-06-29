// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#ifndef LUA_PIXMAP_H
#define LUA_PIXMAP_H

#include <QExplicitlySharedDataPointer>
#include <QImage>
#include <sol/sol.hpp>

class Pixmap {
public:
  Pixmap(int width, int height);
  ~Pixmap();

  Pixmap(const Pixmap &) = default;
  Pixmap &operator=(const Pixmap &) = default;

  Pixmap copy() const;
  Pixmap copyPart(int x, int y, int w, int h) const;

  int width() const;
  int height() const;

  static void registerSol(sol::state &lua);

private:
  struct NoInit {};
  Pixmap(NoInit) {}

  struct Data : QSharedData {
    QImage m_pixmap;

    Data(int w, int h) : m_pixmap(w, h, QImage::Format_ARGB32_Premultiplied) {}
    Data(QImage &&image) : m_pixmap(std::forward<QImage>(image)) {}
    Data(const Data &) = delete;
    Data &operator=(const Data &) = delete;
  };

  QExplicitlySharedDataPointer<Data> m_data;
};

#endif // LUA_PIXMAP_H
