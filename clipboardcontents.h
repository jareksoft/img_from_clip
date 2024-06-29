// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#ifndef CLIPBOARDCONTENTS_H
#define CLIPBOARDCONTENTS_H

#include <QImage>
#include <QQmlEngine>
#include <QSharedDataPointer>

class ClipboardContentsData;

class ClipboardContents {
  Q_GADGET
  QML_ELEMENT
public:
  enum class Type { Nothing, Text, Html, Image };
  Q_ENUM(Type)

  ClipboardContents();
  ClipboardContents(const ClipboardContents &);
  ClipboardContents &operator=(const ClipboardContents &);
  ~ClipboardContents();

  QByteArray hash() const noexcept;

  Type type() const noexcept;

  QString text() const noexcept;
  void setText(const QString &text);

  QString html() const noexcept;
  void setHtml(const QString &html);

  QImage image() const noexcept;
  void setImage(const QImage &image);

private:
  QSharedDataPointer<ClipboardContentsData> data;
};

#endif // CLIPBOARDCONTENTS_H
