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
