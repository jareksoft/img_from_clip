#include "systray.h"
#include <QApplication>
#include <QDebug>
#include <QMenu>

SysTray::SysTray(QQuickItem *parent)
    : QQuickItem(parent)
{
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

    m_toolTipChange = m_toolTip.addNotifier([&]() {
        qDebug() << "TT = " << m_toolTip.value();
        m_icon->setToolTip(m_toolTip.value());
    });

    m_toolTip.setBinding([&]() -> QString {
        if (m_isActive.value())
            return SysTray::tr("Image capture active");
        return SysTray::tr("Image capture not active");
    });
}

void SysTray::notifyInfo(const QString &info)
{
    if (m_notificationsAllowed.value()) {
        m_icon->showMessage(tr("Image Captured"), info, QSystemTrayIcon::Information, 1000);
    }
}

void SysTray::notifyError(const QString &error)
{
    if (m_notificationsAllowed.value()) {
        m_icon->showMessage(tr("Capture error"), error, QSystemTrayIcon::Warning, 1000);
    }
}

QString SysTray::iconPath() const
{
    return m_iconPath;
}

void SysTray::setIconPath(const QString &newIconPath)
{
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
