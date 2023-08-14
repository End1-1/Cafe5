#include "os.h"
#include <memory>
#include <cstdio>
#include <array>

OS::OS()
{

}

QString OS::p(const QString &cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(_popen(cmd.toLatin1().data(), "r"), _pclose);
    if (!pipe)  {
        return "";
    }
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL) {
            result += buffer.data();
        }
    }
    return QString(result.data());
}
