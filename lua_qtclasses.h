// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#ifndef LUA_QTCLASSES_H
#define LUA_QTCLASSES_H

#include <QColor>
#include <QDebug>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <sol/sol.hpp>

namespace sol {

template <typename Handler>
inline bool sol_lua_check(sol::types<QString>, lua_State *L, int index,
                          Handler &&handler, sol::stack::record &tracking) {
  const int absolute_index = lua_absindex(L, index);
  const bool success =
      sol::stack::check<const char *>(L, absolute_index, handler);
  tracking.use(1);
  return success;
}

inline QString sol_lua_get(sol::types<QString>, lua_State *L, int index,
                           sol::stack::record &tracking) {
  int absoluteIndex = lua_absindex(L, index);

  std::string string = sol::stack::get<std::string>(L, absoluteIndex);
  tracking.use(1);

  return QString::fromStdString(string);
}

inline int sol_lua_push(sol::types<QString>, lua_State *L, QString &string) {
  std::string str(string.toStdString());
  lua_pushlstring(L, str.c_str(), str.size());
  return 1;
}

template <>
struct lua_type_of<QString>
    : std::integral_constant<sol::type, sol::type::string> {};

} // namespace sol

#define LUA_QT_PRINTER(X)                                                      \
  inline std::string to_string(const X &value) {                               \
    return QDebug::toString(value).toStdString();                              \
  }

LUA_QT_PRINTER(QColor)
LUA_QT_PRINTER(QRgb)
LUA_QT_PRINTER(QSize)
LUA_QT_PRINTER(QPoint)
LUA_QT_PRINTER(QRect)

struct LuaQtClasses {
  static void registerLua(sol::state &lua) {
    using namespace std::string_view_literals;

    auto point_type = lua.new_usertype<QPoint>(
        "QPoint", sol::constructors<QPoint(), QPoint(int, int)>());
    point_type["x"] = sol::property(&QPoint::x, &QPoint::setX);
    point_type["y"] = sol::property(&QPoint::y, &QPoint::setY);
    point_type["manhattanLength"] =
        sol::readonly_property(&QPoint::manhattanLength);

    auto size_type = lua.new_usertype<QSize>(
        "QSize", sol::constructors<QSize(), QSize(int, int)>());
    size_type["width"] = sol::property(&QSize::width, &QSize::setWidth);
    size_type["height"] = sol::property(&QSize::height, &QSize::setHeight);
    size_type["transpose"] = &QSize::transpose;
    size_type["transposed"] = &QSize::transposed;
    size_type["expandedTo"] = &QSize::expandedTo;
    size_type["boundedTo"] = &QSize::boundedTo;

    auto rect_type = lua.new_usertype<QRect>(
        "QRect", sol::constructors<QRect(), QRect(int, int, int, int),
                                   QRect(const QPoint &, const QPoint &),
                                   QRect(const QPoint &, const QSize &)>());

    rect_type["width"] = sol::property(&QRect::width, &QRect::setWidth);
    rect_type["height"] = sol::property(&QRect::height, &QRect::setHeight);
    rect_type["x"] = sol::property(&QRect::x, &QRect::setX);
    rect_type["y"] = sol::property(&QRect::y, &QRect::setY);
    rect_type["left"] = sol::property(&QRect::left, &QRect::setLeft);
    rect_type["top"] = sol::property(&QRect::top, &QRect::setTop);
    rect_type["right"] = sol::property(&QRect::right, &QRect::setRight);
    rect_type["bottom"] = sol::property(&QRect::bottom, &QRect::setBottom);
    rect_type["size"] = sol::property(&QRect::size, &QRect::setSize);
    rect_type["center"] = sol::readonly_property(&QRect::center);
    rect_type["translated"] = sol::overload(
        sol::resolve<QRect(int, int) const>(&QRect::translated),
        sol::resolve<QRect(const QPoint &) const>(&QRect::translated));

    auto color_type = lua.new_usertype<QColor>(
        "QColor", sol::constructors<QColor(), QColor(int, int, int),
                                    QColor(int, int, int, int),
                                    QColor(const QString &)>());

    color_type["red"] = sol::property(&QColor::red, &QColor::setRed);
    color_type["green"] = sol::property(&QColor::green, &QColor::setGreen);
    color_type["blue"] = sol::property(&QColor::blue, &QColor::setBlue);
    color_type["alpha"] = sol::property(&QColor::alpha, &QColor::setAlpha);

    color_type["redF"] = sol::property(&QColor::redF, &QColor::setRedF);
    color_type["greenF"] = sol::property(&QColor::greenF, &QColor::setGreenF);
    color_type["blueF"] = sol::property(&QColor::blueF, &QColor::setBlueF);
    color_type["alphaF"] = sol::property(&QColor::alphaF, &QColor::setAlphaF);

    auto qt_ns = lua.create_table("Qt");

    qt_ns.new_enum("Orientation"sv, "Horizontal"sv, Qt::Horizontal,
                   "Vertical"sv, Qt::Vertical);

    qt_ns.new_enum("PenStyle"sv, "NoPen"sv, Qt::NoPen, "SolidLine"sv,
                   Qt::SolidLine, "DashLine"sv, Qt::DashLine, "DotLine"sv,
                   Qt::DotLine, "DashDotLine"sv, Qt::DashDotLine,
                   "DashDotDotLine"sv, Qt::DashDotDotLine);

    qt_ns.new_enum("AspectRatioMode"sv, "KeepAspectRatio"sv,
                   Qt::KeepAspectRatio, "IgnoreAspectRatio"sv,
                   Qt::IgnoreAspectRatio, "KeepAspectRatioByExpanding"sv,
                   Qt::KeepAspectRatioByExpanding);
  }
};

#endif // LUA_QTCLASSES_H
