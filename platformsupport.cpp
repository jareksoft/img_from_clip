// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#include "platformsupport.h"

int PlatformSupport::notificationsAllowed() const {
  return m_notificationsAllowed;
}

void PlatformSupport::notify(NotifyClass type, const QString &title,
                             const QString &description) {
  notifyWithImage(type, title, description, QUrl());
}

QString PlatformSupport::showUrl(const QUrl &url) { return url.toLocalFile(); }
