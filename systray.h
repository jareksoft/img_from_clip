#ifndef SYSTRAY_H
#define SYSTRAY_H

#include <QProperty>
#include <QQuickItem>
#include <QSystemTrayIcon>

class SysTray : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool isActive READ default WRITE default BINDABLE isActiveBindable)
    Q_PROPERTY(QString iconPath READ iconPath WRITE setIconPath NOTIFY iconPathChanged)
public:
    explicit SysTray(QQuickItem *parent = nullptr);

    auto isActiveBindable() -> QBindable<bool> { return {&m_isActive}; }

    [[nodiscard]] QString iconPath() const;
    void setIconPath(const QString &newIconPath);

public slots:
    void notifyInfo(const QString &);
    void notifyError(const QString &);

signals:
    void iconPathChanged(QString iconPath);

private:
    QSystemTrayIcon *m_icon;
    QProperty<bool> m_isActive{false};
    QProperty<QString> m_toolTip;
    QPropertyNotifier m_toolTipChange;
    QString m_iconPath;
};

#endif // SYSTRAY_H
