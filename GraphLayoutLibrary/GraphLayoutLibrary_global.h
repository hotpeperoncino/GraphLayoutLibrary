#ifndef GRAPHLAYOUTLIBRARY_GLOBAL_H
#define GRAPHLAYOUTLIBRARY_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(GRAPHLAYOUTLIBRARY_LIBRARY)
#  define GRAPHLAYOUTLIBRARYSHARED_EXPORT Q_DECL_EXPORT
#else
#  define GRAPHLAYOUTLIBRARYSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // GRAPHLAYOUTLIBRARY_GLOBAL_H
