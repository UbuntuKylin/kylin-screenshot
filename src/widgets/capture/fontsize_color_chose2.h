#ifndef FONTSIZE_COLOR_CHOSE2_H
#define FONTSIZE_COLOR_CHOSE2_H

#include <QWidget>
#include <QFrame>
class FontSize_Color_Chose2 : public QWidget
{
    Q_OBJECT
public:
        explicit FontSize_Color_Chose2(QWidget *parent = nullptr);
        QVector<QRect> m_colorAreaList;
        QVector<QColor> m_colorList =
        {
            QColor(219,2,15),
            QColor(36,87,217),
            QColor(85,209,57),
            QColor(211,154,44),
            QColor(251,66,136),
            QColor(25,25,25),
            QColor(98,102,113),
            QColor(255,255,255)
        };
        //设置小三角的起始位置
        void setStartPos(int startX);
        //设置小三角的宽和高
        void setTriangleInfo(int width,int height );
       //设置中间区域
        void setCenterWidget(QWidget* widget = nullptr);
        //void mouseMoveEvent(QMouseEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
protected:
        void paintEvent(QPaintEvent *event) ;
signals:
        void  colorSelected2(QColor color);
        void  font_size_change2(int thickness);
private:
        //小三角的起始位置
        int m_startx;
        //小三角的宽
        int m_triangleWidth;
        //小三角的高
        int m_triangleHeight;
        int  extrasize = 6;
        double Rect_h,Rect_w;
        int radius;
        double Rect_x,Rect_y;
        double Start_x, Start_y;
        QFrame *line1;
public:
        QColor color;
        QRect color_rect;
};

#endif // FONTSIZE_COLOR_CHOSE2_H
