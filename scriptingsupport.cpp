// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#include "scriptingsupport.h"
#include "config.h"

ScriptingSupport::ScriptingSupport(QObject *parent) : QObject{parent} {}

bool ScriptingSupport::scriptingSupported() {
#ifdef LUA_FOUND
  return true;
#else
  return false;
#endif
}

QString ScriptingSupport::scriptText() const { return m_scriptText; }

void ScriptingSupport::setScriptText(const QString &newScriptText) {
  if (m_scriptText == newScriptText)
    return;
  m_scriptText = newScriptText;
  emit scriptTextChanged();
}

QString ScriptingSupport::tryCompile(const QString &script) {
#ifdef LUA_FOUND
  return m_engine.tryCompile(script);
#else
  Q_UNUSED(script);
  return QStringLiteral("Not available");
#endif
}

QString ScriptingSupport::tryRun(const QString &script) {
#ifdef LUA_FOUND
  return m_engine.tryRun(script);
#else
  Q_UNUSED(script);
  return QStringLiteral("Not available");
#endif
}
