#if defined(Q_OS_IOS)
#include <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#import <CoreFoundation/CoreFoundation.h>
#endif

#if !__has_feature(objc_arc)
#error "ARC is off"
#endif

PlatformSupport::PlatformSupport(QObject *parent)
    : QObject{parent}
{
    platformInit();
}

PlatformSupport::~PlatformSupport() = default;

void PlatformSupport::platformInit()
{
    m_notificationsAllowed = false;

    QTimer::singleShot(0, this, [&]() {
        __block QPointer<PlatformSupport> selfRef(this);
        [[UNUserNotificationCenter currentNotificationCenter]
            getNotificationSettingsWithCompletionHandler:^(UNNotificationSettings *settings) {
                if (auto self = selfRef.get()) {
                    switch (settings.authorizationStatus) {
                    case UNAuthorizationStatusAuthorized:
                    case UNAuthorizationStatusProvisional:
                    case UNAuthorizationStatusEphemeral:
                        self->m_notificationsAllowed = true;
                        break;
                    default:
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
                      completionHandler:^(BOOL granted, NSError *_Nullable error) {
                          if (auto self = selfRef.get()) {
                              self->m_notificationsAllowed = (granted != NO);
                          }
                      }];
}

bool PlatformSupport::requiresNotificationPermission() const
{
    return true;
}
