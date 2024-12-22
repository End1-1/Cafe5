#include "logwriter.h"
#include "serverthread.h"
#include "configini.h"
#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>
#include <signal.h>
#include <unistd.h>

QString APPDIR;

// Функция для обработки сигналов
void handleSignal(int signal) {
    LogWriter::write(LogWriterLevel::verbose, "", QString("Received signal: %1").arg(signal));
    if (signal == SIGTERM || signal == SIGINT) {
        LogWriter::write(LogWriterLevel::verbose, "", "Stopping the server...");
        QCoreApplication::quit();
    }
}

void daemonize() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    // Если это родительский процесс, завершить его
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // Создать новый сеанс
    if (setsid() < 0) {
        perror("setsid failed");
        exit(EXIT_FAILURE);
    }

    // Игнорировать сигналы управления терминалом
    signal(SIGHUP, SIG_IGN);

    // Создать еще один дочерний процесс, чтобы избежать повторного управления терминалом
    pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // Изменить текущий каталог на корень
    if (chdir("/") < 0) {
        perror("chdir failed");
        exit(EXIT_FAILURE);
    }

    // Закрыть стандартные потоки
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // Инициализация переменных
    APPDIR = QFileInfo(QString(argv[0])).path() + "/";
    ConfigIni::init(APPDIR + "config.ini");
    LogWriter::fCurrentLevel = 100;
    LogWriter::write(LogWriterLevel::verbose, "", "Daemon started");

    // Установка обработчиков сигналов
    signal(SIGTERM, handleSignal);
    signal(SIGINT, handleSignal);

    // Демонизация процесса
    if (argc > 1 && QString(argv[1]).toLower() == "--daemon") {
        daemonize();
    }

    // Запуск серверного потока
    ServerThread server(APPDIR);
    QObject::connect(&server, &ServerThread::finished, &app, &QCoreApplication::quit);

    server.run();

    // Запуск основного цикла приложения
    return app.exec();
}
