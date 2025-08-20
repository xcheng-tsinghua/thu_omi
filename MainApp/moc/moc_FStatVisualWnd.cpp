/****************************************************************************
** Meta object code from reading C++ file 'FStatVisualWnd.h'
**
** Created: Tue Aug 19 21:24:12 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../FStatVisualWnd.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FStatVisualWnd.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FStatVisualWnd[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_FStatVisualWnd[] = {
    "FStatVisualWnd\0\0PlotPic1()\0"
};

void FStatVisualWnd::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FStatVisualWnd *_t = static_cast<FStatVisualWnd *>(_o);
        switch (_id) {
        case 0: _t->PlotPic1(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FStatVisualWnd::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FStatVisualWnd::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_FStatVisualWnd,
      qt_meta_data_FStatVisualWnd, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FStatVisualWnd::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FStatVisualWnd::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FStatVisualWnd::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FStatVisualWnd))
        return static_cast<void*>(const_cast< FStatVisualWnd*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int FStatVisualWnd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
