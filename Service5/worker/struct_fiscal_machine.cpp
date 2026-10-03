#include "struct_fiscal_machine.h"
QList<FiscalMachine> fiscalMachines;
FiscalMachine getFiscalMachine(int id)
{
    FiscalMachine found;
    for (auto const &fm : fiscalMachines) {
        if (fm.id == id) {
            found = fm; // last match wins (reload may briefly leave duplicates)
        }
    }
    return found;
}
