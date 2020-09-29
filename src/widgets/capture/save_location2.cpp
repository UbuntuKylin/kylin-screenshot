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
#define TRIANGLE_WIDTH 10    //小三角的宽度
#define TRIANGLE_HEIGHT 5    //小三角的高度
#define BORDER_RADIUS 10      //窗口边角弧度

Save_Location2::Save_Location2(QWidget *parent)
    :  QWidget(parent)
    , m_startx(13)
    , radius(5)
    , m_triangleWidth(TRIANGLE_WIDTH)
    , m_triangleHeight(TRIANGLE_HEIGHT)
{
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
    for (int i =0;i<3;i++)
    {
       m_TypeList.append(QRect(m_startx,107,radius*2, radius*2));
       m_startx += 40;
    }
}
void Save_Location2::setStartPos(double startX)
{
      m_startx = startX;
}
void Save_Location2::setCenterWidget(QWidget *widget)
{
    savedir = new QLabel("存储位置",this);
    savedir->move(13,21);
    SaveDir = new  QPushButton(this);
    SaveDir->resize(118,25);
    SaveDir->move(13,50);
    SaveDir->setStyleSheet("QPushButton{font-family:'宋体';font-size:100px;color:rgb(0,0,0,255);");
    savetype = new QLabel("存储格式",this);
    QStringList a = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    SaveDir->setText(a.at(0));
    savetype->move(13,82);
    label1 = new  QLabel("jpg",this);
    label1->move(24,100);
    label2 = new  QLabel("png",this);
    label2->move(64,100);
    label3 = new  QLabel("bmp",this);
    label3->move(104,100);
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
    painter.setBrush(QColor(225,225,225,180));
    //
    //
    QPolygon trianglePolygon;
    trianglePolygon << QPoint(m_startx,height()- m_triangleWidth - SHADOW_WIDTH);
    trianglePolygon << QPoint(m_startx+m_triangleWidth/2,height()-SHADOW_WIDTH);
    trianglePolygon << QPoint(m_startx + m_triangleWidth,height()-m_triangleHeight-SHADOW_WIDTH);
    QPainterPath drawPath;
    drawPath.addRoundRect(QRect(0 , 0 ,width()-SHADOW_WIDTH *2,height()- m_triangleWidth - SHADOW_WIDTH),
                                        BORDER_RADIUS,BORDER_RADIUS);
    drawPath.addPolygon(trianglePolygon);
    painter.drawPath(drawPath);
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
