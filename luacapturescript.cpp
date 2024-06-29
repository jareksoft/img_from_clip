#include "luacapturescript.h"

LuaCaptureScript::LuaCaptureScript(QObject *parent) : QObject{parent} {
  L = luaL_newstate();
  Q_ASSERT(L != nullptr);
  luaL_openlibs(L);
}

LuaCaptureScript::~LuaCaptureScript() { lua_close(L); }
