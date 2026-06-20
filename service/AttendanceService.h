#ifndef ATTENDANCESERVICE_H
#define ATTENDANCESERVICE_H

#include "../dao/AttendanceDAO.h"
#include "../dao/EmployeeDAO.h"

class AttendanceService
{
public:
    bool addAttendance(const Attendance &attendance);
    bool updateAttendance(const Attendance &attendance);
    bool deleteAttendance(int id);
    bool existsAttendance(int employeeId, const QDate &date);
    QList<Attendance> getAttendanceByDate(const QDate &date);
    QList<Attendance> getAttendanceByEmployee(int employeeId);
    QMap<QString, int> countAttendanceByEmployee(int employeeId);
    int findIdByEmployeeAndDate(int employeeId, const QDate &date);

private:
    AttendanceDAO m_attendanceDAO;
    EmployeeDAO m_employeeDAO;
    bool validateAttendance(const Attendance &attendance, bool isUpdate);
};

#endif
