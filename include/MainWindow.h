#ifndef QT_CALCULATOR_MAINWINDOW_H
#define QT_CALCULATOR_MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include "ExpressionBuilder.h"
#include <QPushButton>
#include "../ui/ui_calculator_form.h"
#include "Configuration.h"
#include "ExpEvalScope.h"

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    explicit MainWindow(QWidget* parent, Configuration& storage);
private slots:
    void updateNumberFormat(bool checked);
    void showAboutModal();
    void clearHistory();
    void historyItemSelected(QListWidgetItem* item);
private:
    Ui_CalculatorForm form;
    Configuration config;
    QString lastResult = "0";
    ExpEvalScope scope;
    double lastDouble = 0;
    ExpressionBuilder builder;
    char currentFormat = 'g';

    void connectButtons();
    void processKey(int key, QString text = "", Qt::KeyboardModifiers modifiers = Qt::NoModifier);
    void calculate();
    void buttonReleasedEvent(QPushButton* button);
    void pushResult();
    void keyPressEvent(QKeyEvent *event) override;
    void loadConfiguration();
    void loadHistory();
};

#endif //QT_CALCULATOR_MAINWINDOW_H
