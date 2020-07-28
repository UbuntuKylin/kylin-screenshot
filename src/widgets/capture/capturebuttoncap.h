#ifndef CAPTUREBUTTONCAP_H
#define CAPTUREBUTTONCAP_H


#include <QPushButton>
#include <QMap>
#include <QVector>
#include <QLabel>
#include <QHBoxLayout>

class QWidget;
class QPropertyAnimation;
class CaptureToolCap;

class CaptureButtonCap : public QPushButton {
    Q_OBJECT

public:
    enum ButtonType
    {
        TYPE_SCREENCUT,
        TYPE_SCREENCAP,
        TYPE_SCREENMOUSECURSOR,
        TYPE_SCREENVOICE,
        TYPE_SCREENMOUSE,
        TYPE_SCREENCAPOPTION,
        TYPE_SCREENCAPEXIT,
        TYPE_SCREENCAPSTART,
    };
public:
    CaptureButtonCap() = delete;
    explicit CaptureButtonCap(const ButtonType t, QWidget *parent = nullptr);
};

#endif // CAPTUREBUTTONCAP_H
