/****************************************************************************
** Meta object code from reading C++ file 'adipfinal.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../adipfinal.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'adipfinal.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_adipfinal_t {
    QByteArrayData data[16];
    char stringdata0[273];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_adipfinal_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_adipfinal_t qt_meta_stringdata_adipfinal = {
    {
QT_MOC_LITERAL(0, 0, 9), // "adipfinal"
QT_MOC_LITERAL(1, 10, 23), // "on_importButton_clicked"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 21), // "on_saveButton_clicked"
QT_MOC_LITERAL(4, 57, 26), // "on_executionButton_clicked"
QT_MOC_LITERAL(5, 84, 28), // "on_object_slicButton_clicked"
QT_MOC_LITERAL(6, 113, 22), // "on_clearButton_clicked"
QT_MOC_LITERAL(7, 136, 22), // "on_redcheckBox_clicked"
QT_MOC_LITERAL(8, 159, 7), // "checked"
QT_MOC_LITERAL(9, 167, 24), // "on_greencheckBox_clicked"
QT_MOC_LITERAL(10, 192, 24), // "on_blackcheckBox_clicked"
QT_MOC_LITERAL(11, 217, 25), // "on_yellowcheckBox_clicked"
QT_MOC_LITERAL(12, 243, 11), // "eventFilter"
QT_MOC_LITERAL(13, 255, 3), // "obj"
QT_MOC_LITERAL(14, 259, 7), // "QEvent*"
QT_MOC_LITERAL(15, 267, 5) // "event"

    },
    "adipfinal\0on_importButton_clicked\0\0"
    "on_saveButton_clicked\0on_executionButton_clicked\0"
    "on_object_slicButton_clicked\0"
    "on_clearButton_clicked\0on_redcheckBox_clicked\0"
    "checked\0on_greencheckBox_clicked\0"
    "on_blackcheckBox_clicked\0"
    "on_yellowcheckBox_clicked\0eventFilter\0"
    "obj\0QEvent*\0event"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_adipfinal[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   64,    2, 0x08 /* Private */,
       3,    0,   65,    2, 0x08 /* Private */,
       4,    0,   66,    2, 0x08 /* Private */,
       5,    0,   67,    2, 0x08 /* Private */,
       6,    0,   68,    2, 0x08 /* Private */,
       7,    1,   69,    2, 0x08 /* Private */,
       9,    1,   72,    2, 0x08 /* Private */,
      10,    1,   75,    2, 0x08 /* Private */,
      11,    1,   78,    2, 0x08 /* Private */,
      12,    2,   81,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Void, QMetaType::Bool,    8,
    QMetaType::Bool, QMetaType::QObjectStar, 0x80000000 | 14,   13,   15,

       0        // eod
};

void adipfinal::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<adipfinal *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->on_importButton_clicked(); break;
        case 1: _t->on_saveButton_clicked(); break;
        case 2: _t->on_executionButton_clicked(); break;
        case 3: _t->on_object_slicButton_clicked(); break;
        case 4: _t->on_clearButton_clicked(); break;
        case 5: _t->on_redcheckBox_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->on_greencheckBox_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->on_blackcheckBox_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->on_yellowcheckBox_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: { bool _r = _t->eventFilter((*reinterpret_cast< QObject*(*)>(_a[1])),(*reinterpret_cast< QEvent*(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject adipfinal::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_adipfinal.data,
    qt_meta_data_adipfinal,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *adipfinal::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *adipfinal::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_adipfinal.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int adipfinal::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 10;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
