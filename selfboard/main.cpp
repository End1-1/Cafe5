#include <QApplication>

#include "dlgstart.h"
#include "selfboarddisplay.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("Jazzve"));
    QCoreApplication::setApplicationName(QStringLiteral("SelfBoard"));

    auto *start = new DlgStart();
    start->setAttribute(Qt::WA_DeleteOnClose);
    SelfBoardDisplay::showFullscreen(start);

    return app.exec();
}
