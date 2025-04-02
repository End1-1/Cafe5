#include "fileversion.h"
#include <QFile>
#include <QProcess>
#include <QRegularExpression>
#include <QApplication>

#ifdef Q_OS_WIN
#include <windows.h>
#elif defined(Q_OS_LINUX)
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    unsigned char e_ident[16];
    uint16_t      e_type;
    uint16_t      e_machine;
    uint32_t      e_version;
    uint64_t      e_entry;
    uint64_t      e_phoff;
    uint64_t      e_shoff;
    uint32_t      e_flags;
    uint16_t      e_ehsize;
    uint16_t      e_phentsize;
    uint16_t      e_phnum;
    uint16_t      e_shentsize;
    uint16_t      e_shnum;
    uint16_t      e_shstrndx;
} Elf64_Ehdr;

typedef struct {
    uint32_t   sh_name;
    uint32_t   sh_type;
    uint64_t   sh_flags;
    uint64_t   sh_addr;
    uint64_t   sh_offset;
    uint64_t   sh_size;
    uint32_t   sh_link;
    uint32_t   sh_info;
    uint64_t   sh_addralign;
    uint64_t   sh_entsize;
} Elf64_Shdr;

typedef struct {
    uint32_t      st_name;
    unsigned char st_info;
    unsigned char st_other;
    uint16_t      st_shndx;
    uint64_t      st_value;
    uint64_t      st_size;
} Elf64_Sym;

#define ELFMAG0 0x7F
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'
#endif

QString FileVersion::getVersionString(QString fName)
{
#ifdef Q_OS_WIN
    // Windows implementation
    DWORD dwHandle;
    DWORD dwLen = GetFileVersionInfoSize(fName.toStdWString().c_str(), &dwHandle);
    if (dwLen == 0)
        return "";
    BYTE *lpData = new BYTE[dwLen];
    if(!GetFileVersionInfo(fName.toStdWString().c_str(), dwHandle, dwLen, lpData)) {
        delete[] lpData;
        return "";
    }
    VS_FIXEDFILEINFO *lpBuffer = nullptr;
    UINT uLen;
    if(VerQueryValue(lpData, QString("\\").toStdWString().c_str(), (LPVOID * ) &lpBuffer, &uLen)) {
        QString version = QString::number((lpBuffer->dwFileVersionMS >> 16) & 0xffff) + "." +
                          QString::number((lpBuffer->dwFileVersionMS) & 0xffff) + "." +
                          QString::number((lpBuffer->dwFileVersionLS >> 16) & 0xffff) + "." +
                          QString::number((lpBuffer->dwFileVersionLS) & 0xffff);
        delete[] lpData;
        return version;
    }
    delete[] lpData;
    return "";
#elif defined(Q_OS_LINUX)
    // Linux implementation
    // Method 1: Try to get version from executable itself (ELF)
    QString version = getElfVersion(fName);
    if (!version.isEmpty())
        return version;
    // Method 2: Try to get version via --version flag
    version = getVersionFromProcess(fName);
    if (!version.isEmpty())
        return version;
    // Method 3: Try to read version from file metadata
    return getVersionFromPackageManager(fName);
#else
    // Other platforms not supported
    return "";
#endif
}

#ifdef Q_OS_LINUX
QString FileVersion::getElfVersion(const QString &fName)
{
    int fd = open(fName.toUtf8().constData(), O_RDONLY);
    if (fd == -1) {
        return "";
    }
    struct stat st;
    if (fstat(fd, &st) == -1) {
        close(fd);
        return "";
    }
    void *map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
        close(fd);
        return "";
    }
    QString version;
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)map;
    // Проверка ELF magic
    if (ehdr->e_ident[0] != ELFMAG0 ||
            ehdr->e_ident[1] != ELFMAG1 ||
            ehdr->e_ident[2] != ELFMAG2 ||
            ehdr->e_ident[3] != ELFMAG3) {
        munmap(map, st.st_size);
        close(fd);
        return "";
    }
    // Поиск секции .dynstr (динамические строки)
    Elf64_Shdr *shdr = (Elf64_Shdr *)((char *)map + ehdr->e_shoff);
    char *shstrtab = (char *)map + shdr[ehdr->e_shstrndx].sh_offset;
    for (int i = 0; i < ehdr->e_shnum; ++i) {
        if (strcmp(shstrtab + shdr[i].sh_name, ".dynstr") == 0) {
            char *dynstr = (char *)map + shdr[i].sh_offset;
            size_t dynstr_size = shdr[i].sh_size;
            // Ищем строки, содержащие "version"
            for (size_t j = 0; j < dynstr_size; ++j) {
                if (strncmp(dynstr + j, "version", 7) == 0) {
                    version = QString(dynstr + j);
                    break;
                }
            }
            break;
        }
    }
    munmap(map, st.st_size);
    close(fd);
    return version;
}

QString FileVersion::getVersionFromProcess(const QString &fName)
{
    QProcess process;
    process.start(fName, {"--version"});
    if (!process.waitForFinished(1000))
        return "";
    QByteArray output = process.readAllStandardOutput();
    if (output.isEmpty())
        output = process.readAllStandardError();
    // Try to extract version number
    QRegularExpression re("(\\d+\\.\\d+\\.\\d+)");
    QRegularExpressionMatch match = re.match(output);
    if (match.hasMatch())
        return match.captured(1);
    return "";
}

QString FileVersion::getVersionFromPackageManager(const QString &fName)
{
    // Try dpkg (Debian/Ubuntu)
    QProcess dpkg;
    dpkg.start("dpkg", {"-S", fName});
    if (dpkg.waitForFinished(1000)) {
        QString package = dpkg.readAllStandardOutput().split(':').first().trimmed();
        if (!package.isEmpty()) {
            dpkg.start("dpkg", {"-s", package});
            if (dpkg.waitForFinished(1000)) {
                QRegularExpression re("Version: (.+)");
                QRegularExpressionMatch match = re.match(dpkg.readAllStandardOutput());
                if (match.hasMatch())
                    return match.captured(1);
            }
        }
    }
    // Try rpm (RedHat/Fedora)
    QProcess rpm;
    rpm.start("rpm", {"-qf", fName});
    if (rpm.waitForFinished(1000)) {
        QString package = rpm.readAllStandardOutput().trimmed();
        if (!package.isEmpty()) {
            rpm.start("rpm", {"-qi", package});
            if (rpm.waitForFinished(1000)) {
                QRegularExpression re("Version\\s+: (.+)");
                QRegularExpressionMatch match = re.match(rpm.readAllStandardOutput());
                if (match.hasMatch())
                    return match.captured(1);
            }
        }
    }
    return "";
}
#endif

QString FileVersion::getVersionString()
{
    // Default implementation - get version of current executable
    return getVersionString(QCoreApplication::applicationFilePath());
}

FileVersion::FileVersion()
{
    // Constructor
}
