#include "AttendanceWidget.h"

#include "../database/EmployeeComboHelper.h"

#include <QComboBox>
#include <QDateEdit>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QAbstractItemView>

AttendanceWidget::AttendanceWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    auto *formBox = new QGroupBox("出勤信息", this);
    auto *formLayout = new QGridLayout(formBox);

    m_employeeCombo = new QComboBox(formBox);
    m_dateEdit = new QDateEdit(QDate::currentDate(), formBox);
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDisplayFormat("yyyy-MM-dd");
    m_statusCombo = new QComboBox(formBox);
    m_statusCombo->addItems({"正常", "迟到", "早退", "请假", "缺勤"});
    m_remarkEdit = new QLineEdit(formBox);

    formLayout->addWidget(new QLabel("员工：", formBox), 0, 0);
    formLayout->addWidget(m_employeeCombo, 0, 1);
    formLayout->addWidget(new QLabel("日期：", formBox), 0, 2);
    formLayout->addWidget(m_dateEdit, 0, 3);
    formLayout->addWidget(new QLabel("出勤状态：", formBox), 1, 0);
    formLayout->addWidget(m_statusCombo, 1, 1);
    formLayout->addWidget(new QLabel("备注：", formBox), 1, 2);
    formLayout->addWidget(m_remarkEdit, 1, 3);

    auto *saveButton = new QPushButton("保存出勤", this);
    saveButton->setObjectName("PrimaryButton");
    auto *updateButton = new QPushButton("修改出勤", this);
    auto *deleteButton = new QPushButton("删除出勤", this);
    deleteButton->setObjectName("DangerButton");
    auto *queryDateButton = new QPushButton("查询当天出勤", this);
    auto *queryEmployeeButton = new QPushButton("查询员工历史", this);
    auto *refreshButton = new QPushButton("刷新", this);
    auto *clearButton = new QPushButton("清空", this);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(updateButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(queryDateButton);
    buttonLayout->addWidget(queryEmployeeButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addStretch();

    m_table = new QTableWidget(this);
    m_table->setColumnCount(7);
    m_table->setHorizontalHeaderLabels({"ID", "员工ID", "员工编号", "姓名", "日期", "状态", "备注"});
    m_table->hideColumn(0);
    m_table->hideColumn(1);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);

    mainLayout->addWidget(formBox);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(m_table, 1);

    connect(saveButton, &QPushButton::clicked, this, [this]() {
        Attendance attendance = formAttendance();
        const int existingId = m_attendanceService.findIdByEmployeeAndDate(attendance.employeeId, attendance.attendanceDate);
        if (existingId > 0) {
            if (QMessageBox::question(this, "覆盖确认", "该员工当天已有出勤记录，是否覆盖？") != QMessageBox::Yes) {
                return;
            }
            attendance.id = existingId;
            if (m_attendanceService.updateAttendance(attendance)) {
                QMessageBox::information(this, "操作成功", "出勤记录已覆盖。");
                clearForm();
                loadByDate();
            }
            return;
        }

        if (m_attendanceService.addAttendance(attendance)) {
            QMessageBox::information(this, "操作成功", "出勤保存成功。");
            clearForm();
            loadByDate();
        }
    });
    connect(updateButton, &QPushButton::clicked, this, [this]() {
        Attendance attendance = formAttendance();
        attendance.id = m_currentId;
        if (m_attendanceService.updateAttendance(attendance)) {
            QMessageBox::information(this, "操作成功", "出勤修改成功。");
            clearForm();
            loadByDate();
        }
    });
    connect(deleteButton, &QPushButton::clicked, this, [this]() {
        if (m_currentId <= 0) {
            QMessageBox::warning(this, "业务提示", "请先选择要删除的出勤记录。");
            return;
        }
        if (QMessageBox::question(this, "删除确认", "确定要删除该出勤记录吗？") == QMessageBox::Yes
            && m_attendanceService.deleteAttendance(m_currentId)) {
            QMessageBox::information(this, "操作成功", "出勤删除成功。");
            clearForm();
            loadByDate();
        }
    });
    connect(queryDateButton, &QPushButton::clicked, this, [this]() { loadByDate(); });
    connect(queryEmployeeButton, &QPushButton::clicked, this, [this]() {
        fillTable(m_attendanceService.getAttendanceByEmployee(m_employeeCombo->currentData().toInt()));
    });
    connect(refreshButton, &QPushButton::clicked, this, [this]() {
        loadEmployees();
        loadByDate();
    });
    connect(clearButton, &QPushButton::clicked, this, [this]() { clearForm(); });
    connect(m_table, &QTableWidget::cellClicked, this, [this](int row, int) { onRowSelected(row); });

    loadEmployees();
    loadByDate();
}

Attendance AttendanceWidget::formAttendance() const
{
    Attendance attendance;
    attendance.id = m_currentId;
    attendance.employeeId = m_employeeCombo->currentData().toInt();
    attendance.attendanceDate = m_dateEdit->date();
    attendance.status = m_statusCombo->currentText();
    attendance.remark = m_remarkEdit->text().trimmed();
    return attendance;
}

void AttendanceWidget::loadEmployees()
{
    EmployeeComboHelper::loadEmployeesInto(m_employeeCombo, m_employeeDAO);
}

void AttendanceWidget::loadByDate()
{
    fillTable(m_attendanceService.getAttendanceByDate(m_dateEdit->date()));
}

void AttendanceWidget::fillTable(const QList<Attendance> &records)
{
    m_table->setRowCount(records.size());
    for (int row = 0; row < records.size(); ++row) {
        const Attendance &attendance = records[row];
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(attendance.id)));
        m_table->setItem(row, 1, new QTableWidgetItem(QString::number(attendance.employeeId)));
        m_table->setItem(row, 2, new QTableWidgetItem(attendance.employeeNo));
        m_table->setItem(row, 3, new QTableWidgetItem(attendance.employeeName));
        m_table->setItem(row, 4, new QTableWidgetItem(attendance.attendanceDate.toString("yyyy-MM-dd")));
        m_table->setItem(row, 5, new QTableWidgetItem(attendance.status));
        m_table->setItem(row, 6, new QTableWidgetItem(attendance.remark));
    }
}

void AttendanceWidget::clearForm()
{
    m_currentId = 0;
    if (m_employeeCombo->count() > 0) {
        m_employeeCombo->setCurrentIndex(0);
    }
    m_statusCombo->setCurrentIndex(0);
    m_remarkEdit->clear();
    m_table->clearSelection();
}

void AttendanceWidget::onRowSelected(int row)
{
    m_currentId = m_table->item(row, 0)->text().toInt();
    const int employeeId = m_table->item(row, 1)->text().toInt();
    const int employeeIndex = m_employeeCombo->findData(employeeId);
    if (employeeIndex >= 0) {
        m_employeeCombo->setCurrentIndex(employeeIndex);
    }
    m_dateEdit->setDate(QDate::fromString(m_table->item(row, 4)->text(), "yyyy-MM-dd"));
    m_statusCombo->setCurrentText(m_table->item(row, 5)->text());
    m_remarkEdit->setText(m_table->item(row, 6)->text());
}


