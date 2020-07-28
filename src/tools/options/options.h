#ifndef OPTIONS_H
#define OPTIONS_H

#include <QLabel>
#include <QHBoxLayout>
#include "src/tools/abstractactiontool.h"
class Options : public AbstractActionTool {
    Q_OBJECT
public:
    explicit Options(QObject *parent = nullptr);

    bool closeOnButtonPressed() const;

    QIcon icon(const QColor &background, bool inEditor) const override;
    QString name() const override;
    static QString nameID();
    QString description() const override;

    CaptureTool* copy(QObject *parent = nullptr) override;

public slots:
    void pressed(const CaptureContext &context) override;


};

#endif // OPTIONS_H
