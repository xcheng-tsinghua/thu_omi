/****************************************************************************
** Meta object code from reading C++ file 'FInsFeaGraph.h'
**
** Created: Wed Aug 20 14:04:44 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../FInsFeaGraph.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FInsFeaGraph.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FInsFeaGraph[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x08,
      24,   13,   13,   13, 0x08,
      47,   13,   13,   13, 0x08,
      60,   13,   13,   13, 0x08,
      76,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FInsFeaGraph[] = {
    "FInsFeaGraph\0\0onClose()\0onComputeInsFeaGraph()\0"
    "onSampling()\0onCollapseAll()\0onExpandAll()\0"
};

void FInsFeaGraph::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FInsFeaGraph *_t = static_cast<FInsFeaGraph *>(_o);
        switch (_id) {
        case 0: _t->onClose(); break;
        case 1: _t->onComputeInsFeaGraph(); break;
        case 2: _t->onSampling(); break;
        case 3: _t->onCollapseAll(); break;
        case 4: _t->onExpandAll(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FInsFeaGraph::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FInsFeaGraph::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_FInsFeaGraph,
      qt_meta_data_FInsFeaGraph, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FInsFeaGraph::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FInsFeaGraph::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FInsFeaGraph::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FInsFeaGraph))
        return static_cast<void*>(const_cast< FInsFeaGraph*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int FInsFeaGraph::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
static const uint qt_meta_data_FInsFeaGraphTreeWidget[] = {

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

static const char qt_meta_stringdata_FInsFeaGraphTreeWidget[] = {
    "FInsFeaGraphTreeWidget\0"
};

void FInsFeaGraphTreeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FInsFeaGraphTreeWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FInsFeaGraphTreeWidget::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_FInsFeaGraphTreeWidget,
      qt_meta_data_FInsFeaGraphTreeWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FInsFeaGraphTreeWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FInsFeaGraphTreeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FInsFeaGraphTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FInsFeaGraphTreeWidget))
        return static_cast<void*>(const_cast< FInsFeaGraphTreeWidget*>(this));
    return QTreeWidget::qt_metacast(_clname);
}

int FInsFeaGraphTreeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
