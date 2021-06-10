#ifndef SHORTCUTWIDGET_H
#define SHORTCUTWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <QPaintEvent>
#include <QPushButton>
#include <QFont>
#include "src/tools/capturecontext.h"
class ShortCutWidget : public QWidget
{
    Q_OBJECT

public:
    ShortCutWidget(QWidget *parent = nullptr);
    ~ShortCutWidget();

protected:
    void paintEvent(QPaintEvent *e);

private:
// void initInfoTable();
    void initLabel();
    QList<QLabel *> labels;
    static QVector<const char *> m_keys;
    static QVector<const char *> m_description;
    QLabel *appIcon;
    QLabel *appName;
    QPushButton *minLabel;
    QPushButton *exitLabel;
    QLabel *tableName;
    QIcon *windowIcon;
    QLabel *windowTitle;
    QFont font;
    CaptureContext context;
};
#endif // WIDGET_H
