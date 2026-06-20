#include "AttendanceService.h"

#include <QMessageBox>
#include <QStringList>

bool AttendanceService::validateAttendance(const Attendance &attendance, bool isUpdate)
{
    if (attendance.employeeId <= 0) {
        QMessageBox::warning(nullptr, "业务提示", "请选择员工。");
        return false;
    }
    if (!attendance.attendanceDate.isValid()) {
        QMessageBox::warning(nullptr, "业务提示", "请选择有效的出勤日期。");
        return false;
    }

    const QStringList validStatuses = {"正常", "迟到", "早退", "请假", "缺勤"};
    if (!validStatuses.contains(attendance.status)) {
        QMessageBox::warning(nullptr, "业务提示", "请选择正确的出勤状态。");
        return false;
    }

    Employee employee = m_employeeDAO.getById(attendance.employeeId);
    if (employee.id == 0 || !employee.active) {
        QMessageBox::warning(nullptr, "业务提示", "只能为在职员工记录出勤。");
        return false;
    }

    const int excludeId = isUpdate ? attendance.id : 0;
    if (m_attendanceDAO.existsAttendance(attendance.employeeId, attendance.attendanceDate, excludeId)) {
        QMessageBox::warning(nullptr, "业务提示", "同一员工同一天不能重复添加出勤记录。");
        return false;
    }

    return true;
}

bool AttendanceService::addAttendance(const Attendance &attendance)
{
    if (!validateAttendance(attendance, false)) {
        return false;
    }
    return m_attendanceDAO.addAttendance(attendance);
}

bool AttendanceService::updateAttendance(const Attendance &attendance)
{
    if (attendance.id <= 0) {
        QMessageBox::warning(nullptr, "业务提示", "请先选择要修改的出勤记录。");
        return false;
    }
    if (!validateAttendance(attendance, true)) {
        return false;
    }
    return m_attendanceDAO.updateAttendance(attendance);
}

bool AttendanceService::deleteAttendance(int id)
{
    if (id <= 0) {
        QMessageBox::warning(nullptr, "业务提示", "请先选择要删除的出勤记录。");
        return false;
    }
    return m_attendanceDAO.deleteAttendance(id);
}

bool AttendanceService::existsAttendance(int employeeId, const QDate &date)
{
    return m_attendanceDAO.existsAttendance(employeeId, date);
}

QList<Attendance> AttendanceService::getAttendanceByDate(const QDate &date)
{
    return m_attendanceDAO.getAttendanceByDate(date);
}

QList<Attendance> AttendanceService::getAttendanceByEmployee(int employeeId)
{
    return m_attendanceDAO.getAttendanceByEmployee(employeeId);
}

QMap<QString, int> AttendanceService::countAttendanceByEmployee(int employeeId)
{
    return m_attendanceDAO.countAttendanceByEmployee(employeeId);
}

int AttendanceService::findIdByEmployeeAndDate(int employeeId, const QDate &date)
{
    return m_attendanceDAO.findIdByEmployeeAndDate(employeeId, date);
}
