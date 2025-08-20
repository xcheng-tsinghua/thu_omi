/****************************************************************************
** Meta object code from reading C++ file 'FToolPathDlg.h'
**
** Created: Tue Aug 19 21:24:09 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../FToolPathDlg.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FToolPathDlg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FToolPathDlg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      41,   13,   13,   13, 0x08,
      54,   13,   13,   13, 0x08,
      70,   13,   13,   13, 0x08,
      82,   13,   13,   13, 0x08,
     100,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FToolPathDlg[] = {
    "FToolPathDlg\0\0preper_save(FToolPathDlg*)\0"
    "onToolPath()\0onCreateNURBS()\0onMPtDist()\0"
    "onSurfaceRecons()\0onSelSurf()\0"
};

void FToolPathDlg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FToolPathDlg *_t = static_cast<FToolPathDlg *>(_o);
        switch (_id) {
        case 0: _t->preper_save((*reinterpret_cast< FToolPathDlg*(*)>(_a[1]))); break;
        case 1: _t->onToolPath(); break;
        case 2: _t->onCreateNURBS(); break;
        case 3: _t->onMPtDist(); break;
        case 4: _t->onSurfaceRecons(); break;
        case 5: _t->onSelSurf(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FToolPathDlg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FToolPathDlg::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_FToolPathDlg,
      qt_meta_data_FToolPathDlg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FToolPathDlg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FToolPathDlg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FToolPathDlg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FToolPathDlg))
        return static_cast<void*>(const_cast< FToolPathDlg*>(this));
    return QWidget::qt_metacast(_clname);
}

int FToolPathDlg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void FToolPathDlg::preper_save(FToolPathDlg * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_ToolMove_thread[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      22,   17,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ToolMove_thread[] = {
    "ToolMove_thread\0\0Temp\0getaddress(FToolPathDlg*)\0"
};

void ToolMove_thread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ToolMove_thread *_t = static_cast<ToolMove_thread *>(_o);
        switch (_id) {
        case 0: _t->getaddress((*reinterpret_cast< FToolPathDlg*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ToolMove_thread::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ToolMove_thread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_ToolMove_thread,
      qt_meta_data_ToolMove_thread, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ToolMove_thread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ToolMove_thread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ToolMove_thread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ToolMove_thread))
        return static_cast<void*>(const_cast< ToolMove_thread*>(this));
    return QThread::qt_metacast(_clname);
}

int ToolMove_thread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
