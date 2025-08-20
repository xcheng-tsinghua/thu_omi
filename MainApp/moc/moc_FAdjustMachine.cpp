/****************************************************************************
** Meta object code from reading C++ file 'FAdjustMachine.h'
**
** Created: Tue Aug 19 21:24:10 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../FAdjustMachine.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FAdjustMachine.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FAdjustMachine[] = {

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
      16,   15,   15,   15, 0x08,
      33,   15,   15,   15, 0x08,
      50,   15,   15,   15, 0x08,
      67,   15,   15,   15, 0x08,
      83,   15,   15,   15, 0x08,
     108,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FAdjustMachine[] = {
    "FAdjustMachine\0\0OnMachinefilex()\0"
    "OnMachinefiley()\0OnMachinefilez()\0"
    "OnConfirmCoor()\0OnOpenCloseMachineComp()\0"
    "OnAdjustProbeQuit()\0"
};

void FAdjustMachine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FAdjustMachine *_t = static_cast<FAdjustMachine *>(_o);
        switch (_id) {
        case 0: _t->OnMachinefilex(); break;
        case 1: _t->OnMachinefiley(); break;
        case 2: _t->OnMachinefilez(); break;
        case 3: _t->OnConfirmCoor(); break;
        case 4: _t->OnOpenCloseMachineComp(); break;
        case 5: _t->OnAdjustProbeQuit(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FAdjustMachine::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FAdjustMachine::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FAdjustMachine,
      qt_meta_data_FAdjustMachine, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FAdjustMachine::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FAdjustMachine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FAdjustMachine::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FAdjustMachine))
        return static_cast<void*>(const_cast< FAdjustMachine*>(this));
    return QWidget::qt_metacast(_clname);
}

int FAdjustMachine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
