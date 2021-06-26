#ifndef INFOWIDGET_H
#define INFOWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QFont>
#include <QPaintEvent>
#include "src/tools/capturecontext.h"

class infoWidget : public QWidget
{
    Q_OBJECT

public:
    infoWidget(QWidget *parent = nullptr);
    ~infoWidget();

protected:
    void paintEvent(QPaintEvent *event);
private:
    CaptureContext context;
    QPushButton *m_exitButton;
    QFont font;
    // QVBoxLayout *m_layout;
    QLabel *m_appIcon;
    QLabel *m_appName;
    QLabel *m_appVersion;
    QLabel *m_Descript;
    QLabel *m_EmailInfo;
};
#endif // WIDGET_H
