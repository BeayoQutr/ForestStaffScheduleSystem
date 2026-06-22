#include "DashboardWidget.h"
#include "../model/ScheduleWeek.h"

#include <QDate>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QtGlobal>
#include <QVBoxLayout>
#include <QAbstractItemView>

DashboardWidget::DashboardWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(22, 20, 22, 22);
    mainLayout->setSpacing(16);

    auto *summaryLayout = new QGridLayout();
    summaryLayout->setSpacing(14);
    summaryLayout->addWidget(createStatCard("员工总人数", &m_totalEmployeesValue), 0, 0);
    summaryLayout->addWidget(createStatCard("在职员工", &m_activeEmployeesValue), 0, 1);
    summaryLayout->addWidget(createStatCard("今日出勤记录", &m_todayAttendanceValue), 0, 2);
    summaryLayout->addWidget(createStatCard("本周排班记录", &m_weekScheduleValue), 0, 3);

    auto *tableLayout = new QGridLayout();
    tableLayout->setSpacing(16);

    m_scheduleTable = new QTableWidget(this);
    m_scheduleTable->setColumnCount(9);
    m_scheduleTable->setHorizontalHeaderLabels({"员工编号", "姓名", "周一", "周二", "周三", "周四", "周五", "周六", "周日"});
    m_scheduleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_scheduleTable->verticalHeader()->setVisible(false);
    m_scheduleTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_scheduleTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_scheduleTable->setAlternatingRowColors(true);

    m_attendanceTable = new QTableWidget(this);
    m_attendanceTable->setColumnCount(4);
    m_attendanceTable->setHorizontalHeaderLabels({"员工编号", "姓名", "状态", "备注"});
    m_attendanceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_attendanceTable->verticalHeader()->setVisible(false);
    m_attendanceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_attendanceTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_attendanceTable->setAlternatingRowColors(true);

    auto *scheduleBox = new QGroupBox("本周排班速览", this);
    auto *scheduleLayout = new QVBoxLayout(scheduleBox);
    scheduleLayout->addWidget(m_scheduleTable);

    auto *attendanceBox = new QGroupBox("今日出勤速览", this);
    auto *attendanceLayout = new QVBoxLayout(attendanceBox);
    attendanceLayout->addWidget(m_attendanceTable);

    tableLayout->addWidget(scheduleBox, 0, 0);
    tableLayout->addWidget(attendanceBox, 0, 1);

    mainLayout->addLayout(summaryLayout);
    mainLayout->addLayout(tableLayout, 1);

    refreshData();
}

QFrame *DashboardWidget::createStatCard(const QString &name, QLabel **valueLabel)
{
    auto *card = new QFrame(this);
    card->setObjectName("StatCard");
    card->setMinimumHeight(108);

    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 14, 18, 14);
    layout->setSpacing(8);

    auto *nameLabel = new QLabel(name, card);
    nameLabel->setObjectName("StatName");

    *valueLabel = new QLabel("0", card);
    (*valueLabel)->setObjectName("StatValue");

    layout->addWidget(nameLabel);
    layout->addWidget(*valueLabel);
    layout->addStretch();
    return card;
}

void DashboardWidget::refreshData()
{
    const QDate today = QDate::currentDate();
    const QDate weekStart = ScheduleWeek::startDate(today);
    const QList<Attendance> todayRecords = m_attendanceService.getAttendanceByDate(today);
    const QList<Schedule> weekSchedules = m_scheduleService.getSchedulesByWeek(weekStart);

    m_totalEmployeesValue->setText(QString::number(m_employeeDAO.countAllEmployees()));
    m_activeEmployeesValue->setText(QString::number(m_employeeDAO.countActiveEmployees()));
    m_todayAttendanceValue->setText(QString::number(todayRecords.size()));
    m_weekScheduleValue->setText(QString::number(weekSchedules.size()));

    fillScheduleTable(weekSchedules);
    fillAttendanceTable(todayRecords);
}

void DashboardWidget::fillScheduleTable(const QList<Schedule> &schedules)
{
    const int rowCount = qMin(schedules.size(), 8);
    m_scheduleTable->setRowCount(rowCount);
    for (int row = 0; row < rowCount; ++row) {
        const Schedule &schedule = schedules[row];
        m_scheduleTable->setItem(row, 0, new QTableWidgetItem(schedule.employeeNo));
        m_scheduleTable->setItem(row, 1, new QTableWidgetItem(schedule.employeeName));
        m_scheduleTable->setItem(row, 2, new QTableWidgetItem(schedule.mondayShift));
        m_scheduleTable->setItem(row, 3, new QTableWidgetItem(schedule.tuesdayShift));
        m_scheduleTable->setItem(row, 4, new QTableWidgetItem(schedule.wednesdayShift));
        m_scheduleTable->setItem(row, 5, new QTableWidgetItem(schedule.thursdayShift));
        m_scheduleTable->setItem(row, 6, new QTableWidgetItem(schedule.fridayShift));
        m_scheduleTable->setItem(row, 7, new QTableWidgetItem(schedule.saturdayShift));
        m_scheduleTable->setItem(row, 8, new QTableWidgetItem(schedule.sundayShift));
    }
}

void DashboardWidget::fillAttendanceTable(const QList<Attendance> &records)
{
    const int rowCount = qMin(records.size(), 8);
    m_attendanceTable->setRowCount(rowCount);
    for (int row = 0; row < rowCount; ++row) {
        const Attendance &attendance = records[row];
        m_attendanceTable->setItem(row, 0, new QTableWidgetItem(attendance.employeeNo));
        m_attendanceTable->setItem(row, 1, new QTableWidgetItem(attendance.employeeName));
        m_attendanceTable->setItem(row, 2, new QTableWidgetItem(attendance.status));
        m_attendanceTable->setItem(row, 3, new QTableWidgetItem(attendance.remark));
    }
}
