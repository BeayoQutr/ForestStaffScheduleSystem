#include "../model/ScheduleWeek.h"

#include <iostream>

namespace {

bool expectEqual(const QDate &actual, const QDate &expected, const char *scenario)
{
    if (actual == expected) {
        return true;
    }
    std::cerr << scenario << ": expected " << expected.toString("yyyy-MM-dd").toStdString()
              << ", got " << actual.toString("yyyy-MM-dd").toStdString() << '\n';
    return false;
}

} // namespace

int main()
{
    bool passed = true;
    passed &= expectEqual(ScheduleWeek::startDate(QDate(2026, 6, 22)), QDate(2026, 6, 22),
                          "Monday remains the week start");
    passed &= expectEqual(ScheduleWeek::startDate(QDate(2026, 6, 24)), QDate(2026, 6, 22),
                          "A mid-week schedule is visible on that week's dashboard");
    passed &= expectEqual(ScheduleWeek::startDate(QDate(2026, 6, 28)), QDate(2026, 6, 22),
                          "Sunday belongs to the preceding Monday");
    passed &= expectEqual(ScheduleWeek::endDateExclusive(QDate(2026, 6, 24)), QDate(2026, 6, 29),
                          "The dashboard query range covers exactly seven days");
    passed &= expectEqual(ScheduleWeek::startDate(QDate()), QDate(),
                          "An invalid date stays invalid");
    return passed ? 0 : 1;
}
