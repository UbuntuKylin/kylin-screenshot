#include "cut.h"

Cut::Cut(QObject *parent) : AbstractActionTool(parent) {


}
bool Cut::closeOnButtonPressed() const
{
     return false;
}

QIcon Cut::icon(const QColor &background, bool inEditor) const
{
    Q_UNUSED(background);
    return inEditor ?  QIcon(QStringLiteral(":/img/material/black/") + "cut.svg") :
                      QIcon(QStringLiteral(":/img/material/black/") + "cut.svg");
}
QString Cut::name() const
{
    return tr("Cut");
}
QString  Cut::nameID()
{
     return QLatin1String("");
}
QString Cut::description() const
{
    return tr("cut");
}

CaptureTool* Cut::copy(QObject *parent)
{
     return new Cut(parent);
}


void Cut::pressed(const CaptureContext &context)
{
    Q_UNUSED(context);
    emit requestAction(REQ_CUT);
}
