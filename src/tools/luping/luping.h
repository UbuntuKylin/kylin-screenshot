#ifndef LUPING_H
#define LUPING_H

#include "src/tools/abstractactiontool.h"
class LuPing : public AbstractActionTool {
    Q_OBJECT
public:
    explicit LuPing(QObject *parent = nullptr);

    bool closeOnButtonPressed() const;

    QIcon icon(const QColor &background, bool inEditor) const override;
    QString name() const override;
    static QString nameID();
    QString description() const override;

    CaptureTool* copy(QObject *parent = nullptr) override;

public slots:
    void pressed(const CaptureContext &context) override;

};

#endif // LUPING_H
