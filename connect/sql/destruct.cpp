
#include "Table.h"
#include "SelectStatement.h"

namespace tsql {


    TableRef::~TableRef() {
        delete name;
        delete alias;
        delete select;
        delete list;
    }


} // namespace tsql
