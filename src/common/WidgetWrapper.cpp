#include "WidgetWrapper.h"

WidgetWrapper::WidgetWrapper()
{

}

QComboBoxWithSignal::QComboBoxWithSignal(QWidget* parent)
    : QComboBox(parent) {}

void QComboBoxWithSignal::showPopup() {
    emit popupShown();
    QComboBox::showPopup();
}

void QComboBoxWithSignal::hidePopup() {
    emit popupHidden();
    QComboBox::hidePopup();
}

QSpinBoxWithSignal::QSpinBoxWithSignal(QWidget* parent)
    : QSpinBox(parent) {}

void QSpinBoxWithSignal::focusInEvent(QFocusEvent* event) {
    emit focusIn();
    QSpinBox::focusInEvent(event);
}

void QSpinBoxWithSignal::focusOutEvent(QFocusEvent* event) {
    emit focusOut();
    QSpinBox::focusOutEvent(event);
}


#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

#define TRANSPARENT_WINDOW_FLAGS (Qt::Window | Qt::BypassWindowManagerHint | Qt::FramelessWindowHint | \
        Qt::WindowStaysOnTopHint | Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus)

#define TRANSPARENT_WINDOW_ATTRIBUTES() {\
}

#else

#define TRANSPARENT_WINDOW_FLAGS (Qt::Window | Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint | \
        Qt::WindowStaysOnTopHint)

// Replacement for Qt::WindowTransparentForInput based on X11 'shape' extension as described here:
// http://shallowsky.com/blog/programming/translucent-window-click-thru.html
#define TRANSPARENT_WINDOW_ATTRIBUTES() {\
    setAttribute(Qt::WA_X11DoNotAcceptFocus); \
    int shape_event_base, shape_error_base; \
    if(IsPlatformX11()) { \
        if(XShapeQueryExtension(QX11Info::display(), &shape_event_base, &shape_error_base)) { \
            Region region = XCreateRegion(); \
            XShapeCombineRegion(QX11Info::display(), winId(), ShapeInput, 0, 0, region, ShapeSet); \
            XDestroyRegion(region); \
        } \
    } \
}

#endif

ScreenLabelWindow::ScreenLabelWindow(QWidget* parent, const QString &text)
    : QWidget(parent, TRANSPARENT_WINDOW_FLAGS) {
    TRANSPARENT_WINDOW_ATTRIBUTES();
    m_text = text;
    m_font = QFont("Sans", 18, QFont::Bold);
    QFontMetrics fm(m_font);
    setFixedSize(fm.size(Qt::TextSingleLine, m_text) + QSize(60, 40));
    setWindowOpacity(0.75);
}

void ScreenLabelWindow::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setPen(QColor(0, 0, 0));
    painter.setBrush(QColor(0, 255, 0));
    painter.drawRect(QRectF(0.5, 0.5, (qreal) width() - 1.0, (qreal) height() - 1.0));
    painter.setFont(m_font);
    painter.drawText(0, 0, width(), height(), Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextSingleLine, m_text);
}

RecordingFrameWindow::RecordingFrameWindow(QWidget* parent)
    : QWidget(parent, TRANSPARENT_WINDOW_FLAGS) {
    TRANSPARENT_WINDOW_ATTRIBUTES();
    QImage image(16, 16, QImage::Format_RGB32);
    for(size_t j = 0; j < (size_t) image.height(); ++j) {
        uint32_t *row = (uint32_t*) image.scanLine(j);
        for(size_t i = 0; i < (size_t) image.width(); ++i) {
            //row[i] = ((i + j) % 16 < 8)? 0xffbfbfff : 0xff9f9fdf;
            row[i] = ((i + j) % 16 < 8)? 0xffff8080 : 0xff8080ff;
        }
    }
    m_texture = QPixmap::fromImage(image);
    UpdateMask();
}

void RecordingFrameWindow::UpdateMask() {
    if(QX11Info::isCompositingManagerRunning()) {
        clearMask();
        setWindowOpacity(0.25);
    } else {
        setMask(QRegion(0, 0, width(), height()).subtracted(QRegion(3, 3, width() - 6, height() - 6)));
        setWindowOpacity(1.0);
    }
}

void RecordingFrameWindow::resizeEvent(QResizeEvent *event) {
    Q_UNUSED(event);
    UpdateMask();
}

void RecordingFrameWindow::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    m_texture.setDevicePixelRatio(devicePixelRatioF());
#endif
    painter.setPen(QColor(0, 0, 0, 128));
    painter.setBrush(Qt::NoBrush);
    painter.drawTiledPixmap(0, 0, width(), height(), m_texture);
    painter.drawRect(QRectF(0.5, 0.5, (qreal) width() - 1.0, (qreal) height() - 1.0));
}








