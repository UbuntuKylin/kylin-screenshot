/*
 *
 * Copyright : 2020 KylinSoft Co., Ltd.
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
#include "save_location2.h"
#include <QHBoxLayout>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QDebug>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#define SHADOW_WIDTH  15     //小三角的阴影宽度
#define TRIANGLE_WIDTH 15    //小三角的宽度
#define TRIANGLE_HEIGHT 15    //小三角的高度
#define BORDER_RADIUS 10       //窗口边角弧度

Save_Location2::Save_Location2(QWidget *parent)
    :  QWidget(parent)
    , m_startx(5)
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
    //setFixedSize(100,100);
}
void Save_Location2::setStartPos(int startX)
{
      m_startx = startX;
}
void Save_Location2::setCenterWidget(QWidget *widget)
{
    vLayout = new QVBoxLayout(this);
    hMainLayout = new  QHBoxLayout();
    //hMainLayout->addWidget(widget);
    savedir = new QLabel("存储位置");
    SaveDir = new  QPushButton();
    QStringList a = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    SaveDir->setText(a.at(0));
    savetype = new QLabel("存储格式");

    jpg = new QRadioButton("jpg");
    png = new QRadioButton("png");
    png->setChecked(true);
    bmp = new QRadioButton("bmp");

    hMainLayout->addWidget(jpg);
    hMainLayout->addWidget(png);
    hMainLayout->addWidget(bmp);
    vLayout->addWidget(savedir);
    vLayout->addWidget(SaveDir);
    vLayout->addWidget(savetype);
    vLayout->addLayout(hMainLayout);
    vLayout->setSpacing(0);
    vLayout->setContentsMargins(0,0,SHADOW_WIDTH+m_triangleWidth+5,m_triangleHeight+SHADOW_WIDTH);
    connect(jpg,&QRadioButton::clicked,
            this,&Save_Location2::save_type_click);
    connect(png,&QRadioButton::clicked,
            this,&Save_Location2::save_type_click);
    connect(bmp,&QRadioButton::clicked,
            this,&Save_Location2::save_type_click);

}
void Save_Location2::setTriangleInfo(int width, int height)
{
    m_triangleWidth = width;
    m_triangleHeight = height;
}

void Save_Location2::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(225,225,225));
    //
    QPolygon trianglePolygon;
    trianglePolygon << QPoint(m_startx, height()- m_triangleWidth - SHADOW_WIDTH);
    trianglePolygon << QPoint(m_startx+m_triangleWidth/2,height()-SHADOW_WIDTH);
    trianglePolygon << QPoint(m_startx + m_triangleWidth,height()-m_triangleHeight - SHADOW_WIDTH);

    QPainterPath drawPath;
    drawPath.addRoundRect(QRect(0 , 0 ,width()-SHADOW_WIDTH *2,height()- m_triangleWidth - SHADOW_WIDTH),
                          BORDER_RADIUS,BORDER_RADIUS);

    drawPath.addPolygon(trianglePolygon);
    painter.drawPath(drawPath);
}
void Save_Location2::save_type_click()
{
    emit save_type_clicked();
}
