// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#include "platformsupport.h"
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QImage>
#include <QTimer>

PlatformSupport::PlatformSupport(QObject *parent) : QObject{parent} {
  platformInit();
  QTimer::singleShot(100, this,
                     &PlatformSupport::permissionStateFetchCompletion);
}

PlatformSupport::~PlatformSupport() = default;

void PlatformSupport::platformInit() {
  if (!QDBusConnection::sessionBus().isConnected()) {
    qWarning() << "Could not connect to session bus";
  } else {
    m_notificationIf =
        new QDBusInterface(QStringLiteral("org.freedesktop.Notifications"),
                           QStringLiteral("/org/freedesktop/Notifications"),
                           QStringLiteral("org.freedesktop.Notifications"),
                           QDBusConnection::sessionBus(), this);
  }
}

void PlatformSupport::requestNotifications() {}

bool PlatformSupport::requiresNotificationPermission() const { return false; }

void PlatformSupport::notifyWithImage(NotifyClass type, const QString &title,
                                      const QString &description,
                                      const QUrl &imagePath) {
  QVariantMap hints;
  QString iconName = QStringLiteral("dialog-information");
  QVector<QString> actionMap;
  qint32 expireTimeout = 10000;

  if (!m_notificationIf)
    return;

  hints.insert(QStringLiteral("urgency"), 1);
  switch (type) {
  case NotifyClass::Error:
    hints.insert(QStringLiteral("category"), QStringLiteral("device.error"));
    iconName = QStringLiteral("dialog-error");
    break;
  case NotifyClass::Info:
    hints.insert(QStringLiteral("category"), QStringLiteral("transfer"));
    break;
  }
  hints.insert(QStringLiteral("resident"), QVariant(false));
  if (!imagePath.isEmpty()) {
    QImage pixmap(imagePath.toLocalFile());
    if (!pixmap.isNull()) {
      qDebug() << "Loaded image";

      pixmap = pixmap.scaled(128, 128, Qt::KeepAspectRatio,
                             Qt::SmoothTransformation);

      QDBusArgument arg;
      arg.beginStructure();
      arg << pixmap.width();
      arg << pixmap.height();
      arg << pixmap.bytesPerLine();
      arg << pixmap.hasAlphaChannel();

      int channels;
      if (pixmap.isGrayscale())
        channels = 1;
      else {
        channels = 3;
        if (pixmap.hasAlphaChannel())
          channels = 4;
      }

      arg << pixmap.depth() / channels;
      arg << channels;

      arg << QByteArray((const char *)pixmap.constBits(), pixmap.sizeInBytes());
      arg.endStructure();
      hints.insert(QStringLiteral("image-data"), QVariant::fromValue(arg));
    } else {
      qWarning() << "Could not parse image";
    }
  }

  QList<QVariant> args;
  args.push_back(QCoreApplication::applicationName());
  args.push_back(quint32(0));
  args.push_back(iconName);
  args.push_back(title);
  args.push_back(description);
  args.push_back(actionMap);
  args.push_back(hints);
  args.push_back(qint32(expireTimeout));

  QDBusReply<quint32> result = m_notificationIf->callWithArgumentList(
      QDBus::CallMode::AutoDetect, QStringLiteral("Notify"), args);

  if (!result.isValid()) {
    qWarning() << "Could not post notification" << result.error();
  }
}
