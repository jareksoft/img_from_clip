#ifndef LUACAPTURESCRIPT_H
#define LUACAPTURESCRIPT_H

#include "lua.hpp"
#include <QObject>

class LuaCaptureScript : public QObject {
  Q_OBJECT
public:
  explicit LuaCaptureScript(QObject *parent = nullptr);
  ~LuaCaptureScript() override;

private:
  lua_State *L = nullptr;
};

#endif // LUACAPTURESCRIPT_H
