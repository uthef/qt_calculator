#include "Segment.h"

Segment::Segment(SegmentType type) : _type(type) {

}

Segment::Segment(SegmentType type, QChar qchar) : _type(type) {
    str.append(qchar);
}

Segment::Segment(SegmentType type, QString& string) : _type(type) {
    str = QString(string);
}

void Segment::pop() {
    if (str.isEmpty()) return;
    str.erase(str.end() - 1);
}

void Segment::append(QString part) {
    str.append(part);
}

qsizetype Segment::length() {
    return str.length();
}

bool Segment::isEmpty() {
    return str.isEmpty();
}

QChar Segment::back() {
    return str.back();
}

QString Segment::value() {
    return str;
}

void Segment::drop() {
    str = "";
}

SegmentType Segment::type() {
    return _type;
}
