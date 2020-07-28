#include "options.h"

Options::Options(QObject *parent) : AbstractActionTool(parent)
{
}
bool Options::closeOnButtonPressed() const
{
     return false;
}

QIcon Options::icon(const QColor &background, bool inEditor) const
{
    Q_UNUSED(background);
   // return inEditor ?  QIcon(QStringLiteral(":/img/material/black/") + "content-copy.svg") :
     //                 QIcon(QStringLiteral(":/img/material/white/") + "content-copy.svg");
    return QIcon();
}
QString Options::name() const
{
    return tr("Options");
}
QString Options::nameID()
{
     return QLatin1String("");
}
QString Options::description() const
{
    return tr("options tool");
    //return tr("图片保存配置");
}

CaptureTool* Options::copy(QObject *parent)
{
     return new Options(parent);
}

void Options::pressed(const CaptureContext &context)
{
    Q_UNUSED(context);
    emit requestAction(REQ_OPTIONS);
}
