// This file is licensed under either of licenses:
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-License-Identifier: BSD-2-Clause

#ifndef RSVG_HELPER_H
#define RSVG_HELPER_H

#include "config.h"
#include <QImage>

#ifdef USE_LIBRSVG_RENDERER
struct RsvgRender {
  static QImage renderSvg(const QString &svg, double scale);
};
#endif

#endif // RSVG_HELPER_H
