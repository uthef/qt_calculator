#include "ExpEvalScope.h"
#include <ExpressionBuilder.h>

extern "C" {
    #include "expeval/expeval.h"
    #include "expeval/math_operations.h"
}

ExpEvalScope::ExpEvalScope()
{
    constants = {
        { "PI", M_PI },
        { "E", M_E },
        EXPEVAL_CONSTANT_TERMINATOR
    };

    functions = {
        { "sqrt", [](double x) { return sqrt(x); } },
        EXPEVAL_FUNCTION_TERMINATOR
    };

    operators = {
        { SUB_SIGN, EXPEVAL_LOW_PRIORITY_DEFAULT, expeval_subtract },
        { MULT_SIGN, EXPEVAL_HIGH_PRIORITY_DEFAULT, expeval_multiply },
        { DIV_SIGN, EXPEVAL_HIGH_PRIORITY_DEFAULT, expeval_divide },
        { "^", EXPEVAL_MAX_PRIORITY, [](double a, double b) { return pow(a, b); } },
        { "%", EXPEVAL_MAX_PRIORITY - 1, [](double a, double b) { return a / 100 * b; } },
        EXPEVAL_OPERATOR_TERMINATOR
    };
}
