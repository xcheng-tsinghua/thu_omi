/****************************************************************************
** Meta object code from reading C++ file 'AutoInsFea.h'
**
** Created: Wed Aug 20 14:04:44 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../AutoInsFea.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AutoInsFea.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AutoInsFea[] = {

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
      12,   11,   11,   11, 0x08,
      22,   11,   11,   11, 0x08,
      42,   11,   11,   11, 0x08,
      58,   11,   11,   11, 0x08,
      72,   11,   11,   11, 0x08,
      86,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_AutoInsFea[] = {
    "AutoInsFea\0\0onClose()\0onSearchingInsFea()\0"
    "onCollapseAll()\0onExpandAll()\0"
    "onAddInsFea()\0onFilterParaSetting()\0"
};

void AutoInsFea::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AutoInsFea *_t = static_cast<AutoInsFea *>(_o);
        switch (_id) {
        case 0: _t->onClose(); break;
        case 1: _t->onSearchingInsFea(); break;
        case 2: _t->onCollapseAll(); break;
        case 3: _t->onExpandAll(); break;
        case 4: _t->onAddInsFea(); break;
        case 5: _t->onFilterParaSetting(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData AutoInsFea::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AutoInsFea::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_AutoInsFea,
      qt_meta_data_AutoInsFea, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AutoInsFea::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AutoInsFea::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AutoInsFea::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AutoInsFea))
        return static_cast<void*>(const_cast< AutoInsFea*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int AutoInsFea::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
static const uint qt_meta_data_AutoInsFeaTreeWidget[] = {

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

static const char qt_meta_stringdata_AutoInsFeaTreeWidget[] = {
    "AutoInsFeaTreeWidget\0"
};

void AutoInsFeaTreeWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData AutoInsFeaTreeWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AutoInsFeaTreeWidget::staticMetaObject = {
    { &QTreeWidget::staticMetaObject, qt_meta_stringdata_AutoInsFeaTreeWidget,
      qt_meta_data_AutoInsFeaTreeWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AutoInsFeaTreeWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AutoInsFeaTreeWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AutoInsFeaTreeWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AutoInsFeaTreeWidget))
        return static_cast<void*>(const_cast< AutoInsFeaTreeWidget*>(this));
    return QTreeWidget::qt_metacast(_clname);
}

int AutoInsFeaTreeWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTreeWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_FilterParasDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x08,
      28,   15,   15,   15, 0x08,
      53,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FilterParasDlg[] = {
    "FilterParasDlg\0\0onConfirm()\0"
    "onOnlyCylinderCheckBox()\0onOnlyPlaneCheckBox()\0"
};

void FilterParasDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FilterParasDlg *_t = static_cast<FilterParasDlg *>(_o);
        switch (_id) {
        case 0: _t->onConfirm(); break;
        case 1: _t->onOnlyCylinderCheckBox(); break;
        case 2: _t->onOnlyPlaneCheckBox(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FilterParasDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FilterParasDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FilterParasDlg,
      qt_meta_data_FilterParasDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FilterParasDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FilterParasDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FilterParasDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FilterParasDlg))
        return static_cast<void*>(const_cast< FilterParasDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int FilterParasDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
