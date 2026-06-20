#include "LoginDialog.h"

#include "../dao/UserDAO.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("系统登录");
    setFixedSize(360, 220);

    auto *titleLabel = new QLabel("林场员工排班管理系统", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #24513d;");

    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText("请输入用户名");
    m_usernameEdit->setText("admin");

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("请输入密码");
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    auto *formLayout = new QFormLayout();
    formLayout->addRow("用户名：", m_usernameEdit);
    formLayout->addRow("密码：", m_passwordEdit);

    auto *loginButton = new QPushButton("登录", this);
    loginButton->setObjectName("PrimaryButton");
    auto *exitButton = new QPushButton("退出", this);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(exitButton);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(28, 24, 28, 22);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    connect(loginButton, &QPushButton::clicked, this, [this]() { handleLogin(); });
    connect(exitButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, [this]() { handleLogin(); });
}

void LoginDialog::handleLogin()
{
    const QString username = m_usernameEdit->text().trimmed();
    const QString password = m_passwordEdit->text();
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "登录提示", "用户名和密码不能为空。");
        return;
    }

    UserDAO userDAO;
    if (userDAO.login(username, password)) {
        accept();
    } else {
        QMessageBox::warning(this, "登录失败", "用户名或密码错误。");
    }
}
