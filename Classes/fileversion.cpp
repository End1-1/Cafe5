#include "fileversion.h"
#include <QFile>
#include <QProcess>
#include <QRegularExpression>
#include <QApplication>

#ifdef Q_OS_WIN
    #include <windows.h>
#elif defined(Q_OS_LINUX)
    #include <elf.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/mman.h>
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
    if (fd == -1)
        return "";
    struct stat st;
    if (fstat(fd, &st) {
    close(fd);
        return "";
    }
    void *map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
    close(fd);
        return "";
    }
    QString version;
    ElfW(Ehdr) *ehdr = (ElfW(Ehdr) *)map;
    if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) == 0) {
    // This is ELF file, search for version string
    char *strtab = nullptr;
    size_t strtabsz = 0;
    ElfW(Shdr) *shdr = (ElfW(Shdr) *)((char *)map + ehdr->e_shoff);
        for (int i = 0; i < ehdr->e_shnum; ++i) {
            if (shdr[i].sh_type == SHT_STRTAB) {
                strtab = (char *)map + shdr[i].sh_offset;
                strtabsz = shdr[i].sh_size;
                break;
            }
        }
        if (strtab) {
            ElfW(Shdr) *dynsym = nullptr;
            for (int i = 0; i < ehdr->e_shnum; ++i) {
                if (shdr[i].sh_type == SHT_DYNSYM) {
                    dynsym = &shdr[i];
                    break;
                }
            }
            if (dynsym) {
                ElfW(Sym) *sym = (ElfW(Sym) *)((char *)map + dynsym->sh_offset);
                size_t count = dynsym->sh_size / sizeof(ElfW(Sym));
                for (size_t i = 0; i < count; ++i) {
                    if (sym[i].st_name < strtabsz) {
                        const char *name = strtab + sym[i].st_name;
                        if (strstr(name, "version") || strstr(name, "Version")) {
                            // Found version-related symbol
                            version = QString(name);
                            break;
                        }
                    }
                }
            }
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
