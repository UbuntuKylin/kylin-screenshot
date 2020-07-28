#include "screencapstart.h"

ScreenCapStart::ScreenCapStart(QObject *parent) : CaptureToolCap(parent)
{

}
QString ScreenCapStart::name() const
{
    return tr("Options");
}
QString ScreenCapStart::nameID()
{
     return QLatin1String("");
}
QString ScreenCapStart::description() const
{
    return tr("options tool");
    //return tr("图片保存配置");
}
bool ScreenCapStart::isValid() const
{
    return true;
}
