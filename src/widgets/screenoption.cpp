#include "screenoption.h"

ScreenOption::ScreenOption(QWidget *parent) : QWidget(parent)
{
    //设置窗体边框隐藏
    setWindowFlag(Qt::FramelessWindowHint);
    //设置窗体透明
    //setAttribute(Qt::WA_TranslucentBackground);
    vbox = new QVBoxLayout(this);
    locate_label = new QLabel("locate_label");
    type_label = new QLabel("type_label");
    voice_label = new QLabel("voice_label");
    audio_label = new QLabel("audio_label");
    locate_btn = new  QPushButton();
    type_btn = new QPushButton();
    type_btn->setText("MP4");
    voice_btn = new QPushButton();
    audio_btn = new QPushButton();
    vbox->addWidget(locate_label);
    vbox->addWidget(locate_btn);
    vbox->addWidget(type_label);
    vbox->addWidget(type_btn);
    vbox->addWidget(voice_label);
    vbox->addWidget(voice_btn);
    vbox->addWidget(audio_label);
    vbox->addWidget(audio_btn);
}

void ScreenOption::type(QString types)
{
    type_btn->setText(types);
}
