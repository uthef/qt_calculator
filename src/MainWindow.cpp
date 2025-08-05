#include <QDialog>
#include "MainWindow.h"
#include "ExpressionBuilder.h"
#include <QWidgetList>
#include <QStringListModel>
#include "AboutPopup.h"
#include "MainWindowUtils.h"

extern "C" {
    #include "expeval/expeval.h"
}

MainWindow::MainWindow(QWidget* parent, Configuration& storage) : QMainWindow(parent), form(), builder(form.exp_field), config(storage) {
    this->form.setupUi(this);

    form.exp_field->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    form.exp_field->document()->setDocumentMargin(12);

    builder = ExpressionBuilder(form.exp_field);

    loadConfiguration();
    loadHistory();
    connectButtons();
}

void MainWindow::loadConfiguration() {
    currentFormat = config.exactNumber ? 'f' : 'g';
    form.actionUpdateNumberFormat->setChecked(config.exactNumber);

    calculate();
}

void MainWindow::loadHistory() {
    form.history->clear();

    for (int i = (int)config.history.size() - 1; i >= 0; i--) {
        auto entry = config.history[i].toObject();

        if (entry.empty() || !entry.contains(EXPRESSION_KEY) || !entry.contains(RESULT_KEY)) continue;

        auto exp = entry[EXPRESSION_KEY].toString().toStdString();
        form.history->addItem(formatEntry(exp, entry[RESULT_KEY].toDouble(0)));
    }
}

void MainWindow::connectButtons() {
    auto gridLayout = this->form.gridLayout;

    for (int i = 0; i < gridLayout->count(); i++) {
        auto widget = gridLayout->itemAt(i)->widget();
        auto *button = dynamic_cast<QPushButton *>(widget);

        if (!button) continue;

        QPushButton::connect(button, &QPushButton::released, [this, button]() {
            buttonReleasedEvent(button);
        });
    }
}

void MainWindow::calculate() {
    auto expression = form.exp_field->toPlainText();

    expeval_context context;
    context.constants = scope.constants.data();
    context.functions = scope.functions.data();
    context.operators = scope.operators.data();

    expeval_result result = expeval(expression.toStdString().c_str(), &context);

    if (result.code == EXPEVAL_OK) {
        lastDouble = result.value;
        lastResult = QString::number(lastDouble, abs(lastDouble) > 1e20 ? 'g' : currentFormat, 7);
        form.result_label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        form.result_label->setForegroundRole(QPalette::Text);

        if (lastResult == "inf") {
            form.result_label->setText("∞");
            return;
        }

        if (lastResult == "nan") {
            form.result_label->setText("?");
            return;
        }

        form.result_label->setText(lastResult);

        return;
    }

    lastResult = QString();

    const char* msg = 0;

    switch(result.code) {
        case EXPEVAL_HANGING_OPERATOR:
        case EXPEVAL_BRACKET_NOT_CLOSED:
            msg = "Unfinished expression";
            break;
        default:
            msg = "Invalid expression";
            break;
        case EXPEVAL_INTERNAL_ERROR:
        case EXPEVAL_UNKNOWN:
        case EXPEVAL_MEMORY_ALLOCATION_IMPOSSIBLE:
            msg = "Program error";
            break;
    }

    form.result_label->setText(msg);
    form.result_label->setAlignment(Qt::AlignCenter);
    form.result_label->setForegroundRole(QPalette::Accent);
}

void MainWindow::buttonReleasedEvent(QPushButton* button) {
    auto name = button->objectName();
    QString btText = button->text();

    if (name.startsWith("back")) builder.goBack();
    else if (name.startsWith("clear")) builder.clearText();
    else if (name.startsWith("num")) builder.pushDigit(btText.back());
    else if (name.startsWith("period")) builder.pushPeriod();
    else if (name.startsWith("op")) {
        btText.replace("−", "-"); // replacing long dash
        builder.pushOperator(btText.back());
    }
    else if (name.startsWith("bracket")) builder.pushSmartBracket();
    else if (name.startsWith("pow")) {
        bool pushed = builder.pushOperator("^");

        if (name.endsWith("2_bt") && pushed)
            builder.pushDigit('2');
    }
    else if (name.startsWith("eq")) {
        pushResult();
    }
    else if (name.startsWith("sqrt")) {
        auto funcName = QString("sqrt");
        builder.pushFunction(funcName);
    }
    else if (name.startsWith("pi")) {
        auto constName = QString("PI");
        builder.pushConst(constName);
    }
    else if (name.startsWith("e")) {
        auto constName = QString("E");
        builder.pushConst(constName);
    }
    else return;

    calculate();
}

void MainWindow::pushResult() {
    if (lastResult.isEmpty() || lastResult == "inf" || lastResult == "nan") return;

    if (builder.segmentCount() > 1) {
        auto exp = form.exp_field->toPlainText().toStdString();
        auto popped = config.pushHistoryEntry(exp, lastDouble);

        if (popped) {
            form.history->takeItem(MAX_HISTORY_LENGTH - 1);
        }

        form.history->insertItem(0, formatEntry(exp, lastDouble));
        config.save();
    }

    builder.pushResult(lastResult);
}

void MainWindow::processKey(int key, QString text, Qt::KeyboardModifiers modifiers) {
    if (key == Qt::Key_Backspace && modifiers == Qt::NoModifier) {
        builder.goBack();
        calculate();
        return;
    }

    if (key == Qt::Key_Delete && modifiers == Qt::NoModifier) {
        builder.clearText();
        calculate();
        return;
    }

    if (text.isEmpty()) return;

    QChar backChar = text.back();
    char latinBackChar = backChar.toLatin1();

    if (isdigit(latinBackChar)) {
        builder.pushDigit(text.back());
        calculate();
        return;
    }

    switch (latinBackChar) {
        case '.':
            builder.pushPeriod();
            calculate();
            break;
        case '+':
        case '^':
        case '%':
            builder.pushOperator(backChar);
            calculate();
            break;
        case '-':
            builder.pushOperator("˗");
            calculate();
            break;
        case '*':
            builder.pushOperator("×");
            calculate();
            break;
        case '/':
            builder.pushOperator("÷");
            calculate();
            break;
        case '(':
            builder.pushOpeningBracket();
            calculate();
            break;
        case ')':
            builder.pushClosingBracket();
            calculate();
            break;
        case '=':
            pushResult();
            break;
        default:
            break;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)  {
    processKey(event->key(), event->text(), event->modifiers());
}

// SLOTS
void MainWindow::clearHistory() {
    form.history->clear();
    config.history = QJsonArray();
    config.save();
}

void MainWindow::updateNumberFormat(bool checked)  {
    currentFormat = checked ? 'f' : 'g';
    calculate();

    config.exactNumber = checked;
    config.save();

    loadHistory();
}

void MainWindow::showAboutModal() {
    auto dialog = new AboutPopup(this);
    dialog->show();
}

void MainWindow::historyItemSelected(QListWidgetItem *item) {
    auto text = item->text();
    auto eqSignIndex = text.indexOf('=');
    if (eqSignIndex == -1) eqSignIndex = text.size() - 1;

    builder.clearText();

    auto slice  = text.sliced(0, eqSignIndex)
            .replace(SUB_SIGN, "-")
            .replace(MULT_SIGN, "*")
            .replace(DIV_SIGN, "/");


    for (qsizetype i = 0; i < slice.size(); i++) {
        auto& c = slice[i];
        QString id = "";

        while (isalpha(c.toLatin1()) && i < slice.size()) {
            id.push_back(c);
            c = slice[++i];
        }

        if (!id.isEmpty()) {
            if (i < slice.size() - 1 && slice[i] == '(') {
                builder.pushFunction(id);
            }
            else {
                builder.pushConst(id);
                i--;
            }

            continue;
        }

        processKey(c.toLatin1(), c);
    }

    calculate();
}
