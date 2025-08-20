/****************************************************************************
** Meta object code from reading C++ file 'SetCoorPrecisely.h'
**
** Created: Tue Aug 19 21:24:10 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../SetCoorPrecisely.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SetCoorPrecisely.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SetCoorPrecisely[] = {

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
      18,   17,   17,   17, 0x09,
      35,   17,   17,   17, 0x09,
      52,   17,   17,   17, 0x09,
      65,   17,   17,   17, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_SetCoorPrecisely[] = {
    "SetCoorPrecisely\0\0onSelDataFile1()\0"
    "onSelDataFile2()\0onCalOrign()\0"
    "onGetModel(int)\0"
};

void SetCoorPrecisely::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SetCoorPrecisely *_t = static_cast<SetCoorPrecisely *>(_o);
        switch (_id) {
        case 0: _t->onSelDataFile1(); break;
        case 1: _t->onSelDataFile2(); break;
        case 2: _t->onCalOrign(); break;
        case 3: _t->onGetModel((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SetCoorPrecisely::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SetCoorPrecisely::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SetCoorPrecisely,
      qt_meta_data_SetCoorPrecisely, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SetCoorPrecisely::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SetCoorPrecisely::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SetCoorPrecisely::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SetCoorPrecisely))
        return static_cast<void*>(const_cast< SetCoorPrecisely*>(this));
    return QWidget::qt_metacast(_clname);
}

int SetCoorPrecisely::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
