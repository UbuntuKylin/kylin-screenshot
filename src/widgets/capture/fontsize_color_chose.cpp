/*
 *
 * Copyright:   2020 KylinSoft Co., Ltd.
 * Authors:
 *   huanhuan zhang <zhanghuanhuan@cs2c.com.cn.com>
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 */
#include "fontsize_color_chose.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#define SHADOW_WIDTH  5     //小三角的阴影宽度
#define TRIANGLE_WIDTH 10    //小三角的宽度
#define TRIANGLE_HEIGHT 5  //小三角的高度
#define BORDER_RADIUS 10       //窗口边角弧度

FontSize_Color_Chose::FontSize_Color_Chose(QWidget *parent)
    : QWidget(parent)
    , m_startx(5)
    , m_triangleWidth(TRIANGLE_WIDTH)
    , m_triangleHeight(TRIANGLE_HEIGHT)
    , radius(1.5)
{
    setCursor(Qt::ArrowCursor);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    //设置阴影边框
    auto shadowEffect = new QGraphicsDropShadowEffect(this);
    shadowEffect->setOffset(0,0);
    shadowEffect->setColor(Qt::gray);
    shadowEffect->setBlurRadius(BORDER_RADIUS);
    this->setGraphicsEffect(shadowEffect);
    this->setMouseTracking(true);
    Start_x = 13;
    Start_y = 47;
    for (int i =0;i<4;i++)
    {
       m_colorAreaList.append(QRect(Start_x,Start_y,radius*2, radius*2));
       Start_x += radius*2;
       Start_x+= 10;
       Start_y -= 2;
       radius +=2;
    }
     Start_x =94;
     Start_y = 31.5;
    for (int i=4;i<8;i++)
    {
        m_colorAreaList.append(QRect(Start_x,Start_y,14,14));
         Start_x += 24;
    }
    Start_x  = 94;
    Start_y = 51;
    for (int i=8;i<12;i++)
    {
         m_colorAreaList.append(QRect(Start_x,Start_y,14,14));
         Start_x += 24;
    }
}
void FontSize_Color_Chose::setStartPos(double startX)
{
    m_startx = startX;
}
void FontSize_Color_Chose::setTriangleInfo(double width, double height)
{
    m_triangleWidth = width;
    m_triangleHeight = height;
}
void FontSize_Color_Chose::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(225,225,225,180));
    //
    QPolygon trianglePolygon;
    trianglePolygon << QPoint(m_startx, m_triangleWidth + SHADOW_WIDTH);
    trianglePolygon << QPoint(m_startx+m_triangleWidth/2,SHADOW_WIDTH);
    trianglePolygon << QPoint(m_startx + m_triangleWidth,m_triangleHeight + SHADOW_WIDTH);
    QPainterPath drawPath;
    drawPath.addRoundRect(QRect(SHADOW_WIDTH , m_triangleHeight + SHADOW_WIDTH ,
                                width()-SHADOW_WIDTH *2 ,height() -SHADOW_WIDTH *2 -m_triangleHeight),
                         BORDER_RADIUS,BORDER_RADIUS);
    drawPath.addPolygon(trianglePolygon);
    painter.drawPath(drawPath);
    for (int i=0;i<4;i++)
    {
        QRect rect = m_colorAreaList.at(i);
        if (color_rect == rect){
        Rect_h = rect.height()+4;
        Rect_w = rect.width() +4;
        painter.setBrush(QColor(255,255,255));
        painter.drawEllipse(QRect(rect.x()-2,rect.y()-2,Rect_h,Rect_w));
        }
        painter.setBrush(QColor(25,25,25));
        painter.drawEllipse(rect);
    }
    painter.setBrush(QColor(25,25,25));
    painter.drawRect(85,36,1,17);
    for (int i =4;i<12;i++)
    {
        QRect rect = m_colorAreaList.at(i);
        QColor color1 =  m_colorList.at(i-4);
        if (color == color1){
             Rect_h= rect.height()+4;
             Rect_w = rect.width() +4;
             painter.setBrush(QColor(255,255,255));
             painter.drawRect(QRect(rect.x()-2,rect.y()-2,Rect_h,Rect_w));
        }
        painter.setBrush(color1);
        painter.drawRect(m_colorAreaList.at(i));
    }
}
void FontSize_Color_Chose::mousePressEvent(QMouseEvent *e) {
    for (int i = 0; i < 4; i++) {
        if (m_colorAreaList.at(i).contains(e->pos())) {
           color_rect = m_colorAreaList.at(i);
            emit font_size_change(i*8) ;
            update();
            break;
         }
    }
    for (int i = 4; i < 12; i++) {
        if (m_colorAreaList.at(i).contains(e->pos())) {
           color = m_colorList.at(i-4);
           emit colorSelected(color);
           update();
           break;
         }
    }
}
