#ifndef QT_CALCULATOR_SEGMENT_H
#define QT_CALCULATOR_SEGMENT_H

#include "SegmentType.h"
#include <QString>

struct Segment {
private:
    SegmentType _type;
    QString str;
public:
    bool flag1 = false;
    bool flag2 = false;

    explicit Segment(SegmentType type) : _type(type) {

    }

    Segment(SegmentType type, QChar qchar) : _type(type) {
        str.append(qchar);
    }

    Segment(SegmentType type, QString& string) : _type(type) {
        str = QString(string);
    }

    void pop() {
        if (str.isEmpty()) return;

        str.erase(str.end() - 1);
    }

    void append(QString part) {
        str.append(part);
    }

    qsizetype length() {
        return str.length();
    }

    bool isEmpty() {
        return str.isEmpty();
    }

    QChar back() {
        return str.back();
    }

    QString value() {
        return str;
    }

    void drop() {
        str = "";
    }

    SegmentType type() {
        return _type;
    }
};

#endif //QT_CALCULATOR_SEGMENT_H
