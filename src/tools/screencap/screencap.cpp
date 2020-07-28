#include "screencap.h"

ScreenCap::ScreenCap(QObject *parent) : CaptureToolCap(parent)
{

}
QString ScreenCap::name() const
{
    return tr("Options");
}
QString ScreenCap::nameID()
{
     return QLatin1String("");
}
QString ScreenCap::description() const
{
    return tr("options tool");
    //return tr("图片保存配置");
}
bool ScreenCap::isValid() const
{
    return true;
}
