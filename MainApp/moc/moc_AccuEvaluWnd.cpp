/****************************************************************************
** Meta object code from reading C++ file 'AccuEvaluWnd.h'
**
** Created: Tue Aug 19 21:24:11 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../AccuEvaluWnd.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AccuEvaluWnd.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AccuEvaluWnd[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x0a,
      30,   13,   13,   13, 0x0a,
      48,   13,   13,   13, 0x0a,
      63,   13,   13,   13, 0x0a,
      79,   13,   13,   13, 0x0a,
      92,   13,   13,   13, 0x0a,
     129,  115,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_AccuEvaluWnd[] = {
    "AccuEvaluWnd\0\0onToAccuEvalu()\0"
    "onToEvaluReport()\0onFInsFeaDel()\0"
    "onChartRowDel()\0onEvaluAll()\0"
    "onRemoveAllEvaluItem()\0printerPixmap\0"
    "printPreviewSlot(QPrinter*)\0"
};

void AccuEvaluWnd::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AccuEvaluWnd *_t = static_cast<AccuEvaluWnd *>(_o);
        switch (_id) {
        case 0: _t->onToAccuEvalu(); break;
        case 1: _t->onToEvaluReport(); break;
        case 2: _t->onFInsFeaDel(); break;
        case 3: _t->onChartRowDel(); break;
        case 4: _t->onEvaluAll(); break;
        case 5: _t->onRemoveAllEvaluItem(); break;
        case 6: _t->printPreviewSlot((*reinterpret_cast< QPrinter*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData AccuEvaluWnd::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AccuEvaluWnd::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_AccuEvaluWnd,
      qt_meta_data_AccuEvaluWnd, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AccuEvaluWnd::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AccuEvaluWnd::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AccuEvaluWnd::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AccuEvaluWnd))
        return static_cast<void*>(const_cast< AccuEvaluWnd*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int AccuEvaluWnd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
