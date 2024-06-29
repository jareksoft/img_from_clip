#ifndef CAPTUREOBJECT_H
#define CAPTUREOBJECT_H

#include "lua_qtclasses.h"
#include <QObject>

class CaptureObject : public std::enable_shared_from_this<CaptureObject> {
  Q_GADGET
public:
  CaptureObject();
  ~CaptureObject();

  CaptureObject(const CaptureObject &) = delete;
  CaptureObject &operator=(const CaptureObject &) = delete;
};

#endif // CAPTUREOBJECT_H
