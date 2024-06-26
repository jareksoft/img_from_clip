#ifndef CLIPBOARDSOURCE_H
#define CLIPBOARDSOURCE_H

#include "clipboardcontents.h"
#include <QObject>
#include <QProperty>
#include <QQmlEngine>
#include <QTimer>

class ClipboardSource : public QObject {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(bool monitoringActive READ monitoringActive WRITE
                 setMonitoringActive NOTIFY monitoringActiveChanged)
  Q_PROPERTY(bool htmlAllowed READ htmlAllowed WRITE setHtmlAllowed BINDABLE
                 htmlAllowedBindable)
public:
  explicit ClipboardSource(QObject *parent = nullptr);

  [[nodiscard]] bool monitoringActive() const;
  void setMonitoringActive(bool newMonitoringActive);

  [[nodiscard]] bool htmlAllowed() const;
  void setHtmlAllowed(bool newHtmlAllowed);
  QBindable<bool> htmlAllowedBindable() { return {&m_htmlAllowed}; }

signals:
  void monitoringActiveChanged(bool monitoringActive);
  void newCapture(const ClipboardContents &);

private:
  ClipboardContents captureOnce();
  void clipboardChanged();
  void handleApplicationState(Qt::ApplicationState);
  bool updateCaptureCache(const ClipboardContents &);

private:
  bool m_monitoringActive = false;
  QProperty<bool> m_htmlAllowed{false};
  QTimer m_periodicChecker;
  QList<QByteArray> m_lastHashes;
  QTimer m_updateCombiner;
};

#endif // CLIPBOARDSOURCE_H
