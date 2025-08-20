/****************************************************************************
** Meta object code from reading C++ file 'FDataWnd.h'
**
** Created: Tue Aug 19 21:24:11 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../FDataWnd.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FDataWnd.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FDataWnd[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      34,   10,    9,    9, 0x0a,
      70,   66,    9,    9, 0x0a,
     113,   96,    9,    9, 0x0a,
     156,   66,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_FDataWnd[] = {
    "FDataWnd\0\0dspTheo,mesNum,dspError\0"
    "onResetPointData(bool,int,bool)\0num\0"
    "onUpdateMesPointData(int)\0current,previous\0"
    "onSetHighlightPnt(QModelIndex,QModelIndex)\0"
    "onUpdateError(int)\0"
};

void FDataWnd::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FDataWnd *_t = static_cast<FDataWnd *>(_o);
        switch (_id) {
        case 0: _t->onResetPointData((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 1: _t->onUpdateMesPointData((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->onSetHighlightPnt((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 3: _t->onUpdateError((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FDataWnd::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FDataWnd::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_FDataWnd,
      qt_meta_data_FDataWnd, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FDataWnd::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FDataWnd::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FDataWnd::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FDataWnd))
        return static_cast<void*>(const_cast< FDataWnd*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int FDataWnd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
