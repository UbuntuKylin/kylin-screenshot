#include "screencapexit.h"

ScreenCapExit::ScreenCapExit(QObject *parent) : CaptureToolCap(parent)
{

}
QString ScreenCapExit::name() const
{
    return tr("Options");
}
QString ScreenCapExit::nameID()
{
     return QLatin1String("");
}
QString ScreenCapExit::description() const
{
    return tr("options tool");
    //return tr("图片保存配置");
}
bool ScreenCapExit::isValid() const
{
    return true;
}
