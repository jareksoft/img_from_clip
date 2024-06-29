#include "luacapturescript.h"

LuaCaptureScript::LuaCaptureScript(QObject *parent) : QObject{parent} {
  L.open_libraries();
}

QString LuaCaptureScript::tryCompile(const QString &script) {
  sol::state localL;
  QByteArray scriptutf = script.toUtf8();

  localL.open_libraries();
  auto result = localL.load_buffer(scriptutf.constData(), scriptutf.size(),
                                   "compiled_script");
  if (!result.valid()) {
    auto error = result.get<sol::error>();
    return QString::fromUtf8(QByteArrayView(error.what()));
  }

  return QString();
}

LuaCaptureScript::~LuaCaptureScript() = default;

void LuaCaptureScript::loadScript(const QString &script,
                                  const QString &scriptName) {
  QByteArray utf = script.toUtf8();
  L.load(sol::string_view(utf.constData(), utf.size()),
         scriptName.toStdString());
}
