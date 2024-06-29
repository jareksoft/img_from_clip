#ifndef RENDERCONFIGURATION_H
#define RENDERCONFIGURATION_H

#include <QProperty>
#include <QQmlEngine>
#include <QQuickItem>

class RenderConfiguration : public QObject {
  Q_OBJECT
  QML_ELEMENT
  Q_PROPERTY(bool svg READ default WRITE default BINDABLE svgBindable)
  Q_PROPERTY(bool png READ default WRITE default BINDABLE pngBindable)
  Q_PROPERTY(bool jpg READ default WRITE default BINDABLE jpgBindable)
  Q_PROPERTY(bool scale15 READ default WRITE default BINDABLE scale15Bindable)
  Q_PROPERTY(bool scale2 READ default WRITE default BINDABLE scale2Bindable)
  Q_PROPERTY(bool scale3 READ default WRITE default BINDABLE scale3Bindable)
  Q_PROPERTY(bool scale4 READ default WRITE default BINDABLE scale4Bindable)
public:
  explicit RenderConfiguration(QObject *parent = nullptr);
  ~RenderConfiguration() override;

  QBindable<bool> svgBindable() { return {&m_svg}; }
  QBindable<bool> pngBindable() { return {&m_png}; }
  QBindable<bool> jpgBindable() { return {&m_jpg}; }
  QBindable<bool> scale15Bindable() { return {&m_scale15}; }
  QBindable<bool> scale2Bindable() { return {&m_scale2}; }
  QBindable<bool> scale3Bindable() { return {&m_scale3}; }
  QBindable<bool> scale4Bindable() { return {&m_scale4}; }

private:
  QProperty<bool> m_svg{true};
  QProperty<bool> m_png{false};
  QProperty<bool> m_jpg{false};
  QProperty<bool> m_scale15{false};
  QProperty<bool> m_scale2{false};
  QProperty<bool> m_scale3{false};
  QProperty<bool> m_scale4{false};
};

#endif // RENDERCONFIGURATION_H
