#ifndef QT_CALCULATOR_MAINWINDOWUTILS_H
#define QT_CALCULATOR_MAINWINDOWUTILS_H

#include <QString>
#include <string>
#include <streval.h>
#include <ExpressionBuilder.h>

using std::string;

static QString formatEntry(string& exp, double res)  {
    QString str;
    str.append(exp);
    str.append(" = ");
    str.append(QString::number(res, 'g'));

    return str;
}

static void initializeScope(uthef::scope& scope) {
    scope.put(U'+', [](double a, double b) { return a + b; }, 0);
    scope.put(SUB_SIGN.toStdU32String().back(), [](double a, double b) { return a - b; }, 0);
    scope.put(MULT_SIGN.toStdU32String().back(), [](double a, double b) { return a * b; }, 1);
    scope.put(DIV_SIGN.toStdU32String().back(), [](double a, double b) { return a / b; }, 1);
    scope.put(U'^', [](double a, double b) { return pow(a, b); }, 2);
    scope.put(U'%', [](double a, double b) { return a / 100 * b; }, 2);

    scope.put("sqrt", [](double x) { return sqrt(x); });

    scope.put("PI", M_PI);
    scope.put("E", M_E);
}

#endif //QT_CALCULATOR_MAINWINDOWUTILS_H
