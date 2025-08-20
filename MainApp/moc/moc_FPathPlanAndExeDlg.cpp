/****************************************************************************
** Meta object code from reading C++ file 'FPathPlanAndExeDlg.h'
**
** Created: Wed Aug 20 14:36:57 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../FPathPlanAndExeDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FPathPlanAndExeDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FPathPlanAndExeDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x08,
      31,   19,   19,   19, 0x08,
      44,   19,   19,   19, 0x08,
      62,   55,   19,   19, 0x08,
      96,   55,   19,   19, 0x08,
     135,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FPathPlanAndExeDlg[] = {
    "FPathPlanAndExeDlg\0\0onCancel()\0"
    "onPathPlan()\0onExport()\0button\0"
    "onButtonClicked(QAbstractButton*)\0"
    "onColliButtonClicked(QAbstractButton*)\0"
    "onDataTransfer()\0"
};

void FPathPlanAndExeDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FPathPlanAndExeDlg *_t = static_cast<FPathPlanAndExeDlg *>(_o);
        switch (_id) {
        case 0: _t->onCancel(); break;
        case 1: _t->onPathPlan(); break;
        case 2: _t->onExport(); break;
        case 3: _t->onButtonClicked((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 4: _t->onColliButtonClicked((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 5: _t->onDataTransfer(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FPathPlanAndExeDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FPathPlanAndExeDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FPathPlanAndExeDlg,
      qt_meta_data_FPathPlanAndExeDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FPathPlanAndExeDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FPathPlanAndExeDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FPathPlanAndExeDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FPathPlanAndExeDlg))
        return static_cast<void*>(const_cast< FPathPlanAndExeDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int FPathPlanAndExeDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
