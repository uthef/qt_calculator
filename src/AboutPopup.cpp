#include "AboutPopup.h"

AboutPopup::AboutPopup(QWidget* parent) : QDialog(parent), layout() {
    layout.setupUi(this);
}

void AboutPopup::done(int i) {
    deleteLater();
    QDialog::done(i);
}
