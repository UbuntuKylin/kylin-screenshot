// Copyright(c) 2017-2019 Alejandro Sirgo Rica & Contributors
//
// This file is part of Flameshot.
//
//     Flameshot is free software: you can redistribute it and/or modify
//     it under the terms of the GNU General Public License as published by
//     the Free Software Foundation, either version 3 of the License, or
//     (at your option) any later version.
//
//     Flameshot is distributed in the hope that it will be useful,
//     but WITHOUT ANY WARRANTY; without even the implied warranty of
//     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//     GNU General Public License for more details.
//
//     You should have received a copy of the GNU General Public License
//     along with Flameshot.  If not, see <http://www.gnu.org/licenses/>.

// Based on Lightscreen areadialog.h, Copyright 2017  Christian Kaiser <info@ckaiser.com.ar>
// released under the GNU GPL2  <https://www.gnu.org/licenses/gpl-2.0.txt>

// Based on KDE's KSnapshot regiongrabber.cpp, revision 796531, Copyright 2007 Luca Gugelmann <lucag@student.ethz.ch>
// released under the GNU LGPL  <http://www.gnu.org/licenses/old-licenses/library.txt>

#pragma once

#include "capturebutton.h"
#include "src/tools/capturecontext.h"
#include "src/tools/capturetool.h"
#include "src/utils/confighandler.h"
#include "src/widgets/capture/selectionwidget.h"
#include "src/widgets/panel/utilitypanel.h"
#include "buttonhandler.h"
#include <QWidget>
#include <QUndoStack>
#include <QPointer>
#include "fontsize_color_chose.h"
#include "fontsize_color_chose2.h"
#include "save_location.h"
#include "save_location2.h"
#include "font_options.h"
#include "font_options2.h"
#include "src/widgets/widget.h"

class QPaintEvent;
class QResizeEvent;
class QMouseEvent;
class CaptureModification;
class QNetworkAccessManager;
class QNetworkReply;
class ColorPicker;
class Screenshot;
class NotifierBox;
class HoverEventFilter;

class CaptureWidget : public QWidget {
    Q_OBJECT

public:

    explicit CaptureWidget(const uint id = 0,
                           const QString &savePath = QString(),
                           bool fullScreen = true,
                           QWidget *parent = nullptr);
    ~CaptureWidget();

    QString file;
    void updateButtons();
    QPixmap pixmap();
    QVector<CaptureButton*> vectorButtons;
    QPoint  *font_color_point;
    Widget *screenCap;
    void hide_window();
    void show_window();

public slots:
    void deleteToolwidgetOrClose();
    void ClickedSavedir();
    void ClickedSavedir2();
    void ClickedSaveType();
    void ClickedSaveType2();
signals:
    void captureTaken(uint id, QPixmap p);
    void captureFailed(uint id);
    void colorChanged(const QColor &c);
    void thicknessChanged(const int thickness);

private slots:

    // TODO replace with tools
    void copyScreenshot();
    void saveScreenshot();
    void undo();
    void redo();
    void togglePanel();
    void childEnter();
    void childLeave();

    void leftResize();
    void rightResize();
    void upResize();
    void downResize();

    void setState(CaptureButton *b);
    void processTool(CaptureTool *t);
    void handleButtonSignal(CaptureTool::Request r);
    void setDrawColor(const QColor &c);
    void setDrawThickness(const int &t);
    void font_type_changed(QFont f);

    void  font_italic_clicked(bool);
    void  font_bold_clicked(bool);
    void  font_delete_clicked(bool);
    void  font_underline_clicked(bool);

    void font_options_defult();
    void font_options2_defult();
    void fontsize_color_chose_default();
    void fontsize_color_chose2_default();

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void wheelEvent(QWheelEvent *);
    void resizeEvent(QResizeEvent *);
    void moveEvent(QMoveEvent *);

    // Context information
    CaptureContext m_context;
    FontSize_Color_Chose *font_color;
    FontSize_Color_Chose2 *font_color2;
    Save_Location *save_location;
    Save_Location2 *save_location2;
    Font_Options *font_options;
    Font_Options2 *font_options2;

    // Main ui color
    QColor m_uiColor;
    // Secondary ui color
    QColor m_contrastUiColor;

    // Outside selection opacity
    int m_opacity;
    // utility flags
    bool m_mouseIsClicked;
    bool m_rightClick;
    bool m_newSelection;
    bool m_grabbing;
    bool m_showInitialMsg;
    bool m_captureDone;
    bool m_previewEnabled;
    bool m_adjustmentButtonPressed;

    QPixmap mypixmap;
    QPixmap pixmap2;
    QPixmap crosspixmap;
    double w,h;
private:
    void initContext(const QString &savePath, bool fullscreen);
    void initPanel();
    void initSelection();
    void initShortcuts();
    void updateSizeIndicator();
    void updateCursor();
    void pushToolToStack();
    void makeChild(QWidget *w);

    QRect extendedSelection() const;
    QRect extendedRect(QRect *r) const;

    QUndoStack m_undoStack;
    QPointer<CaptureButton> m_sizeIndButton;
    // Last pressed button
    QPointer<CaptureButton> m_activeButton;
    QPointer<CaptureTool> m_activeTool;
    QPointer<QWidget> m_toolWidget;

    ButtonHandler *m_buttonHandler;
    UtilityPanel *m_panel;
    ColorPicker *m_colorPicker;
    ConfigHandler m_config;
    NotifierBox *m_notifierBox;
    HoverEventFilter *m_eventFilter;
    SelectionWidget *m_selection;

    QPoint m_dragStartPoint;
    SelectionWidget::SideType m_mouseOverHandle;
    uint m_id;


};
