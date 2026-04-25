#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
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
int ARGC;
char **ARGV;
QString APPDIR;

// --- Прототипы функций ---
VOID SvcInstall(void);
VOID WINAPI SvcMain(DWORD, LPTSTR *);
VOID WINAPI SvcCtrlHandler(DWORD);
VOID SvcInit(DWORD, LPTSTR *);
VOID ReportSvcStatus(DWORD, DWORD, DWORD);
VOID SvcReportEvent(LPTSTR);

int main(int argc, char *argv[])
{
    ARGC = argc;
    ARGV = argv;

    // Вычисляем путь к папке с бинарником (чтобы конфиги читались верно)
    APPDIR = QFileInfo(QString::fromLocal8Bit(argv[0])).absolutePath() + "/";

    if (argc > 1) {
        QString arg1 = QString::fromLocal8Bit(argv[1]).toLower();
        if (arg1 == "--install") {
            SvcInstall();
            return 0;
        }
        if (arg1 == "--run") {
            // Консольный режим
            qputenv("QT_QPA_PLATFORM", "minimal");
            QCoreApplication app(argc, argv);

            ConfigIni::init(APPDIR + "config.ini");
            LogWriter::fCurrentLevel = 100;
            LogWriter::write(LogWriterLevel::verbose, "", "Running in console mode");

            ServerThread server(APPDIR);
            server.run();
            return app.exec();
        }
    }

    // Режим сервиса
    SERVICE_TABLE_ENTRY DispatchTable[] = {{(LPWSTR) SVCNAME, (LPSERVICE_MAIN_FUNCTION) SvcMain}, {NULL, NULL}};

    if (!StartServiceCtrlDispatcher(DispatchTable)) {
        SvcReportEvent((LPWSTR) TEXT("StartServiceCtrlDispatcher"));
    }

    return 0;
}

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
    // КРИТИЧНО: Ставим переменные ДО создания QCoreApplication
    qputenv("QT_QPA_PLATFORM", "minimal");
    qputenv("QT_QPA_GENERIC_PLUGINS", "");

    // КРИТИЧНО: Создаем объект приложения в потоке SvcMain
    QCoreApplication app(ARGC, ARGV);

    // Инициализация ресурсов внутри потока сервиса
    ConfigIni::init(APPDIR + "config.ini");
    LogWriter::fCurrentLevel = 100;
    LogWriter::write(LogWriterLevel::verbose, "", "Service thread started, app initialized");

    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

    // Запускаем твой сервер
    ServerThread server(APPDIR);
    server.run();

    // Запускаем цикл событий Qt. Он заблокирует поток до вызова app.quit()
    app.exec();

    ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
}

VOID WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
    switch (dwCtrl) {
    case SERVICE_CONTROL_STOP:
        ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
        // Выход из app.exec() в функции SvcInit
        QCoreApplication::quit();
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

    if (!GetModuleFileName(NULL, szPath, MAX_PATH))
        return;

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
        printf("Service installed successfully\n");
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
