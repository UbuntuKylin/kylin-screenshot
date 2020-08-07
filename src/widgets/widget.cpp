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
#include "widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)

{
    left_label = new QLabel("left_label",this);
    left_label->move(20,20);
    top_label = new QLabel("left_label",this);
    top_label->move(20,100);
    width_label = new QLabel("left_label",this);
    width_label->move(20,180);
    Height_label = new QLabel("Height_label",this);
    Height_label->move(20,260);
    left_box = new QSpinBox(this);
    left_box->move(120,20);
    top_box  = new QSpinBox(this);
    top_box->move(120,100);
    width_box = new QSpinBox(this);
    width_box ->move(120,180);
    Height_box = new QSpinBox(this);
    Height_box->move(120,260);

    fullscreen_radio = new QRadioButton(this);
    fullscreen_radio->setText("fullscreen");
    fullscreen_radio->move(300,50);
    select_radio = new QRadioButton(this);
    select_radio->setText("select_area");
    select_radio->move(300,200);
    mouse_push  = new QPushButton(this);
    mouse_push->setText("off_mouse");
    mouse_push->move(20,320);
    cursor_push = new QPushButton(this);
    cursor_push->setText("off_cursor");
    cursor_push->move(120,320);
    voice_push = new QPushButton(this);
    voice_push->setText("off_voice");
    voice_push->move(220,320);
    option_push = new QPushButton(this);
    option_push->setText("option");
    option_push->move(320,320);
    start_push = new QPushButton(this);
    start_push->setText("start");
    start_push->move(420,320);

    left_box->setRange(0, 100);
    left_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    top_box->setRange(0, 100);
    top_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    width_box->setRange(0, 100);
    width_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    Height_box->setRange(0, 100);
    Height_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    so = new ScreenOption(this);
    so->move(350,350);
    so->hide();
    st = new ScreenType(this);
    st->move(430,380);
    st->hide();
    click = start_click = mouse_click = mousecursor_click =voice_click = screen_type = false;
    connect(option_push,&QPushButton::clicked,this,&Widget::update_screen_option_state);
    connect(start_push,&QPushButton::clicked,this,&Widget::update_screen_start_state);
    connect(mouse_push,&QPushButton::clicked,this,&Widget::update_screen_mouse_state);
    connect(cursor_push,&QPushButton::clicked,this,&Widget::update_screen_mousecursor_state);
    connect(voice_push,&QPushButton::clicked,this,&Widget::update_screen_voice_state);
    connect(so->type_btn,&QPushButton::clicked,this,&Widget::update_screen_type);
    connect(st,SIGNAL(Type_change(QString)),so,SLOT(type(QString)));
    this->resize(800,600);
}
void Widget::update_screen_option_state()
{
    click = !click ;
    if(click)
    {
        so->show();
    }
    else
    {
        so->hide();
    }
    qDebug()<<"click"<<click;
}
void Widget::update_screen_start_state()
{
    start_click = !start_click ;
    if(start_click)
    {
        start_push->setText("end");
    }
    else
    {
        start_push->setText("start");
    }
    qDebug()<<"start_click"<<start_click;
}
void  Widget::update_screen_mouse_state()
{
    mouse_click = !mouse_click;
    if (mouse_click)
    {
       mouse_push->setText("on_mouse");
    }
    else
    {
        mouse_push->setText("off_mouse");
    }
    qDebug()<<"mouse_click"<<mouse_click;
}

void Widget::update_screen_mousecursor_state()
{
    mousecursor_click = !mousecursor_click;
    if (mousecursor_click)
    {
        cursor_push->setText("on_cursor");
    }
    else
    {
        cursor_push->setText("off_cursor");
    }
    qDebug()<<"mousecursor_click"<<mousecursor_click;
}

void Widget::update_screen_voice_state()
{
    voice_click = !voice_click;
    if(voice_click)
    {
        voice_push->setText("on_voice");
    }
    else
    {
        voice_push->setText("off_voice");
    }
    qDebug()<<"on_voice"<<voice_click;
}
void Widget::update_screen_type()
{
    screen_type = !screen_type;
    if (screen_type)
    {
        st->show();
    }
    else
    {
        st->hide();
    }
    qDebug()<<"screen_type"<<screen_type;
}
Widget::~Widget()
{
  this->destroy();
}

