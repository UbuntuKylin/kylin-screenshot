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

#include "linetool.h"
#include <QPainter>

namespace {

#define PADDING_VALUE 2

}

LineTool::LineTool(QObject *parent) : AbstractTwoPointTool(parent) {
    m_supportsOrthogonalAdj = true;
    m_supportsDiagonalAdj = true;
}

QIcon LineTool::icon(const QColor &background, bool inEditor) const {
    //Q_UNUSED(inEditor);
    //return QIcon(iconPath(background) + "line.svg");
    Q_UNUSED(background);
    return inEditor ?  QIcon(QStringLiteral(":/img/material/black/") + "line.svg") :
                      QIcon(QStringLiteral(":/img/material/white/") + "line.svg");
}
QString LineTool::name() const {
    return tr("Line");
}

QString LineTool::nameID() {
    return QLatin1String("");
}

QString LineTool::description() const {
    return tr("Set the Line as the paint tool");
}

CaptureTool* LineTool::copy(QObject *parent) {
    return new LineTool(parent);
}

void LineTool::process(QPainter &painter, const QPixmap &pixmap, bool recordUndo) {
    if (recordUndo) {
        updateBackup(pixmap);
    }
    painter.setPen(QPen(m_color, m_thickness));
    painter.drawLine(m_points.first, m_points.second);
}

void LineTool::paintMousePreview(QPainter &painter, const CaptureContext &context) {
    painter.setPen(QPen(context.color, PADDING_VALUE + context.thickness));
    painter.drawLine(context.mousePos, context.mousePos);
}

void LineTool::drawStart(const CaptureContext &context) {
    m_color = context.color;
    m_thickness = context.thickness + PADDING_VALUE;
    m_points.first = context.mousePos;
    m_points.second = context.mousePos;
}

void LineTool::pressed(const CaptureContext &context) {
    Q_UNUSED(context);
}
