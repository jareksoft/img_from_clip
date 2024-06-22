// SPDX-License-Identifier: BSD-2-Clause

#ifndef CLIPMONITOR_H
#define CLIPMONITOR_H

#include <QClipboard>
#include <QDateTime>
#include <QImage>
#include <QObject>
#include <QProperty>
#include <QQmlEngine>
#include <QTimer>

class ClipMonitor : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool active READ default WRITE default BINDABLE activeBindable)
    Q_PROPERTY(QUrl savePath READ default WRITE default BINDABLE savePathBindable)
    Q_PROPERTY(QString savePattern READ default WRITE default BINDABLE savePatternBindable)
    Q_PROPERTY(SaveMode saveMode READ default WRITE default BINDABLE saveModeBindable)
public:
    enum class SaveMode { SVG, PNG, JPG };
    Q_ENUM(SaveMode)

    explicit ClipMonitor(QObject *parent = nullptr);

    auto activeBindable() -> QBindable<bool> { return {&m_activeBinding}; }
    auto savePathBindable() -> QBindable<QUrl> { return {&m_savePath}; }
    auto savePatternBindable() -> QBindable<QString> { return {&m_savePattern}; }
    auto saveModeBindable() -> QBindable<SaveMode> { return {&m_saveMode}; }

    // Can't use trailing return type because of MOC
    Q_INVOKABLE QString makeNewSavePath();

signals:
    void notifyCapture(QString lastPath);
    void saveFailed(QString reason);

private:
    void saveRawSvg(const QString &contents);
    void renderSvgToPng(const QString &contents);
    void renderSvgToJpg(const QString &contents);
    void savePng(const QImage &image);
    void saveSvg(const QImage &image);
    void saveJpg(const QImage &image);
    void createSave(std::function<void(QIODevice *)> callback);

private:
    void clipboardDataChanged();
    auto getLastClipboard() -> std::variant<std::monostate, QImage, QString>;

private:
    QDateTime m_lastCapture;
    QProperty<bool> m_activeBinding;
    QProperty<QUrl> m_savePath;
    QProperty<QString> m_savePattern;
    QProperty<SaveMode> m_saveMode{SaveMode::SVG};
    QClipboard *m_clipboard;
    std::variant<std::monostate, QImage, QString> m_lastClipboard;
    QTimer *m_pollTimer;
    QPropertyNotifier m_timerBinding;
};

#endif // CLIPMONITOR_H
