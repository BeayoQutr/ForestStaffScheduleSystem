#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <QDate>
#include <QString>

class Schedule
{
public:
    int id = 0;
    int employeeId = 0;
    QString employeeNo;
    QString employeeName;
    QDate weekStartDate;
    QString mondayShift;
    QString tuesdayShift;
    QString wednesdayShift;
    QString thursdayShift;
    QString fridayShift;
    QString saturdayShift;
    QString sundayShift;
};

#endif
