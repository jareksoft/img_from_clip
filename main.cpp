// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#include "config.h"
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QStandardPaths>
#include <QStorageInfo>
#include <QTranslator>

auto main(int argc, char *argv[]) -> int {
  QApplication::setOrganizationDomain(QStringLiteral("com.jpelczar"));
  QApplication::setOrganizationName(QStringLiteral("jareksoft"));
  QApplication::setApplicationName(QStringLiteral("img_from_clip"));
  QApplication::setApplicationVersion(APP_VERSION_STRING);

  QApplication app(argc, argv);

  QDir().mkpath(
      QStandardPaths::writableLocation(QStandardPaths::CacheLocation));

  QTranslator translator;
  if (translator.load(QLocale(), QStringLiteral("appimg_from_clip"),
                      QStringLiteral("_"), QStringLiteral(":/i18n")))
    QCoreApplication::installTranslator(&translator);

  QQmlApplicationEngine engine;

  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreationFailed, &app,
      []() { QCoreApplication::exit(-1); }, Qt::QueuedConnection);
  engine.loadFromModule("img_from_clip", "Main");

  int ret = app.exec();

  qDebug() << "Exiting";

  return ret;
}
