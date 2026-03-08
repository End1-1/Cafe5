#include "struct_fiscal_machine.h"
QList<FiscalMachine> fiscalMachines;
FiscalMachine getFiscalMachine(int id)
{
    for (auto const &fm : fiscalMachines) {
        if (fm.id == id) {
            return fm;
        }
    }
    return FiscalMachine();
}
