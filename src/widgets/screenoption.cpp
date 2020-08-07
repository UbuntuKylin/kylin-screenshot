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
#include "screenoption.h"

ScreenOption::ScreenOption(QWidget *parent) : QWidget(parent)
{
    //设置窗体边框隐藏
    setWindowFlags(Qt::FramelessWindowHint);
    //设置窗体透明
    //setAttribute(Qt::WA_TranslucentBackground);
    vbox = new QVBoxLayout(this);
    locate_label = new QLabel("locate_label");
    type_label = new QLabel("type_label");
    voice_label = new QLabel("voice_label");
    audio_label = new QLabel("audio_label");
    locate_btn = new  QPushButton();
    type_btn = new QPushButton();
    type_btn->setText("MP4");
    voice_btn = new QPushButton();
    audio_btn = new QPushButton();
    vbox->addWidget(locate_label);
    vbox->addWidget(locate_btn);
    vbox->addWidget(type_label);
    vbox->addWidget(type_btn);
    vbox->addWidget(voice_label);
    vbox->addWidget(voice_btn);
    vbox->addWidget(audio_label);
    vbox->addWidget(audio_btn);
}

void ScreenOption::type(QString types)
{
    type_btn->setText(types);
}
