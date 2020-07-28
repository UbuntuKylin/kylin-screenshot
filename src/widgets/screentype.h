#ifndef SCREENTYPE_H
#define SCREENTYPE_H
#include <QWidget>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QButtonGroup>
class ScreenType : public QWidget
{
    Q_OBJECT
public:
    //ScreenType();
    explicit ScreenType(QWidget *parent = nullptr);
    QButtonGroup *button_type;
    QVBoxLayout *vbox;
    QRadioButton *MP4_TYPE,*GIF_TYPE,*MKV_TYPE,*WEBM_TYPE,*OGG_TYPE;
    QString type;
public slots:
     void onRadioClickType();
signals:
     void Type_change(QString s);
};

#endif // SCREENTYPE_H
