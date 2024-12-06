#include "Smarttrafficsystem.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    SmartTrafficSystem system;
    system.setWindowTitle("Smart Traffic System");
    system.show();

    return app.exec();
}
