#ifndef SCREENCAPEXIT_H
#define SCREENCAPEXIT_H

#include "src/tools/CaptureToolCap.h"
class ScreenCapExit : public CaptureToolCap
{
    Q_OBJECT
public:
    explicit ScreenCapExit(QObject *parent = nullptr);
    QString name() const override;
    static QString nameID();
    QString description() const override;
    bool isValid() const override;
};

#endif // SCREENCAPEXIT_H
