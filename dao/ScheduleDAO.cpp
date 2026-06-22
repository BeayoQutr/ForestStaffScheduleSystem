#include "ScheduleDAO.h"

#include "../database/DatabaseManager.h"
#include "../model/ScheduleWeek.h"

#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

Schedule ScheduleDAO::mapSchedule(const QSqlQuery &query) const
{
    const QSqlRecord record = query.record();
    Schedule schedule;
    schedule.id = record.value("id").toInt();
    schedule.employeeId = record.value("employee_id").toInt();
    schedule.employeeNo = record.value("employee_no").toString();
    schedule.employeeName = record.value("name").toString();
    schedule.weekStartDate = record.value("week_start_date").toDate();
    schedule.mondayShift = record.value("monday_shift").toString();
    schedule.tuesdayShift = record.value("tuesday_shift").toString();
    schedule.wednesdayShift = record.value("wednesday_shift").toString();
    schedule.thursdayShift = record.value("thursday_shift").toString();
    schedule.fridayShift = record.value("friday_shift").toString();
    schedule.saturdayShift = record.value("saturday_shift").toString();
    schedule.sundayShift = record.value("sunday_shift").toString();
    return schedule;
}

bool ScheduleDAO::addSchedule(const Schedule &schedule)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("INSERT INTO schedules (employee_id, week_start_date, monday_shift, tuesday_shift, "
                  "wednesday_shift, thursday_shift, friday_shift, saturday_shift, sunday_shift) "
                  "VALUES (:employee_id, :week_start_date, :monday_shift, :tuesday_shift, :wednesday_shift, "
                  ":thursday_shift, :friday_shift, :saturday_shift, :sunday_shift)");
    query.bindValue(":employee_id", schedule.employeeId);
    query.bindValue(":week_start_date", schedule.weekStartDate.toString("yyyy-MM-dd"));
    query.bindValue(":monday_shift", schedule.mondayShift);
    query.bindValue(":tuesday_shift", schedule.tuesdayShift);
    query.bindValue(":wednesday_shift", schedule.wednesdayShift);
    query.bindValue(":thursday_shift", schedule.thursdayShift);
    query.bindValue(":friday_shift", schedule.fridayShift);
    query.bindValue(":saturday_shift", schedule.saturdayShift);
    query.bindValue(":sunday_shift", schedule.sundayShift);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "添加排班失败：\n" + query.lastError().text());
        return false;
    }
    return true;
}

bool ScheduleDAO::updateSchedule(const Schedule &schedule)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE schedules SET employee_id = :employee_id, week_start_date = :week_start_date, "
                  "monday_shift = :monday_shift, tuesday_shift = :tuesday_shift, wednesday_shift = :wednesday_shift, "
                  "thursday_shift = :thursday_shift, friday_shift = :friday_shift, saturday_shift = :saturday_shift, "
                  "sunday_shift = :sunday_shift WHERE id = :id");
    query.bindValue(":employee_id", schedule.employeeId);
    query.bindValue(":week_start_date", schedule.weekStartDate.toString("yyyy-MM-dd"));
    query.bindValue(":monday_shift", schedule.mondayShift);
    query.bindValue(":tuesday_shift", schedule.tuesdayShift);
    query.bindValue(":wednesday_shift", schedule.wednesdayShift);
    query.bindValue(":thursday_shift", schedule.thursdayShift);
    query.bindValue(":friday_shift", schedule.fridayShift);
    query.bindValue(":saturday_shift", schedule.saturdayShift);
    query.bindValue(":sunday_shift", schedule.sundayShift);
    query.bindValue(":id", schedule.id);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "修改排班失败：\n" + query.lastError().text());
        return false;
    }
    return true;
}

bool ScheduleDAO::deleteSchedule(int id)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM schedules WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "删除排班失败：\n" + query.lastError().text());
        return false;
    }
    return true;
}

bool ScheduleDAO::existsSchedule(int employeeId, const QDate &weekStartDate)
{
    return existsSchedule(employeeId, weekStartDate, 0);
}

bool ScheduleDAO::existsSchedule(int employeeId, const QDate &weekStartDate, int excludeId)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT COUNT(*) FROM schedules WHERE employee_id = :employee_id "
                  "AND week_start_date >= :week_start_date AND week_start_date < :week_end_date "
                  "AND id <> :exclude_id");
    query.bindValue(":employee_id", employeeId);
    query.bindValue(":week_start_date", ScheduleWeek::startDate(weekStartDate).toString("yyyy-MM-dd"));
    query.bindValue(":week_end_date", ScheduleWeek::endDateExclusive(weekStartDate).toString("yyyy-MM-dd"));
    query.bindValue(":exclude_id", excludeId);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "检查重复排班失败：\n" + query.lastError().text());
        return true;
    }
    return query.next() && query.value(0).toInt() > 0;
}

QList<Schedule> ScheduleDAO::getSchedulesByWeek(const QDate &weekStartDate)
{
    QList<Schedule> schedules;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT s.*, e.employee_no, e.name FROM schedules s "
                  "JOIN employees e ON s.employee_id = e.id "
                  "WHERE s.week_start_date >= :week_start_date AND s.week_start_date < :week_end_date "
                  "ORDER BY e.employee_no");
    query.bindValue(":week_start_date", ScheduleWeek::startDate(weekStartDate).toString("yyyy-MM-dd"));
    query.bindValue(":week_end_date", ScheduleWeek::endDateExclusive(weekStartDate).toString("yyyy-MM-dd"));

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "查询周排班失败：\n" + query.lastError().text());
        return schedules;
    }

    while (query.next()) {
        schedules.append(mapSchedule(query));
    }
    return schedules;
}

QList<Schedule> ScheduleDAO::getSchedulesByEmployee(int employeeId)
{
    QList<Schedule> schedules;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT s.*, e.employee_no, e.name FROM schedules s "
                  "JOIN employees e ON s.employee_id = e.id "
                  "WHERE s.employee_id = :employee_id ORDER BY s.week_start_date DESC");
    query.bindValue(":employee_id", employeeId);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "查询员工排班失败：\n" + query.lastError().text());
        return schedules;
    }

    while (query.next()) {
        schedules.append(mapSchedule(query));
    }
    return schedules;
}

QList<Schedule> ScheduleDAO::getSchedulesByEmployeeAndWeek(int employeeId, const QDate &weekStartDate)
{
    QList<Schedule> schedules;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT s.*, e.employee_no, e.name FROM schedules s "
                  "JOIN employees e ON s.employee_id = e.id "
                  "WHERE s.employee_id = :employee_id "
                  "AND s.week_start_date >= :week_start_date AND s.week_start_date < :week_end_date");
    query.bindValue(":employee_id", employeeId);
    query.bindValue(":week_start_date", ScheduleWeek::startDate(weekStartDate).toString("yyyy-MM-dd"));
    query.bindValue(":week_end_date", ScheduleWeek::endDateExclusive(weekStartDate).toString("yyyy-MM-dd"));

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "查询指定员工周排班失败：\n" + query.lastError().text());
        return schedules;
    }

    while (query.next()) {
        schedules.append(mapSchedule(query));
    }
    return schedules;
}
