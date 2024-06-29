#ifndef LUACAPTURESCRIPT_H
#define LUACAPTURESCRIPT_H

#include "lua_pixmap.h"
#include <QObject>
#include <sol/sol.hpp>

class LuaCaptureScript : public QObject {
  Q_OBJECT
public:
  explicit LuaCaptureScript(QObject *parent = nullptr);
  ~LuaCaptureScript() override;

  QString tryCompile(const QString &script);
  QString tryRun(const QString &script);

  static void registerSol(sol::state &lua);

public slots:
  void loadScript(const QString &script, const QString &scriptName);

private:
  sol::state L;
};

#endif // LUACAPTURESCRIPT_H
