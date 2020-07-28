#include "utils.h"

std::vector<QRect> GetScreenGeometries() {
    std::vector<QRect> screen_geometries;
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    for(QScreen *screen :  QApplication::screens()) {
        QRect geometry = screen->geometry();
        qreal ratio = screen->devicePixelRatio();
        screen_geometries.emplace_back(geometry.x(), geometry.y(), lrint((qreal) geometry.width() * ratio), lrint((qreal) geometry.height() * ratio));
    }
#else
    for(int i = 0; i < QApplication::desktop()->screenCount(); ++i) {
        screen_geometries.push_back(QApplication::desktop()->screenGeometry(i));
    }
#endif
    return screen_geometries;
}

QRect CombineScreenGeometries(const std::vector<QRect>& screen_geometries) {
    QRect combined_geometry;
    for(const QRect &geometry : screen_geometries) {
        combined_geometry |= geometry;
    }
    return combined_geometry;
}

// This does some sanity checking on the rubber band rectangle before creating it.
// Rubber bands with width or height zero or extremely large appear to cause problems.
QRect ValidateRubberBandRectangle(QRect rect) {
    std::vector<QRect> screen_geometries = GetScreenGeometries();
    QRect combined_geometry = CombineScreenGeometries(screen_geometries);
    return rect.normalized() & combined_geometry.adjusted(-10, -10, 10, 10);
}

QRect MapToLogicalCoordinates(const QRect& rect) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    for(QScreen *screen :  QApplication::screens()) {
        QRect geometry = screen->geometry();
        qreal ratio = screen->devicePixelRatio();
        QRect physical_geometry(geometry.x(), geometry.y(), lrint((qreal) geometry.width() * ratio), lrint((qreal) geometry.height() * ratio));
        if(physical_geometry.contains(rect.center())) {
            return QRect(
                        geometry.x() + lrint((qreal) (rect.x() - physical_geometry.x()) / ratio - 0.4999),
                        geometry.y() + lrint((qreal) (rect.y() - physical_geometry.y()) / ratio - 0.4999),
                        lrint((qreal) rect.width() / ratio - 0.4999),
                        lrint((qreal) rect.height() / ratio - 0.4999));
        }
    }
#endif
    return rect;
};

QPoint
GetMousePhysicalCoordinates()
{
    if(IsPlatformX11()) {
        Window root, child;
        int root_x, root_y;
        int win_x, win_y;
        unsigned int mask_return;
        //XQueryPointer(QX11Info::display(), QX11Info::appRootWindow(), &root, &child, &root_x, &root_y, &win_x, &win_y, &mask_return);
        return QPoint(root_x, root_y);
    } else {
        return QPoint(0, 0); // TODO: implement for wayland
    }
}

// Tries to find the real window that corresponds to a top-level window (the actual window without window manager decorations).
// Returns None if it can't find the window (probably because the window is not handled by the window manager).
// Based on the xprop source code (http://cgit.freedesktop.org/xorg/app/xprop/tree/clientwin.c).
Window X11FindRealWindow(Display* display, Window window) {

    // is this the real window?
    Atom actual_type;
    int actual_format;
    unsigned long items, bytes_left;
    unsigned char *data = NULL;
    XGetWindowProperty(display, window, XInternAtom(display, "WM_STATE", true),
                       0, 0, false, AnyPropertyType, &actual_type, &actual_format, &items, &bytes_left, &data);
    if(data != NULL)
        XFree(data);
    if(actual_type != None)
        return window;

    // get the child windows
    Window root, parent, *childs;
    unsigned int childcount;
    if(!XQueryTree(display, window, &root, &parent, &childs, &childcount)) {
        return None;
    }

    // recursively call this function for all childs
    Window real_window = None;
    for(unsigned int i = childcount; i > 0; ) {
        --i;
        Window w = X11FindRealWindow(display, childs[i]);
        if(w != None) {
            real_window = w;
            break;
        }
    }

    // free child window list
    if(childs != NULL)
        XFree(childs);

    return real_window;

}

QString GetNewSegmentFile(const QString& file, bool add_timestamp) {
    QFileInfo fi(file);
    QDateTime now = QDateTime::currentDateTime();
    QString newfile;
    unsigned int counter = 0;
    do {
        ++counter;
        newfile = fi.path() + "/" + fi.completeBaseName();
        if(add_timestamp)
            newfile += "-" + now.toString("yyyy-MM-dd_hh.mm.ss");
        if(counter != 1)
            newfile += "-(" + QString::number(counter) + ")";
        if(!fi.suffix().isEmpty())
            newfile += "." + fi.suffix();
    } while(QFileInfo(newfile).exists());
    return newfile;
}

QString ReadableTime(int64_t time_micro) {
    unsigned int time = (time_micro + 500000) / 1000000;
    return QString("%1:%2:%3")
            .arg(time / 3600)
            .arg((time / 60) % 60, 2, 10, QLatin1Char('0'))
            .arg(time % 60, 2, 10, QLatin1Char('0'));
}

QString ReadableWidthHeight(unsigned int width, unsigned int height) {
    if(width == 0 && height == 0)
        return "?";
    return QString::number(width) + "x" + QString::number(height);
}

QString ReadableSizeIEC(uint64_t size, const QString& suffix) {
   if(size < (uint64_t) 10 * 1024)
       return QString::number(size) + " " + suffix;
   if(size < (uint64_t) 10 * 1024 * 1024)
       return QString::number((size + 512) / 1024) + " Ki" + suffix;
   if(size < (uint64_t) 10 * 1024 * 1024 * 1024)
       return QString::number((size / 1024 + 512) / 1024) + " Mi" + suffix;
   return QString::number((size / (1024 * 1024) + 512) / 1024) + " Gi" + suffix;
}

QString ReadableSizeSI(uint64_t size, const QString& suffix) {
    if(size < (uint64_t) 10 * 1000)
        return QString::number(size) + " " + suffix;
    if(size < (uint64_t) 10 * 1000 * 1000)
        return QString::number((size + 500) / 1000) + " k" + suffix;
    if(size < (uint64_t) 10 * 1000 * 1000 * 1000)
        return QString::number((size / 1000 + 500) / 1000) + " M" + suffix;
    return QString::number((size / (1000 * 1000) + 512) / 1024) + " G" + suffix;
}
