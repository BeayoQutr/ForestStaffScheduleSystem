#include "EmployeeWidget.h"

#include <QComboBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>
#include <QAbstractItemView>

EmployeeWidget::EmployeeWidget(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);

    auto *formBox = new QGroupBox("员工信息", this);
    auto *formLayout = new QGridLayout(formBox);

    m_noEdit = new QLineEdit(formBox);
    m_nameEdit = new QLineEdit(formBox);
    m_genderCombo = new QComboBox(formBox);
    m_genderCombo->addItems({"男", "女"});
    m_ageSpin = new QSpinBox(formBox);
    m_ageSpin->setRange(16, 80);
    m_ageSpin->setValue(30);
    m_phoneEdit = new QLineEdit(formBox);
    m_positionEdit = new QLineEdit(formBox);
    m_departmentEdit = new QLineEdit(formBox);

    formLayout->addWidget(new QLabel("员工编号：", formBox), 0, 0);
    formLayout->addWidget(m_noEdit, 0, 1);
    formLayout->addWidget(new QLabel("姓名：", formBox), 0, 2);
    formLayout->addWidget(m_nameEdit, 0, 3);
    formLayout->addWidget(new QLabel("性别：", formBox), 0, 4);
    formLayout->addWidget(m_genderCombo, 0, 5);
    formLayout->addWidget(new QLabel("年龄：", formBox), 1, 0);
    formLayout->addWidget(m_ageSpin, 1, 1);
    formLayout->addWidget(new QLabel("电话：", formBox), 1, 2);
    formLayout->addWidget(m_phoneEdit, 1, 3);
    formLayout->addWidget(new QLabel("岗位：", formBox), 1, 4);
    formLayout->addWidget(m_positionEdit, 1, 5);
    formLayout->addWidget(new QLabel("所属林区/班组：", formBox), 2, 0);
    formLayout->addWidget(m_departmentEdit, 2, 1, 1, 5);

    auto *addButton = new QPushButton("添加", this);
    addButton->setObjectName("PrimaryButton");
    auto *updateButton = new QPushButton("修改", this);
    auto *deleteButton = new QPushButton("删除", this);
    deleteButton->setObjectName("DangerButton");
    auto *searchButton = new QPushButton("查询", this);
    auto *refreshButton = new QPushButton("刷新", this);
    auto *clearButton = new QPushButton("清空", this);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(updateButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(searchButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addStretch();

    m_table = new QTableWidget(this);
    m_table->setColumnCount(9);
    m_table->setHorizontalHeaderLabels({"ID", "员工编号", "姓名", "性别", "年龄", "电话", "岗位", "所属林区/班组", "创建时间"});
    m_table->hideColumn(0);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->verticalHeader()->setVisible(false);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);

    mainLayout->addWidget(formBox);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(m_table, 1);

    connect(addButton, &QPushButton::clicked, this, [this]() {
        if (m_employeeService.addEmployee(formEmployee())) {
            QMessageBox::information(this, "操作成功", "员工添加成功。");
            clearForm();
            loadAll();
        }
    });
    connect(updateButton, &QPushButton::clicked, this, [this]() {
        Employee employee = formEmployee();
        employee.id = m_currentId;
        if (m_employeeService.updateEmployee(employee)) {
            QMessageBox::information(this, "操作成功", "员工修改成功。");
            clearForm();
            loadAll();
        }
    });
    connect(deleteButton, &QPushButton::clicked, this, [this]() {
        if (m_currentId <= 0) {
            QMessageBox::warning(this, "业务提示", "请先选择要删除的员工。");
            return;
        }
        if (QMessageBox::question(this, "删除确认", "确定要将该员工设置为离职吗？") == QMessageBox::Yes
            && m_employeeService.deleteEmployee(m_currentId)) {
            QMessageBox::information(this, "操作成功", "员工已设置为离职。");
            clearForm();
            loadAll();
        }
    });
    connect(searchButton, &QPushButton::clicked, this, [this]() {
        const QString employeeNo = m_noEdit->text().trimmed();
        const QString name = m_nameEdit->text().trimmed();
        if (!employeeNo.isEmpty()) {
            Employee employee = m_employeeService.getByEmployeeNo(employeeNo);
            QList<Employee> employees;
            if (employee.id != 0 && employee.active) {
                employees.append(employee);
            }
            fillTable(employees);
        } else if (!name.isEmpty()) {
            fillTable(m_employeeService.searchByName(name));
        } else {
            loadAll();
        }
    });
    connect(refreshButton, &QPushButton::clicked, this, [this]() { loadAll(); });
    connect(clearButton, &QPushButton::clicked, this, [this]() { clearForm(); });
    connect(m_table, &QTableWidget::cellClicked, this, [this](int row, int) { onRowSelected(row); });

    loadAll();
}

Employee EmployeeWidget::formEmployee() const
{
    Employee employee;
    employee.id = m_currentId;
    employee.employeeNo = m_noEdit->text().trimmed();
    employee.name = m_nameEdit->text().trimmed();
    employee.gender = m_genderCombo->currentText();
    employee.age = m_ageSpin->value();
    employee.phone = m_phoneEdit->text().trimmed();
    employee.position = m_positionEdit->text().trimmed();
    employee.department = m_departmentEdit->text().trimmed();
    employee.active = true;
    return employee;
}

void EmployeeWidget::loadAll()
{
    fillTable(m_employeeService.getAllActiveEmployees());
}

void EmployeeWidget::fillTable(const QList<Employee> &employees)
{
    m_table->setRowCount(employees.size());
    for (int row = 0; row < employees.size(); ++row) {
        const Employee &employee = employees[row];
        m_table->setItem(row, 0, new QTableWidgetItem(QString::number(employee.id)));
        m_table->setItem(row, 1, new QTableWidgetItem(employee.employeeNo));
        m_table->setItem(row, 2, new QTableWidgetItem(employee.name));
        m_table->setItem(row, 3, new QTableWidgetItem(employee.gender));
        m_table->setItem(row, 4, new QTableWidgetItem(QString::number(employee.age)));
        m_table->setItem(row, 5, new QTableWidgetItem(employee.phone));
        m_table->setItem(row, 6, new QTableWidgetItem(employee.position));
        m_table->setItem(row, 7, new QTableWidgetItem(employee.department));
        m_table->setItem(row, 8, new QTableWidgetItem(employee.createdAt.toString("yyyy-MM-dd HH:mm")));
    }
}

void EmployeeWidget::clearForm()
{
    m_currentId = 0;
    m_noEdit->clear();
    m_nameEdit->clear();
    m_genderCombo->setCurrentIndex(0);
    m_ageSpin->setValue(30);
    m_phoneEdit->clear();
    m_positionEdit->clear();
    m_departmentEdit->clear();
    m_table->clearSelection();
}

void EmployeeWidget::onRowSelected(int row)
{
    m_currentId = m_table->item(row, 0)->text().toInt();
    m_noEdit->setText(m_table->item(row, 1)->text());
    m_nameEdit->setText(m_table->item(row, 2)->text());
    m_genderCombo->setCurrentText(m_table->item(row, 3)->text());
    m_ageSpin->setValue(m_table->item(row, 4)->text().toInt());
    m_phoneEdit->setText(m_table->item(row, 5)->text());
    m_positionEdit->setText(m_table->item(row, 6)->text());
    m_departmentEdit->setText(m_table->item(row, 7)->text());
}
