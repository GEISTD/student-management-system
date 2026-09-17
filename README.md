[学生管理系统.md](https://github.com/user-attachments/files/32285722/default.md)
# 学生管理系统 - 代码理解指南

## 📁 项目结构

```
xzProject/code/
├── server/                    # 服务端程序
│   ├── main.cpp               # 程序入口
│   ├── serverwindow.cpp/h     # 服务端UI窗口
│   ├── tcpserver.cpp/h        # TCP服务器核心（处理HTTP请求）
│   ├── databasemanager.cpp/h  # 数据库初始化
│   ├── studentdao.cpp/h       # 学生数据访问
│   ├── userdao.cpp/h          # 用户数据访问
│   ├── scoredao.cpp/h         # 成绩数据访问
│   ├── businesslogic.cpp/h    # 业务逻辑计算
│   ├── student.h              # 学生数据结构
│   ├── user.h                 # 用户数据结构
│   ├── score.h                # 成绩数据结构
│   └── students.db            # SQLite数据库文件
│
└── studentscontroler/         # 客户端程序
    ├── main.cpp               # 程序入口
    ├── UIManager.cpp/h        # UI界面管理（核心）
    ├── networkmanager.cpp/h   # 网络请求管理
    ├── studentdao.cpp/h       # 学生数据访问（调用网络）
    ├── userdao.cpp/h          # 用户数据访问（调用网络）
    ├── scoredao.cpp/h         # 成绩数据访问（调用网络）
    ├── businesslogic.cpp/h    # 业务逻辑计算
    ├── fileexpoter.cpp/h      # 文件导出（CSV）
    ├── student.h              # 学生数据结构（与服务端一致）
    ├── user.h                 # 用户数据结构（与服务端一致）
    └── score.h                # 成绩数据结构（与服务端一致）
```

---

## 📊 核心数据结构

### 1. Student（学生）

| 字段 | 类型 | 说明 |
|------|------|------|
| `id` | int | 数据库主键，自动生成 |
| `studentId` | QString | 学号（如：2024010101） |
| `name` | QString | 姓名 |
| `major` | QString | 专业名称 |
| `className` | QString | 班级名称 |
| `gender` | QString | 性别（"男"/"女"） |
| `birthDate` | QDate | 出生日期 |
| `userId` | int | 关联到 users 表的 id |

### 2. User（用户）

| 字段 | 类型 | 说明 |
|------|------|------|
| `id` | int | 数据库主键 |
| `username` | QString | 用户名（登录用） |
| `passwordHash` | QString | MD5加密后的密码 |
| `role` | QString | 角色（"admin"或"student"） |
| `loginFailCount` | int | 登录失败次数 |
| `lockTime` | QString | 锁定截止时间（空表示未锁定） |

### 3. Score（成绩）

| 字段 | 类型 | 说明 |
|------|------|------|
| `id` | int | 数据库主键 |
| `studentId` | int | 关联学生的 id |
| `subject` | QString | 科目名称 |
| `score` | double | 分数 |
| `examDate` | QDate | 考试日期 |

---

## 🌐 网络通信机制

### 通信流程

```
客户端 ──HTTP POST──► 服务端
    │                    │
    │  JSON请求          │ 解析JSON
    │  {"cmd": "..."}    │ 执行命令
    │                    │
    ◄──JSON响应──────────┤
    {"code": 0, "message": "...", "data": {...}}
```

### 请求格式

```json
{
    "cmd": "命令名称",
    "data": {
        "参数1": "值1",
        "参数2": "值2"
    }
}
```

### 响应格式

| code | 说明 |
|------|------|
| `0` | 成功 |
| `-1` | 失败 |
| `-2` | 未知命令 |

### 客户端发送请求

[networkmanager.cpp](file:///d:/GEIST/xzProject/code/studentscontroler/networkmanager.cpp#L29-L87) 中的 `sendRequest()` 方法：

1. 构造 JSON 请求
2. 发送 HTTP POST 请求
3. 等待响应（最多超时5秒）
4. 解析响应并返回

### 服务端处理请求

[tcpserver.cpp](file:///d:/GEIST/xzProject/code/server/tcpserver.cpp#L157-L239) 中的命令分发：

1. 接收 HTTP 请求
2. 解析 JSON
3. 根据 `cmd` 字段分发到对应的处理函数
4. 返回响应

---

## 🖥️ UI界面结构

### 界面层级

```
QApplication
    └── MainWindow (主窗口)
            ├── LoginDialog (登录对话框) ──登录成功──► MainWidget
            │                                               │
            │                                               ├── 管理员界面 (createAdminUI)
            │                                               │       ├── 学生管理（新增/删除/修改）
            │                                               │       ├── 成绩管理（登记/查看）
            │                                               │       ├── 班级统计（多维度查询）
            │                                               │       └── 用户管理（修改密码/用户名）
            │                                               │
            │                                               └── 学生界面 (createStudentUI)
            │                                               ├── 个人信息查看
            │                                               └── 成绩查看
```

### 关键UI组件

| 组件 | 用途 |
|------|------|
| `LoginDialog` | 登录/注册界面 |
| `MainWidget` | 主界面容器 |
| `QTableWidget` | 数据表格展示 |
| `QComboBox` | 下拉选择（年级/专业/班级/科目） |
| `QPushButton` | 功能按钮 |
| `QMessageBox` | 提示对话框 |

---

## 🔄 常用功能流程

### 1. 登录流程

```
用户输入用户名密码
        │
        ▼
┌──────────────────┐
│ 检查用户名是否存在 │ ── getUser 请求
└──────────────────┘
        │
        ▼
┌──────────────────┐
│ 验证密码是否正确 │ ── validateLogin 请求
└──────────────────┘
        │
        ▼
┌──────────────────┐
│ 创建主界面       │ ── setCurrentUser()
└──────────────────┘
        │
        ▼
┌──────────────────┐
│ 启动心跳检测     │ ── m_heartbeatTimer.start(5000)
└──────────────────┘
```

### 2. 新增学生流程

```
填写学生信息（用户名、密码、姓名、专业、班级等）
        │
        ▼
┌──────────────────┐
│ 创建用户账号     │ ── UserDao::insert()
└──────────────────┘
        │
        ▼
┌──────────────────┐
│ 创建学生记录     │ ── StudentDao::insert()
└──────────────────┘
        │
        ├── 成功 ──► 提示成功，刷新列表
        │
        └── 失败 ──► 自动删除用户账号（回滚）
```

### 3. 心跳检测流程

```
每5秒执行一次 checkServerConnection()
        │
        ▼
┌──────────────────┐
│ 发送 heartbeat   │ ── NetworkManager::checkServerOnline()
└──────────────────┘
        │
        ├── 成功 ──► 继续运行
        │
        └── 失败 ──► 弹出提示框，退出程序
```

---

## 🔍 问题定位指南

### 第一步：判断问题类型

| 现象 | 可能原因 | 排查方向 |
|------|----------|----------|
| **编译错误** | 语法错误、缺少头文件、函数未定义 | 查看编译器报错信息 |
| **程序崩溃** | 空指针、数组越界、内存泄漏 | 查看崩溃提示 |
| **功能不生效** | 逻辑错误、网络不通、数据库问题 | 查看日志或提示信息 |
| **界面显示异常** | 布局问题、样式冲突、缩放问题 | 检查UI代码 |

### 第二步：定位相关文件

| 功能 | 主要文件 |
|------|----------|
| 登录/注册 | [UIManager.cpp](file:///d:/GEIST/xzProject/code/studentscontroler/UIManager.cpp) (LoginDialog) |
| 学生管理 | [UIManager.cpp](file:///d:/GEIST/xzProject/code/studentscontroler/UIManager.cpp) (createAdminUI) |
| 成绩管理 | [UIManager.cpp](file:///d:/GEIST/xzProject/code/studentscontroler/UIManager.cpp) （登记成绩相关） |
| 班级统计 | [UIManager.cpp](file:///d:/GEIST/xzProject/code/studentscontroler/UIManager.cpp) (统计查询相关) |
| 网络请求 | [networkmanager.cpp](file:///d:/GEIST/xzProject/code/studentscontroler/networkmanager.cpp) |
| 服务端处理 | [tcpserver.cpp](file:///d:/GEIST/xzProject/code/server/tcpserver.cpp) |

### 第三步：查看关键代码

#### 网络请求日志

[networkmanager.cpp](file:///d:/GEIST/xzProject/code/studentscontroler/networkmanager.cpp#L44-L86) 中有详细的日志输出：
- 📤 发送请求到: xxx
- 📤 请求 JSON: {...}
- 收到响应（原始）: xxx
- HTTP 状态码: xxx
- 解析后的响应: {...}

#### 服务端命令处理

[tcpserver.cpp](file:///d:/GEIST/xzProject/code/server/tcpserver.cpp#L157-L239) 中的命令分发逻辑，搜索 `cmd == "xxx"` 可以快速定位到对应处理函数。

---

## 📝 给AI的Bug反馈模板

当你遇到问题时，按以下格式描述：

```
【问题描述】
1. 操作步骤：我做了什么（如：点击"新增学生"按钮，填写了姓名、专业等）
2. 预期结果：应该出现什么（如：提示新增成功，列表刷新）
3. 实际结果：实际发生了什么（如：弹出"失败"提示框）
4. 报错信息：如果有完整的报错提示，请复制

【可能的原因】
- 根据现象猜测可能的问题点（如：学生数据插入失败）
- 涉及的功能模块（如：新增学生功能）

【相关文件】
- 根据功能定位表，列出可能相关的文件
```

**示例：**
> 【问题描述】
> 1. 操作步骤：点击"退出登录"按钮
> 2. 预期结果：程序退出
> 3. 实际结果：程序没有退出，而是跳转到登录界面
> 4. 报错信息：无
> 
> 【可能的原因】
> - onLogoutClicked() 方法逻辑不对
> - 应该调用 qApp->quit() 而不是显示登录对话框
> 
> 【相关文件】
> - UIManager.cpp 中的 onLogoutClicked() 方法

---

## 🎯 快速上手建议

1. **先看数据结构**：理解 Student、User、Score 三个核心结构体
2. **再看网络通信**：理解 NetworkManager 和 TcpServer 的工作方式
3. **最后看UI**：理解 UIManager 中各个界面的创建和交互逻辑

遇到问题时，重点关注：
- 网络请求的命令名称是否正确
-DAO层是否正确处理了响应
- UI层是否正确调用了业务逻辑

---

## 📌 重要约定

### 文件命名规范

| 文件类型 | 命名规则 | 示例 |
|----------|----------|------|
| 数据结构 | 小写单词，无后缀 | `student.h` |
| 数据访问 | 单词+Dao | `studentdao.cpp/h` |
| 界面管理 | 首字母大写 | `UIManager.cpp/h` |
| 网络管理 | 首字母大写 | `NetworkManager.cpp/h` |

### 代码风格

- 使用 `// ============================================================` 分隔代码块
- 使用 `qDebug() << "📝 日志信息"` 格式输出日志
- 使用 `QMessageBox` 进行用户提示
- 使用信号槽机制处理界面交互

---

*文档生成时间：2026-07-19*
