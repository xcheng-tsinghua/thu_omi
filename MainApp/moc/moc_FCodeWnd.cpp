/****************************************************************************
** Meta object code from reading C++ file 'FCodeWnd.h'
**
** Created: Tue Aug 19 21:24:11 2025
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../FCodeWnd.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FCodeWnd.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FHighlighter[] = {

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

static const char qt_meta_stringdata_FHighlighter[] = {
    "FHighlighter\0"
};

void FHighlighter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FHighlighter::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FHighlighter::staticMetaObject = {
    { &QSyntaxHighlighter::staticMetaObject, qt_meta_stringdata_FHighlighter,
      qt_meta_data_FHighlighter, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FHighlighter::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FHighlighter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FHighlighter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FHighlighter))
        return static_cast<void*>(const_cast< FHighlighter*>(this));
    return QSyntaxHighlighter::qt_metacast(_clname);
}

int FHighlighter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSyntaxHighlighter::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_FCodeWnd[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      28,    9,    9,    9, 0x0a,
      41,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_FCodeWnd[] = {
    "FCodeWnd\0\0highlightSignal()\0onPauseBtn()\0"
    "onHighlight()\0"
};

void FCodeWnd::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FCodeWnd *_t = static_cast<FCodeWnd *>(_o);
        switch (_id) {
        case 0: _t->highlightSignal(); break;
        case 1: _t->onPauseBtn(); break;
        case 2: _t->onHighlight(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FCodeWnd::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FCodeWnd::staticMetaObject = {
    { &QDockWidget::staticMetaObject, qt_meta_stringdata_FCodeWnd,
      qt_meta_data_FCodeWnd, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FCodeWnd::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FCodeWnd::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FCodeWnd::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FCodeWnd))
        return static_cast<void*>(const_cast< FCodeWnd*>(this));
    return QDockWidget::qt_metacast(_clname);
}

int FCodeWnd::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void FCodeWnd::highlightSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
