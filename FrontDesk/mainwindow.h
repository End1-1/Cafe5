#ifdef FRONTDESK
class C5MainWindow;

extern C5MainWindow* __mainWindow;
#else
class QDialog;
extern QDialog* __mainWindow;
#endif
