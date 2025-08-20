/****************************************************************************
** Meta object code from reading C++ file 'FInsFeaTreeWnd.h'
**
** Created: Wed Aug 20 14:04:47 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../FInsFeaTreeWnd.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FInsFeaTreeWnd.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FInsFeaTreeWnd[] = {

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
      16,   15,   15,   15, 0x0a,
      32,   15,   15,   15, 0x0a,
      48,   15,   15,   15, 0x0a,
      63,   15,   15,   15, 0x0a,
      77,   15,   15,   15, 0x0a,
      89,   15,   15,   15, 0x0a,
     102,   15,   15,   15, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_FInsFeaTreeWnd[] = {
    "FInsFeaTreeWnd\0\0onToPointPlan()\0"
    "onToAccuEvalu()\0onFInsFeaDel()\0"
    "onExpandAll()\0onFoldAll()\0onEvaluAll()\0"
    "onComputeSurfPntNum()\0"
};

void FInsFeaTreeWnd::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FInsFeaTreeWnd *_t = static_cast<FInsFeaTreeWnd *>(_o);
        switch (_id) {
        case 0: _t->onToPointPlan(); break;
        case 1: _t->onToAccuEvalu(); break;
        case 2: _t->onFInsFeaDel(); break;
        case 3: _t->onExpandAll(); break;
        case 4: _t->onFoldAll(); break;
        case 5: _t->onEvaluAll(); break;
        case 6: _t->onComputeSurfPntNum(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FInsFeaTreeWnd::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FInsFeaTreeWnd::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_FInsFeaTreeWnd,
      qt_meta_data_FInsFeaTreeWnd, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FInsFeaTreeWnd::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FInsFeaTreeWnd::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FInsFeaTreeWnd::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FInsFeaTreeWnd))
        return static_cast<void*>(const_cast< FInsFeaTreeWnd*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int FInsFeaTreeWnd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
static const uint qt_meta_data_FInsTreeWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_FInsTreeWidget[] = {
    "FInsTreeWidget\0"
};

void FInsTreeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FInsTreeWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FInsTreeWidget::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_FInsTreeWidget,
      qt_meta_data_FInsTreeWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FInsTreeWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FInsTreeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FInsTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FInsTreeWidget))
        return static_cast<void*>(const_cast< FInsTreeWidget*>(this));
    return QTreeWidget::qt_metacast(_clname);
}

int FInsTreeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
