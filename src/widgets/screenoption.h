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
#ifndef SCREENOPTION_H
#define SCREENOPTION_H
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QButtonGroup>

class ScreenOption : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenOption(QWidget *parent = nullptr);
    QVBoxLayout *vbox;
    QLabel *locate_label, *type_label, *voice_label,*audio_label;
    QPushButton *locate_btn, *type_btn, *voice_btn, *audio_btn;

public slots:
    void type(QString type);
};

#endif // SCREENOPTION_H
