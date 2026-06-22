# 林场员工排班管理系统

一个 C++ 课程设计项目，用 Qt Widgets 做的桌面端员工排班管理工具，数据库用的 MySQL。

## 跑起来

环境：**Qt Creator**（Qt 5 或 6 都行，要带 Widgets 和 Sql 模块）+ **MySQL** + **MySQL ODBC 驱动**。

### 1. 初始化数据库

先把 SQL 导进去：

```bash
mysql -u root -p < sql/forest_schedule_system.sql
```

这会建库 `forest_schedule_system`（UTF8MB4）、建表、插入默认管理员 `admin / 123456`（密码存的是 SHA-256 哈希）。

### 2. 配数据库连接

用的是 **QODBC** 驱动（不是 QMYSQL），所以得先装 MySQL ODBC Connector（64 位的，位数要跟 Qt Kit 一致）。

连接参数优先读 `database.ini`，找不到才用代码里的默认值。开发时建议把 `database.ini.example` 复制一份改成 `database.ini`，填上你的数据库信息：

```ini
[Database]
Driver=MySQL ODBC 9.7 Unicode Driver
Server=127.0.0.1
Port=3306
Database=forest_schedule_system
User=root
Password=你的密码
```

驱动名不一定是 `9.7`，去 ODBC 数据源管理器里看一下你装的是什么版本，改成对应的就行。

### 3. 编译运行

Qt Creator 打开 `CMakeLists.txt`，选个 Desktop Kit，直接构建运行。

或者命令行：

```bash
cmake -S . -B build
cmake --build build --config Release
```

CMake 会先找 Qt6，找不到就回退 Qt5。

## 功能

就是四个模块加一个首页：

- **首页仪表盘** — 四个统计数字（员工总数、在职人数、今日出勤、本周排班），下面挂两个表：本周排班明细（周一到周日全显示）和今日出勤明细。切到首页自动刷新。
- **员工管理** — 增删改查，按姓名或编号搜。删除是软删除（`active = 0`），不会真删数据，不然历史排班和出勤关联就断了。
- **周排班管理** — 按员工 + 周起始日期来排，周一到周日每天选一个班次。同一个员工同一周不能重复排。
- **出勤管理** — 按员工 + 日期记录出勤状态（正常/迟到/早退/请假/缺勤），同一个员工同一天不能重复记录。
- **统计查询** — 按员工统计出勤分布、按员工查排班、按日期查出勤。

## 项目结构

```
ForestStaffScheduleSystem/
├── CMakeLists.txt
├── main.cpp                        # 入口
├── database/                       # 数据库连接
│   ├── Config.h / .cpp             #   读 database.ini，回退默认值
│   ├── DatabaseManager.h / .cpp    #   单例，管理 QODBC 连接
│   └── EmployeeComboHelper.h / .cpp#   员工下拉框加载工具
├── model/                          # 数据类（纯字段，没逻辑）
│   ├── Employee.h / .cpp
│   ├── Schedule.h / .cpp
│   ├── ScheduleWeek.h              # 周视图用的聚合模型
│   └── Attendance.h / .cpp
├── dao/                            # 数据库操作（参数化查询）
│   ├── UserDAO.h / .cpp            #   登录验证 + SHA-256
│   ├── EmployeeDAO.h / .cpp        #   员工 CRUD，软删除
│   ├── ScheduleDAO.h / .cpp        #   排班 CRUD
│   └── AttendanceDAO.h / .cpp      #   出勤 CRUD
├── service/                        # 业务校验
│   ├── EmployeeService.h / .cpp    #   年龄 16-80，编号唯一
│   ├── ScheduleService.h / .cpp    #   员工存在性，排班唯一性
│   └── AttendanceService.h / .cpp  #   员工存在性，出勤唯一性
├── ui/                             # 界面
│   ├── AppStyle.h / .cpp           #   全局样式（森林绿配色）
│   ├── LoginDialog.h / .cpp        #   登录框
│   ├── MainWindow.h / .cpp         #   主窗口（左导航 + 右堆叠）
│   ├── DashboardWidget.h / .cpp    #   首页仪表盘
│   ├── EmployeeWidget.h / .cpp     #   员工管理页
│   ├── ScheduleWidget.h / .cpp     #   排班管理页
│   ├── AttendanceWidget.h / .cpp   #   出勤管理页
│   └── StatisticsWidget.h / .cpp   #   统计查询页
├── tests/
│   └── ScheduleWeekTest.cpp
└── sql/
    └── forest_schedule_system.sql
```

调用的层次是：**UI → Service → DAO → DatabaseManager**。UI 层直接持有 DAO/Service 实例当成员变量用的，没有搞依赖注入那一套——课设项目没必要。

每个 DAO 里都有个私有方法 `mapXxx(const QSqlQuery &)` 把查询结果转成 Model 对象，写法比较统一。SQL 全部用 `prepare() + bindValue()` 做的参数化，防注入。

## 数据库表

4 张表：

- **users** — 用户名、密码（SHA-256）、角色
- **employees** — 员工编号（唯一）、姓名、性别、年龄、电话、岗位、部门、在职状态
- **schedules** — 员工 ID + 周起始日期，周一到周日各一个班次字段，有联合唯一约束
- **attendance_records** — 员工 ID + 日期 + 出勤状态 + 备注，有联合唯一约束

## 几个设计上的选择

1. **为啥用 QODBC 而不是 QMYSQL？** QMYSQL 要自己编译 MySQL 插件，配起来挺折腾的。ODBC 驱动去官网下个安装包就行，省事。

2. **数据库连接用了具名连接**（`"forest_schedule_connection"`），避免跟 Qt 默认连接冲突。DatabaseManager 是个 Meyer's Singleton，全局只有一个实例。

3. **配置文件优先于硬编码**。`database.ini` 放 exe 同目录就能改连接参数，不需要重新编译。开发阶段放项目根目录也行，Config 类会先找当前目录。

4. **密码是 SHA-256 哈希存的**，不是明文。`UserDAO` 里有个静态方法 `hashPassword()`，注册和登录都用它。

5. **员工是软删除**，`DELETE` 实际执行的是 `UPDATE active = 0`。因为排班和出勤都关联员工 ID，真删了引用就乱了。

6. **界面配色抄的林场风格**——森林绿主色（`#173f2e`），强调色用 `#2e7d57`。样式全写在 `AppStyle` 类里，一个 `setAppStyle()` 调用搞定全局 QSS。

## QODBC 连不上怎么办

遇到最多的坑就是 ODBC 驱动问题，按顺序排查：

1. ODBC 驱动装了没？去 MySQL 官网下 **MySQL Connector/ODBC 64-bit**
2. 驱动名对不对？默认代码里写的是 `MySQL ODBC 9.7 Unicode Driver`，你装的版本可能不一样，去"ODBC 数据源管理器 → 驱动程序"看一眼
3. 位数一致吗？ODBC 驱动、Qt Kit、MySQL 必须全是 64 位（或全是 32 位），混用会报"architecture mismatch"
4. MySQL 服务起来了没？数据库 `forest_schedule_system` 建了没？

## 截图

> 有空补上，或者你跑了之后自己截几张吧。

## 许可

随便用
