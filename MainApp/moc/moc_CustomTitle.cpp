/****************************************************************************
** Meta object code from reading C++ file 'CustomTitle.h'
**
** Created: Tue Aug 19 21:24:13 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CustomTitle.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CustomTitle.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CustomTitle[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,
      38,   12,   12,   12, 0x05,
      67,   12,   12,   12, 0x05,
      92,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     119,   12,   12,   12, 0x08,
     140,   12,   12,   12, 0x08,
     165,   12,   12,   12, 0x08,
     186,   12,   12,   12, 0x08,
     209,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CustomTitle[] = {
    "CustomTitle\0\0signalButtonMinClicked()\0"
    "signalButtonRestoreClicked()\0"
    "signalButtonMaxClicked()\0"
    "signalButtonCloseClicked()\0"
    "onButtonMinClicked()\0onButtonRestoreClicked()\0"
    "onButtonMaxClicked()\0onButtonCloseClicked()\0"
    "onRollTitle()\0"
};

void CustomTitle::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CustomTitle *_t = static_cast<CustomTitle *>(_o);
        switch (_id) {
        case 0: _t->signalButtonMinClicked(); break;
        case 1: _t->signalButtonRestoreClicked(); break;
        case 2: _t->signalButtonMaxClicked(); break;
        case 3: _t->signalButtonCloseClicked(); break;
        case 4: _t->onButtonMinClicked(); break;
        case 5: _t->onButtonRestoreClicked(); break;
        case 6: _t->onButtonMaxClicked(); break;
        case 7: _t->onButtonCloseClicked(); break;
        case 8: _t->onRollTitle(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData CustomTitle::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CustomTitle::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CustomTitle,
      qt_meta_data_CustomTitle, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CustomTitle::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CustomTitle::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CustomTitle::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CustomTitle))
        return static_cast<void*>(const_cast< CustomTitle*>(this));
    return QWidget::qt_metacast(_clname);
}

int CustomTitle::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void CustomTitle::signalButtonMinClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void CustomTitle::signalButtonRestoreClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void CustomTitle::signalButtonMaxClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void CustomTitle::signalButtonCloseClicked()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
