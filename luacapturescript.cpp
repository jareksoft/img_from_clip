#include "luacapturescript.h"

LuaCaptureScript::LuaCaptureScript(QObject *parent) : QObject{parent} {
  L.open_libraries();
}

LuaCaptureScript::~LuaCaptureScript() = default;
