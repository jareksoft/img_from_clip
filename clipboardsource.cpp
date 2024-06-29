// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#include "clipboardsource.h"
#include <QApplication>
#include <QClipboard>
#include <QMimeData>

ClipboardSource::ClipboardSource(QObject *parent) : QObject{parent} {
  // Issue single capture to avoid grabbing "startup" contents
  clipboardChanged();

  m_periodicChecker.setInterval(900);
  m_periodicChecker.setSingleShot(false);

  m_updateCombiner.setInterval(100);
  m_updateCombiner.setSingleShot(true);
  connect(&m_updateCombiner, &QTimer::timeout, this, [&]() {
    if (m_monitoringActive) {
      clipboardChanged();
    }
  });

  connect(&m_periodicChecker, &QTimer::timeout, this, [&]() {
    if (m_monitoringActive && !m_updateCombiner.isActive())
      m_updateCombiner.start();
  });

  connect(QApplication::clipboard(), &QClipboard::dataChanged, this, [&]() {
    if (m_monitoringActive && !m_updateCombiner.isActive())
      m_updateCombiner.start();
  });

  connect(qApp, &QApplication::applicationStateChanged, this,
          &ClipboardSource::handleApplicationState);
}

bool ClipboardSource::monitoringActive() const { return m_monitoringActive; }

void ClipboardSource::setMonitoringActive(bool newMonitoringActive) {
  if (m_monitoringActive == newMonitoringActive)
    return;
  m_monitoringActive = newMonitoringActive;
  emit monitoringActiveChanged(m_monitoringActive);

  if (m_monitoringActive) {
    auto capture = captureOnce();
    if (capture.type() != ClipboardContents::Type::Nothing)
      updateCaptureCache(capture);
  } else {
    m_lastHashes.clear();
  }

  handleApplicationState(QApplication::applicationState());
}

bool ClipboardSource::htmlAllowed() const { return m_htmlAllowed; }

void ClipboardSource::setHtmlAllowed(bool newHtmlAllowed) {
  m_htmlAllowed = newHtmlAllowed;
}

ClipboardContents ClipboardSource::captureOnce() {
  ClipboardContents result;
  const auto mimeData = qApp->clipboard()->mimeData();

  if (!mimeData)
    return result;

  if (mimeData->hasImage()) {
    result.setImage(qvariant_cast<QImage>(mimeData->imageData()));
  } else if (mimeData->hasHtml()) {
    result.setHtml(mimeData->html());
  } else if (mimeData->hasText()) {
    result.setText(mimeData->text());
  }
  return result;
}

void ClipboardSource::clipboardChanged() {
  auto capture = captureOnce();
  if (capture.type() == ClipboardContents::Type::Nothing)
    return;
  if (!updateCaptureCache(capture))
    return;
  if (!m_htmlAllowed && capture.type() == ClipboardContents::Type::Html)
    return;
  emit newCapture(capture);
}

void ClipboardSource::handleApplicationState(Qt::ApplicationState) {
  if (m_monitoringActive) {
    if (!m_periodicChecker.isActive()) {
      m_periodicChecker.start();
    }
  } else {
    m_periodicChecker.stop();
  }
}

bool ClipboardSource::updateCaptureCache(const ClipboardContents &capture) {
  auto hash = capture.hash();
  if (m_lastHashes.contains(hash))
    return false;
  if (m_lastHashes.size() > 64) {
    m_lastHashes.takeFirst();
  }
  m_lastHashes.push_back(hash);
  return true;
}
