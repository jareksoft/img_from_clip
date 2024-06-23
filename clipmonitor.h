// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CLIPMONITOR_H
#define CLIPMONITOR_H

#include <QClipboard>
#include <QDateTime>
#include <QImage>
#include <QObject>
#include <QProperty>
#include <QQmlEngine>
#include <QRandomGenerator>
#include <QTimer>

class ClipMonitor : public QObject {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(bool active READ default WRITE default BINDABLE activeBindable)
  Q_PROPERTY(QUrl savePath READ default WRITE default BINDABLE savePathBindable)
  Q_PROPERTY(QString savePattern
                 READ default WRITE default BINDABLE savePatternBindable)
  Q_PROPERTY(
      SaveMode saveMode READ default WRITE default BINDABLE saveModeBindable)
  Q_PROPERTY(
      bool htmlAllowed READ default WRITE default BINDABLE htmlAllowedBindable)
  Q_PROPERTY(
      qreal renderWidth READ default WRITE default BINDABLE renderWidthBindable)
public:
  enum class SaveMode { SVG, PNG, JPG };
  Q_ENUM(SaveMode)

  struct Html {
    QString html;
    bool operator==(const Html &other) const { return html == other.html; }
    bool operator!=(const Html &other) const { return html != other.html; }
    bool operator<(const Html &other) const { return html < other.html; }
  };

  using contents_t = std::variant<std::monostate, QImage, QString, Html>;

  explicit ClipMonitor(QObject *parent = nullptr);

  auto activeBindable() -> QBindable<bool> { return {&m_activeBinding}; }
  auto savePathBindable() -> QBindable<QUrl> { return {&m_savePath}; }
  auto savePatternBindable() -> QBindable<QString> { return {&m_savePattern}; }
  auto saveModeBindable() -> QBindable<SaveMode> { return {&m_saveMode}; }
  auto htmlAllowedBindable() -> QBindable<bool> { return {&m_htmlAllowed}; }
  auto renderWidthBindable() -> QBindable<qreal> { return {&m_renderWidth}; }

  // Can't use trailing return type because of MOC
  Q_INVOKABLE QString makeNewSavePath(QString pattern);

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
  auto getLastClipboard() -> contents_t;

private:
  QDateTime m_lastCapture;
  QProperty<bool> m_activeBinding;
  QProperty<QUrl> m_savePath;
  QProperty<QString> m_savePattern;
  QProperty<SaveMode> m_saveMode{SaveMode::SVG};
  QClipboard *m_clipboard;
  contents_t m_lastClipboard;
  QTimer *m_pollTimer;
  QPropertyNotifier m_timerBinding;
  QProperty<bool> m_htmlAllowed{false};
  QProperty<qreal> m_renderWidth{800.0};
  QRandomGenerator m_rng;
};

#endif // CLIPMONITOR_H
