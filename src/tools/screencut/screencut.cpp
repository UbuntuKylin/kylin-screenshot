#include "screencut.h"

ScreenCut::ScreenCut(QObject *parent) : CaptureToolCap(parent)
{

}
QString ScreenCut::name() const
{
    return tr("Options");
}
QString ScreenCut::nameID()
{
     return QLatin1String("");
}
QString ScreenCut::description() const
{
    return tr("options tool");
    //return tr("图片保存配置");
}
bool ScreenCut::isValid() const
{
    return true;
}
