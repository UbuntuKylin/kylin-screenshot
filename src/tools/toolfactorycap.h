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
