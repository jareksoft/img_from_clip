#ifndef LUACAPTURESCRIPT_H
#define LUACAPTURESCRIPT_H

#include <QObject>
#include <sol/sol.hpp>

class LuaCaptureScript : public QObject {
  Q_OBJECT
public:
  explicit LuaCaptureScript(QObject *parent = nullptr);
  ~LuaCaptureScript() override;

public slots:
  void loadScript(const QString &script, const QString &scriptName);

private:
  sol::state L;
};

#endif // LUACAPTURESCRIPT_H
