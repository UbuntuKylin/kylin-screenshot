#include "screenvoice.h"

ScreenVoice::ScreenVoice(QObject *parent) : CaptureToolCap(parent)
{

}
QString ScreenVoice::name() const
{
    return tr("Options");
}
QString ScreenVoice::nameID()
{
     return QLatin1String("");
}
QString ScreenVoice::description() const
{
    return tr("options tool");
    //return tr("图片保存配置");
}
bool ScreenVoice::isValid() const
{
    return true;
}
