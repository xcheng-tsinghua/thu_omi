/****************************************************************************
** Meta object code from reading C++ file 'EntityHighlight.h'
**
** Created: Tue Aug 19 21:24:11 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../EntityHighlight.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EntityHighlight.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EntityHighlight[] = {

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
      17,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EntityHighlight[] = {
    "EntityHighlight\0\0onShowNearSurf()\0"
};

void EntityHighlight::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EntityHighlight *_t = static_cast<EntityHighlight *>(_o);
        switch (_id) {
        case 0: _t->onShowNearSurf(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData EntityHighlight::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EntityHighlight::staticMetaObject = {
    { &FFeatureTool::staticMetaObject, qt_meta_stringdata_EntityHighlight,
      qt_meta_data_EntityHighlight, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EntityHighlight::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EntityHighlight::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EntityHighlight::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EntityHighlight))
        return static_cast<void*>(const_cast< EntityHighlight*>(this));
    return FFeatureTool::qt_metacast(_clname);
}

int EntityHighlight::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = FFeatureTool::qt_metacall(_c, _id, _a);
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
