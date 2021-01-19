/*
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
#include "mysavedialog.h"
#include <QLineEdit>
#include <QComboBox>
#include <QDebug>
#include <QStandardPaths>
#include "src/utils/filenamehandler.h"

MySaveDialog::MySaveDialog(QWidget *parent) :
    QFileDialog(parent)
{
    setWindowIcon(QIcon("/usr/share/icons/ukui-icon-theme-default/128x128/apps/kylin-screenshot.png"));
    setNameFilter(QLatin1String("Portable Network Graphic file (PNG) (*.png);;BMP file (*.bmp);;JPEG file (*.jpg)"));
    setDirectory(QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).at(0));
    setAcceptMode(QFileDialog::AcceptSave);
    QString a =  FileNameHandler().parsedPattern()+".png";
    this->findChildren<QLineEdit *>("fileNameEdit").at(0)->setText(a);
    connect(this->findChildren<QComboBox *>("fileTypeCombo").at(0), QOverload<int>::of(&QComboBox::currentIndexChanged),
                  [=](int index)
    {
        switch (index) {
        case 0:
            qDebug()<<"change the type is png";
            break;
        case 1:
            qDebug()<<"change the type is bmp";
            break;
        case 2:
            qDebug()<<"change the type is jpg";
            break;
        default:
            break;
        }
        this->findChildren<QLineEdit *>("fileNameEdit").at(0)->setText(a);
    });
}
QString  MySaveDialog::filename()
{
    return  this->findChildren<QLineEdit *>("fileNameEdit").at(0)->text();
}
