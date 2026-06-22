#include "ScheduleWidget.h"

#include "../database/EmployeeComboHelper.h"
#include "../model/ScheduleWeek.h"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QAbstractItemView>

ScheduleWidget::ScheduleWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    auto *formBox = new QGroupBox("周排班信息", this);
    auto *formLayout = new QGridLayout(formBox);

    m_employeeCombo = new QComboBox(formBox);
    m_weekDateEdit = new QDateEdit(QDate::currentDate(), formBox);
    m_weekDateEdit->setCalendarPopup(true);
    m_weekDateEdit->setDisplayFormat("yyyy-MM-dd");

    m_weekDateEdit->setDate(ScheduleWeek::startDate(m_weekDateEdit->date()));

    const QStringList labels = {"周一：", "周二：", "周三：", "周四：", "周五：", "周六：", "周日："};
    for (int i = 0; i < 7; ++i) {
        m_shiftCombos[i] = createShiftCombo(formBox);
    }

    formLayout->addWidget(new QLabel("员工：", formBox), 0, 0);
    formLayout->addWidget(m_employeeCombo, 0, 1);
    formLayout->addWidget(new QLabel("周开始日期：", formBox), 0, 2);
    formLayout->addWidget(m_weekDateEdit, 0, 3);
    for (int i = 0; i < 7; ++i) {
        const int row = 1 + i / 4;
        const int col = (i % 4) * 2;
        formLayout->addWidget(new QLabel(labels[i], formBox), row, col);
        formLayout->addWidget(m_shiftCombos[i], row, col + 1);
    }

    auto *addButton = new QPushButton("添加排班", this);
    addButton->setObjectName("PrimaryButton");
    auto *updateButton = new QPushButton("修改排班", this);
    auto *deleteButton = new QPushButton("删除排班", this);
    deleteButton->setObjectName("DangerButton");
    auto *queryWeekButton = new QPushButton("查询某周排班", this);
    auto *queryEmployeeButton = new QPushButton("查询员工本周排班", this);
    auto *refreshButton = new QPushButton("刷新", this);
    auto *clearButton = new QPushButton("清空", this);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(updateButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(queryWeekButton);
    buttonLayout->addWidget(queryEmployeeButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addStretch();

    m_table = new QTableWidget(this);
    m_table->setColumnCount(12);
    m_table->setHorizontalHeaderLabels({"ID", "员工ID", "员工编号", "姓名", "周开始日期", "周一", "周二", "周三", "周四", "周五", "周六", "周日"});
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

    connect(addButton, &QPushButton::clicked, this, [this]() {
        if (m_scheduleService.addSchedule(formSchedule())) {
            QMessageBox::information(this, "操作成功", "排班添加成功。");
            clearForm();
            loadByWeek();
        }
    });
    connect(updateButton, &QPushButton::clicked, this, [this]() {
        Schedule schedule = formSchedule();
        schedule.id = m_currentId;
        if (m_scheduleService.updateSchedule(schedule)) {
            QMessageBox::information(this, "操作成功", "排班修改成功。");
            clearForm();
            loadByWeek();
        }
    });
    connect(deleteButton, &QPushButton::clicked, this, [this]() {
        if (m_currentId <= 0) {
            QMessageBox::warning(this, "业务提示", "请先选择要删除的排班记录。");
            return;
        }
        if (QMessageBox::question(this, "删除确认", "确定要删除该排班记录吗？") == QMessageBox::Yes
            && m_scheduleService.deleteSchedule(m_currentId)) {
            QMessageBox::information(this, "操作成功", "排班删除成功。");
            clearForm();
            loadByWeek();
        }
    });
    connect(queryWeekButton, &QPushButton::clicked, this, [this]() { loadByWeek(); });
    connect(queryEmployeeButton, &QPushButton::clicked, this, [this]() {
        const int employeeId = m_employeeCombo->currentData().toInt();
        fillTable(m_scheduleService.getSchedulesByEmployeeAndWeek(employeeId, m_weekDateEdit->date()));
    });
    connect(refreshButton, &QPushButton::clicked, this, [this]() {
        loadEmployees();
        loadByWeek();
    });
    connect(clearButton, &QPushButton::clicked, this, [this]() { clearForm(); });
    connect(m_weekDateEdit, &QDateEdit::dateChanged, this, [this](const QDate &date) {
        const QDate weekStart = ScheduleWeek::startDate(date);
        if (date != weekStart) {
            m_weekDateEdit->setDate(weekStart);
        }
    });
    connect(m_table, &QTableWidget::cellClicked, this, [this](int row, int) { onRowSelected(row); });

    loadEmployees();
    loadByWeek();
}

QComboBox *ScheduleWidget::createShiftCombo(QWidget *parent)
{
    auto *combo = new QComboBox(parent);
    combo->addItems({"早班", "中班", "晚班", "巡山", "值班", "休息"});
    return combo;
}

Schedule ScheduleWidget::formSchedule() const
{
    Schedule schedule;
    schedule.id = m_currentId;
    schedule.employeeId = m_employeeCombo->currentData().toInt();
    schedule.weekStartDate = m_weekDateEdit->date();
    schedule.mondayShift = m_shiftCombos[0]->currentText();
    schedule.tuesdayShift = m_shiftCombos[1]->currentText();
    schedule.wednesdayShift = m_shiftCombos[2]->currentText();
    schedule.thursdayShift = m_shiftCombos[3]->currentText();
    schedule.fridayShift = m_shiftCombos[4]->currentText();
    schedule.saturdayShift = m_shiftCombos[5]->currentText();
    schedule.sundayShift = m_shiftCombos[6]->currentText();
    return schedule;
}

void ScheduleWidget::loadEmployees()
{
    EmployeeComboHelper::loadEmployeesInto(m_employeeCombo, m_employeeDAO);
}

void ScheduleWidget::loadByWeek()
{
    fillTable(m_scheduleService.getSchedulesByWeek(m_weekDateEdit->date()));
}

void ScheduleWidget::fillTable(const QList<Schedule> &schedules)
{
    m_table->setRowCount(schedules.size());
    for (int row = 0; row < schedules.size(); ++row) {
        const Schedule &schedule = schedules[row];
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(schedule.id)));
        m_table->setItem(row, 1, new QTableWidgetItem(QString::number(schedule.employeeId)));
        m_table->setItem(row, 2, new QTableWidgetItem(schedule.employeeNo));
        m_table->setItem(row, 3, new QTableWidgetItem(schedule.employeeName));
        m_table->setItem(row, 4, new QTableWidgetItem(schedule.weekStartDate.toString("yyyy-MM-dd")));
        m_table->setItem(row, 5, new QTableWidgetItem(schedule.mondayShift));
        m_table->setItem(row, 6, new QTableWidgetItem(schedule.tuesdayShift));
        m_table->setItem(row, 7, new QTableWidgetItem(schedule.wednesdayShift));
        m_table->setItem(row, 8, new QTableWidgetItem(schedule.thursdayShift));
        m_table->setItem(row, 9, new QTableWidgetItem(schedule.fridayShift));
        m_table->setItem(row, 10, new QTableWidgetItem(schedule.saturdayShift));
        m_table->setItem(row, 11, new QTableWidgetItem(schedule.sundayShift));
    }
}

void ScheduleWidget::clearForm()
{
    m_currentId = 0;
    if (m_employeeCombo->count() > 0) {
        m_employeeCombo->setCurrentIndex(0);
    }
    for (QComboBox *combo : m_shiftCombos) {
        combo->setCurrentIndex(0);
    }
    m_table->clearSelection();
}

void ScheduleWidget::onRowSelected(int row)
{
    m_currentId = m_table->item(row, 0)->text().toInt();
    const int employeeId = m_table->item(row, 1)->text().toInt();
    const int employeeIndex = m_employeeCombo->findData(employeeId);
    if (employeeIndex >= 0) {
        m_employeeCombo->setCurrentIndex(employeeIndex);
    }
    m_weekDateEdit->setDate(QDate::fromString(m_table->item(row, 4)->text(), "yyyy-MM-dd"));
    for (int i = 0; i < 7; ++i) {
        m_shiftCombos[i]->setCurrentText(m_table->item(row, 5 + i)->text());
    }
}
