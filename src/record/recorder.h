#ifndef RECORD_H
#define RECORD_H

#include <QObject>

#include "src/widgets/capture/capturebutton.h"

class ssrtools;
class mypopup;

class Recorder : public QObject
{
    Q_OBJECT
    friend class CaptureWidget;
public:
    explicit Recorder(QWidget *parent = nullptr);
    ~Recorder();

    void updateRecordButtons_sth(CaptureButton *b);

    void OnRecordCursorClicked();
    void OnRecordOptionClicked();
    void OnRecordStartClicked();

private:
    ssrtools* ssr;
    mypopup *mp;
    QPushButton *m_pushbutton_save, *m_pushbutton_cancel;

    QMap<CaptureButton::ButtonType, CaptureTool*> m_activeButtons;
    QMap<CaptureButton::ButtonType, CaptureTool*> m_isolatedButtons;

private slots:
    void record_save_clicked();
    void record_cancel_clicked();
};

#endif // RECORD_H
