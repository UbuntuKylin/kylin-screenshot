// Copyright(c) 2017-2019 Alejandro Sirgo Rica & Contributors
//              2020 KylinSoft Co., Ltd.
// This file is part of Kylin-Screenshot.
//
//     Kylin-Screenshot is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
//
//     Kylin-Screenshot is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License
//     along with Kylin-Screenshot.  If not, see <http://www.gnu.org/licenses/>.

// Based on Lightscreen areadialog.cpp, Copyright 2017  Christian Kaiser <info@ckaiser.com.ar>
// released under the GNU GPL2  <https://www.gnu.org/licenses/gpl-2.0.txt>

// Based on KDE's KSnapshot regiongrabber.cpp, revision 796531, Copyright 2007 Luca Gugelmann <lucag@student.ethz.ch>
// released under the GNU LGPL  <http://www.gnu.org/licenses/old-licenses/library.txt>

#include "capturewidget.h"
#include "src/widgets/capture/hovereventfilter.h"
#include "src/widgets/panel/sidepanelwidget.h"
#include "src/utils/colorutils.h"
#include "src/utils/globalvalues.h"
#include "src/widgets/capture/notifierbox.h"
#include "src/widgets/capture/colorpicker.h"
#include "src/utils/screengrabber.h"
#include "src/utils/systemnotification.h"
#include "src/utils/screenshotsaver.h"
#include "src/core/controller.h"
#include "src/widgets/capture/modificationcommand.h"
#include <QUndoView>
#include <QScreen>
#include <QGuiApplication>
#include <QApplication>
#include <QShortcut>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QBuffer>
#include <QDesktopWidget>
#include <QDebug>
#include <QRegion>
#include <QFileDialog>
#include <QStandardPaths>
// CaptureWidget is the main component used to capture the screen. It scontains an
// are of selection with its respective buttons.
#include "src/widgets/screenrecorder.h"

// enableSaveWIndow
CaptureWidget::CaptureWidget(const uint id, const QString &savePath,
                             bool fullScreen, QWidget *parent) :
    QWidget(parent), m_mouseIsClicked(false), m_rightClick(false),
    m_newSelection(false), m_grabbing(false), m_captureDone(false),
    m_previewEnabled(true), m_adjustmentButtonPressed(false), m_activeButton(nullptr),
    m_activeTool(nullptr), m_toolWidget(nullptr),
    m_mouseOverHandle(SelectionWidget::NO_SIDE), m_id(id)
{
    // Base config of the widget
    m_eventFilter = new HoverEventFilter(this);
    connect(m_eventFilter, &HoverEventFilter::hoverIn,
            this, &CaptureWidget::childEnter);
    connect(m_eventFilter, &HoverEventFilter::hoverOut,
            this, &CaptureWidget::childLeave);
    setAttribute(Qt::WA_DeleteOnClose);
    m_showInitialMsg = m_config.showHelpValue();
    m_opacity = m_config.contrastOpacityValue();
    setMouseTracking(true);
    initContext(savePath, fullScreen);
    initShortcuts();

#ifdef Q_OS_WIN
    // Top left of the whole set of screens
    QPoint topLeft(0,0);
#endif
    if (fullScreen) {
        // Grab Screenshot
        bool ok = true;
        m_context.screenshot = ScreenGrabber().grabEntireDesktop(ok);
        if(!ok) {
            SystemNotification().sendMessage(tr("Unable to capture screen"));
            this->close();
        }
        m_context.origScreenshot = m_context.screenshot;

#ifdef Q_OS_WIN
        setWindowFlags(Qt::WindowStaysOnTopHint
                       | Qt::FramelessWindowHint
                        | Qt::Popup);

        for (QScreen *const screen : QGuiApplication::screens()) {
            QPoint topLeftScreen = screen->geometry().topLeft();
            if (topLeft.x() > topLeftScreen.x() ||
                    topLeft.y() > topLeftScreen.y()) {
                topLeft = topLeftScreen;
            }
        }
        move(topLeft);
#else
        setWindowFlags(Qt::BypassWindowManagerHint
                       | Qt::WindowStaysOnTopHint
                       | Qt::FramelessWindowHint
                       | Qt::Tool);
#endif
        resize(pixmap().size());
    }
    // Create buttons
    m_buttonHandler = new ButtonHandler(this);
    updateButtons();
    QVector<QRect> areas;
    if (m_context.fullscreen) {
        for (QScreen *const screen : QGuiApplication::screens()) {
            QRect r = screen->geometry();
#ifdef Q_OS_WIN
            r.moveTo(r.topLeft() - topLeft);
#endif
            areas.append(r);
        }
    } else {
        areas.append(rect());
    }
    m_buttonHandler->updateScreenRegions(areas);
    m_buttonHandler->hide();

    initSelection();
    updateCursor();
    // Init color picker
    m_colorPicker = new ColorPicker(this);
    font_color = new FontSize_Color_Chose(this);
    font_color2 = new FontSize_Color_Chose2(this);
    save_location = new  Save_Location(this);
    save_location2 = new Save_Location2(this);
    font_options = new  Font_Options(this);
    font_options2 = new Font_Options2(this);

    screenCap = new ScreenRecorder();
    screenCap->hide();

    connect(m_colorPicker, &ColorPicker::colorSelected,
            this, &CaptureWidget::setDrawColor);
    connect(font_color,&FontSize_Color_Chose::font_size_change,
            this,&CaptureWidget::setDrawThickness);
    connect(font_color,&FontSize_Color_Chose::colorSelected,
                    this,&CaptureWidget::setDrawColor);
    connect(font_color2,&FontSize_Color_Chose2::font_size_change2,
            this,&CaptureWidget::setDrawThickness);
    connect(font_color2,&FontSize_Color_Chose2::colorSelected2,
                    this,&CaptureWidget::setDrawColor);
    connect(font_options,&Font_Options::colorSelected,
                    this,&CaptureWidget::setDrawColor);
    connect(font_options,&Font_Options::font_size_Selete,
                    this,&CaptureWidget::setDrawThickness);
    connect(font_options,&Font_Options::font_type_Selete,
                     this,&CaptureWidget::font_type_changed);
    connect(font_options,SIGNAL(font_bold_change(bool)),
                     this,SLOT(font_bold_clicked(bool)));
    connect(font_options,SIGNAL(font_italic_change(bool)),
                     this,SLOT(font_italic_clicked(bool)));
    connect(font_options,SIGNAL(font_underline_change(bool)),
                     this,SLOT(font_underline_clicked(bool)));
    connect(font_options,SIGNAL(font_delete_change(bool)),
                     this,SLOT(font_delete_clicked(bool)));
    connect(font_options2,&Font_Options2::font_size_Selete,
                    this,&CaptureWidget::setDrawThickness);
    connect(font_options2,&Font_Options2::font_type_Selete,
                     this,&CaptureWidget::font_type_changed);
    connect(font_options2,SIGNAL(font_bold_change(bool)),
                     this,SLOT(font_bold_clicked(bool)));
    connect(font_options2,SIGNAL(font_italic_change(bool)),
                     this,SLOT(font_italic_clicked(bool)));
    connect(font_options2,SIGNAL(font_underline_change(bool)),
                     this,SLOT(font_underline_clicked(bool)));
    connect(font_options2,SIGNAL(font_delete_change(bool)),
                     this,SLOT(font_delete_clicked(bool)));
    connect(save_location,&Save_Location::save_type_clicked,
                this,&CaptureWidget::ClickedSaveType);
    connect(save_location2,&Save_Location2::save_type_clicked,
                this,&CaptureWidget::ClickedSaveType2);
    m_colorPicker->hide();
    font_color->setStartPos(80);
    font_color->setTriangleInfo(20, 20);
    font_color->setFixedSize(QSize(225, 150));

    font_color2->setStartPos(80);
    font_color2->setTriangleInfo(20, 20);
    font_color2->setFixedSize(QSize(225, 150));
    save_location->setStartPos(100);
    save_location->setTriangleInfo(10, 10);
    save_location->setFixedSize(QSize(165, 175));
    save_location->setCenterWidget();

    save_location2->setStartPos(90);
    save_location2->setTriangleInfo(10, 10);
    save_location2->setFixedSize(QSize(165, 175));
    save_location2->setCenterWidget();

    font_options->setStartPos(80);
    font_options->setTriangleInfo(10, 10);
    font_options->setFixedSize(QSize(350, 150));
    font_options->setCenterWidget();
    font_options->Font_size->setValue(m_context.thickness);
    font_options2->setStartPos(80);
    font_options2->setTriangleInfo(10, 10);
    font_options2->setFixedSize(QSize(350, 150));
    font_options2->setCenterWidget();
    font_color->hide();
    font_color2->hide();
    save_location->hide();
    save_location2->hide();
    font_options->hide();
    font_options2->hide();
    // Init notification widget
    m_notifierBox = new NotifierBox(this);
    m_notifierBox->hide();
    font_color_point = new QPoint();
    connect(&m_undoStack, &QUndoStack::indexChanged,
            this, [this](int){ this->update(); });
    initPanel();
}

CaptureWidget::~CaptureWidget() {
    if (m_captureDone) {
        emit captureTaken(m_id, this->pixmap());
    } else {
        emit captureFailed(m_id);
    }
    m_config.setdrawThickness(m_context.thickness);
}

// redefineButtons retrieves the buttons configured to be shown with the
// selection in the capture
void CaptureWidget::updateButtons() {
    m_uiColor = m_config.uiMainColorValue();
    m_contrastUiColor = m_config.uiContrastColorValue();
    auto buttons = m_config.getButtons();
    for (const CaptureButton::ButtonType &t: buttons) {
        CaptureButton *b = new CaptureButton(t, this);
        if (t == CaptureButton::TYPE_OPTION) {
            m_sizeIndButton = b;
            m_sizeIndButton->setColor(Qt::black);
        }
        else if (t == CaptureButton::TYPE_SAVE)
        {
            b->setColor(Qt::black);
        }
        else
        {
            b->setColor(m_uiColor);
        }
        makeChild(b);
        connect(b, &CaptureButton::pressedButton, this, &CaptureWidget::setState);
        connect(b->tool(), &CaptureTool::requestAction,
                this, &CaptureWidget::handleButtonSignal);
        vectorButtons << b;
    }
    m_buttonHandler->setButtons(vectorButtons);
}

QPixmap CaptureWidget::pixmap() {
    QPixmap p;
    if (m_toolWidget && m_activeTool) {
        p = m_context.selectedScreenshotArea().copy();
        QPainter painter(&p);
        m_activeTool->process(painter, p);
    } else {
        p = m_context.selectedScreenshotArea();
    }
    return m_context.selectedScreenshotArea();
}

void CaptureWidget::deleteToolwidgetOrClose() {
    if (m_toolWidget) {
        m_toolWidget->deleteLater();
        m_toolWidget = nullptr;
    } else {
        close();
    }
}

void CaptureWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.drawPixmap(0, 0, m_context.screenshot);
    if (m_context.mousePos.x()< m_selection->x()||m_context.mousePos.x()>m_selection->x()+m_selection->width()
            ||m_context.mousePos.y()< m_selection->y()||m_context.mousePos.y()>m_selection->y()+m_selection->height()+m_buttonHandler->size()+20)
       {
           painter.drawPixmap(m_context.mousePos.x()+10,m_context.mousePos.y()+10,pixmap2);
           painter.setOpacity(0.5);
           painter.drawPixmap(m_context.mousePos.x()+10,m_context.mousePos.y()+10,crosspixmap);
           painter.drawText(m_context.mousePos.x()+10,m_context.mousePos.y()+130,tr("%1 , %2")
                             .arg(m_context.mousePos.x()).arg(m_context.mousePos.y()));
           update();
       }
    if (m_activeTool && m_mouseIsClicked) {
        painter.save();
        m_activeTool->process(painter, m_context.screenshot);
        painter.restore();
    } else if (m_activeButton && m_activeButton->tool()->showMousePreview() &&
               m_previewEnabled)
    {
        painter.save();
        m_activeButton->tool()->paintMousePreview(painter, m_context);
        painter.restore();
    }
    QColor overlayColor(0, 0, 0, m_opacity);
    painter.setBrush(overlayColor);
    QRect r;
    if (m_selection->isVisible()) {
        r = m_selection->geometry().normalized().adjusted(0, 0, -1, -1);
    }
    QRegion grey(rect());
    grey = grey.subtracted(r);

    painter.setClipRegion(grey);
    painter.drawRect(-1, -1, rect().width() + 1, rect().height() + 1);
    painter.setClipRect(rect());

    if (m_showInitialMsg) {
        QRect helpRect = QGuiApplication::primaryScreen()->geometry();
        helpRect.moveTo(mapFromGlobal(helpRect.topLeft()));

        QString helpTxt = tr("Select an area with the mouse, or press Esc to exit."
                             "\nPress Enter to capture the screen."
                             "\nPress Right Click to show the color picker."
                             "\nUse the Mouse Wheel to change the thickness of your tool."
                             "\nPress Space to open the side panel.");

        // We draw the white contrasting background for the text, using the
        //same text and options to get the boundingRect that the text will have.
        QRectF bRect = painter.boundingRect(helpRect, Qt::AlignCenter, helpTxt);
        // These four calls provide padding for the rect
        const int margin = QApplication::fontMetrics().height() / 2;
        bRect.setWidth(bRect.width() + margin);
        bRect.setHeight(bRect.height() + margin);
        bRect.setX(bRect.x() - margin);
        bRect.setY(bRect.y() - margin);
        QColor rectColor(m_uiColor);
        rectColor.setAlpha(180);
        QColor textColor((ColorUtils::colorIsDark(rectColor) ?
                              Qt::white : Qt::black));
        painter.setBrush(QBrush(rectColor, Qt::SolidPattern));
        painter.setPen(QPen(textColor));
        painter.drawRect(bRect);
        painter.drawText(helpRect, Qt::AlignCenter, helpTxt);
    }

    if (m_selection->isVisible()) {
        // paint handlers
        painter.setPen(m_uiColor);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setOpacity(0.7);
        painter.setBrush(QColor(195,195,195));
        painter.drawRect((m_selection->geometry().intersected(rect()).x()+7),m_selection->geometry().intersected(rect()).y()-23,70,20);
        painter.drawText((m_selection->geometry().intersected(rect()).x()+10),m_selection->geometry().intersected(rect()).y()-5,tr("%1 * %2")
                         .arg(m_selection->geometry().intersected(rect()).width()).arg(m_selection->geometry().intersected(rect()).height()));

        if((vectorButtons.first()->pos().x()>0 && m_buttonHandler->isVisible())){
            painter.setOpacity(0.2);
            QRect rr = QRect(vectorButtons.first()->pos().x()-10,vectorButtons.first()->pos().y(),
                             GlobalValues::buttonBaseSize()*20+2,GlobalValues::buttonBaseSize()+2);
            painter.drawRoundRect(rr,6,6);
            QPainterPath path;
            QColor color(92,93,95,50);
            int arr[5] = {50,25,20,10,5};
            for(int i=0; i<5; i++)
            {
                QPainterPath path;
                if(i == 1)
                    path.addRect(rr.x(), rr.y(), rr.width(), rr.height());
                else
                   path.addRoundedRect(rr.x()-i, rr.y()-i, rr.width()+i*2,rr.height()+i*2,6,6);
                color.setAlpha(arr[i]);
                painter.setPen(color);
                painter.drawPath(path);

            }
            painter.drawRoundedRect(m_selection->geometry().intersected(rect()).x()+m_selection->geometry().intersected(rect()).width()+GlobalValues::buttonBaseSize()/3-2,
                                    m_selection->geometry().intersected(rect()).y()+GlobalValues::buttonBaseSize()/3-1,
                                    GlobalValues::buttonBaseSize(),
                                    GlobalValues::buttonBaseSize(),
                                    GlobalValues::buttonBaseSize()/2,
                                    GlobalValues::buttonBaseSize()/2);
            painter.setBrush(QColor(0,0,0));
            //两个分隔符
            painter.drawRect(vectorButtons.first()->pos().x()+GlobalValues::buttonBaseSize()*12+12,vectorButtons.first()->pos().y()+GlobalValues::buttonBaseSize()/4, 1, GlobalValues::buttonBaseSize()/2);
            painter.drawRect(vectorButtons.first()->pos().x()+GlobalValues::buttonBaseSize()*16-9,vectorButtons.first()->pos().y()+GlobalValues::buttonBaseSize()/4, 1,GlobalValues::buttonBaseSize()/2);
        }
        update();
        painter.setBrush(m_uiColor);
        for(auto r: m_selection->handlerAreas()) {
            painter.drawRoundRect(r, 100, 100);
        }
    }
}

void CaptureWidget::mousePressEvent(QMouseEvent *e) {
    update();

    if (e->button() == Qt::RightButton) {
       /* m_rightClick = true;
       m_colorPicker->move(e->pos().x()-m_colorPicker->width()/2,
                            e->pos().y()-m_colorPicker->height()/2);
        m_colorPicker->show();*/
    } else if (e->button() == Qt::LeftButton) {
        m_showInitialMsg = false;
        m_mouseIsClicked = true;
        // Click using a tool
        if (m_activeButton) {
            if (m_activeTool) {
                if (m_activeTool->isValid() && m_toolWidget) {
                    pushToolToStack();
                } else {
                    m_activeTool->deleteLater();
                }
                if (m_toolWidget) {
                    m_toolWidget->deleteLater();
                    return;
                }
            }
            m_activeTool = m_activeButton->tool()->copy(this);
            connect(this, &CaptureWidget::colorChanged,
                    m_activeTool, &CaptureTool::colorChanged);
            connect(this, &CaptureWidget::thicknessChanged,
                    m_activeTool, &CaptureTool::thicknessChanged);
            connect(m_activeTool, &CaptureTool::requestAction,
                    this, &CaptureWidget::handleButtonSignal);
            m_activeTool->drawStart(m_context);
            return;
        }
        m_dragStartPoint = e->pos();
        m_selection->saveGeometry();
        // New selection
        if (!m_selection->geometry().contains(e->pos()) &&
                m_mouseOverHandle == SelectionWidget::NO_SIDE)
        {
            m_selection->setGeometry(QRect(e->pos(), e->pos()));
            m_selection->setVisible(false);
            m_newSelection = true;
            m_buttonHandler->hide();
            update();
        } else {
            m_grabbing = true;
        }
    }
    updateCursor();
}

void CaptureWidget::mouseMoveEvent(QMouseEvent *e) {
    m_context.mousePos = e->pos();
    w = 25;
    h = 25;
    mypixmap = mypixmap.grabWidget(this,e->pos().x(),e->pos().y(),w,h);
    QImage crosstmp=mypixmap.toImage();
    QRgb value = qRgb(0,0,255);
    for(int i=0;i<w;i++)
    {
        for(int j=h/2-1;j<h/2;j++)
        {
           crosstmp.setPixel(i,j,value);
        }
    }
    for(int i=w/2-1;i<w/2;i++)
    {
        for(int j=0;j<h;j++)
        {
            crosstmp.setPixel(i,j,value);
        }
    }
    crosspixmap=QPixmap::fromImage( crosstmp.scaled(w*5,h*5,Qt::KeepAspectRatio) );
    pixmap2 = mypixmap.scaled(w * 5,h * 5,Qt::KeepAspectRatio);
    update();
    if (m_mouseIsClicked && !m_activeButton) {
        if (m_buttonHandler->isVisible()) {
            m_buttonHandler->hide();
     }
        if (m_newSelection) {
            m_selection->setVisible(true);
            m_selection->setGeometry(
                    QRect(m_dragStartPoint, m_context.mousePos).normalized());
            update();
        } else if (m_mouseOverHandle == SelectionWidget::NO_SIDE) {
            // Moving the whole selection
            QRect initialRect = m_selection->savedGeometry().normalized();
            QPoint newTopLeft = initialRect.topLeft() + (e->pos() - m_dragStartPoint);
            QRect finalRect(newTopLeft, initialRect.size());
            if (finalRect.left() < rect().left()) {
                finalRect.setLeft(rect().left());
            } else if (finalRect.right() > rect().right()) {
                finalRect.setRight(rect().right());
            }
            if (finalRect.top() < rect().top()) {
                finalRect.setTop(rect().top());
            } else if (finalRect.bottom() > rect().bottom()) {
                finalRect.setBottom(rect().bottom());
            }
            m_selection->setGeometry(finalRect.normalized().intersected(rect()));
            update();
        } else {
            // Dragging a handle
            QRect r = m_selection->savedGeometry();
            QPoint offset = e->pos() - m_dragStartPoint;
            bool symmetryMod = qApp->keyboardModifiers() & Qt::ShiftModifier;

            using sw = SelectionWidget;
            if (m_mouseOverHandle == sw::TOPLEFT_SIDE
                    || m_mouseOverHandle == sw::TOP_SIDE
                    || m_mouseOverHandle == sw::TOPRIGHT_SIDE)
            { // dragging one of the top handles
                r.setTop(r.top() + offset.y());
                if (symmetryMod) {
                    r.setBottom(r.bottom() - offset.y());
                }
            }
            if (m_mouseOverHandle == sw::TOPLEFT_SIDE
                    || m_mouseOverHandle == sw::LEFT_SIDE
                    || m_mouseOverHandle == sw::BOTTONLEFT_SIDE)
            { // dragging one of the left handles
                r.setLeft(r.left() + offset.x());
                if (symmetryMod) {
                    r.setRight(r.right() - offset.x());
                }
            }
            if (m_mouseOverHandle == sw::BOTTONLEFT_SIDE
                    || m_mouseOverHandle == sw::BOTTON_SIDE
                    || m_mouseOverHandle == sw::BOTTONRIGHT_SIDE)
            { // dragging one of the bottom handles
                r.setBottom(r.bottom() + offset.y());
                if (symmetryMod) {
                    r.setTop(r.top() - offset.y());
                }
            }
            if (m_mouseOverHandle == sw::TOPRIGHT_SIDE
                    || m_mouseOverHandle == sw::RIGHT_SIDE
                    || m_mouseOverHandle == sw::BOTTONRIGHT_SIDE)
            { // dragging one of the right handles
                r.setRight(r.right() + offset.x());
                if (symmetryMod) {
                    r.setLeft(r.left() - offset.x());
                }
            }
            m_selection->setGeometry(r.intersected(rect()).normalized());
            update();
        }
    } else if (m_mouseIsClicked && m_activeTool) {
        // drawing with a tool
        if (m_adjustmentButtonPressed) {
            m_activeTool->drawMoveWithAdjustment(e->pos());
        } else {
            m_activeTool->drawMove(e->pos());
        }
        update();
        // Hides the buttons under the mouse. If the mouse leaves, it shows them.
        if (m_buttonHandler->buttonsAreInside()) {
            const bool containsMouse = m_buttonHandler->contains(m_context.mousePos);
            if (containsMouse) {
                m_buttonHandler->hide();
            } else {
                m_buttonHandler->show();
            }
        }
    } else if (m_activeButton && m_activeButton->tool()->showMousePreview()) {
        update();
    } else {
        if (!m_selection->isVisible()) {
            return;
        }
        m_mouseOverHandle = m_selection->getMouseSide(m_context.mousePos);
        updateCursor();
    }
}

void CaptureWidget::mouseReleaseEvent(QMouseEvent *e) {
    if (e->button() == Qt::RightButton) {
        m_colorPicker->hide();
        m_rightClick = false;
    // when we end the drawing we have to register the last  point and
    //add the temp modification to the list of modifications
    } else if (m_mouseIsClicked && m_activeTool) {
       // font_color->hide();
        m_activeTool->drawEnd(m_context.mousePos);
        if (m_activeTool->isValid()) {
            pushToolToStack();
        } else if (!m_toolWidget){
            m_activeTool->deleteLater();
            m_activeTool = nullptr;
        }
    }
    // Show the buttons after the resize of the selection or the creation
    // of a new one.
    if (!m_buttonHandler->isVisible() && m_selection->isVisible()) {
        // Don't go outside
        QRect newGeometry = m_selection->geometry().intersected(rect());
        // normalize
        if (newGeometry.width() <= 0) {
            int left = newGeometry.left();
            newGeometry.setLeft(newGeometry.right());
            newGeometry.setRight(left);
        }
        if (newGeometry.height() <= 0) {
            int top = newGeometry.top();
            newGeometry.setTop(newGeometry.bottom());
            newGeometry.setBottom(top);
        }
        m_selection->setGeometry(newGeometry);
        m_context.selection = extendedRect(&newGeometry);
        updateSizeIndicator();
        m_buttonHandler->updatePosition(newGeometry);
        m_buttonHandler->show();
    }
    m_mouseIsClicked = false;
    m_newSelection = false;
    m_grabbing = false;

    updateCursor();
}

void CaptureWidget::keyPressEvent(QKeyEvent *e) {
    if (!m_selection->isVisible()) {
        return;
    } else if (e->key() == Qt::Key_Up
               && m_selection->geometry().top() > rect().top()) {
        m_selection->move(QPoint(m_selection->x(), m_selection->y() -1));
        QRect newGeometry = m_selection->geometry().intersected(rect());
        m_context.selection = extendedRect(&newGeometry);
        m_buttonHandler->updatePosition(m_selection->geometry());
        update();
    } else if (e->key() == Qt::Key_Down
               && m_selection->geometry().bottom() < rect().bottom()) {
        m_selection->move(QPoint(m_selection->x(), m_selection->y() +1));
        QRect newGeometry = m_selection->geometry().intersected(rect());
        m_context.selection = extendedRect(&newGeometry);
        m_buttonHandler->updatePosition(m_selection->geometry());
        update();
    } else if (e->key() == Qt::Key_Left
               && m_selection->geometry().left() > rect().left()) {
        m_selection->move(QPoint(m_selection->x() -1, m_selection->y()));
        m_buttonHandler->updatePosition(m_selection->geometry());
        update();
    } else if (e->key() == Qt::Key_Right
               && m_selection->geometry().right() < rect().right()) {
        m_selection->move(QPoint(m_selection->x() +1, m_selection->y()));
        QRect newGeometry = m_selection->geometry().intersected(rect());
        m_context.selection = extendedRect(&newGeometry);
        m_buttonHandler->updatePosition(m_selection->geometry());
        update();
    } else if (e->key() == Qt::Key_Control) {
        m_adjustmentButtonPressed = true;
    }
}

void CaptureWidget::keyReleaseEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Control) {
        m_adjustmentButtonPressed = false;
    }
}

void CaptureWidget::wheelEvent(QWheelEvent *e) {
    m_context.thickness += e->delta() / 120;
    m_context.thickness = qBound(0, m_context.thickness, 100);
    QPoint topLeft = qApp->desktop()->screenGeometry(
                qApp->desktop()->screenNumber(QCursor::pos())).topLeft();
    int offset = m_notifierBox->width() / 4;
    m_notifierBox->move(mapFromGlobal(topLeft) + QPoint(offset, offset));
    m_notifierBox->showMessage(QString::number(m_context.thickness));
    if (m_activeButton && m_activeButton->tool()->showMousePreview()) {
        update();
    }
    emit thicknessChanged(m_context.thickness);
}

void CaptureWidget::resizeEvent(QResizeEvent *e) {
    QWidget::resizeEvent(e);
    m_context.widgetDimensions = rect();
    m_context.widgetOffset = mapToGlobal(QPoint(0,0));
    m_panel->setFixedHeight(height());
    if (!m_context.fullscreen) {
        m_buttonHandler->updateScreenRegions(rect());
    }
}

void CaptureWidget::moveEvent(QMoveEvent *e) {
    QWidget::moveEvent(e);
    m_context.widgetOffset = mapToGlobal(QPoint(0,0));
}

void CaptureWidget::initContext(const QString &savePath, bool fullscreen) {
    m_context.widgetDimensions = rect();
    m_context.color = m_config.drawColorValue();
    m_context.savePath = savePath;
    m_context.widgetOffset = mapToGlobal(QPoint(0,0));
    m_context.mousePos= mapFromGlobal(QCursor::pos());
    m_context.thickness = m_config.drawThicknessValue();
    m_context.fullscreen = fullscreen;
    m_context.font_type = QFont("方正黑体");
    m_context.bold = false;
    m_context.italic = false;
    m_context.underline = false;
    m_context.deleteline =  false;
}

void CaptureWidget::initPanel() {
    m_panel = new UtilityPanel(this);
    makeChild(m_panel);
    QRect panelRect = rect();
    if (m_context.fullscreen) {
        panelRect = QGuiApplication::primaryScreen()->geometry();
    }
    panelRect.moveTo(mapFromGlobal(panelRect.topLeft()));
    panelRect.setWidth(m_colorPicker->width() * 3);
    m_panel->setGeometry(panelRect);

    SidePanelWidget *sidePanel =
            new SidePanelWidget(&m_context.screenshot);
    connect(sidePanel, &SidePanelWidget::colorChanged,
            this, &CaptureWidget::setDrawColor);
    connect(sidePanel, &SidePanelWidget::thicknessChanged,
            this, &CaptureWidget::setDrawThickness);
    connect(this, &CaptureWidget::colorChanged,
            sidePanel, &SidePanelWidget::updateColor);
    connect(this, &CaptureWidget::thicknessChanged,
            sidePanel, &SidePanelWidget::updateThickness);
    connect(sidePanel, &SidePanelWidget::togglePanel,
            m_panel, &UtilityPanel::toggle);     
    sidePanel->colorChanged(m_context.color);
    sidePanel->thicknessChanged(m_context.thickness);
    m_panel->pushWidget(sidePanel);
    m_panel->pushWidget(new QUndoView(&m_undoStack, this));
}

void CaptureWidget::initSelection() {
    m_selection = new SelectionWidget(m_uiColor, this);
    connect(m_selection, &SelectionWidget::animationEnded, this, [this](){
        this->m_buttonHandler->updatePosition(this->m_selection->geometry());
    });
    m_selection->setVisible(false);
    m_selection->setGeometry(QRect());
}
void CaptureWidget::setState(CaptureButton *b) {
    if (!b) {
        return;
    }
    if (m_toolWidget) {
        m_toolWidget->deleteLater();
        if (m_activeTool->isValid()) {
            b->setIcon(b->tool()->icon(m_contrastUiColor,false));
            font_color->hide();
            font_color2->hide();
            pushToolToStack();
            qDebug()<<"111111111";
        }
    }
    if (m_activeButton != b) {
        if (b->y()>m_selection->y())
        {
            if(b->tool()->name()=="Options")
            {
                if (b->y()+175 <= QGuiApplication::primaryScreen()->geometry().height())
                {
                    font_color_point->setX(b->x()-80);
                    font_color_point->setY(b->y()+30);
                    b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                    save_location->move(font_color_point->x(),font_color_point->y());
                    save_location->show();
                    connect(save_location->SaveDir,SIGNAL(pressed()),this,SLOT(ClickedSavedir()));
                    qDebug()<<"qqqqqqqq";
                }
                else
                {
                    font_color_point->setX(b->x()-80);
                    font_color_point->setY(b->y()-175);
                    b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                    save_location2->move(font_color_point->x(),font_color_point->y());
                    save_location2->show();
                    connect(save_location2->SaveDir,SIGNAL(pressed()),this,SLOT(ClickedSavedir2()));
                    qDebug()<<"qqqqqqqq";
                }
            }
            else if(b->tool()->name()=="text")
            {
                if (b->y()+175 <= QGuiApplication::primaryScreen()->geometry().height())
                {
                    save_location2->hide();
                    save_location->hide();
                    font_color->hide();
                    font_color_point->setX(b->x()-80);
                    font_color_point->setY(b->y()+30);
                    b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                    font_options->move(font_color_point->x(),font_color_point->y());
                    font_options_defult();
                    font_options->show();

                }
                else
                {
                    save_location2->hide();
                    save_location->hide();
                    font_color->hide();
                    font_color_point->setX(b->x()-80);
                    font_color_point->setY(b->y()-175);
                    b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                    font_options2->move(font_color_point->x(),font_color_point->y());
                    font_options2_defult();
                    font_options2->show();
                }
            }
            else  if (b->tool()->name() != "Cut" && b->tool()->name() != "undo")
            {
                if (b->y()+150 <= QGuiApplication::primaryScreen()->geometry().height())
                {
                    save_location2->hide();
                    save_location->hide();
                    font_color->hide();
                    font_color_point->setX(b->x()-80);
                    font_color_point->setY(b->y()+30);
                    b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                    font_color->move(font_color_point->x(),font_color_point->y());
                    fontsize_color_chose_default();
                    font_color->show();
                }
                else
                {
                    save_location2->hide();
                    save_location->hide();
                    font_color->hide();
                    font_color_point->setX(b->x()-80);
                    font_color_point->setY(b->y()-150);
                    b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                    font_color2->move(font_color_point->x(),font_color_point->y());
                    fontsize_color_chose2_default();
                    font_color2->show();
                }
            }
         }
        else {
            if(b->tool()->name()=="Options")
            {
                if (b->y()-165 >= 0)
                {
                    font_color_point->setX(b->x()-80);
                    font_color_point->setY(b->y()-175);
                    b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                    save_location2->move(font_color_point->x(),font_color_point->y());
                    save_location2->show();
                    connect(save_location2->SaveDir,SIGNAL(pressed()),this,SLOT(ClickedSavedir2()));
                    qDebug()<<"qqqqqqqq";
                }
               else
                {
                    font_color_point->setX(b->x()-80);
                    font_color_point->setY(b->y()+30);
                    b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                    save_location->move(font_color_point->x(),font_color_point->y());
                    save_location->show();
                    connect(save_location->SaveDir,SIGNAL(pressed()),this,SLOT(ClickedSavedir()));
                    qDebug()<<"qqqqqqqq";
                 }
            }
            else if(b->tool()->name()=="text")
            {
                if (b->y()- 165 >= 0)
                {
                    save_location2->hide();
                    save_location->hide();
                    font_color->hide();
                    font_color_point->setX(b->x()-80);
                    font_color_point->setY(b->y()-175);
                    b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                    font_options2->move(font_color_point->x(),font_color_point->y());
                    font_options2_defult();
                    font_options2->show();
                }
                else
                {
                    save_location2->hide();
                    save_location->hide();
                    font_color->hide();
                    font_color_point->setX(b->x()-80);
                    font_color_point->setY(b->y()+30);
                    b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                    font_options->move(font_color_point->x(),font_color_point->y());
                    font_options_defult();
                    font_options->show();
                //connect(save_location->SaveDir,SIGNAL(pressed()),this,SLOT(ClickedSavedir()));
                    qDebug()<<"aaaaaaaaaaaaa2222";
                }
            }
            else  if (b->tool()->name() != "Cut" && b->tool()->name() != "undo")
            {
                if (b->y()-150 >= 0)
                {
                    save_location->hide();
                    save_location2->hide();
                    font_color_point->setX(b->x()-80);
                    font_color_point->setY(b->y()-150);
                    b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                    font_color2->move(font_color_point->x(),font_color_point->y());
                    fontsize_color_chose2_default();
                    font_color2->show();
                }
                else
                {
                    save_location->hide();
                    save_location2->hide();
                    font_color_point->setX(b->x()-80);
                    font_color_point->setY(b->y()+30);
                    b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                    font_color->move(font_color_point->x(),font_color_point->y());
                    fontsize_color_chose2_default();
                    font_color->show();
                }
            }
        }
        processTool(b->tool());
    }
    // Only close activated from button
    if (b->tool()->closeOnButtonPressed()) {
        b->setIcon(b->tool()->icon(m_contrastUiColor,false));
        save_location->hide();
        save_location2->hide();
        font_color->hide();
        font_color2->hide();
        close();
    }
    if (b->tool()->isSelectable()) {
        if (m_activeButton != b) {
            QWidget *confW = b->tool()->configurationWidget();
            m_panel->addToolWidget(confW);
            if (m_activeButton) {
                m_activeButton->setColor(m_uiColor);
                m_activeButton->setIcon(m_activeButton->tool()->icon(m_uiColor,false));
                save_location->hide();
                save_location2->hide();
                font_color2->hide();
                font_color->hide();
                font_options->hide();
                font_options2->hide();
                if (b->y()>m_selection->y())
                {
                    if(b->tool()->name()=="Options")
                    {
                        if (b->y()+175 <= QGuiApplication::primaryScreen()->geometry().height())
                        {
                            font_color_point->setX(b->x()-80);
                            font_color_point->setY(b->y()+30);
                            b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                            save_location->move(font_color_point->x(),font_color_point->y());
                            save_location->show();
                            connect(save_location->SaveDir,SIGNAL(pressed()),this,SLOT(ClickedSavedir()));
                            qDebug()<<"qqqqqqqq";
                        }
                        else
                        {
                            font_color_point->setX(b->x()-80);
                            font_color_point->setY(b->y()-175);
                            b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                            save_location2->move(font_color_point->x(),font_color_point->y());
                            save_location2->show();
                            connect(save_location2->SaveDir,SIGNAL(pressed()),this,SLOT(ClickedSavedir2()));
                            qDebug()<<"qqqqqqqq";
                        }
                    }
                    else if(b->tool()->name()=="text")
                    {
                        if (b->y()+175 <= QGuiApplication::primaryScreen()->geometry().height())
                        {
                            save_location2->hide();
                            save_location->hide();
                            font_color->hide();
                            font_color_point->setX(b->x()-80);
                            font_color_point->setY(b->y()+30);
                            b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                            font_options->move(font_color_point->x(),font_color_point->y());
                            font_options_defult();
                            font_options->show();
                        }
                        else
                        {
                            save_location2->hide();
                            save_location->hide();
                            font_color->hide();
                            font_color_point->setX(b->x()-80);
                            font_color_point->setY(b->y()-175);
                            b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                            font_options2->move(font_color_point->x(),font_color_point->y());
                            font_options2_defult();
                            font_options2->show();
                            qDebug()<<"aaaaaaaaaaaaa2222";
                        }
                    }
                    else  if (b->tool()->name() != "Cut" && b->tool()->name() != "undo")
                    {
                        if (b->y()+150 <= QGuiApplication::primaryScreen()->geometry().height())
                        {
                            save_location2->hide();
                            save_location->hide();
                            font_color->hide();
                            font_color_point->setX(b->x()-80);
                            font_color_point->setY(b->y()+30);
                            b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                            font_color->move(font_color_point->x(),font_color_point->y());
                            fontsize_color_chose_default();
                            font_color->show();
                        }
                        else
                        {
                            save_location2->hide();
                            save_location->hide();
                            font_color->hide();
                            font_color_point->setX(b->x()-80);
                            font_color_point->setY(b->y()-150);
                            b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                            font_color2->move(font_color_point->x(),font_color_point->y());
                            fontsize_color_chose2_default();
                            font_color2->show();
                        }
                    }
                 }
                else {
                    if(b->tool()->name()=="Options")
                    {
                        if (b->y()-165 >= 0)
                        {
                            font_color_point->setX(b->x()-80);
                            font_color_point->setY(b->y()-175);
                            b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                            save_location2->move(font_color_point->x(),font_color_point->y());
                            save_location2->show();
                            connect(save_location2->SaveDir,SIGNAL(pressed()),this,SLOT(ClickedSavedir2()));
                            qDebug()<<"qqqqqqqq";
                        }
                       else
                        {
                            font_color_point->setX(b->x()-80);
                            font_color_point->setY(b->y()+30);
                            b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                            save_location->move(font_color_point->x(),font_color_point->y());
                            save_location->show();
                            connect(save_location->SaveDir,SIGNAL(pressed()),this,SLOT(ClickedSavedir()));
                            qDebug()<<"qqqqqqqq";
                         }
                    }
                    else if(b->tool()->name()=="text")
                    {
                        if (b->y()- 165 >= 0)
                        {
                            save_location2->hide();
                            save_location->hide();
                            font_color->hide();
                            font_color_point->setX(b->x()-80);
                            font_color_point->setY(b->y()-175);
                            b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                            font_options2->move(font_color_point->x(),font_color_point->y());
                            font_options2_defult();
                            font_options2->show();

                            qDebug()<<"aaaaaaaaaaaaa2222";
                        }
                        else
                        {
                            save_location2->hide();
                            save_location->hide();
                            font_color->hide();
                            font_color_point->setX(b->x()-80);
                            font_color_point->setY(b->y()+30);
                            b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                            font_options->move(font_color_point->x(),font_color_point->y());
                            font_options2_defult();
                            font_options->show();
                        //connect(save_location->SaveDir,SIGNAL(pressed()),this,SLOT(ClickedSavedir()));
                            qDebug()<<"aaaaaaaaaaaaa2222";
                        }
                    }
                    else  if (b->tool()->name() != "Cut" && b->tool()->name() != "undo")
                    {
                        if (b->y()-150 >= 0)
                        {
                            save_location->hide();
                            save_location2->hide();
                            font_color_point->setX(b->x()-80);
                            font_color_point->setY(b->y()-150);
                            b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                            font_color2->move(font_color_point->x(),font_color_point->y());
                            fontsize_color_chose2_default();
                            font_color2->show();
                        }
                        else
                        {
                            save_location->hide();
                            save_location2->hide();
                            font_color_point->setX(b->x()-80);
                            font_color_point->setY(b->y()+30);
                            b->setIcon(b->tool()->icon(m_contrastUiColor,true));
                            font_color->move(font_color_point->x(),font_color_point->y());
                            fontsize_color_chose_default();
                            font_color->show();
                        }
                    }
                }
                 }
                 m_activeButton = b;
                 m_activeButton->setColor(m_contrastUiColor);
             }
        else if (m_activeButton) {
                 m_panel->clearToolWidget();
                 m_activeButton->setColor(m_uiColor);
                 b->setIcon(b->tool()->icon(m_uiColor,false));
                 font_color->hide();
                 save_location2->hide();
                 font_color2->hide();
                 save_location->hide();
                 font_options->hide();
                 font_options2->hide();
                 m_activeButton = nullptr;
             }
             update(); // clear mouse preview
         }
     }

     void CaptureWidget::processTool(CaptureTool *t) {
         auto backup = m_activeTool;
         // The tool is active during the pressed().
         m_activeTool = t;
         t->pressed(m_context);
         m_activeTool = backup;
     }

     void CaptureWidget::handleButtonSignal(CaptureTool::Request r) {
         switch (r) {
         case CaptureTool::REQ_CLEAR_MODIFICATIONS:
             m_undoStack.setIndex(0);
             update();
             break;
         case CaptureTool::REQ_CLOSE_GUI:
             close();
             break;
         case CaptureTool::REQ_HIDE_GUI:
             hide();
             break;
         case CaptureTool::REQ_HIDE_SELECTION:
             m_newSelection = true;
             m_selection->setVisible(false);
             updateCursor();
             break;
         case CaptureTool::REQ_SELECT_ALL:
             m_selection->setGeometryAnimated(rect());
             break;
         case CaptureTool::REQ_UNDO_MODIFICATION:
             m_undoStack.undo();
             break;
         case CaptureTool::REQ_REDO_MODIFICATION:
             m_undoStack.redo();
             break;
         case CaptureTool::REQ_REDRAW:
             update();
             break;
         case CaptureTool::REQ_TOGGLE_SIDEBAR:
             m_panel->toggle();
             break;
         case CaptureTool::REQ_SHOW_COLOR_PICKER:
             // TODO
             break;
         case CaptureTool::REQ_MOVE_MODE:
             setState(m_activeButton); // Disable the actual button
             break;
         case CaptureTool::REQ_CAPTURE_DONE_OK:
             m_captureDone = true;
             break;
         case CaptureTool::REQ_CUT:
             //m_captureDone =false;
             //update();
             break;
         case CaptureTool::REQ_LUPING:
             //m_captureDone = true;
             hide_window();
             screenCap->show();
             break;
         case CaptureTool::REQ_OPTIONS:
             //update();
             break;
         case CaptureTool::REQ_ADD_CHILD_WIDGET:
             if (!m_activeTool) {
                 break;
             }
             if (m_toolWidget) {
                 m_toolWidget->deleteLater();
             }
             m_toolWidget = m_activeTool->widget();
             if (m_toolWidget) {
                 makeChild(m_toolWidget);
                 m_toolWidget->move(m_context.mousePos);
                 m_toolWidget->show();
                 m_toolWidget->setFocus();
             }
             break;
         case CaptureTool::REQ_ADD_CHILD_WINDOW:
             if (!m_activeTool) {
                 break;
             } else {
                 QWidget *w = m_activeTool->widget();
                 connect(this, &CaptureWidget::destroyed, w, &QWidget::deleteLater);
                 w->show();
             }
             break;
         case CaptureTool::REQ_ADD_EXTERNAL_WIDGETS:
             if (!m_activeTool) {
                 break;
             } else {
                 QWidget *w = m_activeTool->widget();
                 w->setAttribute(Qt::WA_DeleteOnClose);
                 w->show();
             }
             break;
         default:
             break;
         }
     }

     void CaptureWidget::setDrawColor(const QColor &c) {
         m_context.color = c;
         ConfigHandler().setDrawColor(m_context.color);
         emit colorChanged(c);
     }

     void CaptureWidget::setDrawThickness(const int &t)
     {
         m_context.thickness = qBound(0, t, 100);
         ConfigHandler().setdrawThickness(m_context.thickness);
         emit thicknessChanged(m_context.thickness);
     }

     void CaptureWidget::leftResize() {
         if (m_selection->isVisible() && m_selection->geometry().right() > m_selection->geometry().left()) {
             m_selection->setGeometry(m_selection->geometry() + QMargins(0, 0, -1, 0));
             QRect newGeometry = m_selection->geometry().intersected(rect());
             m_context.selection = extendedRect(&newGeometry);
             m_buttonHandler->updatePosition(m_selection->geometry());
             updateSizeIndicator();
             update();
         }
     }

     void CaptureWidget::rightResize() {
         if (m_selection->isVisible() && m_selection->geometry().right() < rect().right()) {
             m_selection->setGeometry(m_selection->geometry() + QMargins(0, 0, 1, 0));
             QRect newGeometry = m_selection->geometry().intersected(rect());
             m_context.selection = extendedRect(&newGeometry);
             m_buttonHandler->updatePosition(m_selection->geometry());
             updateSizeIndicator();
             update();
         }
     }

     void CaptureWidget::upResize() {
         if (m_selection->isVisible() && m_selection->geometry().bottom() > m_selection->geometry().top()) {
             m_selection->setGeometry(m_selection->geometry() + QMargins(0, 0, 0, -1));
             QRect newGeometry = m_selection->geometry().intersected(rect());
             m_context.selection = extendedRect(&newGeometry);
             m_buttonHandler->updatePosition(m_selection->geometry());
             updateSizeIndicator();
             update();
         }
     }

     void CaptureWidget::downResize() {
         if (m_selection->isVisible() && m_selection->geometry().bottom() < rect().bottom()) {
             m_selection->setGeometry(m_selection->geometry() + QMargins(0, 0, 0, 1));
             QRect newGeometry = m_selection->geometry().intersected(rect());
             m_context.selection = extendedRect(&newGeometry);
             m_buttonHandler->updatePosition(m_selection->geometry());
             updateSizeIndicator();
             update();
         }
     }
     void CaptureWidget::initShortcuts() {
         new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this, SLOT(close()));
         new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_S), this, SLOT(saveScreenshot()));
         new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_C), this, SLOT(copyScreenshot()));
         new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Z), this, SLOT(undo()));
         new QShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z), this, SLOT(redo()));
         new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Right), this, SLOT(rightResize()));
         new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Left), this, SLOT(leftResize()));
         new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Up), this, SLOT(upResize()));
         new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Down), this, SLOT(downResize()));
         new QShortcut(Qt::Key_Space, this, SLOT(togglePanel()));
         new QShortcut(Qt::Key_Escape, this, SLOT(deleteToolwidgetOrClose()));
         new QShortcut(Qt::Key_Return, this, SLOT(copyScreenshot()));
         new QShortcut(Qt::Key_Enter, this, SLOT(copyScreenshot()));
     }

     void CaptureWidget::updateSizeIndicator() {
         if (m_sizeIndButton){
             const QRect &selection = extendedSelection();
             //m_sizeIndButton->setText(QString(" 选项 "));
             //m_sizeIndButton->resize(90,50);
         }
     }

     void CaptureWidget::updateCursor() {
         if (m_rightClick) {
             setCursor(Qt::ArrowCursor);
         } else if (m_grabbing) {
             setCursor(Qt::ClosedHandCursor);
         } else if (!m_activeButton) {
             using sw = SelectionWidget;
             if (m_mouseOverHandle != sw::NO_SIDE){
                 // cursor on the handlers
                 switch (m_mouseOverHandle) {
                 case sw::TOPLEFT_SIDE: case sw::BOTTONRIGHT_SIDE:
                     setCursor(Qt::SizeFDiagCursor);
                     break;
                 case sw::TOPRIGHT_SIDE: case sw::BOTTONLEFT_SIDE:
                     setCursor(Qt::SizeBDiagCursor);
                     break;
                 case sw::LEFT_SIDE: case sw::RIGHT_SIDE:
                     setCursor(Qt::SizeHorCursor);
                     break;
                 case sw::TOP_SIDE: case sw::BOTTON_SIDE:
                     setCursor(Qt::SizeVerCursor);
                     break;
                 default:
                     break;
                 }
             } else if (m_selection->isVisible() &&
                        m_selection->geometry().contains(m_context.mousePos))
             {
                 setCursor(Qt::OpenHandCursor);
             } else {
                 setCursor(Qt::CrossCursor);
             }
         } else {
             setCursor(Qt::CrossCursor);
         }
     }

     void CaptureWidget::pushToolToStack() {
         auto mod = new ModificationCommand(
                     &m_context.screenshot, m_activeTool);
         disconnect(this, &CaptureWidget::colorChanged,
                    m_activeTool, &CaptureTool::colorChanged);
         disconnect(this, &CaptureWidget::thicknessChanged,
                    m_activeTool, &CaptureTool::thicknessChanged);
         if (m_panel->toolWidget()) {
             disconnect(m_panel->toolWidget(), nullptr, m_activeTool, nullptr);
         }
         m_undoStack.push(mod);
         m_activeTool = nullptr;
     }

     void CaptureWidget::makeChild(QWidget *w) {
         w->setParent(this);
         w->installEventFilter(m_eventFilter);
     }

     void CaptureWidget::togglePanel() {
         m_panel->toggle();
     }

     void CaptureWidget::childEnter() {
         m_previewEnabled = false;
         update();
     }

     void CaptureWidget::childLeave() {
         m_previewEnabled = true;
         update();
     }

     void CaptureWidget::copyScreenshot() {
         m_captureDone = true;
         ScreenshotSaver().saveToClipboard(pixmap());
         close();
     }

     void CaptureWidget::saveScreenshot() {
         m_captureDone = true;
         hide();
         if (m_context.savePath.isEmpty()) {
             ScreenshotSaver().saveToFilesystemGUI(pixmap());
         } else {
             ScreenshotSaver().saveToFilesystem(pixmap(), m_context.savePath);
         }
         close();
     }

     void CaptureWidget::undo() {
         m_undoStack.undo();
     }

     void CaptureWidget::redo() {
         m_undoStack.redo();
     }

     QRect CaptureWidget::extendedSelection() const {
         if (!m_selection->isVisible())
             return QRect();
         QRect r = m_selection->geometry();
         return extendedRect(&r);
     }

     QRect CaptureWidget::extendedRect(QRect *r) const {
         auto devicePixelRatio = m_context.screenshot.devicePixelRatio();
         return QRect(r->left()   * devicePixelRatio,
                      r->top()    * devicePixelRatio,
                      r->width()  * devicePixelRatio,
                      r->height() * devicePixelRatio);
     }

     void CaptureWidget::ClickedSavedir()
     {
         QStringList a = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
         hide_window();

         file = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                  a.at(0),
                                                  QFileDialog::ShowDirsOnly
                                                  | QFileDialog::DontResolveSymlinks);
         if (file != nullptr)
         {
            qDebug()<<"asbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
            if (save_location->png->isChecked())
            {
                m_context.saveType =".png";
            }
            else if(save_location->bmp->isChecked())
            {
                save_location->png->setChecked(false);
                m_context.saveType =".bmp";
            }
            else if(save_location->jpg->isChecked())
            {
                save_location->png->setChecked(false);
                m_context.saveType =".jpg";
            }
            save_location->SaveDir->setText(file);
            m_context.savePath = file;
         }
         else
         {
            qDebug()<<"asccccccccccccccccccccccccccccccc";
            save_location->SaveDir->setText(a.at(0));
            m_context.savePath = a.at(0);
            m_context.saveType =".png";
         }

        show_window();
      }
     void CaptureWidget::ClickedSavedir2()
     {
         QStringList a = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
         hide_window();
         qDebug()<<"asaaaaaaaaaaaaaaaaaaaaaaaaa";
         file = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                  a.at(0),
                                                  QFileDialog::ShowDirsOnly
                                                  | QFileDialog::DontResolveSymlinks);
         if (file != nullptr)
         {
            qDebug()<<"asbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
            save_location2->SaveDir->setText(file);
            if (save_location2->png->isChecked())
            {
                m_context.saveType =".png";
                qDebug()<<"png  is clicked";
            }
            else if(save_location2->bmp->isChecked())
            {
                save_location2->png->setChecked(false);
                m_context.saveType =".bmp";
                qDebug()<<"bmp  is clicked";
            }
            else if(save_location2->jpg->isChecked())
            {
                save_location2->png->setChecked(false);
                m_context.saveType =".jpg";
            }
            m_context.savePath = file;
         }
         else
         {
            qDebug()<<"asccccccccccccccccccccccccccccccc";
            save_location2->SaveDir->setText(a.at(0));
            m_context.savePath = a.at(0);
            m_context.saveType =".png";
            qDebug()<<"default is png";
         }
            show_window();
      }

     void CaptureWidget::font_type_changed(QFont f)
     {
         m_context.font_type = f;
     }

     void  CaptureWidget::font_bold_clicked(bool b)
     {
         m_context.bold = b;
     }
     void  CaptureWidget::font_delete_clicked(bool b)
     {
         m_context.deleteline = b;
     }
     void CaptureWidget::font_underline_clicked(bool b)
     {
         m_context.underline =b;

     }
     void CaptureWidget::font_italic_clicked(bool b)
     {
         m_context.italic =b;
     }
     void CaptureWidget::ClickedSaveType()
     {
         if (save_location->png->isChecked())
         {
             m_context.saveType =".png";
         }
         else if(save_location->bmp->isChecked())
         {
             save_location->png->setChecked(false);
             m_context.saveType =".bmp";
         }
         else if(save_location->jpg->isChecked())
         {
             save_location->png->setChecked(false);
             m_context.saveType =".jpg";
         }
     }
     void CaptureWidget::ClickedSaveType2()
     {
         if (save_location2->png->isChecked())
         {
             m_context.saveType =".png";
         }
         else if(save_location2->bmp->isChecked())
         {
             save_location2->png->setChecked(false);
             m_context.saveType =".bmp";
         }
         else if(save_location2->jpg->isChecked())
         {
             save_location2->png->setChecked(false);
             m_context.saveType =".jpg";
         }
     }

     void CaptureWidget::font_options_defult()
     {
         font_options->Font_size->setValue(m_context.thickness);
         font_options->color = m_context.color;
         font_options->Underline = m_context.underline;
         font_options->italic = m_context.italic;
         font_options->bold = m_context.bold;
         font_options->Delete = m_context.deleteline;
     }
     void CaptureWidget::font_options2_defult()
     {
         font_options2->Font_size->setValue(m_context.thickness);
         font_options2->color = m_context.color;
         font_options2->Underline = m_context.underline;
         font_options2->italic = m_context.italic;
         font_options2->bold = m_context.bold;
         font_options2->Delete = m_context.deleteline;

     }
     void CaptureWidget::fontsize_color_chose_default()
     {
          font_color->color = m_context.color;
          int i = m_context.thickness;
          if (i>= 0 && i < 10)
          {
              font_color->color_rect =font_color->m_colorAreaList.at(0);
          }
          else if (i >= 10 && i<20)
          {
              font_color->color_rect =font_color->m_colorAreaList.at(1);
          }
          else if (i >= 20 && i<30)
          {
              font_color->color_rect =font_color->m_colorAreaList.at(2);
          }
          else if (i >= 30)
          {
              font_color->color_rect =font_color->m_colorAreaList.at(3);
          }
          else
          {
                qDebug()<<"Error";
          }
     }
     void CaptureWidget::fontsize_color_chose2_default()
     {
          font_color2->color = m_context.color;
          int i = m_context.thickness;
          if (i>= 0 && i < 10)
          {
              font_color2->color_rect =font_color2->m_colorAreaList.at(0);
          }
          else if (i >= 10 && i<20)
          {
              font_color2->color_rect =font_color2->m_colorAreaList.at(1);
          }
          else if (i >= 20 && i<30)
          {
              font_color2->color_rect =font_color2->m_colorAreaList.at(2);
          }
          else if (i >= 30)
          {
              font_color2->color_rect =font_color->m_colorAreaList.at(3);
          }
          else
          {
                qDebug()<<"Error";
          }
     }
     void CaptureWidget::hide_window()
         {
             setWindowFlags(Qt::BypassWindowManagerHint
                         | Qt::FramelessWindowHint
                         | Qt::Tool);
            this->hide();
         }
         void CaptureWidget::show_window()
         {
             setWindowFlags(Qt::BypassWindowManagerHint
                            | Qt::WindowStaysOnTopHint
                            | Qt::FramelessWindowHint
                            | Qt::Tool);
             this->show();

         }


