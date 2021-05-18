/* Copyright(c) 2017-2019 Alejandro Sirgo Rica & Contributors
*           2020 KylinSoft Co., Ltd.
* This file is part of Kylin-Screenshot.
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
#include "screenshotsaver.h"
#include "src/utils/systemnotification.h"
#include "src/utils/filenamehandler.h"
#include "src/utils/confighandler.h"
#include <QClipboard>
#include <QApplication>
#include <QMessageBox>
#include <QImageWriter>
#include <QStandardPaths>
#include "mysavedialog.h"
QString typeStr = ".png";
QString PathStr = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).at(0);
ScreenshotSaver::ScreenshotSaver()
{
}

void ScreenshotSaver::saveToClipboard(const QPixmap &capture)
{
    SystemNotification().sendMessage(
        QObject::tr("Capture saved to clipboard"));
    QApplication::clipboard()->setPixmap(capture);
}

bool ScreenshotSaver::saveToFilesystem(const QPixmap &capture, const QString &path)
{
    QString completePath = FileNameHandler().generateAbsolutePath(PathStr);  // = FileNameHandler().generateAbsolutePath(path);
    completePath = completePath + typeStr; // QLatin1String(".png");
    bool ok = capture.save(completePath);
    QString saveMessage;
    QString notificationPath = completePath;

    if (ok) {
        ConfigHandler().setSavePath(path);
        saveMessage = QObject::tr("Capture saved as ") + completePath;
        QApplication::clipboard()->setPixmap(capture);
    } else {
        saveMessage = QObject::tr("Error trying to save as ") + completePath;
        notificationPath = "";
    }

    SystemNotification().sendMessage(saveMessage, notificationPath);
    return ok;
}

bool ScreenshotSaver::saveToFilesystem(const QPixmap &capture, const QString &path,
                                       const QString &type)
{
    QString completePath = FileNameHandler().generateAbsolutePath(path);
    completePath = completePath + type;
    bool ok = capture.save(completePath);
    QString saveMessage;
    QString notificationPath = completePath;

    if (ok) {
        ConfigHandler().setSavePath(path);
        saveMessage = QObject::tr("Capture saved as ") + completePath;
    } else {
        saveMessage = QObject::tr("Error trying to save as ") + completePath;
        notificationPath = "";
    }

    SystemNotification().sendMessage(saveMessage, notificationPath);
    return ok;
}

bool ScreenshotSaver::saveToFilesystemGUI(const QPixmap &capture)
{
    bool ok = false;

    while (!ok) {
        MySaveDialog *a = new  MySaveDialog(nullptr);
        if (a->exec() == QFileDialog::Accepted) {
            QString savePath = a->selectedFiles().at(0);

            if (savePath.isNull()) {
                break;
            }

            if (savePath.endsWith(QLatin1String(".png"), Qt::CaseInsensitive))
                typeStr = ".png";
            else if (savePath.endsWith(QLatin1String(".bmp"), Qt::CaseInsensitive))
                typeStr = ".bmp";
            else if (savePath.endsWith(QLatin1String(".jpg"), Qt::CaseInsensitive))
                typeStr = ".jpg";
            else {
                typeStr = ".png";
                savePath += QLatin1String(".png");
            }
            QString name = a->filename();
            QString msg;
            if (!name.startsWith(QChar('.'), Qt::CaseInsensitive)) {
                ok = capture.save(savePath);
            }
            if (ok) {
                QString pathNoFile = savePath.left(savePath.lastIndexOf(QLatin1String("/")));
                ConfigHandler().setSavePath(pathNoFile);
                msg = QObject::tr("Capture saved as ") + savePath;
                SystemNotification().sendMessage(msg, savePath);
                PathStr = pathNoFile;
            } else {
                if (name.contains(QChar('/'))) {
                    msg = QObject::tr("file name can not contains '/'");
                } else if (name.startsWith(QChar('.'), Qt::CaseInsensitive)) {
                    msg = QObject::tr("can not save file as hide file");
                } else {
                    msg = QObject::tr("Error trying to save as ") + savePath;
                }
                QMessageBox saveErrBox(
                    QMessageBox::Warning,
                    QObject::tr("Save Error"),
                    msg);
                saveErrBox.setWindowIcon(QIcon(
                                             "/usr/share/icons/ukui-icon-theme-default/128x128/apps/kylin-screenshot.png"));
                saveErrBox.exec();
            }
        } else {
            return ok;
        }
    }
    return ok;
}
