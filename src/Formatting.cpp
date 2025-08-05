#include "Formatting.h"
#include <ExpressionBuilder.h>

extern "C" {
    #include "expeval/math_operations.h"
}

QString formatEntry(std::string& exp, double res)  {
    QString str;
    str.append(exp);
    str.append(" = ");
    str.append(QString::number(res, 'g'));

    return str;
}
