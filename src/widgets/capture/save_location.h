#ifndef SAVE_LOCATION_H
#define SAVE_LOCATION_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>

class Save_Location : public QWidget
{
    Q_OBJECT
public:
    explicit Save_Location(QWidget *parent = nullptr);
    //设置小三角的起始位置
    void setStartPos(int startX);
    //设置小三角的宽和高
    void setTriangleInfo(int width,int height );
   //设置中间区域
    void setCenterWidget(QWidget* widget = nullptr);
     QPushButton *SaveDir;
     QString file_path ;
     QRadioButton *jpg , *png,*bmp;
signals:
     void  save_type_clicked();
public slots:
     void  save_type_click();

protected:
    void paintEvent(QPaintEvent *event) ;
private:
    QVBoxLayout *vLayout;
    QHBoxLayout *hMainLayout;
    QStringList *file;
    QLabel *savedir , * savetype;
    //小三角的起始位置
    int m_startx;
    int m_startY;
    //小三角的宽
    int m_triangleWidth;
    //小三角的高
    int m_triangleHeight;

};

#endif // SAVE_LOCATION_H
