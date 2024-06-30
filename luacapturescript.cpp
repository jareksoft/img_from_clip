// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#include "luacapturescript.h"
#include "captureobject.h"
#include "lua_qtclasses.h"

LuaCaptureScript::LuaCaptureScript(QObject *parent) : QObject{parent} {
  L.open_libraries();
  registerSol(L);
}

QString LuaCaptureScript::tryCompile(const QString &script) {
  sol::state localL;
  QByteArray scriptutf = script.toUtf8();

  localL.open_libraries();
  registerSol(localL);

  auto result = localL.load_buffer(scriptutf.constData(), scriptutf.size(),
                                   "compiled_script");
  if (!result.valid()) {
    auto error = result.get<sol::error>();
    return QString::fromUtf8(QByteArrayView(error.what()));
  }

  return QString();
}

QString LuaCaptureScript::tryRun(const QString &script) {
  sol::state localL;
  QByteArray scriptutf = script.toUtf8();

  localL.open_libraries();
  registerSol(localL);

  try {
    localL.script(std::string_view(scriptutf.constData(), scriptutf.size()));
    return QString();
  } catch (std::exception &ex) {
    return ex.what();
  }
}

void LuaCaptureScript::registerSol(sol::state &lua) {
  LuaQtClasses::registerLua(lua);
  Pixmap::registerSol(lua);
  CaptureObject::registerSol(lua);

  lua.set_function("print", [&](const sol::variadic_args& va) {
    QString result;
    int n = 0;
    sol::state_view ref(va.lua_state());
    for(auto a : va) {
      ++n;
      if (n > 1)
        result.append("\t");
      result += ref["tostring"](a).get<std::string>();
    }
    emit outputPrint(result);
  });
}

LuaCaptureScript::~LuaCaptureScript() = default;

void LuaCaptureScript::loadScript(const QString &script,
                                  const QString &scriptName) {
  QByteArray utf = script.toUtf8();
  L.load(sol::string_view(utf.constData(), utf.size()),
         scriptName.toStdString());
}
