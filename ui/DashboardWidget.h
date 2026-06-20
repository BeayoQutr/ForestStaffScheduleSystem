#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include "../dao/EmployeeDAO.h"
#include "../service/AttendanceService.h"
#include "../service/ScheduleService.h"

#include <QWidget>

class QLabel;
class QFrame;
class QTableWidget;

class DashboardWidget : public QWidget
{
public:
    explicit DashboardWidget(QWidget *parent = nullptr);
    void refreshData();

private:
    EmployeeDAO m_employeeDAO;
    AttendanceService m_attendanceService;
    ScheduleService m_scheduleService;

    QLabel *m_totalEmployeesValue = nullptr;
    QLabel *m_activeEmployeesValue = nullptr;
    QLabel *m_todayAttendanceValue = nullptr;
    QLabel *m_weekScheduleValue = nullptr;
    QTableWidget *m_scheduleTable = nullptr;
    QTableWidget *m_attendanceTable = nullptr;

    QFrame *createStatCard(const QString &name, QLabel **valueLabel);
    void fillScheduleTable();
    void fillAttendanceTable();
};

#endif
