// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

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
  Q_PROPERTY(bool scriptingSupported READ scriptingSupported CONSTANT FINAL)
  Q_PROPERTY(QString scriptText READ scriptText WRITE setScriptText NOTIFY
                 scriptTextChanged)
public:
  explicit ScriptingSupport(QObject *parent = nullptr);

  static bool scriptingSupported();

  QString scriptText() const;
  void setScriptText(const QString &newScriptText);

  Q_INVOKABLE QString tryCompile(const QString &script);
  Q_INVOKABLE QString tryRun(const QString &script);

signals:
  void scriptTextChanged();
  void outputPrint(const QString& string);

private:
  QString m_scriptText;
#if defined(LUA_FOUND)
  LuaCaptureScript m_engine;
#endif
};

#endif // SCRIPTINGSUPPORT_H
