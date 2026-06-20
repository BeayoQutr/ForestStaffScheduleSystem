#include <QApplication>
#include <QMessageBox>

#include "database/DatabaseManager.h"
#include "ui/AppStyle.h"
#include "ui/LoginDialog.h"
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("林场员工排班管理系统");
    app.setStyleSheet(AppStyle::styleSheet());

    if (!DatabaseManager::instance().connect()) {
        return 1;
    }

    LoginDialog login;
    if (login.exec() == QDialog::Accepted) {
        MainWindow mainWindow;
        mainWindow.show();
        return app.exec();
    }

    DatabaseManager::instance().close();
    return 0;
}
