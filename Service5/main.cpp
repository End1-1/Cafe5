#include <QDebug>
#include <QFileInfo>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QSqlDatabase>
#include <QTranslator>
#include "configini.h"
#include "logwriter.h"
#include "serverthread.h"

// Заголовки Windows API
#include <strsafe.h>
#include <tchar.h>
#include <windows.h>

#define SVCNAME TEXT("Breeze")

// --- Глобальные переменные ---
SERVICE_STATUS gSvcStatus;
SERVICE_STATUS_HANDLE gSvcStatusHandle;
HANDLE ghSvcStopEvent = NULL;
int ARGC;
char **ARGV;
QString APPDIR;

// --- Прототипы функций (чтобы не было C2065) ---
VOID SvcInstall(void);
VOID WINAPI SvcMain(DWORD, LPTSTR *);
VOID WINAPI SvcCtrlHandler(DWORD);
VOID SvcInit(DWORD, LPTSTR *);
VOID ReportSvcStatus(DWORD, DWORD, DWORD);
VOID SvcReportEvent(LPTSTR);

// --- Основная точка входа ---
int main(int argc, char *argv[])
{
    ARGC = argc;
    ARGV = argv;

    // 1. Обязательно для Qt 6 в сервисах: отключаем поиск монитора
    qputenv("QT_QPA_PLATFORM", "offscreen");

    // 2. Инициализация приложения в главном потоке
    QGuiApplication app(argc, argv);

    QString fontPath = qApp->applicationDirPath() + "/ahuni.ttf";
    int fontId = QFontDatabase::addApplicationFont(fontPath);
    if (fontId != -1) {
        QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);
        QFont customFont(family);
        qApp->setFont(customFont);
        LogWriter::write(LogWriterLevel::verbose, "", "Font loaded: " + family);
    } else {
        LogWriter::write(LogWriterLevel::errors, "", "FAILED TO LOAD FONT: " + fontPath);
    }

    APPDIR = QFileInfo(QString(argv[0])).path() + "/";
    ConfigIni::init(APPDIR + "config.ini");
    LogWriter::fCurrentLevel = 100;
    LogWriter::write(LogWriterLevel::verbose, "", "Service process started");

    // Обработка аргументов командной строки
    if (argc > 1) {
        QString arg1 = QString::fromLocal8Bit(argv[1]).toLower();
        if (arg1 == "--install") {
            SvcInstall();
            return 0;
        }
        if (arg1 == "--run") {
            LogWriter::write(LogWriterLevel::verbose, "", "Running in console mode");
            ServerThread server(APPDIR);
            server.run();
            return app.exec(); // Запуск цикла событий Qt
        }
    }

    // Режим работы как Windows Service
    SERVICE_TABLE_ENTRY DispatchTable[] = {{(LPWSTR) SVCNAME, (LPSERVICE_MAIN_FUNCTION) SvcMain}, {NULL, NULL}};

    if (!StartServiceCtrlDispatcher(DispatchTable)) {
        LogWriter::write(LogWriterLevel::errors, "", "StartServiceCtrlDispatcher failed");
        SvcReportEvent((LPWSTR) TEXT("StartServiceCtrlDispatcher"));
    }

    return 0;
}

// --- Реализация функций сервиса ---

VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
    gSvcStatusHandle = RegisterServiceCtrlHandler(SVCNAME, SvcCtrlHandler);

    if (!gSvcStatusHandle) {
        SvcReportEvent((LPWSTR) TEXT("RegisterServiceCtrlHandler"));
        return;
    }

    gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gSvcStatus.dwServiceSpecificExitCode = 0;

    ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);
    SvcInit(dwArgc, lpszArgv);
}

VOID SvcInit(DWORD dwArgc, LPTSTR *lpszArgv)
{
    // Создаем событие для остановки
    ghSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (ghSvcStopEvent == NULL) {
        ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
        return;
    }

    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
    LogWriter::write(LogWriterLevel::verbose, "", "Service is now RUNNING");

    // Запускаем твою серверную логику
    ServerThread server(APPDIR);
    server.run();

    // Ожидаем сигнала остановки от системы (бесконечное ожидание без нагрузки на CPU)
    WaitForSingleObject(ghSvcStopEvent, INFINITE);

    // Когда получили сигнал стоп — выходим из Qt Event Loop
    QCoreApplication::quit();

    ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
}

VOID WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
    switch (dwCtrl) {
    case SERVICE_CONTROL_STOP:
        ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
        SetEvent(ghSvcStopEvent); // Сигналим SvcInit, что пора закругляться
        return;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    default:
        break;
    }
}

VOID ReportSvcStatus(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    gSvcStatus.dwCurrentState = dwCurrentState;
    gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
    gSvcStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        gSvcStatus.dwControlsAccepted = 0;
    else
        gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    if ((dwCurrentState == SERVICE_RUNNING) || (dwCurrentState == SERVICE_STOPPED))
        gSvcStatus.dwCheckPoint = 0;
    else
        gSvcStatus.dwCheckPoint = dwCheckPoint++;

    SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
}

VOID SvcInstall()
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    TCHAR szPath[MAX_PATH];

    if (!GetModuleFileName(NULL, szPath, MAX_PATH)) {
        return;
    }

    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (NULL == schSCManager)
        return;

    schService = CreateService(schSCManager,
                               SVCNAME,
                               SVCNAME,
                               SERVICE_ALL_ACCESS,
                               SERVICE_WIN32_OWN_PROCESS,
                               SERVICE_AUTO_START,
                               SERVICE_ERROR_NORMAL,
                               szPath,
                               NULL,
                               NULL,
                               NULL,
                               NULL,
                               NULL);

    if (schService != NULL) {
        LogWriter::write(LogWriterLevel::verbose, "", "Service installed successfully");
        CloseServiceHandle(schService);
    }
    CloseServiceHandle(schSCManager);
}

VOID SvcReportEvent(LPTSTR szFunction)
{
    HANDLE hEventSource = RegisterEventSource(NULL, SVCNAME);
    if (NULL != hEventSource) {
        TCHAR Buffer[80];
        StringCchPrintf(Buffer, 80, TEXT("%s failed with %d"), szFunction, GetLastError());
        LPCTSTR lpszStrings[2] = {SVCNAME, Buffer};
        ReportEvent(hEventSource, EVENTLOG_ERROR_TYPE, 0, 0, NULL, 2, 0, lpszStrings, NULL);
        DeregisterEventSource(hEventSource);
    }
}
