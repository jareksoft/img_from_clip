#ifndef PLATFORMSUPPORT_H
#define PLATFORMSUPPORT_H

#include <QObject>
#include <QProperty>
#include <QQmlEngine>

class PlatformSupport : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(
        bool notificationsAllowed READ notificationsAllowed BINDABLE notificationsAllowedBindable)
    Q_PROPERTY(bool requiresNotificationPermission READ requiresNotificationPermission CONSTANT)
public:
    explicit PlatformSupport(QObject *parent = nullptr);
    ~PlatformSupport() override;

    bool notificationsAllowed() const;

    QBindable<bool> notificationsAllowedBindable() { return {&m_notificationsAllowed}; }

    bool requiresNotificationPermission() const;

signals:
    void permissionStateFetchCompletion();

public slots:
    void requestNotifications();

private:
    void platformInit();

private:
    QProperty<bool> m_notificationsAllowed{true};
};

#endif // PLATFORMSUPPORT_H
