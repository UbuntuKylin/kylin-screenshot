#ifndef CAPTURETOOLCAP_H
#define CAPTURETOOLCAP_H
#include "src/tools/capturecontext.h"
#include "src/utils/colorutils.h"
#include "src/utils/pathinfo.h"
#include <QIcon>
#include <QPainter>

class CaptureToolCap  : public QObject {

    Q_OBJECT
public:
    enum Request {
        REQ_SCREENCUT,
        REQ_SCRRENCAP,
        REQ_SCREENMOUSECURSOR,
        REQ_VOICE,
        REQ_MOUSE,
        REQ_SCREENCAPOPTION,
        REQ_SCREENCAPEXIT,
        REQ_SCREENSTAR,
     };
    explicit CaptureToolCap(QObject *parent = nullptr) : QObject(parent){}
    virtual bool isValid() const = 0;
    virtual QString name() const = 0;
    static QString nameID();
    virtual QString description() const = 0;
};
#endif // CAPTURETOOLCAP_H
