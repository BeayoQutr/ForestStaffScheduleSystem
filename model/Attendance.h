#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <QDate>
#include <QString>

class Attendance
{
public:
    int id = 0;
    int employeeId = 0;
    QString employeeNo;
    QString employeeName;
    QDate attendanceDate;
    QString status;
    QString remark;
};

#endif
