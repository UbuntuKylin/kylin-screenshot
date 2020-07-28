#ifndef CUT_H
#define CUT_H

#include "src/tools/abstractactiontool.h"
class Cut : public AbstractActionTool {
    Q_OBJECT
public:
    explicit Cut(QObject *parent = nullptr);

    bool closeOnButtonPressed() const;

    QIcon icon(const QColor &background, bool inEditor) const override;
    QString name() const override;
    static QString nameID();
    QString description() const override;

    CaptureTool* copy(QObject *parent = nullptr) override;

public slots:
    void pressed(const CaptureContext &context) override;
};

#endif // CUT_H
