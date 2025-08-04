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

    explicit Segment(SegmentType type);
    Segment(SegmentType type, QChar qchar);
    Segment(SegmentType type, QString& string);

    void pop();
    void append(QString part);
    qsizetype length();
    bool isEmpty();
    QChar back();
    QString value();
    void drop();
    SegmentType type();
};

#endif //QT_CALCULATOR_SEGMENT_H
