// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#if defined(Q_OS_IOS)
#include <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#import <CoreFoundation/CoreFoundation.h>
#endif
#include <QDir>
#include <QImage>
#include <QPointer>
#include <QStandardPaths>
#include <UserNotifications/UserNotifications.h>

#if !__has_feature(objc_arc)
#error "ARC is off"
#endif

#include "platformsupport.h"

static QString kCachePath{QStringLiteral("noti-cache")};

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

    auto location = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir cacheDir(location);
    cacheDir.cd(kCachePath);
    cacheDir.removeRecursively();

    QDir(location).mkpath(kCachePath);
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
                                      const QUrl &url)
{
    UNMutableNotificationContent *content = [[UNMutableNotificationContent alloc] init];
    content.title = title.toNSString();
    content.body = description.toNSString();
    content.sound = [UNNotificationSound defaultSound];
    content.relevanceScore = 0.75;

    if (!url.isEmpty()) {
        QImage image(url.toLocalFile());

        auto location = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        QDir cacheDir(location);
        cacheDir.cd(kCachePath);

        QString randomName = QString::number(QDateTime::currentMSecsSinceEpoch());
        randomName.append(QStringLiteral(".png"));

        QString fullSaveLocation = cacheDir.absoluteFilePath(randomName);

        qDebug() << "Saving name" << fullSaveLocation;

        if (!image.isNull())
            image = image.scaled(256, 256, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        if (!image.isNull() && image.save(fullSaveLocation, "PNG")) {
            UNNotificationAttachment *attachment = [UNNotificationAttachment
                attachmentWithIdentifier:@""
                                     URL:QUrl::fromLocalFile(fullSaveLocation).toNSURL()
                                 options:@{}
                                   error:nil];

            if (attachment) {
                qDebug() << "Attached URL" << url;
                content.attachments = @[attachment];
            } else {
                qDebug() << "Could not attach" << url;
            }
        }
    }

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
