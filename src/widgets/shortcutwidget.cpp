#include "shortcutwidget.h"
#include <QTableWidget>
#include <QHeaderView>
#include <QPainter>
#include <QPushButton>
ShortCutWidget::ShortCutWidget(QWidget *parent) :
    QWidget(parent)
{
    initLabel();
    tableName = new  QLabel(this);
    setWindowTitle(tr("Screenshot"));
    font.setFamily("Noto Sans CJK SC Medium");
    font.setWeight(16);
    tableName->setFont(font);
    tableName->setText(tr("ShortCut"));
    tableName->move(25, 16);
    setFixedSize(400, 506);
}

void ShortCutWidget:: initLabel()
{
    for (int i = 0; i < m_keys.size(); i++) {
        QLabel *lb = new  QLabel(this);
        lb->setFixedSize(352, 38);
        QLabel *labelchild1 = new QLabel;
        QLabel *labelchild2 = new QLabel;
        labelchild1->setText(tr(m_description.at(i)));
        labelchild1->setAlignment(Qt::AlignLeft);
        labelchild2->setText(tr(m_keys.at(i)));
        QHBoxLayout *m_layout = new  QHBoxLayout;
        labelchild1->setFixedSize(208, 38);
        m_layout->addWidget(labelchild1);
        m_layout->addWidget(labelchild2);
        lb->setLayout(m_layout);
        labels.append(lb);
    }
    for (int i = 0; i < m_keys.size(); i++) {
        labels.at(i)->move(25, 56+i*38);
    }
}

QVector<const char *> ShortCutWidget::m_keys =
{
    QT_TR_NOOP("Keypress"),
    "PrtSc",
    "Ctrl + PrtSc",
    "Shift + PrtSc",
    "←↓↑→",
    "SHIFT + ←↓↑→",
    "ESC",
    "CTRL + C",
    "CTRL + S",
    "CTRL + Z",
    QT_TR_NOOP("Mouse Wheel")
};

QVector <const char *> ShortCutWidget::m_description =
{
    QT_TR_NOOP("Description"),
    QT_TR_NOOP("Capturn Full Screen"),
    QT_TR_NOOP("Capture Top Screen"),
    QT_TR_NOOP("Capture Screen selection"),
    QT_TR_NOOP("Move selection 1px"),
    QT_TR_NOOP("Resize selection 1px"),
    QT_TR_NOOP("Quit capture"),
    QT_TR_NOOP("Copy to clipboard"),
    QT_TR_NOOP("Save selection as a file"),
    QT_TR_NOOP("Undo the last modification"),
    QT_TR_NOOP("Change the tool's thickness")
};

void ShortCutWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    painter.setBrush(QBrush(QColor(95, 95, 95)));
    painter.setOpacity(0.10);
    painter.setPen(QColor(95, 95, 95));
    for (int i = 0; i < m_keys.size(); i++) {
        if (0 != i%2) {
            painter.drawRoundedRect(QRect(labels.at(i)->geometry()), 6, 6, Qt::AbsoluteSize);
        }
    }
}

ShortCutWidget::~ShortCutWidget()
{
}
