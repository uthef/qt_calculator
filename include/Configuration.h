#ifndef QT_CALCULATOR_CONFIGURATION_H
#define QT_CALCULATOR_CONFIGURATION_H

#include <string>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

using std::string;

static const char* EXPRESSION_KEY = "exp";
static const char* RESULT_KEY = "res";
static const char* HISTORY_KEY = "history";
static const char* EXACT_NUMBER_KEY = "exactNumber";
static const unsigned int MAX_HISTORY_LENGTH = 20;

class Configuration {
private:
    QJsonObject jsonMap;
    QString fullPath;
    QString dirPath;

    void makeSureDirExists();
public:
    QJsonArray history;
    bool exactNumber;
    Configuration();
    void save();
    bool pushHistoryEntry(string& expression, double result);
};


#endif //QT_CALCULATOR_CONFIGURATION_H
