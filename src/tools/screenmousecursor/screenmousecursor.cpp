#include "screenmousecursor.h"

ScreenMouseCursor::ScreenMouseCursor(QObject *parent) : CaptureToolCap(parent)
{

}
QString ScreenMouseCursor::name() const
{
    return tr("Options");
}
QString ScreenMouseCursor::nameID()
{
     return QLatin1String("");
}
QString ScreenMouseCursor::description() const
{
    return tr("options tool");
    //return tr("图片保存配置");
}
bool ScreenMouseCursor::isValid() const
{
    return true;
}
