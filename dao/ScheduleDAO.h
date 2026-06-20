#ifndef SCHEDULEDAO_H
#define SCHEDULEDAO_H

#include "../model/Schedule.h"

#include <QDate>
#include <QList>

class QSqlQuery;

class ScheduleDAO
{
public:
    bool addSchedule(const Schedule &schedule);
    bool updateSchedule(const Schedule &schedule);
    bool deleteSchedule(int id);
    bool existsSchedule(int employeeId, const QDate &weekStartDate);
    bool existsSchedule(int employeeId, const QDate &weekStartDate, int excludeId);
    QList<Schedule> getSchedulesByWeek(const QDate &weekStartDate);
    QList<Schedule> getSchedulesByEmployee(int employeeId);
    QList<Schedule> getSchedulesByEmployeeAndWeek(int employeeId, const QDate &weekStartDate);

private:
    Schedule mapSchedule(const QSqlQuery &query) const;
};

#endif
