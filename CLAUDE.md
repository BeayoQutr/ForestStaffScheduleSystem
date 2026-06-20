# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## 项目简介

林场员工排班管理系统 — 基于 C++17、Qt Widgets、Qt SQL 和 MySQL 的桌面应用，适合作为课程设计或期末大作业。

## 编译运行

推荐使用 **Qt Creator**：打开 `CMakeLists.txt`，选择 Qt 5 或 Qt 6 的 Desktop Kit（需启用 `Widgets` 和 `Sql` 模块），构建并运行。

命令行方式：

```bash
cmake -S . -B build
cmake --build build --config Release
```

CMake 会自动检测 Qt6，若未找到则回退到 Qt5。

## 数据库

### 初始化

```bash
mysql -u root -p < sql/forest_schedule_system.sql
```

默认管理员：`admin` / `123456`（密码使用 SHA-256 哈希存储，明文不可见）

### 连接方式

项目使用 **QODBC** 驱动连接 MySQL（非 QMYSQL）。连接参数优先从 `database.ini` 读取，若文件不存在则使用 `database/Config.cpp` 中的硬编码默认值。

修改数据库连接：复制 `database.ini.example` 为 `database.ini`，编辑其中参数即可，无需重新编译。

要求安装 **64 位** MySQL ODBC 驱动（当前默认驱动名 `MySQL ODBC 9.7 Unicode Driver`），位数必须与 Qt Kit 一致。

## 架构

整体为三层架构，调用链为 **UI → Service → DAO → DatabaseManager**：

### Model 层 (`model/`)

纯数据类，字段对应数据库列，无业务逻辑：
- `Employee` — 员工（id, employeeNo, name, gender, age, phone, position, department, active）
- `Schedule` — 周排班（employeeId, weekStartDate, 周一到周日各一个 shift 字段）
- `Attendance` — 出勤记录（employeeId, attendanceDate, status, remark）

### DAO 层 (`dao/`)

直接操作数据库，使用参数化查询（`prepare` + `bindValue`），通过 `DatabaseManager::instance().database()` 获取连接。
- `UserDAO` — `login()` 验证（密码经 SHA-256 哈希后比对）；提供静态方法 `hashPassword()`
- `EmployeeDAO` — CRUD + 搜索 + 计数；软删除（`active = 0`）
- `ScheduleDAO` — CRUD + 按周/员工查询 + 唯一性检查
- `AttendanceDAO` — CRUD + 按日期/员工查询 + 统计 + `findIdByEmployeeAndDate()` 精确查找记录 ID

公共模式：每个 DAO 有一个私有 `mapXxx(const QSqlQuery &)` 方法将结果集映射为 Model 对象。

### Service 层 (`service/`)

封装业务校验逻辑：
- `EmployeeService` — 校验必填字段、年龄范围（16-80）、员工编号唯一性
- `ScheduleService` — 校验员工存在性、周起始日期、排班唯一性
- `AttendanceService` — 校验员工存在性、出勤唯一性

Service 层持有对应 DAO 实例作为私有成员，只有读写自身表需求时会额外持有其他 DAO（如 ScheduleService 需要 EmployeeDAO 校验员工是否存在）。

### DatabaseManager (`database/`)

单例（Meyer's Singleton），管理 `QSqlDatabase` 连接生命周期。使用具名连接 `"forest_schedule_connection"` 避免与默认连接冲突。

`Config` 类负责读取 `database.ini` 配置文件，提供 `driver()`, `server()`, `port()`, `databaseName()`, `user()`, `password()` 等静态方法。未找到配置文件时回退到硬编码默认值。

`EmployeeComboHelper` 是一个静态工具类，提供 `loadEmployeesInto(QComboBox*, EmployeeDAO&)` 方法，用于将在职员工列表加载到下拉框并保留当前选中项。`ScheduleWidget`、`AttendanceWidget`、`StatisticsWidget` 均通过它加载员工下拉框，避免重复代码。

### UI 层 (`ui/`)

入口 `main.cpp`：先连接数据库 → 显示登录对话框 → 打开主窗口。

- **MainWindow** — 左侧导航栏（210px 固定宽）+ 顶部标题栏 + 右侧 `QStackedWidget` 的后台管理布局。5 个页面通过 `QButtonGroup` 互斥切换，切换首页时自动刷新统计数据。
- **AppStyle** — 全局 QSS 样式表，定义了统一的森林绿（`#173f2e`、`#2e7d57`）配色、导航按钮选中态、表格斑马纹、统计卡片等样式。
- **DashboardWidget** — 首页驾驶舱，4 个统计卡片（员工总数、在职人数、今日出勤、本周排班）+ 本周排班速览表（完整显示周一到周日 7 天）和今日出勤速览表。切换到首页时自动刷新。
- **LoginDialog** — 登录对话框，调用 `UserDAO::login()`。
- **EmployeeWidget** — 员工管理页，表单 + 表格，支持增删改查。
- **ScheduleWidget** — 周排班管理页，按员工和周次维护每天班次。
- **AttendanceWidget** — 出勤管理页，按员工和日期记录出勤状态。
- **StatisticsWidget** — 统计查询页，按员工或日期查询出勤和排班统计。

UI 页面直接持有 DAO/Service 实例作为成员（非依赖注入），部分页面为简单查询场景直接使用 DAO 跳过 Service。

## Qt 元对象编译

CMake 配置了 `CMAKE_AUTOMOC ON`，所有 `Q_OBJECT` 宏的类会自动运行 moc。新增使用信号/槽的 QObject 子类时需要保留头文件中的 `Q_OBJECT` 声明并在 CMakeLists.txt 中添加文件。
