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
#ifndef TOOLFACTORYCAP_H
#define TOOLFACTORYCAP_H

#include "src/tools/capturecontext.h"
#include "src/utils/colorutils.h"
#include "src/utils/pathinfo.h"
#include <QIcon>
#include <QPainter>

#include "src/tools/screencap/screencap.h"
#include "src/tools/screencapexit/screencapexit.h"
#include "src/tools/screencut/screencut.h"
#include "src/tools/screenmousecursor/screenmousecursor.h"
#include "src/tools/screenmouse/screenmouse.h"
#include "src/tools/screenvoice/screenvoice.h"
#include "src/tools/screencapoption/screencapoption.h"
#include "src/tools/screencapstart/screencapstart.h"
#include  "src/widgets/capture/capturebuttoncap.h"
class ToolFactoryCap  : public QObject {
    Q_OBJECT
public:
    explicit ToolFactoryCap(QObject *parent = nullptr);

    ToolFactoryCap(const ToolFactoryCap &) = delete;
    ToolFactoryCap & operator=(const ToolFactoryCap &) = delete;
    CaptureToolCap* CreateTool(
            CaptureButtonCap::ButtonType t,
            QObject *parent = nullptr);
};

#endif // TOOLFACTORYCAP_H
