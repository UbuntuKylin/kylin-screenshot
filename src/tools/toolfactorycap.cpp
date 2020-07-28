#include "toolfactorycap.h"

ToolFactoryCap::ToolFactoryCap(QObject *parent) : QObject(parent)
{

}
CaptureToolCap* ToolFactoryCap::CreateTool(CaptureButtonCap::ButtonType t, QObject *parent)
{
    CaptureToolCap *tool;
    switch (t) {
     case CaptureButtonCap::TYPE_SCREENCUT:
        tool = new ScreenCut(parent);
        break;
     case CaptureButtonCap::TYPE_SCREENCAP:
        tool = new ScreenCap(parent);
        break;
     case CaptureButtonCap::TYPE_SCREENMOUSECURSOR:
        tool = new ScreenMouseCursor(parent);
        break;
    case CaptureButtonCap::TYPE_SCREENVOICE:
        tool = new ScreenVoice(parent);
        break;
    case CaptureButtonCap::TYPE_SCREENMOUSE:
        tool = new ScreenMouse(parent);
        break;
    case CaptureButtonCap::TYPE_SCREENCAPOPTION:
        tool = new ScreenCapOption(parent);
        break;
    case CaptureButtonCap::TYPE_SCREENCAPEXIT:
        tool = new ScreenCapExit(parent);
        break;
    case CaptureButtonCap::TYPE_SCREENCAPSTART:
        tool = new ScreenCapStart(parent);
        break;
    default:
        tool = nullptr;
        break;
    }
    return tool;
}
