#include "MainWindow.h"

#include "AttendanceWidget.h"
#include "DashboardWidget.h"
#include "EmployeeWidget.h"
#include "ScheduleWidget.h"
#include "StatisticsWidget.h"
#include "../database/DatabaseManager.h"

#include <QButtonGroup>
#include <QDateTime>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("林场员工排班管理系统");
    resize(1180, 720);

    auto *central = new QWidget(this);
    auto *rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    auto *navWidget = new QWidget(central);
    navWidget->setObjectName("NavPane");
    navWidget->setFixedWidth(210);
    auto *navLayout = new QVBoxLayout(navWidget);
    navLayout->setContentsMargins(0, 24, 0, 18);
    navLayout->setSpacing(2);

    auto *brandTitle = new QLabel("林场排班系统", navWidget);
    brandTitle->setObjectName("BrandTitle");
    auto *brandSubTitle = new QLabel("Forest Schedule Console", navWidget);
    brandSubTitle->setObjectName("BrandSubTitle");

    auto *brandLayout = new QVBoxLayout();
    brandLayout->setContentsMargins(20, 0, 18, 18);
    brandLayout->addWidget(brandTitle);
    brandLayout->addWidget(brandSubTitle);
    navLayout->addLayout(brandLayout);

    auto *dashboardButton = createNavButton("系统首页", navWidget);
    auto *employeeButton = createNavButton("员工管理", navWidget);
    auto *scheduleButton = createNavButton("周排班管理", navWidget);
    auto *attendanceButton = createNavButton("出勤管理", navWidget);
    auto *statisticsButton = createNavButton("统计查询", navWidget);
    auto *exitButton = createNavButton("退出系统", navWidget);
    exitButton->setCheckable(false);

    auto *buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);
    buttonGroup->addButton(dashboardButton, 0);
    buttonGroup->addButton(employeeButton, 1);
    buttonGroup->addButton(scheduleButton, 2);
    buttonGroup->addButton(attendanceButton, 3);
    buttonGroup->addButton(statisticsButton, 4);

    navLayout->addWidget(dashboardButton);
    navLayout->addWidget(employeeButton);
    navLayout->addWidget(scheduleButton);
    navLayout->addWidget(attendanceButton);
    navLayout->addWidget(statisticsButton);
    navLayout->addStretch();
    navLayout->addWidget(exitButton);

    auto *contentWidget = new QWidget(central);
    auto *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    auto *topBar = new QWidget(contentWidget);
    topBar->setObjectName("TopBar");
    topBar->setFixedHeight(78);
    auto *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(24, 10, 24, 10);

    auto *titleLayout = new QVBoxLayout();
    titleLayout->setSpacing(4);
    m_titleLabel = new QLabel("系统首页", topBar);
    m_titleLabel->setObjectName("PageTitle");
    m_subTitleLabel = new QLabel("查看员工、排班和出勤的核心概览", topBar);
    m_subTitleLabel->setObjectName("PageSubTitle");
    titleLayout->addWidget(m_titleLabel);
    titleLayout->addWidget(m_subTitleLabel);

    const QString connectionText = DatabaseManager::instance().isOpen() ? "数据库：已连接" : "数据库：未连接";
    auto *statusLabel = new QLabel(connectionText + "    " + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm"), topBar);
    statusLabel->setObjectName("PageSubTitle");
    statusLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    topLayout->addLayout(titleLayout, 1);
    topLayout->addWidget(statusLabel);

    m_stack = new QStackedWidget(contentWidget);
    m_stack->addWidget(new DashboardWidget(m_stack));
    m_stack->addWidget(new EmployeeWidget(m_stack));
    m_stack->addWidget(new ScheduleWidget(m_stack));
    m_stack->addWidget(new AttendanceWidget(m_stack));
    m_stack->addWidget(new StatisticsWidget(m_stack));

    contentLayout->addWidget(topBar);
    contentLayout->addWidget(m_stack, 1);

    rootLayout->addWidget(navWidget);
    rootLayout->addWidget(contentWidget, 1);
    setCentralWidget(central);

    connect(dashboardButton, &QPushButton::clicked, this, [this]() { switchPage(0, "系统首页", "查看员工、排班和出勤的核心概览"); });
    connect(employeeButton, &QPushButton::clicked, this, [this]() { switchPage(1, "员工管理", "维护员工档案、岗位和所属林区信息"); });
    connect(scheduleButton, &QPushButton::clicked, this, [this]() { switchPage(2, "周排班管理", "按员工和周次维护早班、巡山、值班等班次"); });
    connect(attendanceButton, &QPushButton::clicked, this, [this]() { switchPage(3, "出勤管理", "记录和查询员工每日出勤状态"); });
    connect(statisticsButton, &QPushButton::clicked, this, [this]() { switchPage(4, "统计查询", "汇总人员规模、排班和出勤统计"); });
    connect(exitButton, &QPushButton::clicked, this, [this]() {
        if (QMessageBox::question(this, "退出确认", "确定要退出系统吗？") == QMessageBox::Yes) {
            DatabaseManager::instance().close();
            close();
        }
    });

    dashboardButton->setChecked(true);
    switchPage(0, "系统首页", "查看员工、排班和出勤的核心概览");
}

QPushButton *MainWindow::createNavButton(const QString &text, QWidget *parent)
{
    auto *button = new QPushButton(text, parent);
    button->setObjectName("NavButton");
    button->setCheckable(true);
    return button;
}

void MainWindow::switchPage(int index, const QString &title, const QString &subtitle)
{
    m_stack->setCurrentIndex(index);
    m_titleLabel->setText(title);
    m_subTitleLabel->setText(subtitle);

    if (index == 0) {
        static_cast<DashboardWidget *>(m_stack->widget(0))->refreshData();
    }
}
