#include "luping.h"

LuPing::LuPing(QObject *parent) : AbstractActionTool(parent)
{

}
bool LuPing::closeOnButtonPressed() const
{
     return true;
}

QIcon LuPing:: icon(const QColor &background, bool inEditor) const
{
    Q_UNUSED(background);
    return inEditor ?  QIcon(QStringLiteral(":/img/material/black/") + "luping_icon.svg") :
                      QIcon(QStringLiteral(":/img/material/white/") + "luping.svg");
}
QString LuPing::name() const
{
    return tr("LuPing");
}
 QString LuPing:: nameID()
{
     return QLatin1String("");
}
QString LuPing::description() const
{
    return tr("luping");
}
CaptureTool* LuPing::copy(QObject *parent)
{
     return new LuPing(parent);
}


void LuPing::pressed(const CaptureContext &context)
{
    Q_UNUSED(context);
    emit requestAction(REQ_LUPING);
}
