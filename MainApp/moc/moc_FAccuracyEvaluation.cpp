/****************************************************************************
** Meta object code from reading C++ file 'FAccuracyEvaluation.h'
**
** Created: Tue Aug 19 21:24:11 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../FAccuracyEvaluation.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FAccuracyEvaluation.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FAccuracyEvaluation[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      28,   20,   21,   20, 0x08,
      41,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FAccuracyEvaluation[] = {
    "FAccuracyEvaluation\0\0double\0OnComputeS()\0"
    "OnFlatQuit()\0"
};

void FAccuracyEvaluation::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FAccuracyEvaluation *_t = static_cast<FAccuracyEvaluation *>(_o);
        switch (_id) {
        case 0: { double _r = _t->OnComputeS();
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = _r; }  break;
        case 1: _t->OnFlatQuit(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FAccuracyEvaluation::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FAccuracyEvaluation::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FAccuracyEvaluation,
      qt_meta_data_FAccuracyEvaluation, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FAccuracyEvaluation::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FAccuracyEvaluation::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FAccuracyEvaluation::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FAccuracyEvaluation))
        return static_cast<void*>(const_cast< FAccuracyEvaluation*>(this));
    return QWidget::qt_metacast(_clname);
}

int FAccuracyEvaluation::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
