// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#include "platformsupport.h"
#include <QTimer>

PlatformSupport::PlatformSupport(QObject *parent) : QObject{parent} {
  platformInit();
  QTimer::singleShot(100, this,
                     &PlatformSupport::permissionStateFetchCompletion);
}

PlatformSupport::~PlatformSupport() = default;

void PlatformSupport::platformInit() {}

void PlatformSupport::requestNotifications() {}

bool PlatformSupport::requiresNotificationPermission() const { return false; }

void PlatformSupport::notifyWithImage(NotifyClass type, const QString &title,
                                      const QString &description,
                                      const QUrl &) {
  switch (type) {
  case NotifyClass::Error:
    emit genericNotifyError(title, description);
    break;
  case NotifyClass::Info:
    emit genericNotifyInfo(title, description);
    break;
  }
}
