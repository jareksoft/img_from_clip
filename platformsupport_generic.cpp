// SPDX-License-Identifier: BSD-2-Clause

#include <QTimer>
#include "platformsupport.h"

PlatformSupport::PlatformSupport(QObject *parent)
    : QObject{parent}
{
    platformInit();
    QTimer::singleShot(100, this, &PlatformSupport::permissionStateFetchCompletion);
}

PlatformSupport::~PlatformSupport() = default;

void PlatformSupport::platformInit() {}

void PlatformSupport::requestNotifications() {}

bool PlatformSupport::requiresNotificationPermission() const
{
    return false;
}
