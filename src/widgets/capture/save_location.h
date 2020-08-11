/*
 *
 * Copyright: 2020 KylinSoft Co., Ltd.
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
#ifndef SAVE_LOCATION_H
#define SAVE_LOCATION_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QMouseEvent>
class Save_Location : public QWidget
{
    Q_OBJECT
public:
    explicit Save_Location(QWidget *parent = nullptr);
    //设置小三角的起始位置
    void setStartPos(double startX);
    //设置小三角的宽和高
    void setTriangleInfo(double width,double height );
   //设置中间区域
    void setCenterWidget(QWidget* widget = nullptr);
     QPushButton *SaveDir;
     QString file_path ;
     QVector<QRect> m_TypeList;
     QLabel *label1, *label2, *label3;
     QRadioButton *jpg , *png,*bmp;
     void mousePressEvent(QMouseEvent *event) override;
signals:
     void  save_type_clicked(int i);

protected:
    void paintEvent(QPaintEvent *event) ;
private:
    QStringList *file;
    QLabel *savedir , * savetype;
    //小三角的起始位置
    double m_startx;
    double m_startY;
    //小三角的宽
    double  m_triangleWidth;
    //小三角的高
    double m_triangleHeight;
    //RadioButton 半径
    double radius;
    double Rect_h,Rect_w;
public:
    QRect type_rect;
};

#endif // SAVE_LOCATION_H
