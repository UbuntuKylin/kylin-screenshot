/* Copyright(c) 2017-2018 Alejandro Sirgo Rica & Contributors
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

#include "infowindow.h"
#include <QIcon>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QKeyEvent>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
#include <QCursor>
#include <QRect>
#include <QScreen>
#include <QGuiApplication>
#endif

// InfoWindow show basic information about the usage of kylin-screenshot

InfoWindow::InfoWindow(QWidget *parent) : QWidget(parent) {
    setAttribute(Qt::WA_DeleteOnClose);
    //fixed when windows maxed the layout's problem
    this->setFixedSize(700, 750);
    setWindowIcon(QIcon("/usr/share/icons/ukui-icon-theme-default/128x128/apps/kylin-screenshot.png"));
    setWindowTitle(tr("About"));

#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    QRect position = frameGeometry();
    QScreen *screen = QGuiApplication::screenAt(QCursor::pos());
    position.moveCenter(screen->availableGeometry().center());
    move(position.topLeft());
#endif

    m_layout = new QVBoxLayout(this);
    m_layout->setAlignment(Qt::AlignHCenter);
    initLabels();
    initInfoTable();
    initInfoSysTable();
    show();
}


QVector<const char *> InfoWindow::m_keys = {
    "←↓↑→",
    "SHIFT + ←↓↑→",
    "ESC",
    "CTRL + C",
    "CTRL + S",
    "CTRL + Z",
    //QT_TR_NOOP("SPACEBAR"),
    //QT_TR_NOOP("Right Click"),
    QT_TR_NOOP("Mouse Wheel")
};

QVector<const char *> InfoWindow::sys_keys = {
    "PRINT",
    "CTRL + PRINT",
    "SHIFT + PRINT"
};

QVector<const char *> InfoWindow::m_description = {
    QT_TR_NOOP("Move selection 1px"),
    QT_TR_NOOP("Resize selection 1px"),
    QT_TR_NOOP("Quit capture"),
    QT_TR_NOOP("Copy to clipboard"),
    QT_TR_NOOP("Save selection as a file"),
    QT_TR_NOOP("Undo the last modification"),
    //QT_TR_NOOP("Toggle visibility of sidebar with options of the selected tool"),
    //QT_TR_NOOP("Show color picker"),
    QT_TR_NOOP("Change the tool's thickness")
};

QVector<const char *> InfoWindow::m_sys_description = {
    QT_TR_NOOP("Capturn Full Screen"),
    QT_TR_NOOP("Capture Top Screen"),
    QT_TR_NOOP("Capture Screen selection")
};

void InfoWindow::initInfoSysTable() {
    m_layout->addStretch();
    QLabel *sysshortcutsTitleLabel = new QLabel(tr("<u><b>sysShortcuts</b></u>"), this);
    sysshortcutsTitleLabel->setAlignment(Qt::AlignHCenter);;
    m_layout->addWidget(sysshortcutsTitleLabel);
    QTableWidget *table = new QTableWidget(this);
    table->setToolTip(tr("Available System shortcuts."));

    m_layout->addWidget(table);
    table->setColumnCount(2);
    table->setRowCount(sys_keys.size());
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setFocusPolicy(Qt::NoFocus);
    table->verticalHeader()->hide();
    // header creation
    QStringList names;
    names  << tr("Key") << tr("Description");
    table->setHorizontalHeaderLabels(names);

    //add content
    for (int i= 0; i < sys_keys.size(); ++i){
        table->setItem(i, 0, new QTableWidgetItem(tr(sys_keys.at(i))));
        table->setItem(i, 1, new QTableWidgetItem(tr(m_sys_description.at(i))));
    }

    // Read-only table items
    for (int x = 0; x < table->rowCount(); ++x) {
        for (int y = 0; y < table->columnCount(); ++y) {
            QTableWidgetItem *item = table->item(x, y);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        }
    }

    // adjust size
    table->resizeColumnsToContents();
    table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSizePolicy(QSizePolicy::Expanding,
                                             QSizePolicy::Expanding);
}

void InfoWindow::initInfoTable() {
    QTableWidget *table = new QTableWidget(this);
    table->setToolTip(tr("Available shortcuts in the screen capture mode."));

    m_layout->addWidget(table);

    table->setColumnCount(2);
    table->setRowCount(m_keys.size());
    table->setSelectionMode(QAbstractItemView::NoSelection);
    table->setFocusPolicy(Qt::NoFocus);
    table->verticalHeader()->hide();
    // header creation
    QStringList names;
    names  << tr("Key") << tr("Description");
    table->setHorizontalHeaderLabels(names);

    //add content
    for (int i= 0; i < m_keys.size(); ++i){
        table->setItem(i, 0, new QTableWidgetItem(tr(m_keys.at(i))));
        table->setItem(i, 1, new QTableWidgetItem(tr(m_description.at(i))));
    }

    // Read-only table items
    for (int x = 0; x < table->rowCount(); ++x) {
        for (int y = 0; y < table->columnCount(); ++y) {
            QTableWidgetItem *item = table->item(x, y);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        }
    }

    // adjust size
    table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    table->resizeColumnsToContents();
//    table->resizeRowsToContents();

    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSizePolicy(QSizePolicy::Expanding,
                                             QSizePolicy::Expanding);
    m_layout->addStretch();
}

void InfoWindow::initLabels() {
    m_layout->addStretch();
    QLabel *licenseTitleLabel = new QLabel(tr("<u><b>License</b></u>"), this);
    licenseTitleLabel->setAlignment(Qt::AlignHCenter);
    m_layout->addWidget(licenseTitleLabel);
    QLabel *licenseLabel = new QLabel(QStringLiteral("GPLv3+"), this);
    licenseLabel->setAlignment(Qt::AlignHCenter);
    m_layout->addWidget(licenseLabel);
    m_layout->addStretch();

    QLabel *versionTitleLabel = new QLabel(tr("<u><b>Version</b></u>"), this);
    versionTitleLabel->setAlignment(Qt::AlignHCenter);
    m_layout->addWidget(versionTitleLabel);
    QString versionMsg = tr("Screenshot") + "v1.0.0" + "\n" +
            tr("Compiled with Qt")
            + QT_VERSION_STR;
    QLabel *versionLabel = new QLabel(versionMsg, this);
    versionLabel->setAlignment(Qt::AlignHCenter);
    m_layout->addWidget(versionLabel);
    m_layout->addStretch();
    m_layout->addSpacing(10);
    QLabel *shortcutsTitleLabel = new QLabel(tr("<u><b>Shortcuts</b></u>"), this);
    shortcutsTitleLabel->setAlignment(Qt::AlignHCenter);;
    m_layout->addWidget(shortcutsTitleLabel);
    m_layout->addSpacing(10);
}

void InfoWindow::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Escape) {
            close();
    }
}
