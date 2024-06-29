#include "captureobject.h"

CaptureObject::CaptureObject() {}

CaptureObject::~CaptureObject() {}

QString CaptureObject::path() const { return m_path; }

void CaptureObject::setPath(const QString &newPath) { m_path = newPath; }

QString CaptureObject::saveMode() const { return m_saveMode; }

void CaptureObject::setSaveMode(const QString &newSaveMode) {
  m_saveMode = newSaveMode;
}

void CaptureObject::registerSol(sol::state &lua) {
  auto capture_class = lua.new_usertype<CaptureObject>(
      "CaptureObject", "new",
      sol::factories([]() { return std::make_shared<CaptureObject>(); }),
      "path", sol::property(&CaptureObject::path, &CaptureObject::setPath),
      "saveMode",
      sol::property(&CaptureObject::saveMode, &CaptureObject::setSaveMode),
      "width", sol::property(&CaptureObject::width, &CaptureObject::setWidth),
      "height",
      sol::property(&CaptureObject::height, &CaptureObject::setHeight),
      "contents",
      sol::property(&CaptureObject::contents, &CaptureObject::setContents));
}

int CaptureObject::width() const { return m_width; }

void CaptureObject::setWidth(int newWidth) { m_width = newWidth; }

int CaptureObject::height() const { return m_height; }

void CaptureObject::setHeight(int newHeight) { m_height = newHeight; }

QString CaptureObject::contents() const { return m_contents; }

void CaptureObject::setContents(const QString &newContents) {
  m_contents = newContents;
}
