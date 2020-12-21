﻿/* Copyright(c) 2017-2019 Alejandro Sirgo Rica & Contributors
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
#include "buttonhandler.h"
#include "src/utils/globalvalues.h"
#include <QPoint>
#include <QGuiApplication>
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
// ButtonHandler is a habdler for every active button. It makes easier to
// manipulate the buttons as a unit.

ButtonHandler::ButtonHandler(const QVector<CaptureButton*> &v,
                             QObject *parent) :
    QObject(parent)
{
    setButtons(v);
    init();
}

ButtonHandler::ButtonHandler(QObject *parent) :
    QObject(parent)
{
    init();
}

void ButtonHandler::hide() {
    for (CaptureButton *b: m_vectorButtons)
        b->hide();
}

void ButtonHandler::show() {
    if (m_vectorButtons.isEmpty() || m_vectorButtons.first()->isVisible()) {
        return;
    }
    for (CaptureButton *b: m_vectorButtons)
        b->animatedShow();
}

bool ButtonHandler::isVisible() const {
    bool ret = true;
    for (const CaptureButton *b: m_vectorButtons) {
        if (!b->isVisible()) {
            ret = false;
            break;
        }
    }
    return ret;
}

bool ButtonHandler::buttonsAreInside() const {
    return m_buttonsAreInside;
}

size_t ButtonHandler::size() const {
    return m_vectorButtons.size();
}

// updatePosition updates the position of the buttons around the
// selection area. Ignores the sides blocked by the end of the screen.
// When the selection is too small it works on a virtual selection with
// the original in the center.
void ButtonHandler::updatePosition(const QRect &selection) {
    resetRegionTrack();
    const int vecLength = m_vectorButtons.size();
    if (vecLength == 0) {
        return;
    }
    // Copy of the selection area for internal modifications
    m_selection = intersectWithAreas(selection);
    updateBlockedSides();
    ensureSelectionMinimunSize();
    // Indicates the actual button to be moved
    int elemIndicator = 0;
    while (elemIndicator < vecLength) {
        // Add them inside the area when there is no more space
        if (m_allSidesBlocked) {
            m_selection = selection;
            positionButtonsInside(elemIndicator);
            break; // the while
            qDebug()<<"qq"<<elemIndicator;
        }
        // Number of buttons per row column
        int buttonsPerRow = 19; 
		//(m_selection.width() + m_separator) / (m_buttonExtendedSize);
        int buttonsPerCol = 19; 
		//(m_selection.height() + m_separator) / (m_buttonExtendedSize);
        // Buttons to be placed in the corners
        int extraButtons = (vecLength - elemIndicator) -
                (buttonsPerRow + buttonsPerCol) * 2;
        int elemsAtCorners = extraButtons > 4 ? 4 : extraButtons;
        int maxExtra = 2;
        if (m_oneHorizontalBlocked) {
            maxExtra = 1;
        } else if (m_horizontalyBlocked) {
            maxExtra = 0;
        }
        int elemCornersTop = qBound(0, elemsAtCorners, maxExtra);
        elemsAtCorners -= elemCornersTop;
        int elemCornersBotton = qBound(0, elemsAtCorners, maxExtra);

        // Add buttons at the button of the seletion
        if (!m_blockedBotton) {
            int addCounter = buttonsPerRow + elemCornersBotton;
            // Don't add more than we have
            addCounter = qBound(0, addCounter, vecLength - elemIndicator);
            QPoint center = QPoint(m_selection.center().x(),
                                   m_selection.bottom() + m_separator);
            QPoint end2 = QPoint(m_selection.center().x()+m_buttonExtendedSize*10,
                                m_selection.bottom() + m_separator);
            if(!m_screenRegions.contains(QPoint(m_selection.center().x()-m_buttonExtendedSize*8,m_selection.bottom() + m_separator)))
            {
                center.setX(m_buttonExtendedSize*8+16);
            }
            if(!m_screenRegions.contains(end2))
            {
                center.setX(QApplication::desktop()->width()-m_buttonExtendedSize*9);
            }
            if (addCounter > buttonsPerRow) {
                adjustHorizontalCenter(center);
            }
            // ElemIndicator, elemsAtCorners
            QVector<QPoint> positions = horizontalPoints(center, addCounter, true);
            moveButtonsToPoints(positions, elemIndicator);
        }
        // Add buttons at the right side of the seletion
        if (!m_blockedRight && elemIndicator < vecLength) {
            int addCounter = buttonsPerCol;
            addCounter = qBound(0, addCounter, vecLength - elemIndicator);
            QPoint center = QPoint(m_selection.right()+m_buttonExtendedSize*9,
                                   m_selection.center().y());
            QPoint end2 = QPoint(center.x(),
                                m_selection.top()+m_selection.width()/2+m_separator*2);
            QPoint end = QPoint(center.x()+m_buttonExtendedSize*9,
                                center.y());
            if(!m_screenRegions.contains(end2))
            {
                center.setY(QApplication::desktop()->height()-m_buttonExtendedSize);
            }
            if(!m_screenRegions.contains(end))
            {
                center.setX(QApplication::desktop()->width()-m_buttonExtendedSize*9);
            }
            if (addCounter == buttonsPerRow) {
                adjustHorizontalCenter(center);
            }
            QVector<QPoint> positions = horizontalPoints(center, addCounter, true);
            moveButtonsToPoints(positions, elemIndicator);
        }
        // Add buttons at the top of the seletion
        if (!m_blockedTop && elemIndicator < vecLength) {
            int addCounter = buttonsPerRow + elemCornersTop;
            addCounter = qBound(0, addCounter, vecLength - elemIndicator);
            QPoint center = QPoint(m_selection.center().x(),+
                                   m_selection.top() - m_buttonExtendedSize);
            QPoint end2 = QPoint(m_selection.center().x()+m_buttonExtendedSize*10,
                                m_selection.bottom() + m_separator);
            if(m_selection.center().x()- m_buttonExtendedSize*8< 0)
            {
                center.setX(m_buttonExtendedSize*8+16);
            }
            if(!m_screenRegions.contains(end2))
            {
                 center.setX(QApplication::desktop()->width()-m_buttonExtendedSize*9);
            }
            if (addCounter == buttonsPerRow) {
                adjustHorizontalCenter(center);
            }
            QVector<QPoint> positions = horizontalPoints(center, addCounter, true);
            moveButtonsToPoints(positions, elemIndicator);
        }
        // Add buttons at the left side of the seletion
        if (!m_blockedLeft && elemIndicator < vecLength) {
            int addCounter = buttonsPerCol;
            addCounter = qBound(0, addCounter, vecLength - elemIndicator);

            QPoint center = QPoint(m_selection.left()-m_buttonExtendedSize*9,
                                   m_selection.center().y());
            QPoint end2 = QPoint(center.x(),
                                m_selection.top() +m_selection.width()/2+m_separator*2);
            QPoint end = QPoint(center.x()-m_buttonExtendedSize*9,
                                 center.y());
            if(!m_screenRegions.contains(end2))
            {
                center.setY(QApplication::desktop()->height()-m_buttonExtendedSize);
            }
            if(!m_screenRegions.contains(end))
            {
                center.setX(368);
            }
            if (addCounter == buttonsPerRow) {
                adjustHorizontalCenter(center);
            }
            QVector<QPoint> positions = horizontalPoints(center, addCounter, true);
            //QVector<QPoint> positions = verticalPoints(center, addCounter, true);
            moveButtonsToPoints(positions, elemIndicator);
        }
        // If there are elements for the next cycle, increase the size of the
        // base areasetStyleSheet
        if (elemIndicator < vecLength && !(m_allSidesBlocked)) {
            expandSelection();
        }
        qDebug()<<"qqqqqqq"<<elemIndicator;
        updateBlockedSides();
    }
}

// horizontalPoints is an auxiliary method for the button position computation.
// starts from a known center and keeps adding elements horizontally
// and returns the computed positions.
QVector<QPoint> ButtonHandler::horizontalPoints(
        const QPoint &center, const int elements, const bool leftToRight) const
{
    QVector<QPoint> res;
    // Distance from the center to start adding buttons
    int shift = 0;
    if (elements % 2 == 0) {
        shift = m_buttonExtendedSize * (elements / 2) - (m_separator / 2);
    } else {
        shift = m_buttonExtendedSize * ((elements-1) / 2) + m_buttonBaseSize / 2;
    }
    if (!leftToRight) { shift -= m_buttonBaseSize; }
    int x = leftToRight ? center.x() - shift :
                          center.x() + shift;
    QPoint i(x, center.y());
    while (elements > res.length()) {
        res.append(i);
        leftToRight ? i.setX(i.x() + m_buttonExtendedSize) :
                      i.setX(i.x() - m_buttonExtendedSize);
    }
    return res;
}

// verticalPoints is an auxiliary method for the button position computation.
// starts from a known center and keeps adding elements vertically
// and returns the computed positions.
QVector<QPoint> ButtonHandler::verticalPoints(
        const QPoint &center, const int elements, const bool upToDown) const
{
    QVector<QPoint> res;
    // Distance from the center to start adding buttons
    int shift = 0;
    if (elements % 2 == 0) {
        shift = m_buttonExtendedSize * (elements / 2) - (m_separator / 2);
    } else {
        shift = m_buttonExtendedSize * ((elements-1) / 2) + m_buttonBaseSize / 2;
    }
    if (!upToDown) { shift -= m_buttonBaseSize; }
    int y = upToDown ? center.y() - shift :
                       center.y() + shift;
    QPoint i(center.x(),y);
    while (elements > res.length()) {
        res.append(i);
        upToDown ? i.setY(i.y() + m_buttonExtendedSize) :
                      i.setY(i.y() - m_buttonExtendedSize);
    }
    return res;
}

QRect ButtonHandler::intersectWithAreas(const QRect &rect) {
    QRect res;
    for(const QRect &r : m_screenRegions.rects()) {
        QRect temp = rect.intersected(r);
        if (temp.height() * temp.width() > res.height() * res.width()) {
            res = temp;
        }
    }
    return res;
}

void ButtonHandler::init() {
    m_separator = GlobalValues::buttonBaseSize() / 2;
}

void ButtonHandler::resetRegionTrack() {
    m_buttonsAreInside = false;
}

void ButtonHandler::updateBlockedSides() {
    const int EXTENSION = m_separator * 2 + m_buttonBaseSize;
    const int EXTENSIONS = 735;
    // Right  （右下   右上）
    QPoint pointA(m_selection.right() + EXTENSIONS,
                m_selection.bottom());
    QPoint pointB(pointA.x(),
                m_selection.top());
    m_blockedRight = !(m_screenRegions.contains(pointA) &&
                       m_screenRegions.contains(pointB));
    // Left	（左下  左上）
    pointA.setX(m_selection.left() - EXTENSIONS);
    pointB.setX(pointA.x());
    m_blockedLeft = !(m_screenRegions.contains(pointA) &&
                      m_screenRegions.contains(pointB));
    // Bottom	 (左下  右下)
    pointA = QPoint(m_selection.left(),
                    m_selection.bottom() + EXTENSION);
    pointB = QPoint(m_selection.right(),
                    pointA.y());
    m_blockedBotton = !(m_screenRegions.contains(pointA) &&
                        m_screenRegions.contains(pointB));
    // Top	（左上  右上）
    pointA.setY(m_selection.top() - EXTENSION);
    pointB.setY(pointA.y());
    m_blockedTop = !(m_screenRegions.contains(pointA) &&
                     m_screenRegions.contains(pointB));
    // Auxiliary
    m_oneHorizontalBlocked = (!m_blockedTop && m_blockedBotton) ||
            (m_blockedBotton && !m_blockedTop);
    m_horizontalyBlocked = (m_blockedBotton && m_blockedTop);
    m_allSidesBlocked = (m_blockedLeft && m_horizontalyBlocked && m_blockedRight);
}

void ButtonHandler::expandSelection() {
    int &s = m_buttonExtendedSize;
    m_selection = m_selection + QMargins(s, s, s, s);
    m_selection = intersectWithAreas(m_selection);
}

void ButtonHandler::positionButtonsInside(int index) {
    // Position the buttons in the botton-center of the main but inside of the
    // selection.
    QRect mainArea = m_selection;
    mainArea = intersectWithAreas(mainArea);
    const int buttonsPerRow = (mainArea.width()) / (m_buttonExtendedSize);
    if (buttonsPerRow == 0) {
        return;
    }
    QPoint center = QPoint(mainArea.center().x(),
                           mainArea.bottom() - m_buttonExtendedSize);

    while (m_vectorButtons.size() > index) {
        int addCounter = buttonsPerRow;
        addCounter = qBound(0, addCounter, m_vectorButtons.size() - index);
        QVector<QPoint> positions = horizontalPoints(center, addCounter, true);
        moveButtonsToPoints(positions, index);
        center.setY(center.y() - m_buttonExtendedSize);
    }

    m_buttonsAreInside = true;
}

void ButtonHandler::ensureSelectionMinimunSize() {
    // Detect if a side is smaller than a button in order to prevent collision
    // and redimension the base area the the base size of a single button per side
    if (m_selection.width() < m_buttonBaseSize) {
         /*if (!m_blockedLeft) {
            m_selection.setX(m_selection.x() -
                             (m_buttonBaseSize-m_selection.width()) / 2);
        }*/
        m_selection.setWidth(m_buttonBaseSize);
    }
    if (m_selection.height() < m_buttonBaseSize) {
        /*if (!m_blockedTop) {
            m_selection.setY(m_selection.y() -
                             (m_buttonBaseSize-m_selection.height()) / 2);
        }*/
        m_selection.setHeight(m_buttonBaseSize);
    }
}

void ButtonHandler::moveButtonsToPoints(
        const QVector<QPoint> &points, int &index)
{
    for (const QPoint &p: points) {
        auto button = m_vectorButtons[index];
        switch (index) {
#ifndef SUPPORT_NEWUI
        case CaptureButton::TYPR_UNDO:
            button->move(p.x()+29,p.y());
            break;
        case CaptureButton::TYPE_OPTION:
            button->move(p.x()+19,p.y());
            break;
        case CaptureButton::TYPE_SAVEAS:
            button->move(p.x()+106,p.y());
            break;
        case CaptureButton::TYPE_CLOSE:
        case CaptureButton::TYPE_COPY:
            button->move(p.x()+84,p.y());
            break;
        case CaptureButton::TYPE_SAVE:
            button->move(p.x()+108,p.y());
            break;
#else
        case CaptureButton::TYPR_UNDO:
        case CaptureButton::TYPE_CLOSE:
        case CaptureButton::TYPE_COPY:
            button->move(p.x()+29,p.y());
            break;
        case CaptureButton::TYPE_SAVE:
            button->move(p.x()+46,p.y());
            break;
#endif

        case CaptureButton::TYPE_PIN:
            if(m_selection.right()+GlobalValues::buttonBaseSize()<=qApp->desktop()->screenGeometry().width())
                   {
                      if (m_selection.y()+GlobalValues::buttonBaseSize() >= qApp->desktop()->screenGeometry().height())
                          button->move(m_selection.right()+GlobalValues::buttonBaseSize()/3,qApp->desktop()->screenGeometry().height()-GlobalValues::buttonBaseSize());
                      else
                          button->move(m_selection.right()+GlobalValues::buttonBaseSize()/3,m_selection.top());
                   }
            else
                   {
                      if (m_selection.y()+GlobalValues::buttonBaseSize() >= qApp->desktop()->screenGeometry().height())
                          button->move(m_selection.left()-GlobalValues::buttonBaseSize(),qApp->desktop()->screenGeometry().height()-GlobalValues::buttonBaseSize());
                      else
                          button->move(m_selection.left()-GlobalValues::buttonBaseSize(),m_selection.top());
                   }
            break;
        default:
            button->move(p);
            break;
        }
        ++index;
    }
}

void ButtonHandler::adjustHorizontalCenter(QPoint &center) {
    if (m_blockedLeft) {
        center.setX(center.x() + m_buttonExtendedSize/2);
    } else if (m_blockedRight) {
        center.setX(center.x() - m_buttonExtendedSize/2);
    }
}

// setButtons redefines the buttons of the button handler
void ButtonHandler::setButtons(const QVector<CaptureButton *> v) {
    if (v.isEmpty())
        return;

    for (CaptureButton *b: m_vectorButtons)
        delete(b);
    m_vectorButtons = v;
    m_buttonBaseSize = GlobalValues::buttonBaseSize()*6/10;
    m_buttonExtendedSize = m_buttonBaseSize + m_separator;
}

bool ButtonHandler::contains(const QPoint &p) const {
    QPoint first(m_vectorButtons.first()->pos());
    QPoint last(m_vectorButtons.last()->pos());
    bool firstIsTopLeft = (first.x() <= last.x() && first.y() <= last.y());
    QPoint topLeft = firstIsTopLeft ? first : last;
    QPoint bottonRight = firstIsTopLeft ? last : first;
    topLeft += QPoint(-m_separator, -m_separator);
    bottonRight += QPoint(m_buttonExtendedSize, m_buttonExtendedSize);
    QRegion r(QRect(topLeft, bottonRight).normalized());
    return r.contains(p);
}

void ButtonHandler::updateScreenRegions(const QVector<QRect> &rects) {
    m_screenRegions = QRegion();
    for (const QRect &rect: rects) {
        m_screenRegions += rect;
    }
}

void ButtonHandler::updateScreenRegions(const QRect &rect) {
    m_screenRegions = QRegion(rect);
}

void ButtonHandler::clearButtons()
{
   m_vectorButtons.clear();
}

