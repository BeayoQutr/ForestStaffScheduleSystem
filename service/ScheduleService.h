#ifndef SCHEDULESERVICE_H
#define SCHEDULESERVICE_H

#include "../dao/EmployeeDAO.h"
#include "../dao/ScheduleDAO.h"

class ScheduleService
{
public:
    bool addSchedule(const Schedule &schedule);
    bool updateSchedule(const Schedule &schedule);
    bool deleteSchedule(int id);
    QList<Schedule> getSchedulesByWeek(const QDate &weekStartDate);
    QList<Schedule> getSchedulesByEmployee(int employeeId);
    QList<Schedule> getSchedulesByEmployeeAndWeek(int employeeId, const QDate &weekStartDate);

private:
    ScheduleDAO m_scheduleDAO;
    EmployeeDAO m_employeeDAO;
    bool validateSchedule(const Schedule &schedule, bool isUpdate);
};

#endif
