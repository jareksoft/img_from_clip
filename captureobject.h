// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#ifndef CAPTUREOBJECT_H
#define CAPTUREOBJECT_H

#include "lua_pixmap.h"
#include "lua_qtclasses.h"
#include <QObject>
#include <QProperty>

class CaptureObject : public std::enable_shared_from_this<CaptureObject> {
  Q_GADGET
  Q_PROPERTY(QString path READ path WRITE setPath BINDABLE pathBindable)
  Q_PROPERTY(QString saveMode READ saveMode WRITE setSaveMode BINDABLE
                 saveModeBindable)
  Q_PROPERTY(int width READ width WRITE setWidth BINDABLE widthBindable)
  Q_PROPERTY(int height READ height WRITE setHeight BINDABLE heightBindable)
  Q_PROPERTY(QString contents READ contents WRITE setContents BINDABLE
                 contentsBindable)
public:
  CaptureObject();
  ~CaptureObject();

  CaptureObject(const CaptureObject &) = delete;
  CaptureObject &operator=(const CaptureObject &) = delete;

  static void registerSol(sol::state &lua);

  QBindable<QString> pathBindable() { return {&m_path}; }
  QBindable<QString> saveModeBindable() { return {&m_saveMode}; }
  QBindable<int> widthBindable() { return {&m_width}; }
  QBindable<int> heightBindable() { return {&m_width}; }
  QBindable<QString> contentsBindable() { return {&m_contents}; }

  QString path() const;
  void setPath(const QString &newPath);

  QString saveMode() const;
  void setSaveMode(const QString &newSaveMode);

  int width() const;
  void setWidth(int newWidth);

  int height() const;
  void setHeight(int newHeight);

  QString contents() const;
  void setContents(const QString &newContents);

  std::optional<Pixmap> image() const;
  void setImage(const std::optional<Pixmap> &image);

private:
  QProperty<QString> m_path;
  QProperty<QString> m_saveMode;
  QProperty<int> m_width{0};
  QProperty<int> m_height;
  QProperty<QString> m_contents;
  std::optional<Pixmap> m_image;
};

#endif // CAPTUREOBJECT_H
