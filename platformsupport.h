// SPDX-License-Identifier: BSD-2-Clause

#ifndef PLATFORMSUPPORT_H
#define PLATFORMSUPPORT_H

#include <QObject>
#include <QProperty>
#include <QQmlEngine>
#include <QTimer>

class PlatformSupport : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(
        int notificationsAllowed READ notificationsAllowed BINDABLE notificationsAllowedBindable)
    Q_PROPERTY(bool requiresNotificationPermission READ requiresNotificationPermission CONSTANT)
public:
    explicit PlatformSupport(QObject *parent = nullptr);
    ~PlatformSupport() override;

    int notificationsAllowed() const;

    QBindable<int> notificationsAllowedBindable() { return {&m_notificationsAllowed}; }

    bool requiresNotificationPermission() const;

signals:
    void permissionStateFetchCompletion();

public slots:
    void requestNotifications();

private:
    void platformInit();

private:
    QProperty<int> m_notificationsAllowed{0};
};

#endif // PLATFORMSUPPORT_H
