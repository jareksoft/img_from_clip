#include "luacapturescript.h"

LuaCaptureScript::LuaCaptureScript(QObject *parent) : QObject{parent} {
  L.open_libraries();
}

LuaCaptureScript::~LuaCaptureScript() = default;

void LuaCaptureScript::loadScript(const QString &script,
                                  const QString &scriptName) {
  QByteArray utf = script.toUtf8();
  L.load(sol::string_view(utf.constData(), utf.size()),
         scriptName.toStdString());
}
