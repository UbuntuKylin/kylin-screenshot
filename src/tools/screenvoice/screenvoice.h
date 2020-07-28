#ifndef SCREENVOICE_H
#define SCREENVOICE_H

#include "src/tools/CaptureToolCap.h"
class ScreenVoice : public CaptureToolCap
{
    Q_OBJECT
public:
    explicit ScreenVoice(QObject *parent = nullptr);
    QString name() const override;
    static QString nameID();
    QString description() const override;
    bool isValid() const override;
};

#endif // SCREENVOICE_H
