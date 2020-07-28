#ifndef SCREENCAP_H
#define SCREENCAP_H
#include "src/tools/CaptureToolCap.h"
class ScreenCap : public CaptureToolCap
{
    Q_OBJECT
public:
    explicit ScreenCap(QObject *parent = nullptr);
    QString name() const override;
    static QString nameID();
    QString description() const override;
    bool isValid() const override;
};

#endif // SCREENCAP_H
