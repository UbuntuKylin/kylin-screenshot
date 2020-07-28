#ifndef WIDGETWRAPPER_H
#define WIDGETWRAPPER_H


#include "src/core/Global.h"


class QComboBoxWithSignal : public QComboBox {
    Q_OBJECT

public:
    QComboBoxWithSignal(QWidget* parent);

    virtual void showPopup() override;
    virtual void hidePopup() override;

signals:
    void popupShown();
    void popupHidden();

};
class QSpinBoxWithSignal : public QSpinBox {
    Q_OBJECT
public:
    QSpinBoxWithSignal(QWidget* parent);
    virtual void focusInEvent(QFocusEvent* event) override;
    virtual void focusOutEvent(QFocusEvent* event) override;
signals:
    void focusIn();
    void focusOut();

};
class ScreenLabelWindow : public QWidget {
    Q_OBJECT
private:
    QString m_text;
    QFont m_font;

public:
    ScreenLabelWindow(QWidget* parent, const QString& text);

protected:
    virtual void paintEvent(QPaintEvent* event) override;

};

class RecordingFrameWindow : public QWidget {
    Q_OBJECT

private:
    QPixmap m_texture;

public:
    RecordingFrameWindow(QWidget* parent);

private:
    void UpdateMask();

protected:
    virtual void resizeEvent(QResizeEvent* event) override;
    virtual void paintEvent(QPaintEvent* event) override;

};



class WidgetWrapper
{
public:
    WidgetWrapper();
};

#endif // WIDGETWRAPPER_H
