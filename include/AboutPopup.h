#ifndef QT_CALCULATOR_ABOUTPOPUP_H
#define QT_CALCULATOR_ABOUTPOPUP_H

#include <QDialog>
#include "../ui/ui_about_popup.h"

class AboutPopup : public QDialog {
private:
    Ui::AboutPopup layout;
public:
    explicit AboutPopup(QWidget* parent);
    void done(int i) override;
};


#endif //QT_CALCULATOR_ABOUTPOPUP_H
