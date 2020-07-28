#ifndef SCREENOPTION_H
#define SCREENOPTION_H
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QButtonGroup>

class ScreenOption : public QWidget
{
    Q_OBJECT
public:
    explicit ScreenOption(QWidget *parent = nullptr);
    QVBoxLayout *vbox;
    QLabel *locate_label, *type_label, *voice_label,*audio_label;
    QPushButton *locate_btn, *type_btn, *voice_btn, *audio_btn;

public slots:
    void type(QString type);
};

#endif // SCREENOPTION_H
