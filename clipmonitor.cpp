// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#include "clipmonitor.h"
#include "rsvg_helper.h"
#include <QApplication>
#include <QBuffer>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QMimeData>
#include <QPainter>
#include <QRandomGenerator>
#include <QSvgGenerator>
#include <QSysInfo>
#include <QTextDocument>
#include <QTextStream>
#include <QtConcurrent>

ClipMonitor::ClipMonitor(QObject *parent) : QObject{parent} {
  m_rng.seed(static_cast<uint32_t>(QDateTime::currentMSecsSinceEpoch()));

  m_source = new ClipboardSource(this);

  QBindable<bool> activeBindable(m_source, "monitoringActive");
  activeBindable.setBinding([&]() { return m_activeBinding.value(); });

  m_source->htmlAllowedBindable().setBinding(
      [&]() { return m_htmlAllowed.value(); });

  connect(m_source, &ClipboardSource::newCapture, this,
          &ClipMonitor::newCapture, Qt::QueuedConnection);
}

auto ClipMonitor::makeNewSavePath(QString pattern) -> QString {
  if (pattern.isEmpty()) {
    pattern = QStringLiteral("{timestamp}");
  }

  pattern.replace(QStringLiteral("{timestamp}"),
                  QString::number(QDateTime::currentSecsSinceEpoch()));
  pattern.replace(QStringLiteral("{date}"),
                  QDate::currentDate().toString(Qt::DateFormat::ISODate));
  pattern.replace(QStringLiteral("{time}"),
                  QTime::currentTime().toString(Qt::DateFormat::ISODate));
  pattern.replace(QStringLiteral("{cpu}"), QSysInfo::currentCpuArchitecture());
  pattern.replace(QStringLiteral("{hostname}"), QSysInfo::machineHostName());
  pattern.replace(QStringLiteral("{rand}"), QString::number(m_rng()));
  pattern.replace(QStringLiteral("{seq}"),
                  QString::number(m_captureSeq.value()));

  pattern.replace(QChar('/'), QChar('_'));
  pattern.replace(QChar('\\'), QChar('_'));
  pattern.replace(QChar(':'), QChar('_'));
  pattern.replace(QChar(';'), QChar('_'));
  pattern.replace(QStringLiteral(".."), QStringLiteral("_"));

  return pattern;
}

ClipMonitor::SaveResult ClipMonitor::saveRawSvg(const QString &fileName,
                                                const QString &contents) {
  return createSave(fileName + QStringLiteral(".svg"),
                    [&](QIODevice *dev) { dev->write(contents.toUtf8()); });
}

ClipMonitor::SaveResult ClipMonitor::renderSvgToPng(const QString &fileName,
                                                    double scale,
                                                    const QString &scaleSuffix,
                                                    const QString &contents) {
#ifdef USE_LIBRSVG_RENDERER
  auto image = RsvgRender::renderSvg(contents, scale);
  if (!image.isNull()) {
    return savePng(fileName, 1.0, scaleSuffix, image);
  }
#else
  QImage image;
  if (image.loadFromData(contents.toUtf8())) {
    return savePng(fileName, scale, scaleSuffix, image);
  }
#endif
  return Error{tr("Could not load source contents")};
}

ClipMonitor::SaveResult ClipMonitor::renderSvgToJpg(const QString &fileName,
                                                    const QString &contents) {
#ifdef USE_LIBRSVG_RENDERER
  auto image = RsvgRender::renderSvg(contents, 1.0);
  if (!image.isNull()) {
    return saveJpg(fileName, image);
  }
#else
  QImage image;
  if (image.loadFromData(contents.toUtf8())) {
    return saveJpg(fileName, image);
  }
#endif
  return Error{tr("Could not load image")};
}

ClipMonitor::SaveResult ClipMonitor::savePng(const QString &fileName,
                                             double scale,
                                             const QString &scaleSuffix,
                                             const QImage &image) {
  if (qFuzzyCompare(scale, 1.0) && scaleSuffix.isEmpty()) {
    return createSave(fileName + QStringLiteral(".png"),
                      [&](QIODevice *dev) { image.save(dev, "PNG"); });
  } else {
    return createSave(
        fileName + scaleSuffix + QStringLiteral(".png"), [&](QIODevice *dev) {
          QSizeF newSize(image.size());
          QSize scaledSize =
              newSize
                  .scaled((double)image.width() * scale,
                          (double)image.height() * scale, Qt::IgnoreAspectRatio)
                  .toSize();

          auto scaledImage = image.scaled(scaledSize, Qt::IgnoreAspectRatio,
                                          Qt::SmoothTransformation);
          scaledImage.save(dev, "PNG");
        });
  }
}

ClipMonitor::SaveResult ClipMonitor::saveSvg(const QString &fileName,
                                             const QImage &image) {
  return createSave(fileName + QStringLiteral(".svg"), [&](QIODevice *dev) {
    QSvgGenerator generator;
    QRect viewBox;
    viewBox.setSize(image.size());

    generator.setOutputDevice(dev);
    generator.setSize(image.size());
    generator.setViewBox(viewBox);
    generator.setTitle(tr("Generated by QSvgGenerator"));
    generator.setDescription(tr("Image imported from clipboard"));

    QPainter painter;
    painter.begin(&generator);
    painter.drawImage(0, 0, image);
    painter.end();
  });
}

ClipMonitor::SaveResult ClipMonitor::saveJpg(const QString &fileName,
                                             const QImage &image) {
  return createSave(fileName + QStringLiteral(".jpg"),
                    [&](QIODevice *dev) { image.save(dev, "JPG"); });
}

ClipMonitor::SaveResult
ClipMonitor::createSave(const QString &fileName,
                        std::function<void(QIODevice *)> callback) {
  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::NewOnly)) {
    return Error{file.errorString()};
  }

  qDebug() << "Saving file" << fileName;

  callback(&file);

  if (file.error()) {
    QString reason = file.errorString();
    file.remove();
    return Error{reason};
  } else {
    return ClipMonitor::SaveResult{SavePath{QUrl::fromLocalFile(fileName)}};
  }
}

void ClipMonitor::newCapture(const ClipboardContents &contents) {
  RenderConfiguration *renderConfiguration = m_renderConfiguration.value();

  // Ignore if not active
  if (!m_activeBinding.value() || !renderConfiguration)
    return;

  QList<ClipMonitor::SaveResult> results;

  auto fileName = QDir(m_savePath.value().toLocalFile())
                      .filePath(makeNewSavePath(m_savePattern.value()));

  if (contents.type() == ClipboardContents::Type::Image) {
    results = savePlainImage(fileName, contents.image());
  } else if (contents.type() == ClipboardContents::Type::Html) {
    if (m_htmlAllowed.value()) {
      if (renderConfiguration->svgBindable().value()) {
        QTextDocument doc;
        doc.setTextWidth(m_renderWidth.value());
        doc.setHtml(contents.html());

        // Prefer rendering vectors instead of bitmap inside SVG
        results.push_back(createSave(fileName, [&](QIODevice *dev) {
          QSvgGenerator generator;

          QRect viewBox;
          viewBox.setSize(doc.size().toSize());

          generator.setOutputDevice(dev);
          generator.setSize(viewBox.size());
          generator.setViewBox(viewBox);
          generator.setTitle(tr("Generated by QSvgGenerator"));
          generator.setDescription(tr("HTML imported from clipboard"));

          QPainter painter;
          painter.begin(&generator);
          doc.drawContents(&painter);
          painter.end();
        }));
      }

      if (renderConfiguration->jpgBindable().value() ||
          renderConfiguration->pngBindable().value()) {
        QTextDocument doc;
        doc.setTextWidth(m_renderWidth.value());
        doc.setHtml(contents.html());
        QImage image(doc.size().toSize(), QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&image);
        painter.setRenderHints(QPainter::Antialiasing |
                               QPainter::SmoothPixmapTransform);
        doc.drawContents(&painter);

        // Use SVG renderer to avoid saving bitmap inside SVG if possible
        results.append(savePlainImage(fileName, image, false));
      }
    } else {
      qWarning() << "HTML not allowed";
    }
  } else if (contents.type() == ClipboardContents::Type::Text) {
    if (!contents.text().contains(QStringLiteral("<svg")))
      return;

    if (renderConfiguration->svgBindable().value()) {
      results.push_back(saveRawSvg(fileName, contents.text()));
    }

    if (renderConfiguration->jpgBindable().value()) {
      results.append(saveSvgImage(fileName, contents.text()));
    }
  }

  if (results.isEmpty())
    return;

  if (std::all_of(std::cbegin(results), std::cend(results),
                  [](const SaveResult &result) {
                    return std::holds_alternative<Error>(result);
                  })) {
    emit saveFailed(std::get<Error>(results.at(0)));
  } else {
    auto it = std::find_if(std::cbegin(results), std::cend(results),
                           [](const SaveResult &result) {
                             return std::holds_alternative<SavePath>(result);
                           });
    if (it == std::cend(results)) {
      qCritical() << "Nothing found ???";
      return;
    }

    m_captureSeq.setValue(m_captureSeq.value() + 1);

    emit notifyCapture(std::get<SavePath>(*it));
  }
}

QList<ClipMonitor::SaveResult>
ClipMonitor::savePlainImage(const QString &fileName, const QImage &image,
                            bool includeSvg) {
  QList<ClipMonitor::SaveResult> result;
  RenderConfiguration *config = m_renderConfiguration.value();
  if (!config)
    return result;

  if (includeSvg && config->svgBindable().value()) {
    result.push_back(saveSvg(fileName, image));
  }

  if (config->jpgBindable().value()) {
    result.push_back(saveJpg(fileName, image));
  }
  if (config->pngBindable().value()) {
    result.push_back(savePng(fileName, 1.0, QString(), image));

    if (config->scale15Bindable().value()) {
      result.push_back(savePng(fileName, 1.5, QStringLiteral("@1.5x"), image));
    }

    if (config->scale2Bindable().value()) {
      result.push_back(savePng(fileName, 2.0, QStringLiteral("@2x"), image));
    }

    if (config->scale3Bindable().value()) {
      result.push_back(savePng(fileName, 3.0, QStringLiteral("@3x"), image));
    }

    if (config->scale4Bindable().value()) {
      result.push_back(savePng(fileName, 4.0, QStringLiteral("@4x"), image));
    }
  }

  return result;
}

QList<ClipMonitor::SaveResult>
ClipMonitor::saveSvgImage(const QString &fileName, const QString &svgContents) {
  QList<ClipMonitor::SaveResult> result;
  RenderConfiguration *config = m_renderConfiguration.value();
  if (!config)
    return result;

  if (config->svgBindable().value()) {
    result.push_back(saveRawSvg(fileName, svgContents));
  }

  if (config->jpgBindable().value()) {
    result.push_back(renderSvgToJpg(fileName, svgContents));
  }

  if (config->pngBindable().value()) {
    result.push_back(renderSvgToPng(fileName, 1.0, QString(), svgContents));

    if (config->scale15Bindable().value()) {
      result.push_back(
          renderSvgToPng(fileName, 1.5, QStringLiteral("@1.5x"), svgContents));
    }

    if (config->scale2Bindable().value()) {
      result.push_back(
          renderSvgToPng(fileName, 2.0, QStringLiteral("@2x"), svgContents));
    }

    if (config->scale3Bindable().value()) {
      result.push_back(
          renderSvgToPng(fileName, 3.0, QStringLiteral("@3x"), svgContents));
    }

    if (config->scale4Bindable().value()) {
      result.push_back(
          renderSvgToPng(fileName, 4.0, QStringLiteral("@4x"), svgContents));
    }
  }

  return result;
}

RenderConfiguration *ClipMonitor::renderConfiguration() const {
  return m_renderConfiguration;
}

void ClipMonitor::setRenderConfiguration(
    RenderConfiguration *newRenderConfiguration) {
  m_renderConfiguration = newRenderConfiguration;
}

void ClipMonitor::setCaptureSeq(qint64 newCaptureSeq) {
  m_captureSeq = newCaptureSeq;
}

qint64 ClipMonitor::captureSeq() const { return m_captureSeq; }
