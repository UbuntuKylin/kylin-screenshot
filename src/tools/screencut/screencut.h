#ifndef SCREENCUT_H
#define SCREENCUT_H

#include "src/tools/CaptureToolCap.h"
class ScreenCut : public CaptureToolCap
{
    Q_OBJECT
public:

    explicit ScreenCut(QObject *parent = nullptr);
    QString name() const override;
    static QString nameID();
    QString description() const override;
    bool isValid() const override;
};

#endif // SCREENCUT_H
