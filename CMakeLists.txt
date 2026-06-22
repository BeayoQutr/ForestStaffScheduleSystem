cmake_minimum_required(VERSION 3.16)

project(ForestStaffScheduleSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets Sql QUIET)
if(Qt6_FOUND)
    set(QT_LIBS Qt6::Widgets Qt6::Sql)
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets Sql)
    set(QT_LIBS Qt5::Widgets Qt5::Sql)
endif()

add_executable(ForestStaffScheduleSystem
    main.cpp
    database/Config.h
    database/Config.cpp
    database/DatabaseManager.h
    database/DatabaseManager.cpp
    database/EmployeeComboHelper.h
    database/EmployeeComboHelper.cpp
    model/Employee.h
    model/Employee.cpp
    model/Schedule.h
    model/Schedule.cpp
    model/ScheduleWeek.h
    model/Attendance.h
    model/Attendance.cpp
    dao/UserDAO.h
    dao/UserDAO.cpp
    dao/EmployeeDAO.h
    dao/EmployeeDAO.cpp
    dao/ScheduleDAO.h
    dao/ScheduleDAO.cpp
    dao/AttendanceDAO.h
    dao/AttendanceDAO.cpp
    service/EmployeeService.h
    service/EmployeeService.cpp
    service/ScheduleService.h
    service/ScheduleService.cpp
    service/AttendanceService.h
    service/AttendanceService.cpp
    ui/AppStyle.h
    ui/AppStyle.cpp
    ui/DashboardWidget.h
    ui/DashboardWidget.cpp
    ui/LoginDialog.h
    ui/LoginDialog.cpp
    ui/MainWindow.h
    ui/MainWindow.cpp
    ui/EmployeeWidget.h
    ui/EmployeeWidget.cpp
    ui/ScheduleWidget.h
    ui/ScheduleWidget.cpp
    ui/AttendanceWidget.h
    ui/AttendanceWidget.cpp
    ui/StatisticsWidget.h
    ui/StatisticsWidget.cpp
)

target_link_libraries(ForestStaffScheduleSystem PRIVATE ${QT_LIBS})

include(CTest)
if(BUILD_TESTING)
    add_executable(ScheduleWeekTest tests/ScheduleWeekTest.cpp)
    target_link_libraries(ScheduleWeekTest PRIVATE ${QT_LIBS})
    add_test(NAME ScheduleWeekTest COMMAND ScheduleWeekTest)
endif()

if(Qt6_FOUND AND COMMAND qt_finalize_executable)
    qt_finalize_executable(ForestStaffScheduleSystem)
endif()
