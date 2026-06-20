# 林场员工排班管理系统

基于 **C++17 + Qt Widgets + Qt SQL + MySQL** 的桌面应用，实现员工管理、周排班、出勤记录与统计查询，适合作为课程设计或期末大作业。

## 功能概览

| 模块 | 功能 |
|------|------|
| 🔐 登录 | SHA-256 密码哈希验证，默认管理员账号 `admin / 123456` |
| 📊 首页驾驶舱 | 4 张统计卡片（员工总数、在职人数、今日出勤、本周排班）+ 本周排班速览 + 今日出勤速览 |
| 👤 员工管理 | 增删改查，支持按姓名/编号搜索，软删除（`active = 0`） |
| 📅 周排班管理 | 按员工和周次维护周一到周日每天班次（早班、巡山、值班等） |
| ✅ 出勤管理 | 按员工和日期记录出勤状态（正常/迟到/早退/请假/缺勤） |
| 📈 统计查询 | 按员工统计出勤分布，按员工/周查询排班，按日期查询出勤 |

## 技术栈

- **语言**: C++17
- **UI 框架**: Qt 5 / Qt 6（Widgets 模块）
- **数据库访问**: Qt SQL（QODBC 驱动连接 MySQL）
- **数据库**: MySQL 8.x
- **构建工具**: CMake ≥ 3.16
- **编码**: UTF-8（数据库 UTF8MB4）

## 项目结构

```text
ForestStaffScheduleSystem/
├── CMakeLists.txt                  # CMake 构建配置（Qt6 优先，回退 Qt5）
├── main.cpp                        # 入口：连接数据库 → 登录 → 主窗口
├── database/                       # 数据库连接与配置
│   ├── Config.h / .cpp             # 读取 database.ini，不存在时回退硬编码默认值
│   ├── DatabaseManager.h / .cpp    # 单例，管理 QODBC 连接生命周期（具名连接）
│   └── EmployeeComboHelper.h / .cpp# 工具：将在职员工加载到下拉框
├── model/                          # 数据模型（纯数据类，无业务逻辑）
│   ├── Employee.h / .cpp           # 员工
│   ├── Schedule.h / .cpp           # 周排班
│   └── Attendance.h / .cpp         # 出勤记录
├── dao/                            # 数据访问层（参数化 SQL，prepare + bindValue）
│   ├── UserDAO.h / .cpp            # 登录验证 + SHA-256 哈希
│   ├── EmployeeDAO.h / .cpp        # 员工 CRUD + 搜索 + 计数；软删除
│   ├── ScheduleDAO.h / .cpp        # 排班 CRUD + 按周/员工查询 + 唯一性检查
│   └── AttendanceDAO.h / .cpp      # 出勤 CRUD + 按日期/员工查询 + 统计 + 精确查找
├── service/                        # 业务服务层（校验逻辑）
│   ├── EmployeeService.h / .cpp    # 校验必填字段、年龄 16-80、编号唯一性
│   ├── ScheduleService.h / .cpp    # 校验员工存在性、周起始日期、排班唯一性
│   └── AttendanceService.h / .cpp  # 校验员工存在性、出勤状态合法性、日期唯一性
├── ui/                             # 用户界面层
│   ├── AppStyle.h / .cpp           # 全局 QSS 样式表（森林绿配色）
│   ├── LoginDialog.h / .cpp        # 登录对话框
│   ├── MainWindow.h / .cpp         # 主窗口：左侧导航 + 顶部标题栏 + QStackedWidget
│   ├── DashboardWidget.h / .cpp    # 首页驾驶舱：统计卡片 + 排班/出勤速览表
│   ├── EmployeeWidget.h / .cpp     # 员工管理页：表单 + 表格
│   ├── ScheduleWidget.h / .cpp     # 周排班管理页
│   ├── AttendanceWidget.h / .cpp   # 出勤管理页
│   └── StatisticsWidget.h / .cpp   # 统计查询页
└── sql/
    └── forest_schedule_system.sql  # 数据库初始化脚本（建库 + 建表 + 默认管理员）
```

## 架构

```
┌─────────────────────────────────────────┐
│                  UI 层                   │
│  LoginDialog → MainWindow (Stacked)     │
│  Dashboard / Employee / Schedule /      │
│  Attendance / Statistics               │
└────────────────┬────────────────────────┘
                 │ 调用
┌────────────────▼────────────────────────┐
│               Service 层                 │
│  业务校验（必填、范围、唯一性、关联）      │
└────────────────┬────────────────────────┘
                 │ 调用
┌────────────────▼────────────────────────┐
│                DAO 层                    │
│  SQL 参数化查询（prepare + bindValue）    │
└────────────────┬────────────────────────┘
                 │ 获取连接
┌────────────────▼────────────────────────┐
│           DatabaseManager（单例）        │
│  QODBC 具名连接，生命周期管理            │
└─────────────────────────────────────────┘
```

- **Model** 层是纯数据结构，字段与数据库列对应，不含业务逻辑
- **DAO** 层通过 `DatabaseManager::instance().database()` 获取连接，每个 DAO 有私有 `mapXxx()` 方法将结果集映射为 Model 对象
- **Service** 层持有对应 DAO 实例，负责校验；需要跨表校验时额外持有其他 DAO
- **UI** 层页面持有 DAO/Service 实例作为成员（非依赖注入），简单查询场景可直接使用 DAO

## 数据库

### 表结构

| 表名 | 说明 | 关键字段 |
|------|------|---------|
| `users` | 系统用户 | username, password (SHA-256), role |
| `employees` | 员工信息 | employee_no (UNIQUE), name, gender, age, phone, position, department, active |
| `schedules` | 周排班 | employee_id (FK), week_start_date, monday_shift ~ sunday_shift, UNIQUE(employee_id, week_start_date) |
| `attendance_records` | 出勤记录 | employee_id (FK), attendance_date, status, remark, UNIQUE(employee_id, attendance_date) |

### 初始化

```bash
mysql -u root -p < sql/forest_schedule_system.sql
```

该脚本会创建数据库 `forest_schedule_system`（UTF8MB4），建立 4 张表，并插入默认管理员。

**默认管理员**：用户名 `admin`，密码 `123456`（SHA-256 哈希存储，明文不可见）。

## 数据库连接配置

项目使用 **QODBC** 驱动连接 MySQL（非 QMYSQL），需要安装 **64 位 MySQL ODBC 驱动**（位数必须与 Qt Kit 一致）。

### 方式一：配置文件（推荐）

复制 `database.ini.example` 为 `database.ini`，放在可执行文件同目录（或项目源码根目录用于开发阶段），按需修改：

```ini
[Database]
Driver=MySQL ODBC 9.7 Unicode Driver
Server=127.0.0.1
Port=3306
Database=forest_schedule_system
User=root
Password=your_password
```

若配置文件不存在，程序自动使用 `database/Config.cpp` 中的硬编码默认值，无需重新编译。

### 方式二：修改源码

直接修改 `database/Config.cpp` 中 `readValue()` 各方法的默认值参数。

## 编译运行

### Qt Creator（推荐）

1. 打开 `CMakeLists.txt`
2. 选择 Qt 5 或 Qt 6 的 Desktop Kit（需启用 `Widgets` 和 `Sql` 模块）
3. 构建并运行

### 命令行

```bash
cmake -S . -B build
cmake --build build --config Release
```

CMake 自动检测 Qt6，若未找到则回退到 Qt5。

### QODBC 驱动问题排查

如果运行时提示数据库连接失败：

1. **确认 MySQL ODBC 驱动已安装**：搜索"MySQL ODBC Connector 64-bit"下载安装
2. **确认驱动名称匹配**：默认 `MySQL ODBC 9.7 Unicode Driver`，可通过 ODBC 数据源管理器查看已安装驱动名称，然后在 `database.ini` 中修改 `Driver` 字段
3. **确认位数一致**：ODBC 驱动、Qt Kit、MySQL 客户端库必须同为 64 位（或同为 32 位）
4. **确认 MySQL 服务已启动**，且 `forest_schedule_system` 数据库已创建

## 界面说明

### 主窗口布局

```
┌──────────┬──────────────────────────────────────┐
│  品牌区   │  页面标题 / 副标题        数据库状态   │
│  Logo    ├──────────────────────────────────────┤
│          │                                      │
│ ● 首页   │                                      │
│   员工   │        QStackedWidget                │
│   排班   │        （5 个页面切换）                │
│   出勤   │                                      │
│   统计   │                                      │
│          │                                      │
│  退出    │                                      │
└──────────┴──────────────────────────────────────┘
  NavPane              TopBar + Content
  (210px)
```

- 左侧导航按钮通过 `QButtonGroup` 互斥切换
- 切换到首页时自动刷新统计数据
- "退出系统"按钮带确认对话框

### 全局样式

`AppStyle` 定义了统一的森林绿配色（主色 `#173f2e`、强调色 `#2e7d57`），包括：
- 导航按钮选中态（左侧绿色边框高亮）
- 表格斑马纹、隐藏行号
- 统计卡片白色圆角卡片样式
- 主操作按钮（绿色）和危险操作按钮（红色）
- 输入框焦点绿色边框

## 核心设计决策

| 决策 | 说明 |
|------|------|
| QODBC 而非 QMYSQL | QMYSQL 需要自行编译 MySQL 插件，ODBC 驱动安装即用，减少部署问题 |
| 具名连接 | 使用 `"forest_schedule_connection"` 避免与 Qt 默认连接冲突 |
| 软删除员工 | `DELETE` 改为 `UPDATE active = 0`，保留历史排班和出勤数据的引用完整性 |
| SHA-256 密码 | 不存储明文密码，登录时对输入做 SHA-256 后比对 |
| 参数化查询 | 所有 SQL 使用 `prepare()` + `bindValue()`，防止 SQL 注入 |
| 配置文件优先 | `database.ini` 可在运行时修改，无需重新编译 |
