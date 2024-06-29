#ifndef SCRIPTINGSUPPORT_H
#define SCRIPTINGSUPPORT_H

#include "config.h"
#include <QObject>
#include <QQmlEngine>

#if defined(LUA_FOUND)
#include "luacapturescript.h"
#endif

class ScriptingSupport : public QObject {
  Q_OBJECT
  QML_ELEMENT
public:
  explicit ScriptingSupport(QObject *parent = nullptr);

signals:
};

#endif // SCRIPTINGSUPPORT_H
