#ifndef ATTENDANCEWIDGET_H
#define ATTENDANCEWIDGET_H

#include "../dao/EmployeeDAO.h"
#include "../service/AttendanceService.h"

#include <QWidget>

class QComboBox;
class QDateEdit;
class QLineEdit;
class QTableWidget;

class AttendanceWidget : public QWidget
{
public:
    explicit AttendanceWidget(QWidget *parent = nullptr);

private:
    int m_currentId = 0;
    EmployeeDAO m_employeeDAO;
    AttendanceService m_attendanceService;
    QComboBox *m_employeeCombo = nullptr;
    QDateEdit *m_dateEdit = nullptr;
    QComboBox *m_statusCombo = nullptr;
    QLineEdit *m_remarkEdit = nullptr;
    QTableWidget *m_table = nullptr;

    Attendance formAttendance() const;
    void loadEmployees();
    void loadByDate();
    void fillTable(const QList<Attendance> &records);
    void clearForm();
    void onRowSelected(int row);
    int findExistingRecordId(int employeeId, const QDate &date);
};

#endif
