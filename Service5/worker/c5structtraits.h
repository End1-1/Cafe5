#pragma once
#include <QVariant>

template<typename T>
struct StructTraits;

template<typename T>
struct ColumnDef {
    const char* header;
    QVariant(*getter)(const T&);
};

template<typename T> struct SelectorName;
