// SPDX-License-Identifier: BSD-2-Clause

#include "platformsupport.h"

int PlatformSupport::notificationsAllowed() const {
  return m_notificationsAllowed;
}

void PlatformSupport::notify(NotifyClass type, const QString &title,
                             const QString &description) {
  notifyWithImage(type, title, description, QString());
}
