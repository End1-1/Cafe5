#include "fileversion.h"
#include <windows.h>

QString FileVersion::getVersionString(QString fName)
{
    // first of all, GetFileVersionInfoSize
    DWORD dwHandle;
    DWORD dwLen = GetFileVersionInfoSize(fName.toStdWString().c_str(), &dwHandle);

    // GetFileVersionInfo
    BYTE *lpData = new BYTE[dwLen];
    if(!GetFileVersionInfo(fName.toStdWString().c_str(), dwHandle, dwLen, lpData)) {
        delete[] lpData;
        return "";
    }

    // VerQueryValue
    VS_FIXEDFILEINFO *lpBuffer = nullptr;
    UINT uLen;
    if(VerQueryValue(lpData, QString("\\").toStdWString().c_str(), (LPVOID*)&lpBuffer, &uLen)) {
        return
            QString::number((lpBuffer->dwFileVersionMS >> 16) & 0xffff) + "." +
            QString::number((lpBuffer->dwFileVersionMS) & 0xffff ) + "." +
            QString::number((lpBuffer->dwFileVersionLS >> 16 ) & 0xffff ) + "." +
            QString::number((lpBuffer->dwFileVersionLS) & 0xffff );
    }
    return "";
}

FileVersion::FileVersion()
{

}
