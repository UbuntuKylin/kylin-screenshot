#include "infowidget.h"
#include <QDebug>
#include <QTextBlockFormat>
#include <QDebug>
#include <QScrollBar>
#include <QPainter>
#include <QPainterPath>
#include <QDesktopServices>
#include <QVector4D>

infoWidget::infoWidget(QWidget *parent) :
    QWidget(parent)
{
    context.style_settings = new QGSettings("org.ukui.style");
    context.style_name = context.style_settings->get("style-name").toString();
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

    m_exitButton->setProperty("isWindowButton", 0x2);
    m_exitButton->setProperty("useIconHighlightEffect", 0x8);
    m_exitButton->setFlat(true);
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
    m_Descript->setReadOnly(true);
    font.setFamily("Noto Sans CJK SC Regular");
    font.setBold(false);
    font.setPixelSize(14);
    m_Descript->setFont(font);
    m_Descript->setText(tr(
                            "Screenshot, developed by KylinSoftware, includes programs that can be run on a computer, usually with software development tools."));
    QString locale = QLocale::system().name();
    m_Descript->verticalScrollBar()->hide();
    m_Descript->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    m_Descript->setMinimumWidth(324);
    m_Descript->setFrameShape(QFrame::NoFrame);
    m_Descript->move(32, 238);

// QPalette pl = m_Descript->palette();
// pl.setBrush(QPalette::Base, QBrush(QColor(255, 0, 0, 0)));

// m_Descript->setPalette(pl);
    m_EmailInfo = new QLabel(this);
    connect(m_EmailInfo, &QLabel::linkActivated, this, [=](const QString url) {
        QDesktopServices::openUrl(QUrl(url));
    });
    m_EmailInfo->setFixedSize(350, 32);
    m_EmailInfo->setFont(font);
    QString SUPPORT;
    if ((context.style_name.compare("ukui-dark") == 0)
        || (context.style_name.compare("ukui-black") == 0)) {
        SUPPORT
            =
                "<a href= \"mailto://support@kylinos.cn\" style=\"color:white\">support@kylinos.cn</a>";
    } else {
        SUPPORT
            =
                "<a href= \"mailto://support@kylinos.cn\" style=\"color:black\">support@kylinos.cn</a>";
    }
    m_EmailInfo->setText(tr("SUPPORT:%1").arg(SUPPORT));
    if (locale == "zh_CN") {
        QTextBlockFormat blockFormat;
        blockFormat.setLineHeight(10, QTextBlockFormat::LineDistanceHeight);
        auto textCursor = m_Descript->textCursor();
        textCursor.setBlockFormat(blockFormat);
        m_Descript->setTextCursor(textCursor);
        m_Descript->setFixedHeight(120);
        m_EmailInfo->move(32, 310);
    } else {
        QTextBlockFormat blockFormat;
        blockFormat.setLineHeight(6, QTextBlockFormat::LineDistanceHeight);
        auto textCursor = m_Descript->textCursor();
        textCursor.setBlockFormat(blockFormat);
        m_Descript->setTextCursor(textCursor);
        m_Descript->setFixedHeight(140);
        setFixedSize(388, 410);
        m_EmailInfo->move(32, 350);
    }
    setProperty("useCustomShadow", true); // 启用协议
    setProperty("customShadowDarkness", 1.0);    // 阴影暗度
    setProperty("customShadowWidth", 10);    // 阴影边距大小
    setProperty("customShadowRadius", QVector4D(6, 6, 6, 6));   // 阴影圆角，必须大于0，这个值应该和frameless窗口本身绘制的形状吻合
    setProperty("customShadowMargins", QVector4D(10, 10, 10, 10));   // 阴影与窗口边缘的距离，一般和customShadowWidth保持一致
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
