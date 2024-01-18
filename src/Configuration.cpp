#include "Configuration.h"

#include <QDebug>
#include <QStandardPaths>
#include <QFile>
#include <QDir>

Configuration::Configuration() {
    auto locations = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    this->fullPath = locations.first();

    QByteArray jsonBytes;

    dirPath = fullPath;

    makeSureDirExists();

    if (fullPath.back() != '/') {
        fullPath.append("/");
    }

    fullPath.append("data.json");

    QFile file(fullPath);

    file.open(QIODeviceBase::ReadWrite);
    jsonBytes = file.readAll();
    file.close();

    jsonMap = QJsonDocument::fromJson(jsonBytes).object();

    this->exactNumber = jsonMap[EXACT_NUMBER_KEY].toVariant().toBool();
    this->history = jsonMap[HISTORY_KEY].toArray();
}

void Configuration::save() {
    jsonMap[EXACT_NUMBER_KEY] = exactNumber;
    jsonMap[HISTORY_KEY] = history;

    auto document = QJsonDocument(jsonMap);

    makeSureDirExists();

    QFile file(fullPath);
    file.open(QIODeviceBase::WriteOnly);

    file.write(document.toJson(QJsonDocument::Compact));

    file.flush();
    file.close();
}

bool Configuration::pushHistoryEntry(std::string& expression, double result) {
    bool popped = false;
    auto entry = QJsonObject();
    entry[EXPRESSION_KEY] = expression.c_str();
    entry[RESULT_KEY] = result;

    if (history.size() >= MAX_HISTORY_LENGTH) {
        history.pop_front();
        popped = true;
    }

    history.push_back(entry);
    return popped;
}

void Configuration::makeSureDirExists() {
    QDir dir;

    if (!dir.exists(dirPath)) {
        dir.mkdir(dirPath);
    }
}