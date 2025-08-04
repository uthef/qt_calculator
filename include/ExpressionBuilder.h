#ifndef QT_CALCULATOR_EXPRESSIONBUILDER_H
#define QT_CALCULATOR_EXPRESSIONBUILDER_H

#include <QTextEdit>
#include <vector>
#include "Segment.h"

static int MAX_SEGMENTS = 150;
static const char* SUB_SIGN = "˗";
static const char* DIV_SIGN = "÷";
static const char* MULT_SIGN = "×";

class ExpressionBuilder {
private:
    QTextEdit* textEdit;
    std::vector<Segment> segments;
    size_t unclosedBrackets = 0;

    void popSegments();
    size_t idx();
    void appendText(const QString& str);
    void pushNum(QString& number);
    bool reachedMaxSegments() const;

public:
    explicit ExpressionBuilder(QTextEdit* textEdit);
    void goBack(bool keepSegment = false);
    void pushDigit(QChar digit);
    void pushPeriod();
    bool pushOperator(QString c);
    void pushFunction(QString& name);
    void pushConst(QString& name);
    void pushSmartBracket();
    void pushOpeningBracket();
    void pushClosingBracket();
    void pushResult(QString& number);
    void clearText();
    size_t segmentCount() const;
};

#endif //QT_CALCULATOR_EXPRESSIONBUILDER_H
