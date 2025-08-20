/****************************************************************************
** Meta object code from reading C++ file 'FTreeWnd.h'
**
** Created: Tue Aug 19 21:24:12 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../FTreeWnd.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FTreeWnd.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FTreeWnd[] = {

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
      10,    9,    9,    9, 0x0a,
      25,    9,    9,    9, 0x0a,
      34,    9,    9,    9, 0x0a,
      45,    9,    9,    9, 0x0a,
      57,    9,    9,    9, 0x0a,
      68,    9,    9,    9, 0x0a,
      82,    9,    9,    9, 0x0a,
      94,    9,    9,    9, 0x0a,
     111,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_FTreeWnd[] = {
    "FTreeWnd\0\0onRenameItem()\0onTest()\0"
    "onConfig()\0onNewFeat()\0onAddEle()\0"
    "onExpandAll()\0onFoldAll()\0onShowNearSurf()\0"
    "onAutoInsFea()\0"
};

void FTreeWnd::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FTreeWnd *_t = static_cast<FTreeWnd *>(_o);
        switch (_id) {
        case 0: _t->onRenameItem(); break;
        case 1: _t->onTest(); break;
        case 2: _t->onConfig(); break;
        case 3: _t->onNewFeat(); break;
        case 4: _t->onAddEle(); break;
        case 5: _t->onExpandAll(); break;
        case 6: _t->onFoldAll(); break;
        case 7: _t->onShowNearSurf(); break;
        case 8: _t->onAutoInsFea(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FTreeWnd::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FTreeWnd::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_FTreeWnd,
      qt_meta_data_FTreeWnd, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FTreeWnd::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FTreeWnd::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FTreeWnd::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FTreeWnd))
        return static_cast<void*>(const_cast< FTreeWnd*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int FTreeWnd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
static const uint qt_meta_data_FTreeWidget[] = {

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

static const char qt_meta_stringdata_FTreeWidget[] = {
    "FTreeWidget\0"
};

void FTreeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FTreeWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FTreeWidget::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_FTreeWidget,
      qt_meta_data_FTreeWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FTreeWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FTreeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FTreeWidget))
        return static_cast<void*>(const_cast< FTreeWidget*>(this));
    return QTreeWidget::qt_metacast(_clname);
}

int FTreeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
