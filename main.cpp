#include <QApplication>
#include <QQmlApplicationEngine>
#include "config.h"

int main(int argc, char *argv[])
{
    QApplication::setOrganizationDomain(QStringLiteral("com.jpelczar"));
    QApplication::setOrganizationName(QStringLiteral("jareksoft"));
    QApplication::setApplicationName(QStringLiteral("img_from_clip"));
    QApplication::setApplicationVersion(APP_VERSION_STRING);

    QApplication app(argc, argv);

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
