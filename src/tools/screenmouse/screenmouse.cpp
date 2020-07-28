#include "screenmouse.h"

ScreenMouse::ScreenMouse(QObject *parent) : CaptureToolCap(parent)
{

}
QString ScreenMouse::name() const
{
    return tr("Options");
}
QString ScreenMouse::nameID()
{
     return QLatin1String("");
}
QString ScreenMouse::description() const
{
    return tr("options tool");
    //return tr("图片保存配置");
}
bool ScreenMouse::isValid() const
{
    return true;
}
