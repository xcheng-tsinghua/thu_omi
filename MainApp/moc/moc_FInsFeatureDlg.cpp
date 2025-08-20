/****************************************************************************
** Meta object code from reading C++ file 'FInsFeatureDlg.h'
**
** Created: Tue Aug 19 21:24:12 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../FInsFeatureDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FInsFeatureDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FInsFeatureDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   16,   15,   15, 0x08,
      40,   15,   15,   15, 0x08,
      61,   15,   15,   15, 0x08,
      86,   15,   15,   15, 0x08,
     101,   15,   15,   15, 0x08,
     117,   15,   15,   15, 0x08,
     142,  135,   15,   15, 0x08,
     176,  135,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FInsFeatureDlg[] = {
    "FInsFeatureDlg\0\0i\0ChooseTargetFace(int)\0"
    "GenerateInsFeature()\0ExitGenerateInsFeature()\0"
    "onDeleteFace()\0onAIS_FaceSel()\0"
    "onRemoveAllSurf()\0button\0"
    "onButtonClicked(QAbstractButton*)\0"
    "onDirSelButtonClicked(QAbstractButton*)\0"
};

void FInsFeatureDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FInsFeatureDlg *_t = static_cast<FInsFeatureDlg *>(_o);
        switch (_id) {
        case 0: _t->ChooseTargetFace((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->GenerateInsFeature(); break;
        case 2: _t->ExitGenerateInsFeature(); break;
        case 3: _t->onDeleteFace(); break;
        case 4: _t->onAIS_FaceSel(); break;
        case 5: _t->onRemoveAllSurf(); break;
        case 6: _t->onButtonClicked((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        case 7: _t->onDirSelButtonClicked((*reinterpret_cast< QAbstractButton*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FInsFeatureDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FInsFeatureDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FInsFeatureDlg,
      qt_meta_data_FInsFeatureDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FInsFeatureDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FInsFeatureDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FInsFeatureDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FInsFeatureDlg))
        return static_cast<void*>(const_cast< FInsFeatureDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int FInsFeatureDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
