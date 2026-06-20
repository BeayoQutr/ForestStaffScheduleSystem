#include "AttendanceDAO.h"

#include "../database/DatabaseManager.h"

#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>

Attendance AttendanceDAO::mapAttendance(const QSqlQuery &query) const
{
    const QSqlRecord record = query.record();
    Attendance attendance;
    attendance.id = record.value("id").toInt();
    attendance.employeeId = record.value("employee_id").toInt();
    attendance.employeeNo = record.value("employee_no").toString();
    attendance.employeeName = record.value("name").toString();
    attendance.attendanceDate = record.value("attendance_date").toDate();
    attendance.status = record.value("status").toString();
    attendance.remark = record.value("remark").toString();
    return attendance;
}

bool AttendanceDAO::addAttendance(const Attendance &attendance)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("INSERT INTO attendance_records (employee_id, attendance_date, status, remark) "
                  "VALUES (:employee_id, :attendance_date, :status, :remark)");
    query.bindValue(":employee_id", attendance.employeeId);
    query.bindValue(":attendance_date", attendance.attendanceDate);
    query.bindValue(":status", attendance.status);
    query.bindValue(":remark", attendance.remark);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "保存出勤失败：\n" + query.lastError().text());
        return false;
    }
    return true;
}

bool AttendanceDAO::updateAttendance(const Attendance &attendance)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("UPDATE attendance_records SET employee_id = :employee_id, attendance_date = :attendance_date, "
                  "status = :status, remark = :remark WHERE id = :id");
    query.bindValue(":employee_id", attendance.employeeId);
    query.bindValue(":attendance_date", attendance.attendanceDate);
    query.bindValue(":status", attendance.status);
    query.bindValue(":remark", attendance.remark);
    query.bindValue(":id", attendance.id);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "修改出勤失败：\n" + query.lastError().text());
        return false;
    }
    return true;
}

bool AttendanceDAO::deleteAttendance(int id)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("DELETE FROM attendance_records WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "删除出勤失败：\n" + query.lastError().text());
        return false;
    }
    return true;
}

bool AttendanceDAO::existsAttendance(int employeeId, const QDate &date)
{
    return existsAttendance(employeeId, date, 0);
}

bool AttendanceDAO::existsAttendance(int employeeId, const QDate &date, int excludeId)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT COUNT(*) FROM attendance_records WHERE employee_id = :employee_id "
                  "AND attendance_date = :attendance_date AND id <> :exclude_id");
    query.bindValue(":employee_id", employeeId);
    query.bindValue(":attendance_date", date);
    query.bindValue(":exclude_id", excludeId);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "检查重复出勤失败：\n" + query.lastError().text());
        return true;
    }
    return query.next() && query.value(0).toInt() > 0;
}

QList<Attendance> AttendanceDAO::getAttendanceByDate(const QDate &date)
{
    QList<Attendance> records;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT a.*, e.employee_no, e.name FROM attendance_records a "
                  "JOIN employees e ON a.employee_id = e.id "
                  "WHERE a.attendance_date = :attendance_date ORDER BY e.employee_no");
    query.bindValue(":attendance_date", date);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "按日期查询出勤失败：\n" + query.lastError().text());
        return records;
    }

    while (query.next()) {
        records.append(mapAttendance(query));
    }
    return records;
}

QList<Attendance> AttendanceDAO::getAttendanceByEmployee(int employeeId)
{
    QList<Attendance> records;
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT a.*, e.employee_no, e.name FROM attendance_records a "
                  "JOIN employees e ON a.employee_id = e.id "
                  "WHERE a.employee_id = :employee_id ORDER BY a.attendance_date DESC");
    query.bindValue(":employee_id", employeeId);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "查询员工出勤历史失败：\n" + query.lastError().text());
        return records;
    }

    while (query.next()) {
        records.append(mapAttendance(query));
    }
    return records;
}

int AttendanceDAO::findIdByEmployeeAndDate(int employeeId, const QDate &date)
{
    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT id FROM attendance_records "
                  "WHERE employee_id = :employee_id AND attendance_date = :attendance_date LIMIT 1");
    query.bindValue(":employee_id", employeeId);
    query.bindValue(":attendance_date", date);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "查询出勤记录 ID 失败：\n" + query.lastError().text());
        return 0;
    }
    return query.next() ? query.value(0).toInt() : 0;
}

QMap<QString, int> AttendanceDAO::countAttendanceByEmployee(int employeeId)
{
    QMap<QString, int> result;
    const QStringList statuses = {"正常", "迟到", "早退", "请假", "缺勤"};
    for (const QString &status : statuses) {
        result.insert(status, 0);
    }

    QSqlQuery query(DatabaseManager::instance().database());
    query.prepare("SELECT status, COUNT(*) AS total FROM attendance_records "
                  "WHERE employee_id = :employee_id GROUP BY status");
    query.bindValue(":employee_id", employeeId);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "数据库错误", "统计员工出勤失败：\n" + query.lastError().text());
        return result;
    }

    while (query.next()) {
        result[query.value(0).toString()] = query.value(1).toInt();
    }
    return result;
}
