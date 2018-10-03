/****************************************************************************
** Meta object code from reading C++ file 'dlgface.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Waiter/dlgface.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dlgface.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DlgFace_t {
    QByteArrayData data[13];
    char stringdata0[163];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DlgFace_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DlgFace_t qt_meta_stringdata_DlgFace = {
    {
QT_MOC_LITERAL(0, 0, 7), // "DlgFace"
QT_MOC_LITERAL(1, 8, 13), // "newConnection"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 10), // "handleHall"
QT_MOC_LITERAL(4, 34, 3), // "obj"
QT_MOC_LITERAL(5, 38, 10), // "handleMenu"
QT_MOC_LITERAL(6, 49, 10), // "handleConf"
QT_MOC_LITERAL(7, 60, 12), // "handleSocket"
QT_MOC_LITERAL(8, 73, 24), // "on_btnConnection_clicked"
QT_MOC_LITERAL(9, 98, 18), // "on_btnExit_clicked"
QT_MOC_LITERAL(10, 117, 22), // "on_tblHall_itemClicked"
QT_MOC_LITERAL(11, 140, 17), // "QTableWidgetItem*"
QT_MOC_LITERAL(12, 158, 4) // "item"

    },
    "DlgFace\0newConnection\0\0handleHall\0obj\0"
    "handleMenu\0handleConf\0handleSocket\0"
    "on_btnConnection_clicked\0on_btnExit_clicked\0"
    "on_tblHall_itemClicked\0QTableWidgetItem*\0"
    "item"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DlgFace[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x08 /* Private */,
       3,    1,   55,    2, 0x08 /* Private */,
       5,    1,   58,    2, 0x08 /* Private */,
       6,    1,   61,    2, 0x08 /* Private */,
       7,    1,   64,    2, 0x08 /* Private */,
       8,    0,   67,    2, 0x08 /* Private */,
       9,    0,   68,    2, 0x08 /* Private */,
      10,    1,   69,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QJsonObject,    4,
    QMetaType::Void, QMetaType::QJsonObject,    4,
    QMetaType::Void, QMetaType::QJsonObject,    4,
    QMetaType::Void, QMetaType::QJsonObject,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 11,   12,

       0        // eod
};

void DlgFace::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DlgFace *_t = static_cast<DlgFace *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->newConnection(); break;
        case 1: _t->handleHall((*reinterpret_cast< const QJsonObject(*)>(_a[1]))); break;
        case 2: _t->handleMenu((*reinterpret_cast< const QJsonObject(*)>(_a[1]))); break;
        case 3: _t->handleConf((*reinterpret_cast< const QJsonObject(*)>(_a[1]))); break;
        case 4: _t->handleSocket((*reinterpret_cast< const QJsonObject(*)>(_a[1]))); break;
        case 5: _t->on_btnConnection_clicked(); break;
        case 6: _t->on_btnExit_clicked(); break;
        case 7: _t->on_tblHall_itemClicked((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject DlgFace::staticMetaObject = {
    { &C5Dialog::staticMetaObject, qt_meta_stringdata_DlgFace.data,
      qt_meta_data_DlgFace,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DlgFace::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DlgFace::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DlgFace.stringdata0))
        return static_cast<void*>(this);
    return C5Dialog::qt_metacast(_clname);
}

int DlgFace::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = C5Dialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
