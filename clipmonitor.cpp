#include "clipmonitor.h"
#include <QApplication>
#include <QBuffer>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QMimeData>
#include <QPainter>
#include <QSvgGenerator>
#include <QTextStream>

ClipMonitor::ClipMonitor(QObject *parent)
    : QObject{parent}
    , m_clipboard(QApplication::clipboard())
{
    m_lastCapture = QDateTime::currentDateTimeUtc();
    connect(m_clipboard, &QClipboard::dataChanged, this, &ClipMonitor::clipboardDataChanged);
}

auto ClipMonitor::makeNewSavePath() -> QString
{
    QString pattern = m_savePattern.value();
    if (pattern.isEmpty())
        pattern = QStringLiteral("{timestamp}");

    pattern.replace(QStringLiteral("{timestamp}"),
                    QString::number(QDateTime::currentSecsSinceEpoch()));

    switch (m_saveMode.value()) {
    case SaveMode::SVG:
        pattern.append(QStringLiteral(".svg"));
        break;
    case SaveMode::PNG:
        pattern.append(QStringLiteral(".png"));
        break;
    case SaveMode::JPG:
        pattern.append(QStringLiteral(".jpg"));
        break;
    }

    return pattern;
}

void ClipMonitor::saveRawSvg(const QString &contents)
{
    createSave([&](QIODevice *dev) { dev->write(contents.toUtf8()); });
}

void ClipMonitor::renderSvgToPng(const QString &contents)
{
    QImage image;
    if (image.loadFromData(contents.toUtf8())) {
        savePng(image);
    }
}

void ClipMonitor::renderSvgToJpg(const QString &contents)
{
    QImage image;
    if (image.loadFromData(contents.toUtf8())) {
        saveJpg(image);
    }
}

void ClipMonitor::savePng(const QImage &image)
{
    createSave([&](QIODevice *dev) { image.save(dev, "PNG"); });
}

void ClipMonitor::saveSvg(const QImage &image)
{
    createSave([&](QIODevice *dev) {
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

void ClipMonitor::saveJpg(const QImage &image)
{
    createSave([&](QIODevice *dev) { image.save(dev, "JPG"); });
}

void ClipMonitor::createSave(std::function<void(QIODevice *)> callback)
{
    QDir dir(m_savePath.value().toLocalFile());

    if (!dir.exists()) {
        emit saveFailed(tr("Save directory does not exist"));
        return;
    }

    auto path = dir.absoluteFilePath(makeNewSavePath());

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::NewOnly)) {
        emit saveFailed(file.errorString());
        return;
    }

    callback(&file);

    if (file.error()) {
        emit saveFailed(file.errorString());
        file.remove();
    } else {
        emit notifyCapture(path);
    }
}

void ClipMonitor::clipboardDataChanged()
{
    // Ignore if not active
    if (!m_activeBinding.value())
        return;

    auto now = QDateTime::currentDateTimeUtc();

    // Throttle and ignore first signal
    if (m_lastCapture.secsTo(now) < 1)
        return;

    m_lastCapture = now;
    const auto mimeData = m_clipboard->mimeData();

    qDebug() << "MIME is " << mimeData->formats();

    // Try text first
    QString textContents = mimeData->text();

    QString capturePattern = m_savePattern.value();

    // Check if we have SVG
    if (!textContents.isEmpty()) {
        if (textContents.contains(QStringLiteral("<svg"))) {
            switch (m_saveMode) {
            case SaveMode::SVG:
                saveRawSvg(textContents);
                break;
            case SaveMode::PNG:
                renderSvgToPng(textContents);
                break;
            case SaveMode::JPG:
                renderSvgToJpg(textContents);
                break;
            }
        }
    }
}
