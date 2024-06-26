#include "clipboardcontents.h"
#include <QCryptographicHash>

class ClipboardContentsData : public QSharedData {
public:
  ClipboardContents::Type m_type = ClipboardContents::Type::Nothing;
  QByteArray m_hash;
  QString m_string;
  QImage m_image;
};

ClipboardContents::ClipboardContents() : data(new ClipboardContentsData) {}

ClipboardContents::ClipboardContents(const ClipboardContents &rhs)
    : data{rhs.data} {}

ClipboardContents &ClipboardContents::operator=(const ClipboardContents &rhs) {
  if (this != &rhs)
    data.operator=(rhs.data);
  return *this;
}

ClipboardContents::~ClipboardContents() {}

QByteArray ClipboardContents::hash() const noexcept { return data->m_hash; }

ClipboardContents::Type ClipboardContents::type() const noexcept {
  return data->m_type;
}

QString ClipboardContents::text() const noexcept {
  return data->m_type == Type::Text ? data->m_string : QString();
}

void ClipboardContents::setText(const QString &text) {
  QCryptographicHash hash(QCryptographicHash::Sha512);
  hash.addData(text.toUtf8());

  data->m_string = text;
  data->m_image = QImage();
  data->m_type = Type::Text;
  data->m_hash = hash.result();
}

QString ClipboardContents::html() const noexcept {
  return data->m_type == Type::Html ? data->m_string : QString();
}

void ClipboardContents::setHtml(const QString &html) {
  QCryptographicHash hash(QCryptographicHash::Sha512);
  hash.addData(html.toUtf8());

  data->m_string = html;
  data->m_image = QImage();
  data->m_type = Type::Html;
  data->m_hash = hash.result();
}

QImage ClipboardContents::image() const noexcept {
  return data->m_type == Type::Image ? data->m_image : QImage();
}

void ClipboardContents::setImage(const QImage &image) {
  QCryptographicHash hash(QCryptographicHash::Sha512);
  QByteArrayView view(image.bits(), image.sizeInBytes());

  while (!view.isEmpty()) {
    auto chunk = view.left(65536).toByteArray();
    hash.addData(chunk);
    view = view.sliced(chunk.size());
  }

  data->m_image = image;
  data->m_string = QString();
  data->m_type = Type::Image;
  data->m_hash = hash.result();
}
