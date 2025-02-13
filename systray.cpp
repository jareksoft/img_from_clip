// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#include "systray.h"
#include <QApplication>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>

SysTray::SysTray(QQuickItem *parent) : QQuickItem(parent) {
  if (!QSystemTrayIcon::isSystemTrayAvailable()) {
    qWarning() << "Systray not present";
  } else {
    qDebug() << "Systray present";
  }

  m_icon = new QSystemTrayIcon(this);

  auto trayIconMenu = new QMenu();

  auto quitAction = new QAction(tr("&Quit"), this);
  connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);

  trayIconMenu->addAction(quitAction);

  m_icon->setContextMenu(trayIconMenu);

  m_toolTipChange =
      m_toolTip.addNotifier([&]() { m_icon->setToolTip(m_toolTip.value()); });

  m_toolTip.setBinding([&]() -> QString {
    if (m_isActive.value())
      return SysTray::tr("Image capture active");
    return SysTray::tr("Image capture not active");
  });

  connect(
      m_icon, &QSystemTrayIcon::activated, this,
      [&](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
          emit iconClicked();
        }
      },
      Qt::QueuedConnection);
}

void SysTray::notifyInfo(const QString &title, const QString &info) {
  if (m_notificationsAllowed.value()) {
    m_icon->showMessage(title, info, QSystemTrayIcon::Information, 5000);
  }
}

void SysTray::notifyError(const QString &title, const QString &error) {
  if (m_notificationsAllowed.value()) {
    m_icon->showMessage(title, error, QSystemTrayIcon::Warning, 5000);
  }
}

void SysTray::aboutApp() {
  QMessageBox::about(
      nullptr, tr("ImgFromClip"),
      tr("ImgFromClip allows user to capture image or SVG copied into "
         "clipboard "
         "and capture it into specific location on disk "
         "automatically.<br/>Application icon:<br/>"
         "<a href=\"https://www.flaticon.com/free-icons/screenshot\" "
         "title=\"screenshot icons\">Screenshot icons created by icon_small - "
         "Flaticon</a>"));
}

void SysTray::aboutQt() { QApplication::aboutQt(); }

QString SysTray::iconPath() const { return m_iconPath; }

void SysTray::setIconPath(const QString &newIconPath) {
  if (m_iconPath == newIconPath)
    return;
  m_iconPath = newIconPath;
  emit iconPathChanged(m_iconPath);

  QPixmap icon(newIconPath);

  if (icon.isNull()) {
    qWarning() << "Failed to load icon" << newIconPath;
  } else {
    qDebug() << "Icon loaded";
  }

  m_icon->setIcon(icon);
  m_icon->show();
}
