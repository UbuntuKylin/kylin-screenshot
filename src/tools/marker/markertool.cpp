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
#include "markertool.h"
#include <QPainter>

namespace {

#define PADDING_VALUE 14

}

MarkerTool::MarkerTool(QObject *parent) : AbstractTwoPointTool(parent) {
    m_supportsOrthogonalAdj = true;
    m_supportsDiagonalAdj = true;
}

QIcon MarkerTool::icon(const QColor &background, bool inEditor) const {
   // Q_UNUSED(inEditor);
    //return QIcon(iconPath(background) + "marker.svg");
    Q_UNUSED(background);
    return inEditor ?  QIcon(QStringLiteral(":/img/material/black/") + "marker.svg") :
                      QIcon(QStringLiteral(":/img/material/white/") + "marker.svg");
}
#ifdef SUPPORT_UKUI
QIcon MarkerTool::icon(const QColor &background, bool inEditor, const CaptureContext &context) const {
    Q_UNUSED(background);
    if((context.style_name.compare("ukui-dark")==0) || (context.style_name.compare("ukui-black")==0)){
        return inEditor ?  QIcon(QStringLiteral(":/img/material/black/") + "marker.svg") :
                          QIcon(QStringLiteral(":/img/material/dark-theme/") + "marker.png");
    }
    //if((context.style_name.compare("ukui-white")==0) || (context.style_name.compare("ukui-default")==0) || (context.style_name.compare("ukui-light")==0)){
    else{
        return inEditor ?  QIcon(QStringLiteral(":/img/material/black/") + "marker.svg") :
                      QIcon(QStringLiteral(":/img/material/white/") + "marker.svg");
    }
}
#endif
QString MarkerTool::name() const {
    return tr("Marker");
}

QString MarkerTool::nameID() {
    return QLatin1String("");
}

QString MarkerTool::description() const {
    return tr("Set the Marker as the paint tool");
}

CaptureTool* MarkerTool::copy(QObject *parent) {
    return new MarkerTool(parent);
}

void MarkerTool::process(QPainter &painter, const QPixmap &pixmap, bool recordUndo) {
    if (recordUndo) {
        updateBackup(pixmap);
    }
    painter.setCompositionMode(QPainter::CompositionMode_Multiply);
    painter.setOpacity(0.35);
    painter.setPen(QPen(m_color, m_thickness));
    painter.drawLine(m_points.first, m_points.second);
}

void MarkerTool::paintMousePreview(QPainter &painter, const CaptureContext &context) {
    painter.setCompositionMode(QPainter::CompositionMode_Multiply);
    painter.setOpacity(0.35);
    painter.setPen(QPen(context.color, PADDING_VALUE + context.thickness));
    painter.drawLine(context.mousePos, context.mousePos);
}

void MarkerTool::drawStart(const CaptureContext &context) {
    m_color = context.color;
    m_thickness = context.thickness + PADDING_VALUE;
    m_points.first = context.mousePos;
    m_points.second = context.mousePos;
}

void MarkerTool::pressed(const CaptureContext &context) {
    Q_UNUSED(context);
}

void MarkerTool::thicknessChanged(const int th) {
    m_thickness = th + PADDING_VALUE;
}
