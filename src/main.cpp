#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    Configuration storage;

    QApplication a(argc, argv);
    a.setStyle("Fusion");
    
    QApplication::setWindowIcon(QIcon(":/images/icon256.png"));

    MainWindow window(nullptr, storage);
    window.show();

    return QApplication::exec();
}
