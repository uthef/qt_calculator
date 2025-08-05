#ifndef QT_CALCULATOR_EXPEVAL_SCOPE
#define QT_CALCULATOR_EXPEVAL_SCOPE

#include <vector>
#include "expeval/constant.h"
#include "expeval/function.h"
#include "expeval/operator.h"

extern "C" {
    #include "expeval/expeval.h"
}

class ExpEvalScope {
public:
    std::vector<expeval_constant> constants;
    std::vector<expeval_function> functions;
    std::vector<expeval_operator> operators;

    ExpEvalScope();
    expeval_result evaluate(const char* expr);
};

#endif
