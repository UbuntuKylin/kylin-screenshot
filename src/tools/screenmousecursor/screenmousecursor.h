#ifndef SCREENMOUSECURSOR_H
#define SCREENMOUSECURSOR_H

#include "src/tools/CaptureToolCap.h"
class ScreenMouseCursor : public CaptureToolCap
{
    Q_OBJECT
public:
    explicit ScreenMouseCursor(QObject *parent = nullptr);
    QString name() const override;
    static QString nameID();
    QString description() const override;
    bool isValid() const override;
};

#endif // SCREENMOUSECURSOR_H
