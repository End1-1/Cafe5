#ifndef PROXYTABLEWIDGETDATABASE_H
#define PROXYTABLEWIDGETDATABASE_H


class C5TableWidget;
class C5Database;

class ProxyTableWidgetDatabase
{
public:
    ProxyTableWidgetDatabase();
    static void fillTableWidgetRowFromDatabase(C5Database *db, C5TableWidget *t);
};

#endif // PROXYTABLEWIDGETDATABASE_H
