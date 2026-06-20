#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QPushButton;
class QStackedWidget;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QStackedWidget *m_stack = nullptr;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_subTitleLabel = nullptr;

    QPushButton *createNavButton(const QString &text, QWidget *parent);
    void switchPage(int index, const QString &title, const QString &subtitle);
};

#endif
