#include "StatisticsWidget.h"

#include "../database/EmployeeComboHelper.h"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QMap>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QAbstractItemView>

StatisticsWidget::StatisticsWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);

    auto *topBox = new QGroupBox("统计条件", this);
    auto *topLayout = new QGridLayout(topBox);
    m_employeeCombo = new QComboBox(topBox);
    m_dateEdit = new QDateEdit(QDate::currentDate(), topBox);
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("yyyy-MM-dd");

    m_totalLabel = new QLabel("员工总人数：0", topBox);
    m_activeLabel = new QLabel("在职员工人数：0", topBox);

    auto *attendanceStatsButton = new QPushButton("统计员工出勤", topBox);
    attendanceStatsButton->setObjectName("PrimaryButton");
    auto *weekScheduleButton = new QPushButton("查询周排班总览", topBox);
    auto *dateAttendanceButton = new QPushButton("查询当天出勤", topBox);
    auto *refreshButton = new QPushButton("刷新统计", topBox);

    topLayout->addWidget(new QLabel("员工：", topBox), 0, 0);
    topLayout->addWidget(m_employeeCombo, 0, 1);
    topLayout->addWidget(new QLabel("日期：", topBox), 0, 2);
    topLayout->addWidget(m_dateEdit, 0, 3);
    topLayout->addWidget(m_totalLabel, 1, 0);
    topLayout->addWidget(m_activeLabel, 1, 1);
    topLayout->addWidget(attendanceStatsButton, 1, 2);
    topLayout->addWidget(weekScheduleButton, 1, 3);
    topLayout->addWidget(dateAttendanceButton, 1, 4);
    topLayout->addWidget(refreshButton, 1, 5);

    m_resultEdit = new QTextEdit(this);
    m_resultEdit->setReadOnly(true);
    m_resultEdit->setFixedHeight(120);

    m_table = new QTableWidget(this);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);

    mainLayout->addWidget(topBox);
    mainLayout->addWidget(m_resultEdit);
    mainLayout->addWidget(m_table, 1);

    connect(attendanceStatsButton, &QPushButton::clicked, this, [this]() { showEmployeeAttendanceStats(); });
    connect(weekScheduleButton, &QPushButton::clicked, this, [this]() { showWeekSchedules(); });
    connect(dateAttendanceButton, &QPushButton::clicked, this, [this]() { showDateAttendance(); });
    connect(refreshButton, &QPushButton::clicked, this, [this]() {
        loadEmployees();
        refreshCounts();
        refreshData();
    });

    loadEmployees();
    refreshCounts();
    refreshData();
}

void StatisticsWidget::loadEmployees()
{
    EmployeeComboHelper::loadEmployeesInto(m_employeeCombo, m_employeeDAO);
}

void StatisticsWidget::refreshCounts()
{
    m_totalLabel->setText("员工总人数：" + QString::number(m_employeeDAO.countAllEmployees()));
    m_activeLabel->setText("在职员工人数：" + QString::number(m_employeeDAO.countActiveEmployees()));
}

void StatisticsWidget::refreshData()
{
    switch (m_viewMode) {
    case ViewMode::EmployeeAttendance:
        showEmployeeAttendanceStats();
        break;
    case ViewMode::WeekSchedules:
        showWeekSchedules();
        break;
    case ViewMode::DateAttendance:
        showDateAttendance();
        break;
    }
}

void StatisticsWidget::showEmployeeAttendanceStats()
{
    m_viewMode = ViewMode::EmployeeAttendance;
    const int employeeId = m_employeeCombo->currentData().toInt();
    const QString employeeText = m_employeeCombo->currentText();
    QMap<QString, int> stats = m_attendanceService.countAttendanceByEmployee(employeeId);

    m_resultEdit->setText(QString("员工：%1\n正常：%2 次，迟到：%3 次，早退：%4 次，请假：%5 次，缺勤：%6 次")
                              .arg(employeeText)
                              .arg(stats.value("正常"))
                              .arg(stats.value("迟到"))
                              .arg(stats.value("早退"))
                              .arg(stats.value("请假"))
                              .arg(stats.value("缺勤")));

    m_table->setColumnCount(2);
    m_table->setHorizontalHeaderLabels({"出勤状态", "次数"});
    m_table->setRowCount(5);
    const QStringList statuses = {"正常", "迟到", "早退", "请假", "缺勤"};
    for (int row = 0; row < statuses.size(); ++row) {
        m_table->setItem(row, 0, new QTableWidgetItem(statuses[row]));
        m_table->setItem(row, 1, new QTableWidgetItem(QString::number(stats.value(statuses[row]))));
    }
}

void StatisticsWidget::showWeekSchedules()
{
    m_viewMode = ViewMode::WeekSchedules;
    QDate weekStart = m_dateEdit->date().addDays(1 - m_dateEdit->date().dayOfWeek());
    const QList<Schedule> schedules = m_scheduleService.getSchedulesByWeek(weekStart);

    m_resultEdit->setText("周排班总览：" + weekStart.toString("yyyy-MM-dd") + " 至 " + weekStart.addDays(6).toString("yyyy-MM-dd"));
    m_table->setColumnCount(10);
    m_table->setHorizontalHeaderLabels({"员工编号", "姓名", "周一", "周二", "周三", "周四", "周五", "周六", "周日", "周开始日期"});
    m_table->setRowCount(schedules.size());
    for (int row = 0; row < schedules.size(); ++row) {
        const Schedule &schedule = schedules[row];
        m_table->setItem(row, 0, new QTableWidgetItem(schedule.employeeNo));
        m_table->setItem(row, 1, new QTableWidgetItem(schedule.employeeName));
        m_table->setItem(row, 2, new QTableWidgetItem(schedule.mondayShift));
        m_table->setItem(row, 3, new QTableWidgetItem(schedule.tuesdayShift));
        m_table->setItem(row, 4, new QTableWidgetItem(schedule.wednesdayShift));
        m_table->setItem(row, 5, new QTableWidgetItem(schedule.thursdayShift));
        m_table->setItem(row, 6, new QTableWidgetItem(schedule.fridayShift));
        m_table->setItem(row, 7, new QTableWidgetItem(schedule.saturdayShift));
        m_table->setItem(row, 8, new QTableWidgetItem(schedule.sundayShift));
        m_table->setItem(row, 9, new QTableWidgetItem(schedule.weekStartDate.toString("yyyy-MM-dd")));
    }
}

void StatisticsWidget::showDateAttendance()
{
    m_viewMode = ViewMode::DateAttendance;
    const QList<Attendance> records = m_attendanceService.getAttendanceByDate(m_dateEdit->date());

    m_resultEdit->setText("当天出勤情况：" + m_dateEdit->date().toString("yyyy-MM-dd") +
                          "\n记录数量：" + QString::number(records.size()));
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"员工编号", "姓名", "日期", "状态", "备注"});
    m_table->setRowCount(records.size());
    for (int row = 0; row < records.size(); ++row) {
        const Attendance &attendance = records[row];
        m_table->setItem(row, 0, new QTableWidgetItem(attendance.employeeNo));
        m_table->setItem(row, 1, new QTableWidgetItem(attendance.employeeName));
        m_table->setItem(row, 2, new QTableWidgetItem(attendance.attendanceDate.toString("yyyy-MM-dd")));
        m_table->setItem(row, 3, new QTableWidgetItem(attendance.status));
        m_table->setItem(row, 4, new QTableWidgetItem(attendance.remark));
    }
}
