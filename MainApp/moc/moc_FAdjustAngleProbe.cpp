/****************************************************************************
** Meta object code from reading C++ file 'FAdjustAngleProbe.h'
**
** Created: Wed Aug 20 14:04:45 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../FAdjustAngleProbe.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FAdjustAngleProbe.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FAdjustAngleProbe[] = {

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
      19,   18,   18,   18, 0x08,
      40,   18,   18,   18, 0x08,
      62,   18,   18,   18, 0x08,
      82,   18,   18,   18, 0x08,
      97,   18,   18,   18, 0x08,
     111,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FAdjustAngleProbe[] = {
    "FAdjustAngleProbe\0\0OnAdjustProbeStart()\0"
    "OnAdjustProbeCreate()\0OnAdjustProbeQuit()\0"
    "OnImportData()\0OnTCPConfig()\0OnOPCUA()\0"
};

void FAdjustAngleProbe::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FAdjustAngleProbe *_t = static_cast<FAdjustAngleProbe *>(_o);
        switch (_id) {
        case 0: _t->OnAdjustProbeStart(); break;
        case 1: _t->OnAdjustProbeCreate(); break;
        case 2: _t->OnAdjustProbeQuit(); break;
        case 3: _t->OnImportData(); break;
        case 4: _t->OnTCPConfig(); break;
        case 5: _t->OnOPCUA(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FAdjustAngleProbe::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FAdjustAngleProbe::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FAdjustAngleProbe,
      qt_meta_data_FAdjustAngleProbe, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FAdjustAngleProbe::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FAdjustAngleProbe::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FAdjustAngleProbe::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FAdjustAngleProbe))
        return static_cast<void*>(const_cast< FAdjustAngleProbe*>(this));
    return QWidget::qt_metacast(_clname);
}

int FAdjustAngleProbe::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
