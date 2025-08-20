/****************************************************************************
** Meta object code from reading C++ file 'CompWThickTab.h'
**
** Created: Tue Aug 19 21:24:10 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CompWThickTab.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CompWThickTab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CompWThickTab[] = {

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
      15,   14,   14,   14, 0x08,
      23,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CompWThickTab[] = {
    "CompWThickTab\0\0onCal()\0onSave()\0"
};

void CompWThickTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CompWThickTab *_t = static_cast<CompWThickTab *>(_o);
        switch (_id) {
        case 0: _t->onCal(); break;
        case 1: _t->onSave(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData CompWThickTab::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CompWThickTab::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CompWThickTab,
      qt_meta_data_CompWThickTab, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CompWThickTab::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CompWThickTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CompWThickTab::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CompWThickTab))
        return static_cast<void*>(const_cast< CompWThickTab*>(this));
    return QWidget::qt_metacast(_clname);
}

int CompWThickTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
