#ifndef SCHEDULEWEEK_H
#define SCHEDULEWEEK_H

#include <QDate>

namespace ScheduleWeek {

inline QDate startDate(const QDate &date)
{
    return date.isValid() ? date.addDays(1 - date.dayOfWeek()) : QDate();
}

inline QDate endDateExclusive(const QDate &date)
{
    const QDate start = startDate(date);
    return start.isValid() ? start.addDays(7) : QDate();
}

} // namespace ScheduleWeek

#endif
