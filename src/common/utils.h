#ifndef UTILS_H
#define UTILS_H

#include "src/core/Global.h"
#include <QApplication>

std::vector<QRect> GetScreenGeometries();

QRect CombineScreenGeometries(const std::vector<QRect>& screen_geometries);

// This does some sanity checking on the rubber band rectangle before creating it.
// Rubber bands with width or height zero or extremely large appear to cause problems.
QRect ValidateRubberBandRectangle(QRect rect);

QRect MapToLogicalCoordinates(const QRect& rect);

QPoint GetMousePhysicalCoordinates();

// Tries to find the real window that corresponds to a top-level window (the actual window without window manager decorations).
// Returns None if it can't find the window (probably because the window is not handled by the window manager).
// Based on the xprop source code (http://cgit.freedesktop.org/xorg/app/xprop/tree/clientwin.c).
Window X11FindRealWindow(Display* display, Window window);

QString GetNewSegmentFile(const QString& file, bool add_timestamp);

QString ReadableTime(int64_t time_micro);

QString ReadableWidthHeight(unsigned int width, unsigned int height);

QString ReadableSizeIEC(uint64_t size, const QString& suffix);

QString ReadableSizeSI(uint64_t size, const QString& suffix);

#endif // UTILS_H
