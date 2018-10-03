/****************************************************************************
** Meta object code from reading C++ file 'dlgorder.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.10.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Waiter/dlgorder.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dlgorder.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.10.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DlgOrder_t {
    QByteArrayData data[18];
    char stringdata0[259];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DlgOrder_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DlgOrder_t qt_meta_stringdata_DlgOrder = {
    {
QT_MOC_LITERAL(0, 0, 8), // "DlgOrder"
QT_MOC_LITERAL(1, 9, 15), // "handleOpenTable"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 3), // "obj"
QT_MOC_LITERAL(4, 30, 11), // "saveAndQuit"
QT_MOC_LITERAL(5, 42, 11), // "handleError"
QT_MOC_LITERAL(6, 54, 3), // "err"
QT_MOC_LITERAL(7, 58, 3), // "msg"
QT_MOC_LITERAL(8, 62, 23), // "on_tblPart1_itemClicked"
QT_MOC_LITERAL(9, 86, 17), // "QTableWidgetItem*"
QT_MOC_LITERAL(10, 104, 4), // "item"
QT_MOC_LITERAL(11, 109, 23), // "on_tblPart2_itemClicked"
QT_MOC_LITERAL(12, 133, 24), // "on_tblDishes_itemClicked"
QT_MOC_LITERAL(13, 158, 19), // "on_btnPlus1_clicked"
QT_MOC_LITERAL(14, 178, 18), // "on_btnMin1_clicked"
QT_MOC_LITERAL(15, 197, 20), // "on_btnPlus05_clicked"
QT_MOC_LITERAL(16, 218, 19), // "on_btnMin05_clicked"
QT_MOC_LITERAL(17, 238, 20) // "on_btnCustom_clicked"

    },
    "DlgOrder\0handleOpenTable\0\0obj\0saveAndQuit\0"
    "handleError\0err\0msg\0on_tblPart1_itemClicked\0"
    "QTableWidgetItem*\0item\0on_tblPart2_itemClicked\0"
    "on_tblDishes_itemClicked\0on_btnPlus1_clicked\0"
    "on_btnMin1_clicked\0on_btnPlus05_clicked\0"
    "on_btnMin05_clicked\0on_btnCustom_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DlgOrder[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x08 /* Private */,
       4,    1,   72,    2, 0x08 /* Private */,
       5,    2,   75,    2, 0x08 /* Private */,
       8,    1,   80,    2, 0x08 /* Private */,
      11,    1,   83,    2, 0x08 /* Private */,
      12,    1,   86,    2, 0x08 /* Private */,
      13,    0,   89,    2, 0x08 /* Private */,
      14,    0,   90,    2, 0x08 /* Private */,
      15,    0,   91,    2, 0x08 /* Private */,
      16,    0,   92,    2, 0x08 /* Private */,
      17,    0,   93,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QJsonObject,    3,
    QMetaType::Void, QMetaType::QJsonObject,    3,
    QMetaType::Void, QMetaType::Int, QMetaType::QString,    6,    7,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DlgOrder::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        DlgOrder *_t = static_cast<DlgOrder *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->handleOpenTable((*reinterpret_cast< const QJsonObject(*)>(_a[1]))); break;
        case 1: _t->saveAndQuit((*reinterpret_cast< const QJsonObject(*)>(_a[1]))); break;
        case 2: _t->handleError((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: _t->on_tblPart1_itemClicked((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 4: _t->on_tblPart2_itemClicked((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 5: _t->on_tblDishes_itemClicked((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 6: _t->on_btnPlus1_clicked(); break;
        case 7: _t->on_btnMin1_clicked(); break;
        case 8: _t->on_btnPlus05_clicked(); break;
        case 9: _t->on_btnMin05_clicked(); break;
        case 10: _t->on_btnCustom_clicked(); break;
        default: ;
        }
    }
}

const QMetaObject DlgOrder::staticMetaObject = {
    { &C5Dialog::staticMetaObject, qt_meta_stringdata_DlgOrder.data,
      qt_meta_data_DlgOrder,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *DlgOrder::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DlgOrder::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DlgOrder.stringdata0))
        return static_cast<void*>(this);
    return C5Dialog::qt_metacast(_clname);
}

int DlgOrder::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = C5Dialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
