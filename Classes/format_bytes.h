#pragma once
#include <QString>

inline QString formatBytes(qint64 bytes)
{
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    double size = bytes;
    int i = 0;

    while(size >= 1024 && i < 4) {
        size /= 1024.0;
        ++i;
    }

    return QString("%1 %2").arg(size, 0, 'f', (i == 0 ? 0 : 2)).arg(units[i]);
}
