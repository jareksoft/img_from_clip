// SPDX-License-Identifier: BSD-2-Clause

#if defined(Q_OS_IOS)
#include <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#import <CoreFoundation/CoreFoundation.h>
#endif
#include <QPointer>
#include <UserNotifications/UserNotifications.h>

#if !__has_feature(objc_arc)
#error "ARC is off"
#endif

#include "platformsupport.h"

PlatformSupport::PlatformSupport(QObject *parent)
    : QObject{parent}
{
    platformInit();
}

PlatformSupport::~PlatformSupport() = default;

void PlatformSupport::platformInit()
{
    m_notificationsAllowed = -1;

    QTimer::singleShot(0, this, [&]() {
        __block QPointer<PlatformSupport> selfRef(this);
        [[UNUserNotificationCenter currentNotificationCenter]
            getNotificationSettingsWithCompletionHandler:^(UNNotificationSettings *settings) {
                if (auto self = selfRef.get()) {
                    qDebug() << "auth status is " << settings.authorizationStatus;
                    switch (settings.authorizationStatus) {
                    case UNAuthorizationStatusNotDetermined:
                        m_notificationsAllowed = -1;
                        break;
                    case UNAuthorizationStatusAuthorized:
                    case UNAuthorizationStatusProvisional:
#ifdef Q_OS_IOS
                    case UNAuthorizationStatusEphemeral:
#endif
                        self->m_notificationsAllowed = 1;
                        break;
                    default:
                        self->m_notificationsAllowed = 0;
                        break;
                    }
                    emit self->permissionStateFetchCompletion();
                }
            }];
    });
}

void PlatformSupport::requestNotifications()
{
    __block QPointer<PlatformSupport> selfRef(this);
    [[UNUserNotificationCenter currentNotificationCenter]
        requestAuthorizationWithOptions:(UNAuthorizationOptionAlert)
                      completionHandler:^(BOOL granted, NSError *_Nullable) {
                          if (auto self = selfRef.get()) {
                              qDebug() << "Granted =" << granted;
                              self->m_notificationsAllowed = (granted != NO) ? 1 : 0;
                          }
                      }];
}

bool PlatformSupport::requiresNotificationPermission() const
{
    return true;
}

void PlatformSupport::notifyWithImage(NotifyClass,
                                      const QString &title,
                                      const QString &description,
                                      const QString &)
{
    UNMutableNotificationContent *content = [[UNMutableNotificationContent alloc] init];
    content.title = title.toNSString();
    content.body = description.toNSString();
    content.sound = [UNNotificationSound defaultSound];
    content.relevanceScore = 0.75;
    UNTimeIntervalNotificationTrigger *trigger = [UNTimeIntervalNotificationTrigger
        triggerWithTimeInterval:1
                        repeats:NO];
    UNNotificationRequest *request = [UNNotificationRequest
        requestWithIdentifier:@"grab-plat-notify"
                      content:content
                      trigger:trigger];
    // Schedule the notification.
    UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
    [center addNotificationRequest:request
             withCompletionHandler:^(NSError *err) {
                 Q_UNUSED(err);
             }];
}
