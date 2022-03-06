#include "logwriter.h"
#include "serverthread.h"
#include "databaseconnectionmanager.h"
#include "configini.h"
#include "storemanager.h"
#include "requestmanager.h"
#include "shopmanager.h"
#include "rawdataexchange.h"
#include "monitor.h"
#include "thread.h"
#include <QFileInfo>
#include <QDir>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <QApplication>
#include <QTranslator>

#define SVCNAME TEXT("Breeze")

int ARGC;
char **ARGV;
QString APPDIR;

SERVICE_STATUS          gSvcStatus;
SERVICE_STATUS_HANDLE   gSvcStatusHandle;
HANDLE                  ghSvcStopEvent = NULL;

VOID SvcInstall(void);
VOID WINAPI SvcCtrlHandler( DWORD );
VOID WINAPI SvcMain( DWORD, LPTSTR * );

VOID ReportSvcStatus( DWORD, DWORD, DWORD );
VOID SvcInit( DWORD, LPTSTR * );
VOID SvcReportEvent( LPTSTR );

DWORD WINAPI ThreadProcSocketClient(CONST LPVOID lpParam) {
//    SslSocket *clientSocket = (SslSocket*)lpParam;
//    SocketRW *srw = new SocketRW(clientSocket);
//    srw->go();
//    delete srw;
//    ExitThread(0);
    return 0;
}

DWORD WINAPI ThreadProc(CONST LPVOID lpParam) {
    LogWriter::write(LogWriterLevel::verbose, "", "Start service thread");
    QApplication app(ARGC, ARGV);
    QTranslator t;
    t.load(":/Service5.qm");
    app.installTranslator(&t);
    ConfigIni::init(APPDIR);
    DatabaseConnectionManager::init();
    if (ConfigIni::isTrue("store/init")) {
        StoreManager::init(ConfigIni::value("store/db"));
    }
    if (ConfigIni::isTrue("shop/init")) {
        ShopManager::init(ConfigIni::value("shop/db"));
    }
    RequestManager::init();
    RawDataExchange::init();
    auto *server = new ServerThread(APPDIR);
    auto *thread = new Thread();
    thread->connect(thread, &QThread::started, server, &ServerThread::run);
    thread->connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    server->connect(server, &ServerThread::endOfWork, &app, &QApplication::quit);
    server->connect(server, &ServerThread::endOfWork, thread, &Thread::quit);
    server->moveToThread(thread);
    thread->start();
    app.exec();
    LogWriter::write(LogWriterLevel::verbose, "", "Exit service thread");
    return 0;
}


void __cdecl main(int argc, char *argv[])
{
    ARGC = argc;
    ARGV = argv;
    APPDIR = QFileInfo(QString(argv[0])).path() + "/";
    LogWriter::fCurrentLevel = 100;
    LogWriter::setFile(LogWriterLevel::verbose, QString("%1/%2/%3/Logs/%4_verbose.log").arg(QDir::tempPath()).arg(_APPLICATION_).arg(_MODULE_).arg(QDate::currentDate().toString("dd_MM_yyyy")));
    LogWriter::setFile(LogWriterLevel::warning, QString("%1/%2/%3/Logs/%4_warning.log").arg(QDir::tempPath()).arg(_APPLICATION_).arg(_MODULE_).arg(QDate::currentDate().toString("dd_MM_yyyy")));
    LogWriter::setFile(LogWriterLevel::errors, QString("%1/%2/%3/Logs/%4_error.log").arg(QDir::tempPath()).arg(_APPLICATION_).arg(_MODULE_).arg(QDate::currentDate().toString("dd_MM_yyyy")));
    LogWriter::write(LogWriterLevel::verbose, "", "Service started");

    if (QString(argv[1]).toLocal8Bit() == "gui") {
        QApplication a(argc, argv);
        Monitor w;
        w.show();
        a.exec();
        return;
    }

    if (QString(argv[1]).toLocal8Bit() == "install") {
        SvcInstall();
        return;
    }

    if (QString(argv[1]).toLower() == "run") {
        ThreadProc(NULL);
        return;
    }

    // TO_DO: Add any additional services for the process to this table.
    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        {(LPWSTR) SVCNAME, (LPSERVICE_MAIN_FUNCTION) SvcMain},
        {NULL, NULL}
    };

    LogWriter::write(LogWriterLevel::verbose, "", "Starting service: main");
    if (!StartServiceCtrlDispatcher(DispatchTable)) {
        LogWriter::write(LogWriterLevel::errors, "", QString("Service could not started. %1").arg(GetLastError()));
        SvcReportEvent((LPWSTR)TEXT("StartServiceCtrlDispatcher"));
    }
}

VOID SvcInstall()
{
    SC_HANDLE schSCManager;
    SC_HANDLE schService;
    TCHAR szPath[MAX_PATH];

    if(!GetModuleFileName(0, (LPWSTR) szPath, MAX_PATH))
    {
        LogWriter::write(LogWriterLevel::errors, "", QString("Cannot install service (%1)").arg(GetLastError()));
        return;
    }

    schSCManager = OpenSCManager(
        NULL,                    // local computer
        NULL,                    // ServicesActive database
        SC_MANAGER_ALL_ACCESS);  // full access rights

    if (NULL == schSCManager)
    {
        LogWriter::write(LogWriterLevel::errors, "", QString("OpenSCManager failed (%1)").arg(GetLastError()));
        return;
    }

    // Create the service

    schService = CreateService(
        schSCManager,              // SCM database
        SVCNAME,                   // name of service
        SVCNAME,                   // service name to display
        SERVICE_ALL_ACCESS,        // desired access
        SERVICE_WIN32_OWN_PROCESS, // service type
        SERVICE_AUTO_START,      // start type
        SERVICE_ERROR_NORMAL,      // error control type
        szPath,                    // path to service's binary
        NULL,                      // no load ordering group
        NULL,                      // no tag identifier
        NULL,                      // no dependencies
        NULL,                      // LocalSystem account
        NULL);                     // no password

    if (schService == NULL) {
        LogWriter::write(LogWriterLevel::errors, "", QString("CreateService failed (%1)").arg(GetLastError()));
        CloseServiceHandle(schSCManager);
        return;
    } else {
        LogWriter::write(LogWriterLevel::verbose, "", QString("Service installed successfully"));
    }

    CloseServiceHandle(schService);
    CloseServiceHandle(schSCManager);
}

//
// Purpose:
//   Entry point for the service
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
//
// Return value:
//   None.
//
VOID WINAPI SvcMain( DWORD dwArgc, LPTSTR *lpszArgv )
{
    LogWriter::write(LogWriterLevel::verbose, "", "Starting service: SvcMain");
    gSvcStatusHandle = RegisterServiceCtrlHandler(
        SVCNAME,
        SvcCtrlHandler);

    if( !gSvcStatusHandle )
    {
        SvcReportEvent((LPWSTR)TEXT("RegisterServiceCtrlHandler"));
        return;
    }

    // These SERVICE_STATUS members remain as set here

    gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gSvcStatus.dwServiceSpecificExitCode = 0;

    // Report initial status to the SCM

    ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

    // Perform service-specific initialization and work.

    SvcInit(dwArgc, lpszArgv);
}

//
// Purpose:
//   The service code
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
//
// Return value:
//   None
//
VOID SvcInit( DWORD dwArgc, LPTSTR *lpszArgv)
{
    LogWriter::write(LogWriterLevel::verbose, "", "Starting service: SvcInit");
    // TO_DO: Declare and set any required variables.
    //   Be sure to periodically call ReportSvcStatus() with
    //   SERVICE_START_PENDING. If initialization fails, call
    //   ReportSvcStatus with SERVICE_STOPPED.

    // Create an event. The control handler function, SvcCtrlHandler,
    // signals this event when it receives the stop control code.

    ghSvcStopEvent = CreateEvent(
                         NULL,    // default security attributes
                         TRUE,    // manual reset event
                         FALSE,   // not signaled
                         NULL);   // no name

    if ( ghSvcStopEvent == NULL)
    {
        ReportSvcStatus( SERVICE_STOPPED, NO_ERROR, 0 );
        return;
    }

    // Report running status when initialization is complete.

    ReportSvcStatus( SERVICE_RUNNING, NO_ERROR, 0 );

    // TO_DO: Perform work until service stops.
    HANDLE hThread = CreateThread(NULL, 0, &ThreadProc, NULL, 0, NULL);
    LogWriter::write(LogWriterLevel::verbose, "", QString("Socket server thread id: %1").arg(PtrToInt(hThread)));
    while(1)
    {
        // Check whether to stop the service.
        WaitForSingleObject(ghSvcStopEvent, INFINITE);
        ReportSvcStatus( SERVICE_STOPPED, NO_ERROR, 0 );
        LogWriter::write(LogWriterLevel::verbose, "", "DO SOMETHING installed successfully\n");
        return;
    }
}

//
// Purpose:
//   Sets the current service status and reports it to the SCM.
//
// Parameters:
//   dwCurrentState - The current state (see SERVICE_STATUS)
//   dwWin32ExitCode - The system error code
//   dwWaitHint - Estimated time for pending operation,
//     in milliseconds
//
// Return value:
//   None
//
VOID ReportSvcStatus( DWORD dwCurrentState,
                      DWORD dwWin32ExitCode,
                      DWORD dwWaitHint)
{
    static DWORD dwCheckPoint = 1;

    // Fill in the SERVICE_STATUS structure.

    gSvcStatus.dwCurrentState = dwCurrentState;
    gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
    gSvcStatus.dwWaitHint = dwWaitHint;

    if (dwCurrentState == SERVICE_START_PENDING)
        gSvcStatus.dwControlsAccepted = 0;
    else gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    if ( (dwCurrentState == SERVICE_RUNNING) ||
           (dwCurrentState == SERVICE_STOPPED) )
        gSvcStatus.dwCheckPoint = 0;
    else gSvcStatus.dwCheckPoint = dwCheckPoint++;

    // Report the status of the service to the SCM.
    SetServiceStatus( gSvcStatusHandle, &gSvcStatus );
}

//
// Purpose:
//   Called by SCM whenever a control code is sent to the service
//   using the ControlService function.
//
// Parameters:
//   dwCtrl - control code
//
// Return value:
//   None
//
VOID WINAPI SvcCtrlHandler( DWORD dwCtrl )
{
   // Handle the requested control code.

   switch(dwCtrl)
   {
      case SERVICE_CONTROL_STOP:
         ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

         // Signal the service to stop.

         SetEvent(ghSvcStopEvent);
         ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);

         return;

      case SERVICE_CONTROL_INTERROGATE:
         break;

      default:
         break;
   }

}

//
// Purpose:
//   Logs messages to the event log
//
// Parameters:
//   szFunction - name of function that failed
//
// Return value:
//   None
//
// Remarks:
//   The service must have an entry in the Application event log.
//
VOID SvcReportEvent(LPTSTR szFunction)
{
    HANDLE hEventSource;
    LPCTSTR lpszStrings[2];
    TCHAR Buffer[80];

    hEventSource = RegisterEventSource(NULL, SVCNAME);

    if( NULL != hEventSource )
    {
        StringCchPrintf(Buffer, 80, TEXT("%s failed with %d"), szFunction, GetLastError());

        lpszStrings[0] = SVCNAME;
        lpszStrings[1] = Buffer;

        ReportEvent(hEventSource,        // event log handle
                    EVENTLOG_ERROR_TYPE, // event type
                    0,                   // event category
                    EVENTLOG_ERROR_TYPE,           // event identifier
                    NULL,                // no security identifier
                    2,                   // size of lpszStrings array
                    0,                   // no binary data
                    lpszStrings,         // array of strings
                    NULL);               // no binary data

        DeregisterEventSource(hEventSource);
    }
}
