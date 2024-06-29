// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CLIPMONITOR_H
#define CLIPMONITOR_H

#include "clipboardsource.h"
#include "renderconfiguration.h"
#include <QClipboard>
#include <QDateTime>
#include <QFuture>
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
      bool htmlAllowed READ default WRITE default BINDABLE htmlAllowedBindable)
  Q_PROPERTY(
      qreal renderWidth READ default WRITE default BINDABLE renderWidthBindable)
  Q_PROPERTY(
      RenderConfiguration *renderConfiguration READ renderConfiguration WRITE
          setRenderConfiguration BINDABLE renderConfigurationBindable)
  Q_PROPERTY(qint64 captureSeq READ captureSeq WRITE setCaptureSeq BINDABLE
                 captureSeqBindable)
public:
  explicit ClipMonitor(QObject *parent = nullptr);

  auto activeBindable() -> QBindable<bool> { return {&m_activeBinding}; }
  auto savePathBindable() -> QBindable<QUrl> { return {&m_savePath}; }
  auto savePatternBindable() -> QBindable<QString> { return {&m_savePattern}; }
  auto htmlAllowedBindable() -> QBindable<bool> { return {&m_htmlAllowed}; }
  auto renderWidthBindable() -> QBindable<qreal> { return {&m_renderWidth}; }
  auto renderConfigurationBindable() -> QBindable<RenderConfiguration *> {
    return {&m_renderConfiguration};
  }
  auto captureSeqBindable() -> QBindable<qint64> { return {&m_captureSeq}; }

  // Can't use trailing return type because of MOC
  Q_INVOKABLE QString makeNewSavePath(QString pattern);

  [[nodiscard]] RenderConfiguration *renderConfiguration() const;
  void setRenderConfiguration(RenderConfiguration *newRenderConfiguration);

  void setCaptureSeq(qint64 newCaptureSeq);
  qint64 captureSeq() const;

signals:
  void notifyCapture(QUrl lastPath);
  void saveFailed(QString reason);

private:
  struct Error : QString {};
  struct SavePath : QUrl {};

  using SaveResult = std::variant<Error, SavePath>;

  SaveResult saveRawSvg(const QString &fileName, const QString &contents);
  SaveResult renderSvgToPng(const QString &fileName, double scale,
                            const QString &scaleSuffix,
                            const QString &contents);
  SaveResult renderSvgToJpg(const QString &fileName, const QString &contents);
  SaveResult savePng(const QString &fileName, double scale,
                     const QString &scaleSuffix, const QImage &image);
  SaveResult saveSvg(const QString &fileName, const QImage &image);
  SaveResult saveJpg(const QString &fileName, const QImage &image);
  SaveResult createSave(const QString &fileName,
                        std::function<void(QIODevice *)> callback);
  void newCapture(const ClipboardContents &);
  QList<SaveResult> savePlainImage(const QString &fileName, const QImage &image,
                                   bool includeSvg = true);
  QList<SaveResult> saveSvgImage(const QString &fileName,
                                 const QString &svgContents);

private:
  ClipboardSource *m_source;
  QProperty<bool> m_activeBinding;
  QProperty<QUrl> m_savePath;
  QProperty<QString> m_savePattern;
  QProperty<bool> m_htmlAllowed{false};
  QProperty<qreal> m_renderWidth{800.0};
  QRandomGenerator m_rng;
  QProperty<RenderConfiguration *> m_renderConfiguration{nullptr};
  QProperty<qint64> m_captureSeq{1};
};

#endif // CLIPMONITOR_H
