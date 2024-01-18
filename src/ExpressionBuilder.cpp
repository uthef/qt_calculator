#include "ExpressionBuilder.h"

void ExpressionBuilder::popSegments() {
    if (segments.empty()) return;
    segments.pop_back();
}

size_t ExpressionBuilder::idx() {
    return segments.size() - 1;
}

void ExpressionBuilder::appendText(const QString& str) {
    auto currentSegment = &segments[idx()];

    auto cursor = textEdit->textCursor();
    cursor.clearSelection();
    cursor.movePosition(QTextCursor::End);

    textEdit->setTextCursor(cursor);
    textEdit->insertPlainText(str);

    currentSegment->append(QString(str));
}

void ExpressionBuilder::pushNum(QString& number) {
    auto segment = Segment(NUMBER);
    bool closingBracketNeeded = false;

    if (number.startsWith('-')) {
        if (!segments.empty()) {
            closingBracketNeeded = true;
            pushOpeningBracket();
        }
        segment.flag2 = true;
    }
    if (number.contains('.')) segment.flag1 = true;

    segments.emplace_back(segment);
    appendText(number);

    if (closingBracketNeeded) pushClosingBracket();
}

ExpressionBuilder::ExpressionBuilder(QTextEdit* textEdit) {
    this->textEdit = textEdit;
    segments.emplace_back(NUMBER);
}

void ExpressionBuilder::goBack(bool keepSegment) {
    auto text = textEdit->toPlainText();
    if (text.isEmpty()) return;

    auto cursor = textEdit->textCursor();
    cursor.clearSelection();
    cursor.movePosition(QTextCursor::End);
    cursor.deletePreviousChar();

    textEdit->setTextCursor(cursor);

    auto segment = &segments[idx()];

    auto backChar = segment->back();
    segment->pop();

    if (backChar == '-' && segment->type() == OPENING_BRACKET) {
        segment->flag2 = false;
        return;
    }

    if (idx() > 0 && segment->isEmpty() && !keepSegment) {
        popSegments();

        auto ls = &segments[idx()];

        if (ls->type() == FUNCTION) {
            goBack();
        }

        if (segment->type() == CLOSING_BRACKET || segment->type() == OPENING_BRACKET) {
            if (backChar == '(') unclosedBrackets--;
            else if (backChar == ')') unclosedBrackets++;
        }

        return;
    }

    if (segment->type() == NUMBER) {
        if (backChar == '.') segment->flag1 = false;
        else if (backChar == '-') segment->flag2 = false;
        return;
    }

    while (!segment->isEmpty()) {
        goBack();
    }

}

void ExpressionBuilder::ExpressionBuilder::pushDigit(QChar digit) {
    auto segment = &segments[idx()];

    if (reachedMaxSegments() && segment->type() != NUMBER) return;

    if (segment->type() == OPERATOR || segment->type() == OPENING_BRACKET) {
        segments.emplace_back(NUMBER);
        appendText(digit);
    }
    else if (segment->type() == NUMBER) {
        int maxLength = 15 + segment->flag1 + segment->flag2;
        if (segment->length() >= maxLength) return;

        if (segment->length() - segment->flag2  == 1 && segment->back() == '0') {
            goBack(true);
        }

        appendText(digit);
    }
    else if (segment->type() == CLOSING_BRACKET) {
        pushOperator(MULT_SIGN);

        segments.emplace_back(NUMBER);
        appendText(digit);
    }
    else if (segment->type() == CONSTANT) {
        while (!segment->isEmpty()) {
            goBack();
        }

        segments.emplace_back(NUMBER);

        appendText(digit);
    }
    else return;
}

void ExpressionBuilder::pushPeriod() {
    auto segment = &segments[idx()];

    if (reachedMaxSegments() && segment->type() != NUMBER) return;

    if (segment->type() == OPERATOR || segment->type() == OPENING_BRACKET || segment->type() == CLOSING_BRACKET || ((idx() == 0 || segment->type() == NUMBER) && segment->length() - segment->flag2 == 0)) {
        if (segment->type() == CLOSING_BRACKET) {
            pushOperator(MULT_SIGN);
        }

        pushDigit('0');
        segment = &segments[idx()];

        segment->flag1 = true;
        appendText(".");

        return;
    }

    if (!segment->flag1 && segment->type() == NUMBER) {
        segment->flag1 = true;
        appendText(".");
    }
}

bool ExpressionBuilder::pushOperator(QString c) {
    if (reachedMaxSegments()) return false;

    auto lastSegment = &segments[idx()];

    if ((idx() == 0 && lastSegment->length() - lastSegment->flag2 == 0) || lastSegment->type() == OPENING_BRACKET) {
        if (c == SUB_SIGN && !lastSegment->flag2) {
            lastSegment->flag2 = true;
            appendText("-");
            return false;
        }

        return false;
    }

    if (lastSegment->type() == OPENING_BRACKET || (lastSegment->flag2 && lastSegment->length() == 1)) return false;

    auto str = QString::fromStdU32String(U" ") + c + " ";

    if (lastSegment->type() == OPERATOR || (lastSegment->type() == NUMBER && lastSegment->back() == '.')) {
        goBack();
    }

    segments.emplace_back(OPERATOR);
    appendText(str);

    return true;
}

void ExpressionBuilder::pushFunction(QString& name) {
    if (reachedMaxSegments()) return;

    auto pSegment = &segments[idx()];
    auto lastSegment = *pSegment;

    if ((idx() == 0 && lastSegment.isEmpty()) || lastSegment.type() == OPENING_BRACKET || lastSegment.type() == OPERATOR) {
        segments.emplace_back(FUNCTION);
        appendText(name);
        pushOpeningBracket();
        return;
    }

    if (lastSegment.type() == NUMBER || lastSegment.type() == CONSTANT) {
      while (!pSegment->isEmpty()) {
          goBack();
      }

      auto val = lastSegment.value();
      lastSegment.drop();

      segments.emplace_back(FUNCTION);
      appendText(name);
      pushOpeningBracket();

      segments.push_back(lastSegment);
      appendText(val);
    }
}

void ExpressionBuilder::pushConst(QString& name) {
    if (reachedMaxSegments()) return;

    auto segment = &segments[idx()];
    auto newSegment = Segment(CONSTANT);

    if ((segment->type() == NUMBER || segment->type() == CONSTANT) && !segment->isEmpty() && segment->back() != '-' ) {
        while (!segment->isEmpty()) {
            goBack();
        }

        segments.emplace_back(CONSTANT);
        appendText(name);
    }
    else if (segment->type() == OPERATOR || segment->type() == OPENING_BRACKET || (segment->type() == NUMBER && idx() == 0 && segment->length() - segment->flag2 == 0)) {
        segments.emplace_back(CONSTANT);
        appendText(name);
    }
    else if (segment->type() == CLOSING_BRACKET) {
        pushOperator(MULT_SIGN);

        segments.emplace_back(CONSTANT);
        appendText(name);
    }
}

void ExpressionBuilder::pushSmartBracket() {
    auto lastSegment = &segments[idx()];

    bool bracketAllowed = lastSegment->type() == NUMBER || lastSegment->type() == CLOSING_BRACKET || lastSegment->type() == CONSTANT;

    if (idx() > 0 && (bracketAllowed) && unclosedBrackets > 0) {
        pushClosingBracket();
        return;
    }

    pushOpeningBracket();
}

void ExpressionBuilder::pushOpeningBracket() {
    if (reachedMaxSegments()) return;

    auto lastSegment = &segments[idx()];

    if (lastSegment->type() == NUMBER || lastSegment->type() == CONSTANT || lastSegment->type() == CLOSING_BRACKET) {
        pushOperator(MULT_SIGN);
    }

    segments.emplace_back(OPENING_BRACKET);
    unclosedBrackets++;

    appendText("(");
}

void ExpressionBuilder::pushClosingBracket() {
    auto lastSegment = &segments[idx()];
    if (unclosedBrackets == 0 || lastSegment->type() == OPERATOR) return;

    if (lastSegment->type() != OPENING_BRACKET && lastSegment->length() - lastSegment->flag1 - lastSegment->flag2 > 0) {
        if (lastSegment->type() == NUMBER && lastSegment->back() == '.') goBack();

        segments.emplace_back(CLOSING_BRACKET);
        unclosedBrackets--;
        appendText(")");
    }
}

//    void pushPercent() {
//        if (reachedMaxSegments()) return;
//        if (idx() < 2) return;
//
//        auto firstOp = &segments[idx() - 2];
//        auto secondOp = &segments[idx()];
//
//        auto secondOpValue = secondOp->value();
//
//        if (firstOp->type() != NUMBER || secondOp->type() != NUMBER) return;
//
//        if (idx() >= 3) {
//            auto seg = segments[idx() - 3];
//
//            if (seg.type() == OPENING_BRACKET && seg.back() == '-') {
//                secondOpValue.prepend('-');
//            }
//        }
//
//        bool aParsed = false, bParsed = false;
//
//        auto a = firstOp->value().toDouble(&aParsed);
//        auto b = secondOpValue.toDouble(&bParsed);
//
//        if (!aParsed || !bParsed) return;
//
//        auto right = QString::number(a / 100.0 * b);
//
//        while (!secondOp->isEmpty()) {
//            goBack();
//        }
//
//        pushNum(right);
//    }

void ExpressionBuilder::pushResult(QString& number) {
    unclosedBrackets = 0;
    textEdit->clear();
    segments.clear();

    pushNum(number);
}

void ExpressionBuilder::clearText() {
    unclosedBrackets = 0;
    textEdit->clear();
    segments.clear();

    segments.emplace_back(NUMBER);
}

size_t ExpressionBuilder::segmentCount() const {
    return segments.size();
}

bool ExpressionBuilder::reachedMaxSegments() const {
    return segmentCount() >= MAX_SEGMENTS;
}