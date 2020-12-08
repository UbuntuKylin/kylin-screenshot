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

// Based on Lightscreen areadialog.cpp, Copyright 2017  Christian Kaiser <info@ckaiser.com.ar>
// released under the GNU GPL2  <https://www.gnu.org/licenses/gpl-2.0.txt>

// Based on KDE's KSnapshot regiongrabber.cpp, revision 796531, Copyright 2007 Luca Gugelmann <lucag@student.ethz.ch>
// released under the GNU LGPL  <http://www.gnu.org/licenses/old-licenses/library.txt>
#include "src/utils/systemnotification.h"
#include "src/utils/filenamehandler.h"


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
#include <QPainterPath>
#include <QMouseEvent>
#include <QBuffer>
#include <QDesktopWidget>
#include <QDebug>
#include <QRegion>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>
#include <QGraphicsBlurEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
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
    setAttribute(Qt::WA_AlwaysShowToolTips );
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
    connect(screenCap->m_pushbutton_video_select_rectangle, &QPushButton::clicked, screenCap, &ScreenRecorder::on_m_pushbutton_video_select_rectangle_clicked);
    connect(screenCap->m_pushbutton_video_select_window, &QPushButton::clicked, screenCap, &ScreenRecorder::on_m_pushbutton_video_select_window_clicked);

    m_colorPicker->hide();
    font_color->setStartPos(95);
    font_color->setTriangleInfo(20, 10);
    font_color->setFixedSize(QSize(187, 67));

    font_color2->setStartPos(95);
    font_color2->setTriangleInfo(20, 10);
    font_color2->setFixedSize(QSize(187, 62));

    save_location->setStartPos(72);
    save_location->setTriangleInfo(20, 10);
    save_location->setFixedSize(QSize(151, 143));
    save_location->setCenterWidget();

    save_location2->setStartPos(72);
    save_location2->setTriangleInfo(20, 10);
    save_location2->setFixedSize(QSize(151, 135));
    save_location2->setCenterWidget();

    font_options->setStartPos(130);
    font_options->setTriangleInfo(20, 10);
    font_options->setFixedSize(QSize(261, 90));
    font_options->setCenterWidget();
    font_options->Font_size->setValue(m_context.thickness);

    font_options2->setStartPos(130);
    font_options2->setTriangleInfo(20, 10);
    font_options2->setFixedSize(QSize(261, 80));
    font_options2->setCenterWidget();

    font_color->hide();
    font_color2->hide();
    m_context.saveType =".png";
    QStringList a = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    m_context.savePath = a.at(0);
    // Init notification widget
    m_notifierBox = new NotifierBox(this);
    m_notifierBox->hide();
    font_color_point = new QPoint();
    connect(&m_undoStack, &QUndoStack::indexChanged,
            this, [this](int){ this->update(); });
    initPanel();
    QFont ft;
    ft.setPointSize(10);
    SaveAs_btn = new  QPushButton(this);
    SaveAs_btn->setFixedSize(90,30);
    SaveAs_btn->setFont(ft);
    SaveAs_btn->setStyleSheet("QPushButton{background-color:rgb(0,98,240)}");
    SaveAs_btn->setText(tr("save as"));

    hide_ChildWindow();
    connect(SaveAs_btn,SIGNAL(pressed()),this,SLOT(ClickedSaveAsFile()));
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
    m_context.style_name = m_context.style_settings->get("style-name").toString();
    painter.drawPixmap(0, 0, m_context.screenshot);
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
    if (m_context.mousePos.x() >= 0 && m_context.mousePos.x()-144 <= qApp->desktop()->screenGeometry().width()
            && m_context.mousePos.y() >= 0 &&m_context.mousePos.y()-144 <= qApp->desktop()->screenGeometry().height())
           {
               if (!m_selection->isVisible()) {
                   painter.setOpacity(0.5);
                   if(m_context.mousePos.y()+144>=qApp->desktop()->screenGeometry().height())
                   {
                          if(m_context.mousePos.x()+144<=qApp->desktop()->screenGeometry().width()){
                                painter.drawPixmap(m_context.mousePos.x()+25,m_context.mousePos.y()-129,crosspixmap);
                                painter.drawText(m_context.mousePos.x()+25,m_context.mousePos.y()-9,tr("%1 , %2")
                                     .arg(m_context.mousePos.x()).arg(m_context.mousePos.y()));
                          }
                          else
                          {
                                  painter.drawPixmap(m_context.mousePos.x()-144,m_context.mousePos.y()-144,crosspixmap);
                                  painter.drawText(m_context.mousePos.x()-144,m_context.mousePos.y()-24,tr("%1 , %2")
                                     .arg(m_context.mousePos.x()).arg(m_context.mousePos.y()));
                          }
                   }
                   else{
                           if(m_context.mousePos.x()+144>=qApp->desktop()->screenGeometry().width()){
                                painter.drawPixmap(m_context.mousePos.x()-144,m_context.mousePos.y()+10,crosspixmap);
                                painter.drawText(m_context.mousePos.x()-144,m_context.mousePos.y()+130,tr("%1 , %2")
                                     .arg(m_context.mousePos.x()).arg(m_context.mousePos.y()));
                          }
                           else{
                                painter.drawPixmap(m_context.mousePos.x()+25,m_context.mousePos.y()+25,crosspixmap);
                                painter.drawText(m_context.mousePos.x()+25,m_context.mousePos.y()+145,tr("%1 , %2")
                                     .arg(m_context.mousePos.x()).arg(m_context.mousePos.y()));
                           }
                   }
               }
               update();
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
    QFont ft;
    ft.setPointSize(10);
    if (m_selection->isVisible()) {
        // paint handlers
        painter.setPen(m_uiColor);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setOpacity(0.5);
        // draw capture size
        if((m_context.style_name.compare("ukui-white")==0) || (m_context.style_name.compare("ukui-default")==0) || (m_context.style_name.compare("ukui-light")==0)){
        painter.setBrush(QColor(195,195,195));
        painter.drawRoundRect((m_selection->geometry().intersected(rect()).x()),m_selection->geometry().intersected(rect()).y()-37,
                              82,24,4,4);
        painter.setPen(Qt::black);
        painter.setFont(ft);
        painter.drawText((m_selection->geometry().intersected(rect()).x()+2),m_selection->geometry().intersected(rect()).y()-19,tr("%1 * %2")
                         .arg(m_selection->geometry().intersected(rect()).width()).arg(m_selection->geometry().intersected(rect()).height()));
        }
        else if((m_context.style_name.compare("ukui-dark")==0) || (m_context.style_name.compare("ukui-black")==0)){
            painter.setBrush(QColor(0,0,0));
            painter.drawRoundRect((m_selection->geometry().intersected(rect()).x()),m_selection->geometry().intersected(rect()).y()-37,
                                  82,24,4,4);
            painter.setPen(Qt::white);
            painter.setFont(ft);
            painter.drawText((m_selection->geometry().intersected(rect()).x()+2),m_selection->geometry().intersected(rect()).y()-19,tr("%1 * %2")
                             .arg(m_selection->geometry().intersected(rect()).width()).arg(m_selection->geometry().intersected(rect()).height()));
        }
        if((vectorButtons.first()->pos().x()>0 && m_buttonHandler->isVisible())){
            if((m_context.style_name.compare("ukui-white")==0) || (m_context.style_name.compare("ukui-default")==0) || (m_context.style_name.compare("ukui-light")==0)){
                painter.setBrush(QColor(200,200,200));
                painter.setPen(QColor(200,200,200));
                //rect
                QRect rr = QRect(vectorButtons.first()->pos().x()-15,vectorButtons.first()->pos().y(),
                             735,44);
                QRect r1 = rr.adjusted(1,1,-1,-1);
                painter.setOpacity(0);
                QPixmap p = m_context.origScreenshot.copy(r1);
                auto pixelRatio = p.devicePixelRatio();

                QRect selection = QRect(r1.topLeft(), r1.bottomRight()).normalized();
                QRect selectionScaled = QRect(r1.topLeft()* pixelRatio, r1.bottomRight()* pixelRatio).normalized();

                QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
                blur->setBlurRadius(10);
                QGraphicsPixmapItem *item = new QGraphicsPixmapItem (
                                          p.copy(selectionScaled));
                item->setGraphicsEffect(blur);

                QGraphicsScene scene;
                scene.addItem(item);
                scene.render(&painter, selection, QRectF());
                blur->setBlurRadius(10);
                scene.render(&painter, selection, QRectF());
                //scene.render(&painter, selection, QRectF());
                painter.setOpacity(0.66);
                painter.drawRoundRect(rr,6,6);
                painter.drawRoundedRect(vectorButtons.last()->pos().x(),
                                    vectorButtons.last()->pos().y(),
                                    GlobalValues::buttonBaseSize(),
                                    GlobalValues::buttonBaseSize(),
                                    GlobalValues::buttonBaseSize()/2,
                                    GlobalValues::buttonBaseSize()/2);

                QPainterPath path;
                QColor color(92,93,95,50);
                painter.setOpacity(0.8);
                QColor rectColor2(QColor(0,98,240));
                painter.setBrush(rectColor2);
                painter.drawRoundRect(vectorButtons.first()->pos().x()+GlobalValues::buttonBaseSize()*15+16,vectorButtons.first()->pos().y()+GlobalValues::buttonBaseSize()/6,90,30,20,20);
                //两个分隔符
                painter.setBrush(QColor(0,0,0,100));
                painter.setPen(QColor(0,0,0,100));
                painter.drawRect(vectorButtons.first()->pos().x()+GlobalValues::buttonBaseSize()*8+38,vectorButtons.first()->pos().y()+14, 1, 16);
                painter.drawRect(vectorButtons.first()->pos().x()+GlobalValues::buttonBaseSize()*12+21,vectorButtons.first()->pos().y()+14, 1,16);
                painter.setBrush(QColor(255,255,255));
                painter.setPen(QColor(255,255,255));
                painter.drawRect(vectorButtons.first()->pos().x()+GlobalValues::buttonBaseSize()*17+11,vectorButtons.first()->pos().y()+14, 1,16);
                painter.setOpacity(0.5);
            }
            else if((m_context.style_name.compare("ukui-dark")==0) || (m_context.style_name.compare("ukui-black")==0)){
                painter.setBrush(QColor(0,0,0));
                painter.setPen(QColor(0,0,0));
                painter.setOpacity(0.66);
                QRect rr = QRect(vectorButtons.first()->pos().x()-15,vectorButtons.first()->pos().y(),
                             735,44);
                QRect r1 = rr.adjusted(2,2,-2,-2);
                QPixmap p = m_context.origScreenshot.copy(r1);
                auto pixelRatio = p.devicePixelRatio();

                QRect selection = QRect(r1.topLeft(), r1.bottomRight()).normalized();
                QRect selectionScaled = QRect(r1.topLeft()* pixelRatio, r1.bottomRight()* pixelRatio).normalized();

                QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
                blur->setBlurRadius(10);
                QGraphicsPixmapItem *item = new QGraphicsPixmapItem (
                                          p.copy(selectionScaled));
                item->setGraphicsEffect(blur);

                QGraphicsScene scene;
                scene.addItem(item);

                scene.render(&painter, selection, QRectF());
                blur->setBlurRadius(12);
                scene.render(&painter, selection, QRectF());
                scene.render(&painter, selection, QRectF());
                painter.drawRoundRect(rr,6,6);
                painter.drawRoundedRect(vectorButtons.last()->pos().x(),
                                    vectorButtons.last()->pos().y(),
                                    GlobalValues::buttonBaseSize(),
                                    GlobalValues::buttonBaseSize(),
                                    GlobalValues::buttonBaseSize()/2,
                                    GlobalValues::buttonBaseSize()/2);

                QPainterPath path;
                QColor color(92,93,95,50);
                painter.setOpacity(0.8);
                QColor rectColor2(QColor(0,98,240));
                painter.setBrush(rectColor2);
                painter.drawRoundRect(vectorButtons.first()->pos().x()+GlobalValues::buttonBaseSize()*15+16,vectorButtons.first()->pos().y()+GlobalValues::buttonBaseSize()/6,90,30,20,20);

                //两个分隔符
                painter.setBrush(QColor(0,0,0,100));
                painter.setPen(QColor(0,0,0,100));
                painter.drawRect(vectorButtons.first()->pos().x()+GlobalValues::buttonBaseSize()*8+38,vectorButtons.first()->pos().y()+14, 1, 16);
                painter.drawRect(vectorButtons.first()->pos().x()+GlobalValues::buttonBaseSize()*12+21,vectorButtons.first()->pos().y()+14, 1,16);
                painter.drawRect(vectorButtons.first()->pos().x()+GlobalValues::buttonBaseSize()*17+11,vectorButtons.first()->pos().y()+14, 1,16);
                painter.setOpacity(0.5);
            }
        }
        update();
        painter.setBrush(QColor(160,160,160));
        for(auto r: m_selection->handlerAreas()) {
            painter.drawRoundRect(r, 80, 80);
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
    w = 26;
    h = 26;
   //mypixmap = mypixmap.grabWidget(this,e->pos().x()-w/2-1,e->pos().y()-h/2-1,w,h);
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
    for (int i=0;i<1;i++)
    {
        for(int j=0;j<h;j++)
        {
           crosstmp.setPixel(i,j,value);
           crosstmp.setPixel(j,i,value);
        }
    }
    for (int i=w-1;i<w;i++)
    {
        for(int j=0;j<h;j++)
        {
           crosstmp.setPixel(i,j,value);
           crosstmp.setPixel(j,i,value);
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
    m_context.thickness = qBound(0, m_context.thickness, 99);
    QPoint topLeft = qApp->desktop()->screenGeometry(
                qApp->desktop()->screenNumber(QCursor::pos())).topLeft();
    int offset = m_notifierBox->width() / 4;
    m_notifierBox->move(mapFromGlobal(topLeft) + QPoint(offset, offset));
    m_notifierBox->showMessage(QString::number(m_context.thickness));
    if (m_activeButton && m_activeButton->tool()->showMousePreview()) {
        update();
    }
    emit thicknessChanged(m_context.thickness);
    font_options_defult();
    font_options2_defult();
    fontsize_color_chose_default();
    fontsize_color_chose2_default();
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
    m_context.style_settings = new QGSettings("org.ukui.style");
    m_context.style_name = m_context.style_settings->get("style-name").toString();
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
    m_context.style_name = m_context.style_settings->get("style-name").toString();
    if (m_toolWidget) {
        m_toolWidget->deleteLater();
        if (m_activeTool->isValid()) {
            b->setIcon(b->tool()->icon(m_contrastUiColor,false,m_context));
            hide_ChildWindow();
            pushToolToStack();
        }
    }
    if (m_activeButton != b) {
        b->setIcon(b->tool()->icon(m_contrastUiColor,true,m_context));
        show_childwindow(b);
        processTool(b->tool());
    }
    // Only close activated from button
    if (b->tool()->closeOnButtonPressed()) {
        b->setIcon(b->tool()->icon(m_contrastUiColor,false,m_context));
        hide_ChildWindow();
        close();
    }
    if (b->tool()->isSelectable()) {
        if (m_activeButton != b) {
            QWidget *confW = b->tool()->configurationWidget();
            m_panel->addToolWidget(confW);
            if (m_activeButton) {
                m_activeButton->setColor(m_uiColor);
                m_activeButton->setIcon(m_activeButton->tool()->icon(m_uiColor,false,m_context));
                b->setIcon(b->tool()->icon(m_contrastUiColor,true,m_context));
                show_childwindow(b);
             }
             m_activeButton = b;
             m_activeButton->setColor(m_contrastUiColor);
             }
        else if (m_activeButton) {
                 m_panel->clearToolWidget();
                 m_activeButton->setColor(m_uiColor);
                 b->setIcon(b->tool()->icon(m_uiColor,false,m_context));
                 hide_ChildWindow();
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
             //hide_window();
             //screenCap->show();
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
         // new QShortcut(Qt::Key_Space, this, SLOT(togglePanel()));
         new QShortcut(Qt::Key_Escape, this, SLOT(deleteToolwidgetOrClose()));
         new QShortcut(Qt::Key_Return, this, SLOT(copyScreenshot()));
         new QShortcut(Qt::Key_Enter, this, SLOT(copyScreenshot()));
     }

     void CaptureWidget::updateSizeIndicator() {
         if (m_sizeIndButton){
             const QRect &selection = extendedSelection();
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
     void CaptureWidget::ClickedSaveAsFile()
     {
         hide_window();
         bool ok = false;
         while (!ok) {
             QFileDialog fileDialog(this, Qt::CustomizeWindowHint);
             QString savePath = QFileDialog::getSaveFileName(
                         nullptr,
                         QString(),
                         FileNameHandler().absoluteSavePath() + ".png",
                         QLatin1String("Portable Network Graphic file (PNG) (*.png);;BMP file (*.bmp);;JPEG file (*.jpg)"));

             if (savePath.isNull()) {
                 break;
             }

         if (!savePath.endsWith(QLatin1String(".png"), Qt::CaseInsensitive) &&
             !savePath.endsWith(QLatin1String(".bmp"), Qt::CaseInsensitive) &&
             !savePath.endsWith(QLatin1String(".jpg"), Qt::CaseInsensitive)) {
             savePath += QLatin1String(".png");
         }

             ok = pixmap().save(savePath);

             if (ok) {
                 QString pathNoFile = savePath.left(savePath.lastIndexOf(QLatin1String("/")));
                 ConfigHandler().setSavePath(pathNoFile);
                 QString msg = QObject::tr("Capture saved as ") + savePath;
                 SystemNotification().sendMessage(msg, savePath);
             } else {
                 QString msg = QObject::tr("Error trying to save as ") + savePath;
                 QMessageBox saveErrBox(
                             QMessageBox::Warning,
                             QObject::tr("Save Error"),
                             msg);
                 saveErrBox.setWindowIcon(QIcon("/usr/share/icons/ukui-icon-theme-default/128x128/apps/kylin-screenshot.png"));
                 saveErrBox.exec();
             }
         }
         close();
     }
     void CaptureWidget::ClickedSavedir()
     {
         hide_window();
         QFileDialog fileDialog(nullptr, Qt::CustomizeWindowHint);
         fileDialog.setDirectory (m_context.savePath);
         fileDialog.setAcceptMode (QFileDialog::AcceptOpen);
         fileDialog.setFileMode (QFileDialog::DirectoryOnly);
         fileDialog.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Drives | QDir::NoSymLinks);
         if(fileDialog.exec() == QDialog::Accepted){
                 fileDialog.hide();
                 file = fileDialog.selectedFiles()[0];
                 fileDialog.hide();
                 qDebug()<<"select dir:"<<file;
                 save_location->SaveDir->setText(file);
                 save_location->SaveDir->setToolTip(file);
                 m_context.savePath = file;
                 save_location->update();
         }
        fileDialog.hide();
        show_window();
      }
     void CaptureWidget::ClickedSavedir2()
     {
         hide_window();
         QFileDialog fileDialog(nullptr, Qt::CustomizeWindowHint);
         fileDialog.setDirectory (m_context.savePath);
         fileDialog.setAcceptMode (QFileDialog::AcceptOpen);
         fileDialog.setFileMode (QFileDialog::DirectoryOnly);
         fileDialog.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Drives | QDir::NoSymLinks);
         if(fileDialog.exec() == QDialog::Accepted){
                 fileDialog.hide();
                 file = fileDialog.selectedFiles()[0];
                 fileDialog.hide();
                 qDebug()<<"select dir:"<<file;
                 save_location2->SaveDir->setText(file);
                 save_location2->SaveDir->setToolTip(file);
                 m_context.savePath = file;
                 save_location2->update();
         }
         fileDialog.hide();
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
     void CaptureWidget::ClickedSaveType(int i)
     {
         switch (i) {
         case 0:
             m_context.saveType =".jpg";
             break;
         case  1:
             m_context.saveType =".png";
             break;
         case 2:
             m_context.saveType =".bmp";
             break;
         default:
             m_context.saveType =".png";
             break;
         }
     }
     void CaptureWidget::ClickedSaveType2(int i)
     {
         switch (i) {
         case 0:
             m_context.saveType =".jpg";
             break;
         case  1:
             m_context.saveType =".png";
             break;
         case 2:
             m_context.saveType =".bmp";
             break;
         default:
             m_context.saveType =".png";
             break;
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
     void CaptureWidget::savetype_chose_default()
     {
         if(m_context.saveType == ".jpg")
             save_location->type_rect = save_location->m_TypeList.at(0);
         else if(m_context.saveType == ".bmp")
             save_location->type_rect = save_location->m_TypeList.at(2);
         else
             save_location->type_rect = save_location->m_TypeList.at(1);
         save_location->SaveDir->setText(m_context.savePath);
     }
     void CaptureWidget::savetype_chose2_default()
     {
         if(m_context.saveType == ".jpg")
             save_location2->type_rect = save_location->m_TypeList.at(0);
         else if(m_context.saveType == ".bmp")
             save_location2->type_rect = save_location->m_TypeList.at(2);
         else
             save_location2->type_rect = save_location->m_TypeList.at(1);
         save_location2->SaveDir->setText(m_context.savePath);
     }
     void CaptureWidget::hide_window()
         {
             hide();
         }
         void CaptureWidget::show_window()
         {
         this->show();
             if(!isActiveWindow()) //判断是否是活动窗口
             {
                 activateWindow(); //设置成活动窗口
             }
	 setWindowFlags(Qt::BypassWindowManagerHint
                       | Qt::WindowStaysOnTopHint
                       | Qt::FramelessWindowHint
                       | Qt::Tool);
         }
         //format code
         void CaptureWidget::hide_ChildWindow()
         {
                 font_color->hide();
                 font_color2->hide();
                 save_location->hide();
                 save_location2->hide();
                 font_options->hide();
                 font_options2->hide();
                 SaveAs_btn->hide();
         }
         void CaptureWidget::show_childwindow(CaptureButton *b)
         {
             int i = b->m_buttonType;
             switch (i) {
             case CaptureButton::TYPE_OPTION:
                 show_Save_Location_Window(b);
                 break;
             case CaptureButton::TYPE_TEXT:
                 show_Font_Options_Window(b);
                 break;
             case CaptureButton::TYPE_RECT:
             case CaptureButton::TYPE_CIRCLE:
             case CaptureButton::TYPE_LINE:
             case CaptureButton::TYPE_ARROW:
             case CaptureButton::TYPE_PEN:
             case CaptureButton::TYPE_MARKER:
                 show_FontSize_Color_Chose_Window(b);
                 break;
             case CaptureButton::TYPE_SAVEAS:
                 deal_with_SaveAs(b);
                 break;
             default:
                 hide_ChildWindow();
                 break;
             }
         }
         //rect  circle  line arrow  pen marker
         void CaptureWidget::show_FontSize_Color_Chose_Window(CaptureButton *b)
         {
             hide_ChildWindow();
             fontsize_color_chose_default();
             fontsize_color_chose2_default();
             qDebug()<<"show FontSize_Color_Chose_Window";
             if (b->y()>m_selection->y())
             {
                 if (b->y()+150 <= QGuiApplication::primaryScreen()->geometry().height())
                 {
                     font_color_point->setX(b->x()-90);
                     font_color_point->setY(b->y()+50);
                     font_color->move(font_color_point->x(),font_color_point->y());
                     font_color->show();
                 }
                 else
                 {
                     font_color_point->setX(b->x()-90);
                     font_color_point->setY(b->y()-80);
                     font_color2->move(font_color_point->x(),font_color_point->y());
                     font_color2->show();
                 }
             }
             else
             {
                 if (b->y()-80 >= 0)
                 {
                     font_color_point->setX(b->x()-90);
                     font_color_point->setY(b->y()-80);
                     font_color2->move(font_color_point->x(),font_color_point->y());
                     font_color2->show();
                 }
                 else
                 {
                     font_color_point->setX(b->x()-90);
                     font_color_point->setY(b->y()+50);
                     font_color->move(font_color_point->x(),font_color_point->y());
                     font_color->show();
                 }
             }
         }
         //options
         void CaptureWidget::show_Save_Location_Window(CaptureButton *b)
             {
                 hide_ChildWindow();
                 savetype_chose_default();
                 savetype_chose2_default();
                 qDebug()<<"show Save_Location_Window";
                 connect(save_location->SaveDir,SIGNAL(pressed()),this,SLOT(ClickedSavedir()));
                 connect(save_location2->SaveDir,SIGNAL(pressed()),this,SLOT(ClickedSavedir2()));
                 if(b->y()>m_selection->y())
                 {
                    if (b->y()+170 <= QGuiApplication::primaryScreen()->geometry().height())
                    {
                     font_color_point->setX(b->x()-67);
                     font_color_point->setY(b->y()+50);
                     save_location->move(font_color_point->x(),font_color_point->y());
                     save_location->show();
                     }
                    else
                    {
                     font_color_point->setX(b->x()-67);
                     font_color_point->setY(b->y()-155);
                     save_location2->move(font_color_point->x(),font_color_point->y());
                     save_location2->show();
                    }
                 }
                 else
                 {
                     if (b->y()-155>= 0)
                     {
                         font_color_point->setX(b->x()-67);
                         font_color_point->setY(b->y()-155);
                         save_location2->move(font_color_point->x(),font_color_point->y());
                         save_location2->show();
                     }
                    else
                     {
                         font_color_point->setX(b->x()-67);
                         font_color_point->setY(b->y()+50);
                         save_location->move(font_color_point->x(),font_color_point->y());
                         save_location->show();
                      }
                 }
             }
        //text
         void CaptureWidget::show_Font_Options_Window(CaptureButton *b)
             {
                 hide_ChildWindow();
                 font_options_defult();
                 font_options2_defult();
                 qDebug()<<"show Font_Options_Window";
                 if(b->y()>m_selection->y())
                 {
                     if (b->y()+125 <= QGuiApplication::primaryScreen()->geometry().height())
                     {
                         font_color_point->setX(b->x()-125);
                         font_color_point->setY(b->y()+50);
                         font_options->move(font_color_point->x(),font_color_point->y());
                         font_options->show();
                     }
                     else
                     {
                         font_color_point->setX(b->x()-125);
                         font_color_point->setY(b->y()-95);
                         font_options2->move(font_color_point->x(),font_color_point->y());
                         font_options2->show();
                     }
                 }
                 else
                 {
                     if (b->y()-95 >= 0)
                     {
                         font_color_point->setX(b->x()-125);
                         font_color_point->setY(b->y()-95);
                         font_options2->move(font_color_point->x(),font_color_point->y());
                         font_options2->show();

                     }
                     else
                     {
                         font_color_point->setX(b->x()-125);
                         font_color_point->setY(b->y()+50);
                         font_options->move(font_color_point->x(),font_color_point->y());
                         font_options->show();
                     }
                 }
             }
         void CaptureWidget::deal_with_SaveAs(CaptureButton *b)
          {
              hide_ChildWindow();
              setCursor(Qt::ArrowCursor);
              qDebug()<<"show SaveAs";
              if(b->y()+77 <= QGuiApplication::primaryScreen()->geometry().height())
              {
                  font_color_point->setX(b->x()-63);
                  font_color_point->setY(b->y()+40);
              }
              else
              {
                  font_color_point->setX(b->x()-63);
                  font_color_point->setY(b->y()-37);
              }
              SaveAs_btn->move(font_color_point->x(),font_color_point->y());
              SaveAs_btn->show();
          }
