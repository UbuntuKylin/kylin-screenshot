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
#ifndef SAVE_LOCATION2_H
#define SAVE_LOCATION2_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>

class Save_Location2 : public QWidget
{
    Q_OBJECT
public:
    explicit Save_Location2(QWidget *parent = nullptr);
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
    QLabel *savedir , *savetype;

    //小三角的起始位置
    int m_startx;
    int m_startY;
    //小三角的宽
    int m_triangleWidth;
    //小三角的高
    int m_triangleHeight;

};


#endif // SAVE_LOCATION2_H
