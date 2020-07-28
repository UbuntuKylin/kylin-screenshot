#ifndef SCREENMOUSE_H
#define SCREENMOUSE_H

#include "src/tools/CaptureToolCap.h"
class ScreenMouse : public CaptureToolCap
{
    Q_OBJECT
public:
    explicit ScreenMouse(QObject *parent = nullptr);
    QString name() const override;
    static QString nameID();
    QString description() const override;
    bool isValid() const override;
};

#endif // SCREENMOUSE_H
