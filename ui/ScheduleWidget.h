#ifndef SCHEDULEWIDGET_H
#define SCHEDULEWIDGET_H

#include "../dao/EmployeeDAO.h"
#include "../service/ScheduleService.h"

#include <QWidget>

class QComboBox;
class QDateEdit;
class QTableWidget;

class ScheduleWidget : public QWidget
{
public:
    explicit ScheduleWidget(QWidget *parent = nullptr);

private:
    int m_currentId = 0;
    EmployeeDAO m_employeeDAO;
    ScheduleService m_scheduleService;
    QComboBox *m_employeeCombo = nullptr;
    QDateEdit *m_weekDateEdit = nullptr;
    QComboBox *m_shiftCombos[7] = {};
    QTableWidget *m_table = nullptr;

    Schedule formSchedule() const;
    void loadEmployees();
    void loadByWeek();
    void fillTable(const QList<Schedule> &schedules);
    void clearForm();
    void onRowSelected(int row);
    QComboBox *createShiftCombo(QWidget *parent);
};

#endif
