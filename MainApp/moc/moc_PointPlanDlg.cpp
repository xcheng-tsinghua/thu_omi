/****************************************************************************
** Meta object code from reading C++ file 'PointPlanDlg.h'
**
** Created: Wed Aug 20 14:04:48 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../PointPlanDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PointPlanDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PointPlanDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x08,
      32,   13,   13,   13, 0x08,
      50,   13,   13,   13, 0x08,
      68,   13,   13,   13, 0x08,
      85,   13,   13,   13, 0x08,
     100,   13,   13,   13, 0x08,
     120,   13,   13,   13, 0x08,
     137,   13,   13,   13, 0x08,
     152,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_PointPlanDlg[] = {
    "PointPlanDlg\0\0OnPointPlanQuit()\0"
    "OnSelectSurface()\0onAutoPointPlan()\0"
    "OnAutoSampling()\0getMethod(int)\0"
    "OnManualPointPlan()\0OnPathPlanning()\0"
    "OnArrReverse()\0onClearPnts()\0"
};

void PointPlanDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PointPlanDlg *_t = static_cast<PointPlanDlg *>(_o);
        switch (_id) {
        case 0: _t->OnPointPlanQuit(); break;
        case 1: _t->OnSelectSurface(); break;
        case 2: _t->onAutoPointPlan(); break;
        case 3: _t->OnAutoSampling(); break;
        case 4: _t->getMethod((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->OnManualPointPlan(); break;
        case 6: _t->OnPathPlanning(); break;
        case 7: _t->OnArrReverse(); break;
        case 8: _t->onClearPnts(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PointPlanDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PointPlanDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_PointPlanDlg,
      qt_meta_data_PointPlanDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PointPlanDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PointPlanDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PointPlanDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PointPlanDlg))
        return static_cast<void*>(const_cast< PointPlanDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int PointPlanDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
