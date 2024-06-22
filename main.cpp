// SPDX-License-Identifier: BSD-2-Clause

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QTranslator>
#include "config.h"

auto main(int argc, char *argv[]) -> int
{
    QApplication::setOrganizationDomain(QStringLiteral("com.jpelczar"));
    QApplication::setOrganizationName(QStringLiteral("jareksoft"));
    QApplication::setApplicationName(QStringLiteral("img_from_clip"));
    QApplication::setApplicationVersion(APP_VERSION_STRING);

    QApplication app(argc, argv);

    QTranslator translator;
    if (translator.load(QLocale(),
                        QStringLiteral("appimg_from_clip"),
                        QStringLiteral("_"),
                        QStringLiteral(":/i18n")))
        QCoreApplication::installTranslator(&translator);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("img_from_clip", "Main");

    return app.exec();
}
