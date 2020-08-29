/*
 *
 * Copyright: 2020 KylinSoft Co., Ltd.
 * Authors:
 *   huanhuan zhang <zhanghuanhuan@kylinos.cn>
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
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QRadioButton>
#include "screenoption.h"
#include <QFrame>
#include "screentype.h"
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QLabel *left_label, *top_label, *width_label, *Height_label;
    QSpinBox *left_box, *top_box, *width_box, *Height_box;
    QRadioButton *fullscreen_radio,*select_radio;
    QPushButton *mouse_push, *cursor_push, *voice_push, *option_push, *start_push;
    bool click, start_click, mouse_click, mousecursor_click, voice_click,screen_type;
    ScreenOption *so;
    ScreenType *st;

public slots:
    void update_screen_option_state();
    void update_screen_start_state();
    void update_screen_mouse_state();
    void update_screen_mousecursor_state();
    void update_screen_voice_state();
    void update_screen_type();
private:
    QVBoxLayout *m_main_layout = nullptr;

};
#endif // WIDGET_H
