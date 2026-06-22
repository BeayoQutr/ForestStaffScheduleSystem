#include "ScheduleService.h"
#include "../model/ScheduleWeek.h"

#include <QMessageBox>

bool ScheduleService::validateSchedule(const Schedule &schedule, bool isUpdate)
{
    if (schedule.employeeId <= 0) {
        QMessageBox::warning(nullptr, "业务提示", "请选择员工。");
        return false;
    }
    if (!schedule.weekStartDate.isValid()) {
        QMessageBox::warning(nullptr, "业务提示", "请选择有效的周开始日期。");
        return false;
    }

    Employee employee = m_employeeDAO.getById(schedule.employeeId);
    if (employee.id == 0 || !employee.active) {
        QMessageBox::warning(nullptr, "业务提示", "只能给在职员工排班。");
        return false;
    }

    const int excludeId = isUpdate ? schedule.id : 0;
    if (m_scheduleDAO.existsSchedule(schedule.employeeId, schedule.weekStartDate, excludeId)) {
        QMessageBox::warning(nullptr, "业务提示", "同一员工同一周不能重复排班。");
        return false;
    }

    return true;
}

bool ScheduleService::addSchedule(const Schedule &schedule)
{
    if (!validateSchedule(schedule, false)) {
        return false;
    }
    Schedule normalizedSchedule = schedule;
    normalizedSchedule.weekStartDate = ScheduleWeek::startDate(schedule.weekStartDate);
    return m_scheduleDAO.addSchedule(normalizedSchedule);
}

bool ScheduleService::updateSchedule(const Schedule &schedule)
{
    if (schedule.id <= 0) {
        QMessageBox::warning(nullptr, "业务提示", "请先选择要修改的排班记录。");
        return false;
    }
    if (!validateSchedule(schedule, true)) {
        return false;
    }
    Schedule normalizedSchedule = schedule;
    normalizedSchedule.weekStartDate = ScheduleWeek::startDate(schedule.weekStartDate);
    return m_scheduleDAO.updateSchedule(normalizedSchedule);
}

bool ScheduleService::deleteSchedule(int id)
{
    if (id <= 0) {
        QMessageBox::warning(nullptr, "业务提示", "请先选择要删除的排班记录。");
        return false;
    }
    return m_scheduleDAO.deleteSchedule(id);
}

QList<Schedule> ScheduleService::getSchedulesByWeek(const QDate &weekStartDate)
{
    return m_scheduleDAO.getSchedulesByWeek(weekStartDate);
}

QList<Schedule> ScheduleService::getSchedulesByEmployee(int employeeId)
{
    return m_scheduleDAO.getSchedulesByEmployee(employeeId);
}

QList<Schedule> ScheduleService::getSchedulesByEmployeeAndWeek(int employeeId, const QDate &weekStartDate)
{
    return m_scheduleDAO.getSchedulesByEmployeeAndWeek(employeeId, weekStartDate);
}
