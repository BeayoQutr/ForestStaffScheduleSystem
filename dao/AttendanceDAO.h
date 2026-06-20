#ifndef ATTENDANCEDAO_H
#define ATTENDANCEDAO_H

#include "../model/Attendance.h"

#include <QDate>
#include <QList>
#include <QMap>

class QSqlQuery;

class AttendanceDAO
{
public:
    bool addAttendance(const Attendance &attendance);
    bool updateAttendance(const Attendance &attendance);
    bool deleteAttendance(int id);
    bool existsAttendance(int employeeId, const QDate &date);
    bool existsAttendance(int employeeId, const QDate &date, int excludeId);
    QList<Attendance> getAttendanceByDate(const QDate &date);
    QList<Attendance> getAttendanceByEmployee(int employeeId);
    QMap<QString, int> countAttendanceByEmployee(int employeeId);
    int findIdByEmployeeAndDate(int employeeId, const QDate &date);

private:
    Attendance mapAttendance(const QSqlQuery &query) const;
};

#endif
