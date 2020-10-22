/* Copyright(c) 2017-2019 Alejandro Sirgo Rica & Contributors
*               2020 KylinSoft Co., Ltd.
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

#include "copytool.h"
#include "src/utils/screenshotsaver.h"
#include <QPainter>

CopyTool::CopyTool(QObject *parent) : AbstractActionTool(parent) {

}

bool CopyTool::closeOnButtonPressed() const {
    return true;
}

QIcon CopyTool::icon(const QColor &background, bool inEditor, const CaptureContext &context) const {
    //Q_UNUSED(inEditor);
    //return QIcon(iconPath(background) + "content-copy.svg");
    Q_UNUSED(background);
    if((context.style_name.compare("ukui-white")==0) || (context.style_name.compare("ukui-default")==0) || (context.style_name.compare("ukui-light")==0)){
        return inEditor ?  QIcon(QStringLiteral(":/img/material/black/") + "content-copy.svg") :
                           QIcon(QStringLiteral(":/img/material/white/") + "content-copy.svg");
    }
    else if((context.style_name.compare("ukui-dark")==0) || (context.style_name.compare("ukui-black")==0)){
        return inEditor ?  QIcon(QStringLiteral(":/img/material/black/") + "content-copy.svg") :
                           QIcon(QStringLiteral(":/img/material/dark-theme/") + "copy.png");
    }
}
QString CopyTool::name() const {
    return tr("Copy");
}

QString CopyTool::nameID() {
    return QLatin1String("");
}

QString CopyTool::description() const {
    return tr("Copy the selection into the clipboard");
}

CaptureTool* CopyTool::copy(QObject *parent) {
    return new CopyTool(parent);
}

void CopyTool::pressed(const CaptureContext &context) {
    emit requestAction(REQ_CAPTURE_DONE_OK);
    ScreenshotSaver().saveToClipboard(context.selectedScreenshotArea());
}
