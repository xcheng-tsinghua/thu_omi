/****************************************************************************
** Meta object code from reading C++ file 'FGroupPathPlan.h'
**
** Created: Tue Aug 19 21:24:09 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../FGroupPathPlan.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FGroupPathPlan.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FGroupPathPlan[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x08,
      28,   15,   15,   15, 0x08,
      44,   15,   15,   15, 0x08,
      57,   15,   15,   15, 0x08,
      81,   15,   15,   15, 0x08,
      96,   15,   15,   15, 0x08,
     109,   15,   15,   15, 0x08,
     122,   15,   15,   15, 0x08,
     147,  140,   15,   15, 0x08,
     184,   15,   15,   15, 0x08,
     199,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FGroupPathPlan[] = {
    "FGroupPathPlan\0\0onPntPlan()\0onLoadAllPnts()\0"
    "onPathPlan()\0onSaveOrientationItem()\0"
    "onImportPnts()\0onExNCCode()\0onFrameAlt()\0"
    "onPntsTransForm()\0button\0"
    "onRotateDirClicked(QAbstractButton*)\0"
    "onResetModel()\0onSaveTheoPnts()\0"
};

void FGroupPathPlan::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FGroupPathPlan *_t = static_cast<FGroupPathPlan *>(_o);
        switch (_id) {
        case 0: _t->onPntPlan(); break;
        case 1: _t->onLoadAllPnts(); break;
        case 2: _t->onPathPlan(); break;
        case 3: _t->onSaveOrientationItem(); break;
        case 4: _t->onImportPnts(); break;
        case 5: _t->onExNCCode(); break;
        case 6: _t->onFrameAlt(); break;
        case 7: _t->onPntsTransForm(); break;
        case 8: _t->onRotateDirClicked((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 9: _t->onResetModel(); break;
        case 10: _t->onSaveTheoPnts(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FGroupPathPlan::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FGroupPathPlan::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FGroupPathPlan,
      qt_meta_data_FGroupPathPlan, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FGroupPathPlan::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FGroupPathPlan::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FGroupPathPlan::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FGroupPathPlan))
        return static_cast<void*>(const_cast< FGroupPathPlan*>(this));
    return QWidget::qt_metacast(_clname);
}

int FGroupPathPlan::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
