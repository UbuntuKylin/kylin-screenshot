#pragma once

#include <exception>

#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xinerama.h>

#include <X11/Xlib.h>
#include <X11/Xlibint.h>
#include <X11/cursorfont.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <X11/extensions/record.h>
#if 0
//#undef Bool
#undef Status
#undef min
#undef max
#undef CursorShape
#undef Expose
//#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
//#undef None
#undef Unsorted
#undef XButton1
#undef XButton2
#undef Data
#undef Bool
#endif

class X11Exception : public std::exception {
public:
    inline virtual const char* what() const throw() override {
        return "X11Exception";
    }
};
