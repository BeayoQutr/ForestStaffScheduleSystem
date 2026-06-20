#ifndef STATISTICSWIDGET_H
#define STATISTICSWIDGET_H

#include "../dao/EmployeeDAO.h"
#include "../service/AttendanceService.h"
#include "../service/ScheduleService.h"

#include <QWidget>

class QComboBox;
class QDateEdit;
class QLabel;
class QTextEdit;
class QTableWidget;

class StatisticsWidget : public QWidget
{
public:
    explicit StatisticsWidget(QWidget *parent = nullptr);

private:
    EmployeeDAO m_employeeDAO;
    AttendanceService m_attendanceService;
    ScheduleService m_scheduleService;
    QComboBox *m_employeeCombo = nullptr;
    QDateEdit *m_dateEdit = nullptr;
    QLabel *m_totalLabel = nullptr;
    QLabel *m_activeLabel = nullptr;
    QTextEdit *m_resultEdit = nullptr;
    QTableWidget *m_table = nullptr;

    void loadEmployees();
    void refreshCounts();
    void showEmployeeAttendanceStats();
    void showWeekSchedules();
    void showDateAttendance();
};

#endif
