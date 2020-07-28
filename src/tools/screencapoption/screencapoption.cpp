#include "screencapoption.h"

ScreenCapOption::ScreenCapOption(QObject *parent) : CaptureToolCap(parent)
{

}
QString ScreenCapOption::name() const
{
    return tr("Options");
}
QString ScreenCapOption::nameID()
{
     return QLatin1String("");
}
QString ScreenCapOption::description() const
{
    return tr("options tool");
    //return tr("图片保存配置");
}
bool ScreenCapOption::isValid() const
{
    return true;
}
