#ifndef SCREENCAPOPTION_H
#define SCREENCAPOPTION_H

#include "src/tools/CaptureToolCap.h"
class ScreenCapOption  : public CaptureToolCap
{
    Q_OBJECT
public:
    //ScreenCapOption();
    explicit ScreenCapOption(QObject *parent = nullptr);
    QString name() const override;
    static QString nameID();
    QString description() const override;
    bool isValid() const override;
};

#endif // SCREENCAPOPTION_H
