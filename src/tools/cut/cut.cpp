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
#include "cut.h"

Cut::Cut(QObject *parent) : AbstractActionTool(parent) {


}
bool Cut::closeOnButtonPressed() const
{
     return false;
}

QIcon Cut::icon(const QColor &background, bool inEditor) const
{
    Q_UNUSED(background);
    return inEditor ?  QIcon(QStringLiteral(":/img/material/black/") + "cut.svg") :
                      QIcon(QStringLiteral(":/img/material/black/") + "cut.svg");
}
QString Cut::name() const
{
    return tr("Cut");
}
QString  Cut::nameID()
{
     return QLatin1String("");
}
QString Cut::description() const
{
    return tr("cut");
}

CaptureTool* Cut::copy(QObject *parent)
{
     return new Cut(parent);
}


void Cut::pressed(const CaptureContext &context)
{
    Q_UNUSED(context);
    emit requestAction(REQ_CUT);
}
