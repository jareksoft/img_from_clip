// SPDX-License-Identifier: BSD-2-Clause

#ifndef PLATFORMSUPPORT_H
#define PLATFORMSUPPORT_H

#include <QObject>
#include <QProperty>
#include <QQmlEngine>
#include <QTimer>
#include "config.h"

#ifdef USE_DBUS_BACKEND
#include <QDBusInterface>
#endif

class PlatformSupport : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(
        int notificationsAllowed READ notificationsAllowed BINDABLE notificationsAllowedBindable)
    Q_PROPERTY(bool requiresNotificationPermission READ requiresNotificationPermission CONSTANT)
public:
    enum class NotifyClass { Error, Info };
    Q_ENUMS(NotifyClass)

    explicit PlatformSupport(QObject *parent = nullptr);
    ~PlatformSupport() override;

    int notificationsAllowed() const;

    QBindable<int> notificationsAllowedBindable() { return {&m_notificationsAllowed}; }

    bool requiresNotificationPermission() const;

signals:
    void permissionStateFetchCompletion();

public slots:
    void requestNotifications();
    void notify(NotifyClass type, const QString &title, const QString &description);
    void notifyWithImage(NotifyClass type,
                         const QString &title,
                         const QString &description,
                         const QString &imagePath);

private:
    void platformInit();

private:
    QProperty<int> m_notificationsAllowed{0};
#ifdef USE_DBUS_BACKEND
    QDBusInterface *m_notificationIf = nullptr;
#endif
};

#endif // PLATFORMSUPPORT_H
