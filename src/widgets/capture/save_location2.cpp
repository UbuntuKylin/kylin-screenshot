/*
 *
 * Copyright: 2020 KylinSoft Co., Ltd.
 * Authors:
 *   huanhuan zhang <zhanghuanhuan@kylinos.cn>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "save_location2.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#define SHADOW_WIDTH  5     //小三角的阴影宽度
#define TRIANGLE_WIDTH 20    //小三角的宽度
#define TRIANGLE_HEIGHT 10    //小三角的高度
#define BORDER_RADIUS 10      //窗口边角弧度

Save_Location2::Save_Location2(QWidget *parent)
    :  QWidget(parent)
    , m_startx(15)
    , radius(5)
    , m_triangleWidth(TRIANGLE_WIDTH)
    , m_triangleHeight(TRIANGLE_HEIGHT)
{
    context.style_settings = new QGSettings("org.ukui.style");
    context.style_name = context.style_settings->get("style-name").toString();
    this->setMouseTracking(true);
    setCursor(Qt::ArrowCursor);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    //设置阴影边框
    auto shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(0,0);
    shadowEffect->setColor(Qt::gray);
    shadowEffect->setBlurRadius(BORDER_RADIUS);
    this->setGraphicsEffect(shadowEffect);
    m_TypeList.append(QRect(m_startx,99,radius*2, radius*2));
    m_TypeList.append(QRect(m_startx+39,99,radius*2, radius*2));
    m_TypeList.append(QRect(m_startx+80,99,radius*2, radius*2));
}
void Save_Location2::setStartPos(double startX)
{
      m_startx = startX;
}
void Save_Location2::setCenterWidget(QWidget *widget)
{
    QFont ft ("SourceHanSansCN", 9);
    savedir = new QLabel(tr("save location"),this);
    savedir->setFont(ft);
    savedir->setGeometry(14,18,48,12);
    //savedir->move(13,21);
    SaveDir = new  QPushButton(this);
    SaveDir->setGeometry(14,36,118,20);
    //SaveDir->move(13,50);
    ft.setPointSize(9);
    SaveDir->setFont(ft);
    ft.setPointSize(9);
    savetype = new QLabel(tr("save type"),this);
    savetype->setGeometry(14,74,48,12);
    //savetype->move(13,82);
    savetype->setFont(ft);
    ft.setPointSize(9);
    label1 = new  QLabel("jpg",this);
    label1->setFont(ft);
    label1->setGeometry(27,96,17,18);
    //label1->move(24,100);
    label2 = new  QLabel("png",this);
    label2->setFont(ft);
    label2->setGeometry(66,96,23,18);
    //label2->move(64,100);
    label3 = new  QLabel("bmp",this);
    label3->setFont(ft);
    label3->setGeometry(108,96,26,15);
    setContentsMargins(SHADOW_WIDTH,SHADOW_WIDTH, SHADOW_WIDTH, SHADOW_WIDTH);
}
void Save_Location2::setTriangleInfo(double width, double height)
{
    m_triangleWidth = width;
    m_triangleHeight = height;
}

void Save_Location2::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setPen(Qt::NoPen);
    QPolygon trianglePolygon;
    trianglePolygon << QPoint(m_startx,height()- m_triangleHeight);
    trianglePolygon << QPoint(m_startx+m_triangleWidth/2,height());
    trianglePolygon << QPoint(m_startx + m_triangleWidth,height()-m_triangleHeight);
    QPainterPath drawPath;
    drawPath.addRoundRect(QRect(SHADOW_WIDTH, SHADOW_WIDTH,width()-SHADOW_WIDTH,height()- m_triangleHeight-SHADOW_WIDTH),
                                        BORDER_RADIUS,BORDER_RADIUS);
    drawPath.addPolygon(trianglePolygon);
    if((context.style_name.compare("ukui-white")==0) || (context.style_name.compare("ukui-default")==0)|| (context.style_name.compare("ukui-light")==0)){
        painter.setBrush(QColor(225,225,225,180));
        painter.drawPath(drawPath);
    }
    else if((context.style_name.compare("ukui-dark")==0) || (context.style_name.compare("ukui-black")==0)){
        painter.setBrush(QColor(25,25,25,180));
        painter.drawPath(drawPath);
    }
    for (int i=0;i<3;i++)
    {
        QRect rect = m_TypeList.at(i);
        painter.setBrush(QColor(25,25,25));
        painter.drawEllipse(rect);
        Rect_h = rect.height()-2;
        Rect_w = rect.width()-2;
        painter.setBrush(QColor(255,255,255));
        painter.drawEllipse(QRect(rect.x()+1,rect.y()+1,Rect_h,Rect_w));
        if (type_rect == rect){
            Rect_h = rect.height()-1;
            Rect_w = rect.width()-1;
            painter.setBrush(QColor(Qt::blue));
            painter.drawEllipse(QRect(rect.x()+1,rect.y()+1,Rect_h,Rect_w));
        }
   }
}
void Save_Location2::mousePressEvent(QMouseEvent *e) {
    for (int i = 0; i < 3; i++) {
        if (m_TypeList.at(i).contains(e->pos())) {
           type_rect = m_TypeList.at(i);
            emit save_type_clicked(i);
            update();
            break;
         }
    }
}
