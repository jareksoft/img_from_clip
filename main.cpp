// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#include "config.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QSettings>
#include <QStandardPaths>
#include <QStorageInfo>
#include <QTranslator>

auto main(int argc, char *argv[]) -> int {
  QApplication::setOrganizationDomain(QStringLiteral("com.jpelczar"));
  QApplication::setOrganizationName(QStringLiteral("jareksoft"));
  QApplication::setApplicationName(QStringLiteral("img_from_clip"));
  QApplication::setApplicationVersion(APP_VERSION_STRING);

  QApplication app(argc, argv);

  QSettings settings;
  if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE") &&
      settings.value("style").toString().isEmpty()) {
#if defined(Q_OS_MACOS)
    QQuickStyle::setStyle(QString("iOS"));
#elif defined(Q_OS_IOS)
    QQuickStyle::setStyle(QString("iOS"));
#elif defined(Q_OS_WINDOWS)
    QQuickStyle::setStyle(QString("Windows"));
#elif defined(Q_OS_ANDROID)
    QQuickStyle::setStyle(QString("Material"));
#endif
  } else {
    QQuickStyle::setStyle(settings.value("style").toString());
  }

  const QString styleInSettings = settings.value("style").toString();
  if (styleInSettings.isEmpty())
    settings.setValue(QString("style"), QQuickStyle::name());

  QStringList builtInStyles = {QString("CustomStyle"), QString("Basic"),
                               QString("Material"), QString("Universal")};
#if defined(Q_OS_MACOS)
  builtInStyles << QString("iOS");
#elif defined(Q_OS_IOS)
  builtInStyles << QString("iOS");
#elif defined(Q_OS_WINDOWS)
  builtInStyles << QString("Windows");
#endif

  QDir().mkpath(
      QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

  QTranslator translator;
  if (translator.load(QLocale(), QStringLiteral("appimg_from_clip"),
                      QStringLiteral("_"), QStringLiteral(":/i18n")))
    QCoreApplication::installTranslator(&translator);

  QQmlApplicationEngine engine;
  engine.setInitialProperties({{"builtInStyles", builtInStyles}});

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.loadFromModule("img_from_clip", "Main");

  int ret = app.exec();

  qDebug() << "Exiting";

  return ret;
}
