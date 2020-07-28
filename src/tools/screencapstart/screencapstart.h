#ifndef SCREENCAPSTART_H
#define SCREENCAPSTART_H

#include "src/tools/CaptureToolCap.h"
class ScreenCapStart : public CaptureToolCap
{
    Q_OBJECT
public:
    explicit  ScreenCapStart(QObject *parent = nullptr);
    QString name() const override;
    static QString nameID();
    QString description() const override;
    bool isValid() const override;
};

#endif // SCREENCAPSTART_H
