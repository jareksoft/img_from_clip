// SPDX-License-Identifier: GPL-3.0-or-later

#include "platformsupport.h"

int PlatformSupport::notificationsAllowed() const {
  return m_notificationsAllowed;
}

void PlatformSupport::notify(NotifyClass type, const QString &title,
                             const QString &description) {
  notifyWithImage(type, title, description, QUrl());
}

QString PlatformSupport::showUrl(const QUrl &url) { return url.toLocalFile(); }
