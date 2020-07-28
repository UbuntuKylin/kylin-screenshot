#include "font_options2.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#define SHADOW_WIDTH  15     //小三角的阴影宽度
#define TRIANGLE_WIDTH 15    //小三角的宽度
#define TRIANGLE_HEIGHT 15    //小三角的高度
#define BORDER_RADIUS 10       //窗口边角弧度
Font_Options2::Font_Options2(QWidget *parent)
    : QWidget(parent)
    , m_startx(5)
    , m_triangleWidth(TRIANGLE_WIDTH)
    , m_triangleHeight(TRIANGLE_HEIGHT)
    , radius(6)
    ,Delete(false)
    ,Underline(false)
    ,bold(false)
    ,italic(false)
{
    setCursor(Qt::ArrowCursor);
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    //设置阴影边框
    auto shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(0,0);
    shadowEffect->setColor(Qt::gray);
    shadowEffect->setBlurRadius(BORDER_RADIUS);
    this->setGraphicsEffect(shadowEffect);
    this->setMouseTracking(true);
    Start_x  = 220;
    Start_y = 30;
     for (int i=0;i<4;i++)
     {
          m_colorAreaList.append(QRect(Start_x,Start_y,20,20));
          Start_x += 25;
     }
     Start_x  = 220;
     Start_y = 60;
     for (int i=4;i<8;i++)
     {
          m_colorAreaList.append(QRect(Start_x,Start_y,20,20));
          Start_x += 25;
     }

}
void Font_Options2::setStartPos(int startX)
{
    m_startx = startX;
}
void Font_Options2::setTriangleInfo(int width, int height)
{
    m_triangleWidth = width;
    m_triangleHeight = height;
}
void Font_Options2::setCenterWidget(QWidget *widget)
{
    Start_x  = 5;
    Start_y = 30;
    Font_type = new  QFontComboBox(this);
    Font_type->setCurrentFont(QFont("华文黑体"));
    Font_type->move(Start_x,Start_y);
    Font_size = new  QSpinBox(this);
    Font_type->setFixedSize(150,25);
    Font_size->setFixedSize(40,25);
    Font_size->setValue(2);
    Start_x  = 165;
    Start_y = 30;
    Font_size->move(Start_x,Start_y);
    delete_btn = new QPushButton(this);
    delete_btn->setIcon(QIcon(QStringLiteral(":/img/material/white/") + "delete.png"));
    delete_btn->move(5,60);
    Underline_btn= new QPushButton(this);
    Underline_btn->setIcon(QIcon(QStringLiteral(":/img/material/white/") + "underline.png"));
    Underline_btn->move(55,60);
    bold_btn = new QPushButton(this);
    bold_btn->setIcon(QIcon(QStringLiteral(":/img/material/white/") + "bold.png"));
    bold_btn->move(105,60);
    Italic_btn = new QPushButton(this);
    Italic_btn->setIcon(QIcon(QStringLiteral(":/img/material/white/") + "italic.png"));
    Italic_btn->move(155,60);
    delete_btn->setToolTip(tr("StrikeOut"));
    Underline_btn->setToolTip(tr("Underline"));
    bold_btn->setToolTip(tr("Bold"));
    Italic_btn->setToolTip(tr("Italic"));
    delete_btn->setFixedSize(50,20);
    Underline_btn->setFixedSize(50,20);
    bold_btn->setFixedSize(50,20);
    Italic_btn->setFixedSize(50,20);
    connect(Font_size,SIGNAL(valueChanged(int)),
            this,SLOT(font_size_change(int)));
    connect(Font_type,SIGNAL(currentFontChanged(QFont)),
            this,SLOT(font_type_change(QFont)));
    connect(delete_btn,&QPushButton::clicked,
            this,&Font_Options2::font_delete_selete);
    connect(Underline_btn,&QPushButton::clicked,
            this,&Font_Options2::font_underline_selete);
    connect(Italic_btn,&QPushButton::clicked,
            this,&Font_Options2::font_italic_selete);
    connect(bold_btn,&QPushButton::clicked,
            this,&Font_Options2::font_bold_selete);

}
void Font_Options2::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(225,225,225));
    //
    QPolygon trianglePolygon;
    trianglePolygon << QPoint(m_startx,height()- m_triangleWidth - SHADOW_WIDTH);
    trianglePolygon << QPoint(m_startx+m_triangleWidth/2,height()-SHADOW_WIDTH);
    trianglePolygon << QPoint(m_startx + m_triangleWidth,height()-m_triangleHeight - SHADOW_WIDTH);
    QPainterPath drawPath;
    /*drawPath.addRoundRect(QRect(SHADOW_WIDTH , m_triangleHeight + SHADOW_WIDTH ,
                                width()-SHADOW_WIDTH *2 ,height() -SHADOW_WIDTH *2 -m_triangleHeight),
                         BORDER_RADIUS,BORDER_RADIUS);*/
    drawPath.addRoundRect(QRect(0 , 0 ,width()-SHADOW_WIDTH *2,height()- m_triangleWidth - SHADOW_WIDTH),
                                           BORDER_RADIUS,BORDER_RADIUS);
    drawPath.addPolygon(trianglePolygon);
    painter.drawPath(drawPath);
    painter.setBrush(QColor(25,25,25));
    painter.drawRect(213,35,2,40);
    for (int i =0;i<8;i++)
    {
        QRect rect = m_colorAreaList.at(i);
        QColor color1 =  m_colorList.at(i);
        if (color == color1){
             Rect_h= rect.height()+4;
             Rect_w = rect.width() +4;
             painter.setBrush(QColor(255,255,255));
             painter.drawRect(QRect(rect.x()-2,rect.y()-2,Rect_h,Rect_w));
        }
        painter.setBrush(color1);
        painter.drawRect(m_colorAreaList.at(i));
    }
    if (bold)
         bold_btn->setIcon(QIcon(QStringLiteral(":/img/material/black/") + "bold_active.png"));
    else {
        bold_btn->setIcon(QIcon(QStringLiteral(":/img/material/white/") + "bold.png"));
    }
    if (Delete)
    {
         delete_btn->setIcon(QIcon(QStringLiteral(":/img/material/black/") + "delete_active.png"));
    }
    else {
         delete_btn->setIcon(QIcon(QStringLiteral(":/img/material/white/") + "delete.png"));
    }
    if (italic)
         Italic_btn->setIcon(QIcon(QStringLiteral(":/img/material/black/") + "italic_active.png"));
    else {
        Italic_btn->setIcon(QIcon(QStringLiteral(":/img/material/white/") + "italic.png"));
    }
    if (Underline)
    {
         Underline_btn->setIcon(QIcon(QStringLiteral(":/img/material/black/") + "underline_active.png"));
     }
    else {
         Underline_btn->setIcon(QIcon(QStringLiteral(":/img/material/white/") + "underline.png"));
    }
}
void Font_Options2::mousePressEvent(QMouseEvent *e)
{
    for (int i = 0; i < 8; i++) {
        if (m_colorAreaList.at(i).contains(e->pos())) {
            color = m_colorList.at(i);
            emit colorSelected(color);
            update();
            break;
         }
    }
}
void  Font_Options2::font_size_change(int i)
{
    qDebug()<<"text font thickness changed";
    emit font_size_Selete(i);
}
void  Font_Options2::font_type_change(QFont font)
{
    qDebug()<<"text font family changed" <<font ;
    emit font_type_Selete(font);
}
void Font_Options2::font_bold_selete()
{
    bold = !bold;
    if (bold)
         bold_btn->setIcon(QIcon(QStringLiteral(":/img/material/black/") + "bold_active.png"));
    else {
        bold_btn->setIcon(QIcon(QStringLiteral(":/img/material/white/") + "bold.png"));
    }
    emit font_bold_change(bold);
}
void Font_Options2::font_delete_selete()
{
    Delete = !Delete;
    if (Delete)
    {
         delete_btn->setIcon(QIcon(QStringLiteral(":/img/material/black/") + "delete_active.png"));
         qDebug()<<"222333111";
    }
    else {
         delete_btn->setIcon(QIcon(QStringLiteral(":/img/material/white/") + "delete.png"));
         qDebug()<<"111222333";
    }
    emit font_delete_change(Delete);
}
void Font_Options2::font_underline_selete()
{
    Underline = !Underline;
    if (Underline)
    {
         Underline_btn->setIcon(QIcon(QStringLiteral(":/img/material/black/") + "underline_active.png"));
   }
    else {
         Underline_btn->setIcon(QIcon(QStringLiteral(":/img/material/white/") + "underline.png"));
    }
    emit font_underline_change(Underline);
}
void Font_Options2::font_italic_selete()
{
    italic = !italic;
    if (italic)
         Italic_btn->setIcon(QIcon(QStringLiteral(":/img/material/black/") + "italic_active.png"));
    else {
        Italic_btn->setIcon(QIcon(QStringLiteral(":/img/material/white/") + "italic.png"));
    }
    emit font_italic_change(italic);
}
