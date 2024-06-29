// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#ifndef LUA_QTCLASSES_H
#define LUA_QTCLASSES_H

#include <QColor>
#include <QDebug>
#include <QMetaEnum>
#include <QPainter>
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
  template <typename U>
  static inline void registerMetaEnum(sol::table &qt_ns,
                                      const std::string_view &name) {
    QMetaEnum me = QMetaEnum::fromType<U>();
    Q_ASSERT(me.isValid());
    std::vector<std::pair<std::string_view, U>> vec;
    const int keyCount = me.keyCount();
    sol::table target = qt_ns.create(name, keyCount, static_cast<int>(0));
    for (int i = 0; i < keyCount; ++i) {
      target.set(me.key(i), me.value(i));
    }
  }

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
    rect_type.set_function(
        "translated",
        sol::overload(
            sol::resolve<QRect(int, int) const>(&QRect::translated),
            sol::resolve<QRect(const QPoint &) const>(&QRect::translated)));

    auto color_type = lua.new_usertype<QColor>(
        "QColor",
        sol::constructors<QColor(), QColor(int, int, int),
                          QColor(int, int, int, int), QColor(const char *)>());

    color_type["red"] = sol::property(&QColor::red, &QColor::setRed);
    color_type["green"] = sol::property(&QColor::green, &QColor::setGreen);
    color_type["blue"] = sol::property(&QColor::blue, &QColor::setBlue);
    color_type["alpha"] = sol::property(&QColor::alpha, &QColor::setAlpha);

    color_type["redF"] = sol::property(&QColor::redF, &QColor::setRedF);
    color_type["greenF"] = sol::property(&QColor::greenF, &QColor::setGreenF);
    color_type["blueF"] = sol::property(&QColor::blueF, &QColor::setBlueF);
    color_type["alphaF"] = sol::property(&QColor::alphaF, &QColor::setAlphaF);

    auto qt_ns = lua.create_table("Qt");

    registerMetaEnum<Qt::Orientation>(qt_ns, "Orientation");
    registerMetaEnum<Qt::AspectRatioMode>(qt_ns, "AspectRatioMode");
    registerMetaEnum<Qt::BrushStyle>(qt_ns, "BrushStyle");
    registerMetaEnum<Qt::PenStyle>(qt_ns, "PenStyle");
    registerMetaEnum<Qt::ArrowType>(qt_ns, "ArrowType");
    registerMetaEnum<Qt::PenCapStyle>(qt_ns, "PenCapStyle");
    registerMetaEnum<Qt::PenJoinStyle>(qt_ns, "PenJoinStyle");
    registerMetaEnum<Qt::SizeMode>(qt_ns, "SizeMode");
    registerMetaEnum<Qt::TextFormat>(qt_ns, "TextFormat");
    registerMetaEnum<Qt::Key>(qt_ns, "Key");

    auto brush_type = lua.new_usertype<QBrush>(
        "QBrush", sol::constructors<QBrush(), QBrush(const QColor &),
                                    QBrush(const QBrush &)>());

    brush_type["style"] = sol::property(&QBrush::style, &QBrush::setStyle);
    brush_type["color"] = sol::property(
        &QBrush::color,
        sol::resolve<void(const QColor &color)>(&QBrush::setColor));
    brush_type["isOpaque"] = sol::readonly_property(&QBrush::isOpaque);

    auto pen_type = lua.new_usertype<QPen>(
        "QPen",
        sol::constructors<QPen(), QPen(Qt::PenStyle), QPen(const QPen &),
                          QPen(const QBrush &, qreal)>());

    pen_type["style"] = sol::property(&QPen::style, &QPen::setStyle);
    pen_type["width"] = sol::property(&QPen::width, &QPen::setWidth);
    pen_type["widthF"] = sol::property(&QPen::widthF, &QPen::setWidthF);
    pen_type["color"] = sol::property(&QPen::color, &QPen::setColor);
    pen_type["brush"] = sol::readonly_property(&QPen::brush);
    pen_type["setBrush"] = &QPen::setBrush;
    pen_type["cosmetic"] = sol::property(&QPen::isCosmetic, &QPen::setCosmetic);

    auto painter_type =
        lua.new_usertype<QPainter>("QPainter", "new", sol::no_constructor);

    painter_type["setPen"] =
        sol::overload(sol::resolve<void(const QColor &)>(&QPainter::setPen),
                      sol::resolve<void(const QPen &)>(&QPainter::setPen),
                      sol::resolve<void(Qt::PenStyle)>(&QPainter::setPen));

    painter_type["pen"] = sol::readonly_property(&QPainter::pen);

    painter_type["setBrush"] =
        sol::overload(sol::resolve<void(const QBrush &)>(&QPainter::setBrush),
                      sol::resolve<void(Qt::BrushStyle)>(&QPainter::setBrush));

    painter_type["brush"] = sol::readonly_property(&QPainter::brush);

    painter_type["opacity"] =
        sol::property(&QPainter::opacity, &QPainter::setOpacity);

    painter_type["isActive"] = sol::readonly_property(&QPainter::isActive);
    painter_type["finish"] = &QPainter::end;
  }
};

#endif // LUA_QTCLASSES_H
