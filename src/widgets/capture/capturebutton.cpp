/* Copyright(c) 2017-2019 Alejandro Sirgo Rica & Contributors
*             2020 KylinSoft Co., Ltd.
* This file is part of Kylin-Screenshot.
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "capturebutton.h"
#include "src/widgets/capture/capturewidget.h"
#include "src/utils/confighandler.h"
#include "src/tools/capturetool.h"
#include "src/tools/toolfactory.h"
#include "src/utils/globalvalues.h"
#include "src/utils/colorutils.h"
#include <QFont>
#include <QIcon>
#include <QPropertyAnimation>
#include <QToolTip>
#include <QMouseEvent>
#include <QGraphicsDropShadowEffect>
#include <QApplication>

// Button represents a single button of the capture widget, it can enable
// multiple functionality.

CaptureButton::CaptureButton(const ButtonType t, QWidget *parent) : QPushButton(parent),
    m_buttonType(t)
{
    initButton();
    setCursor(Qt::ArrowCursor);
    updateIcon();
    QFont font ( "Noto Sans CJK Regular", 12, 20);
    if (t == TYPE_OPTION)
    {
        label = new  QLabel();
        label2 = new  QLabel();
        layout = new  QHBoxLayout(this);
        label->setAlignment( Qt::AlignCenter);
        label2->setAlignment( Qt::AlignCenter|Qt::AlignHCenter);
        setStyleSheet("QPushButton{font-family:'宋体';font-size:20px;color:rgb(0,0,0,255);}");
        label->setFont(font);
        label->setText("选项");
        if((m_context.style_name.compare("ukui-white")==0) || (m_context.style_name.compare("ukui-default")==0)){
            label2->setPixmap(QPixmap(QStringLiteral(":/img/material/white/down.svg")));
        }
        else if((m_context.style_name.compare("ukui-dark")==0) || (m_context.style_name.compare("ukui-black")==0)){
             label2->setPixmap(QPixmap(QStringLiteral(":/img/material/dark-theme/down.png")));
        }
        layout->addWidget(label);
        layout->addWidget(label2);
        this->setFixedSize(GlobalValues::buttonBaseSize()*2,GlobalValues::buttonBaseSize());
        QFont f = this->font();
        f.setBold(true);
        //setFlat(false);
    }
    else
    {
        if (t == TYPE_SAVE)
        {
            setStyleSheet("QPushButton{font-family:'宋体';font-size:20px;color:rgb(255,255,255,255);}");
            setFlat(false);
            //setStyleSheet(styleSheet());
            //font.setStyle()
            setFont(font);
            setText("保存");
        }       
    }
}

void CaptureButton::initButton() {
    m_context.style_settings = new QGSettings("org.ukui.style");
    m_context.style_name = m_context.style_settings->get("style-name").toString();
    m_tool = ToolFactory().CreateTool(m_buttonType, this);
    setFocusPolicy(Qt::NoFocus);

    if (m_tool->name() =="Options")
    {
        resize(GlobalValues::buttonBaseSize()*2, GlobalValues::buttonBaseSize());
        setMask(QRegion(QRect(-1,-1, GlobalValues::buttonBaseSize()*2+4,
                              GlobalValues::buttonBaseSize()*2+4),
                        QRegion::Rectangle));
    }
    else
    {
        resize(GlobalValues::buttonBaseSize(), GlobalValues::buttonBaseSize());
        setMask(QRegion(QRect(-1,-1, GlobalValues::buttonBaseSize()+2,
                          GlobalValues::buttonBaseSize()+2),
                     QRegion::Rectangle));
    }
    setToolTip(m_tool->description());
    m_emergeAnimation = new  QPropertyAnimation(this, "size", this);
    m_emergeAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    m_emergeAnimation->setDuration(80);
    m_emergeAnimation->setStartValue(QSize(0, 0));
    m_emergeAnimation->setEndValue(
                QSize(GlobalValues::buttonBaseSize(), GlobalValues::buttonBaseSize()));
    auto dsEffect = new QGraphicsDropShadowEffect(this);
    //dsEffect->setBlurRadius(5);
    dsEffect->setOffset(0);
    dsEffect->setColor(QColor(Qt::black));
    setGraphicsEffect(dsEffect);
}

void CaptureButton::updateIcon() {
    setIcon(icon());
    setIconSize(size()*0.6);
    setFlat(true);
}

QVector<CaptureButton::ButtonType> CaptureButton::getIterableButtonTypes() {
    return iterableButtonTypes;
}

QString CaptureButton::globalStyleSheet() {
    QColor mainColor = ConfigHandler().uiMainColorValue();
    QString baseSheet = "CaptureButton {"
                        "border-width:0px;"                     //边框宽度像素
                        "border-radius:0px;"
                        "background-color: %1; color: %3 }"
                        "CaptureButton:hover { background-color: %2; }"
                        "CaptureButton:pressed:!hover { "
                        "background-color: %1; }";
    // define color when mouse is hovering
    QColor contrast = ColorUtils::contrastColor(m_mainColor);

    // foreground color
    QString color = ColorUtils::colorIsDark(mainColor) ? "white" : "black";

    return baseSheet.arg(Qt::blue).arg(Qt::gray)
                .arg(Qt::black);
}

QString CaptureButton::styleSheet() const {
    QString baseSheet = "CaptureButton { "
                        "background-color: %1; color: %3 }"
                        "CaptureButton:hover { background-color: %2; }"
                        "CaptureButton:pressed:!hover { "
                        "background-color: %1; }";
    // define color when mouse is hovering
    QColor contrast = ColorUtils::contrastColor(m_mainColor);
    // foreground color
    QString color = ColorUtils::colorIsDark(m_mainColor) ? "white" : "black";

   // return color;
    return baseSheet.arg(Qt::blue).arg(Qt::blue)
            .arg(Qt::black);
}

// get icon returns the icon for the type of button
QIcon CaptureButton::icon() const {
    return m_tool->icon(m_mainColor, false,m_context);
}

void CaptureButton::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::LeftButton) {
        emit pressedButton(this);
        emit pressed();
    }
}

void CaptureButton::animatedShow() {
    if(!isVisible()) {
        show();
        m_emergeAnimation->start();
        connect(m_emergeAnimation, &QPropertyAnimation::finished, this, [](){
        });
    }
}

CaptureTool *CaptureButton::tool() const {
    return m_tool;
}

void CaptureButton::setColor(const QColor &c) {
    m_mainColor = c;
    setStyleSheet(styleSheet());
    //updateIcon();
}

QColor CaptureButton::m_mainColor = ConfigHandler().uiMainColorValue();

static std::map<CaptureButton::ButtonType, int> buttonTypeOrder {
   //  { CaptureButton::  TYPE_CUT,          0 },
    // { CaptureButton::  TYPE_LUPING,       1 },
     { CaptureButton::  TYPE_RECT,         0 },
     { CaptureButton:: TYPE_CIRCLE,        1 },
     { CaptureButton::  TYPE_LINE,         2 },
     { CaptureButton::  TYPE_ARROW,        3 },
     { CaptureButton:: TYPE_PEN,           4 },
     { CaptureButton:: TYPE_MARKER,        5 },
     { CaptureButton::  TYPE_TEXT,         6 },
     { CaptureButton::  TYPE_BLUR,         7 },
     { CaptureButton:: TYPR_UNDO,          8 },
     { CaptureButton:: TYPE_OPTION,        9 },
     { CaptureButton:: TYPE_CLOSE,         10 },
     { CaptureButton:: TYPE_COPY,          11 },
     { CaptureButton:: TYPE_SAVE,          12 },
     { CaptureButton:: TYPE_SAVEAS,        13 },
     { CaptureButton:: TYPE_PIN,           14 },
};

int CaptureButton::getPriorityByButton(CaptureButton::ButtonType b) {
    auto it = buttonTypeOrder.find(b);
    return it == buttonTypeOrder.cend() ? (int)buttonTypeOrder.size() : it->second;
}

QVector<CaptureButton::ButtonType> CaptureButton::iterableButtonTypes = {
    //CaptureButton:: TYPE_CUT,
    //CaptureButton:: TYPE_LUPING,
    CaptureButton:: TYPE_RECT,
    CaptureButton:: TYPE_CIRCLE,
    CaptureButton:: TYPE_LINE,
    CaptureButton:: TYPE_ARROW,
    CaptureButton:: TYPE_PEN,
    CaptureButton:: TYPE_MARKER,
    CaptureButton:: TYPE_TEXT,
    CaptureButton:: TYPE_BLUR,
    CaptureButton:: TYPR_UNDO,
    CaptureButton:: TYPE_OPTION,
    CaptureButton:: TYPE_CLOSE,
    CaptureButton:: TYPE_COPY,
    CaptureButton:: TYPE_SAVE,
    CaptureButton:: TYPE_SAVEAS,
    CaptureButton:: TYPE_PIN,
};
