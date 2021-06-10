#include "infowidget.h"
#include <QDebug>
#include <QTextBlockFormat>
#include <QDebug>
#include <QScrollBar>
#include <QPainter>
#include <QPainterPath>

infoWidget::infoWidget(QWidget *parent) :
    QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(388, 384);
    m_exitButton = new QPushButton(this);

    m_exitButton->setIcon(QIcon::fromTheme("window-close-symbolic"));
    // m_exitButton->setAttribute(Qt::WA_TranslucentBackground);
    m_exitButton->setFixedSize(24, 24);
    m_exitButton->move(356, 12);
    connect(m_exitButton, &QPushButton::clicked,
            this, &infoWidget::close);
    // 取消按钮默认灰色背景
    QPalette palette = m_exitButton->palette();
    QColor ColorPlaceholderText(255, 255, 255, 0);
    QBrush brush;
    brush.setColor(ColorPlaceholderText);
    palette.setBrush(QPalette::Button, brush);
    palette.setBrush(QPalette::ButtonText, brush);
    m_exitButton->setPalette(palette);
    m_exitButton->setProperty("isWindowButton", 0x1);
    m_exitButton->setProperty("useIconHighlightEffect", 0x2);
    m_appIcon = new  QLabel(this);
    m_appIcon->setPixmap(QPixmap(QIcon::fromTheme("kylin-screenshot").pixmap(QSize(96, 96))));
    m_appIcon->setAlignment(Qt::AlignHCenter);
    m_appIcon->setFixedSize(388, 100);
    m_appIcon->move(0, 56);

    m_appName = new QLabel(this);
    font.setFamily("Noto Sans CJK SC Bold");
    font.setBold(true);
    font.setPixelSize(20);
    m_appName->setFont(font);
    m_appName->setText(tr("screenshot"));
    m_appName->setAlignment(Qt::AlignHCenter);
    m_appName->setFixedSize(388, 30);
    m_appName->move(0, 154);

    m_appVersion = new QLabel(this);
    font.setFamily("Noto Sans CJK SC Regular");
    font.setWeight(14);
    font.setBold(false);
    // font.setPixelSize(14);
    m_appVersion->setFont(font);
    m_appVersion->setText(tr("version:v1.0.0"));
    m_appVersion->setFixedSize(388, 25);
    m_appVersion->move(0, 190);
    m_appVersion->setAlignment(Qt::AlignHCenter);

    m_Descript = new QTextEdit(this);
    // m_Descript->setReadOnly(true);
    font.setFamily("Noto Sans CJK SC Regular");
    font.setBold(false);
    font.setPixelSize(14);
    m_Descript->setFont(font);
    // QTextBlockFormat blockFormat;
    m_Descript->setText(tr(
                            "Screenshot, developed by KylinSoftware, includes programs that can be run on a computer, usually with software development tools."));
    QString locale = QLocale::system().name();
    m_Descript->verticalScrollBar()->hide();
    m_Descript->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    m_Descript->setMinimumWidth(324);
    m_Descript->setFrameShape(QFrame::NoFrame);
    m_Descript->move(32, 238);

    QPalette pl = m_Descript->palette();
    pl.setBrush(QPalette::Base, QBrush(QColor(255, 0, 0, 0)));

    m_Descript->setPalette(pl);
    m_supportFrom = new QLabel(this);
    m_supportFrom->setFixedSize(104, 32);

    m_supportFrom->setText(tr("Service and support teams:"));
    m_supportFrom->setFont(font);

    m_EmailInfo = new QLabel(this);
    m_EmailInfo->setFixedSize(150, 32);

    font.setUnderline(true);
    m_EmailInfo->setFont(font);
    m_EmailInfo->setText("support@kylinos.cn");
    if (locale == "zh_CN") {
        QTextBlockFormat blockFormat;
        blockFormat.setLineHeight(10, QTextBlockFormat::LineDistanceHeight);
        auto textCursor = m_Descript->textCursor();
        textCursor.setBlockFormat(blockFormat);
        m_Descript->setTextCursor(textCursor);
        m_Descript->setFixedHeight(120);
        m_supportFrom->move(32, 310);
        m_EmailInfo->move(147, 310);
    } else {
        QTextBlockFormat blockFormat;
        blockFormat.setLineHeight(6, QTextBlockFormat::LineDistanceHeight);
        auto textCursor = m_Descript->textCursor();
        textCursor.setBlockFormat(blockFormat);
        m_Descript->setTextCursor(textCursor);
        m_Descript->setFixedHeight(140);
        setFixedSize(388, 410);
        m_supportFrom->move(32, 350);
        m_EmailInfo->move(147, 350);
    }
}

void infoWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    // 反锯齿;
    painter.setBrush(QBrush(this->palette().color(QPalette::Base).light(150)));
    painter.setPen(Qt::transparent);
    QRect rect = this->rect();
    rect.setWidth(rect.width() - 1);
    rect.setHeight(rect.height() - 1);
    painter.drawRoundedRect(rect, 6, 6);
    // 也可用QPainterPath 绘制代替 painter.drawRoundedRect(rect, 15, 15);

    QWidget::paintEvent(event);
}

infoWidget::~infoWidget()
{
}
