#include "UIManager.h"
#include "studentdao.h"
#include "userdao.h"
#include "scoredao.h"
#include "businesslogic.h"
#include "fileexpoter.h"
#include "networkmanager.h"
#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QCryptographicHash>
#include <QComboBox>
#include <QDateEdit>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QAbstractItemView>
#include <QCheckBox>
#include <algorithm>
#include <QScrollArea>
#include <QTextEdit>

// ============================================================
// StyleHelper 实现
// ============================================================
StyleHelper::StyleHelper() : m_scaleFactor(1.0) {}

StyleHelper& StyleHelper::instance()
{
    static StyleHelper instance;
    return instance;
}

void StyleHelper::setScaleFactor(double scale)
{
    m_scaleFactor = qMax(0.5, qMin(2.0, scale));
}

double StyleHelper::scaleFactor() const
{
    return m_scaleFactor;
}

QFont StyleHelper::generateFont() const
{
    QFont font("Microsoft YaHei", 14 * m_scaleFactor);
    font.setStyleStrategy(QFont::PreferAntialias);
    return font;
}

QString StyleHelper::generateStylesheet() const
{
    int baseSize = 15 * m_scaleFactor;
    int buttonHeight = 40 * m_scaleFactor;
    int inputHeight = 46 * m_scaleFactor;
    int borderRadius = 8 * m_scaleFactor;
    int smallRadius = 6 * m_scaleFactor;
    int margin = 8 * m_scaleFactor;
    int padding = 12 * m_scaleFactor;

    return QString(R"(
        QWidget {
            font-family: "Microsoft YaHei", "Segoe UI", sans-serif;
            font-size: %1px;
            background-color: #F8FAFC;
            color: #1E293B;
        }
        QMainWindow { background-color: #F8FAFC; }
        QDialog { background-color: #FFFFFF; border-radius: %2px; }
        QLabel { color: #334155; }
        QLineEdit {
            height: %11px;
            padding: %4px %5px;
            border: 1px solid #E2E8F0;
            border-radius: %6px;
            background-color: #FFFFFF;
            selection-background-color: #3B82F6;
            font-size: %1px;
        }
        QLineEdit:focus {
            border-color: #3B82F6;
            border-width: 2px;
            outline: none;
            font-size: %1px;
        }
        QComboBox {
            height: %11px;
            padding: %4px %5px;
            border: 1px solid #E2E8F0;
            border-radius: %6px;
            background-color: #FFFFFF;
            min-width: 140px;
            font-size: %1px;
        }
        QComboBox:focus {
            border-color: #3B82F6;
            border-width: 2px;
            font-size: %1px;
        }
        QComboBox::drop-down {
            border: none;
            width: 30px;
        }
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #64748B;
            margin-right: 8px;
        }
        QDateEdit {
            height: %11px;
            padding: %4px %5px;
            border: 1px solid #E2E8F0;
            border-radius: %6px;
            background-color: #FFFFFF;
            font-size: %1px;
        }
        QDateEdit:focus {
            border-color: #3B82F6;
            border-width: 2px;
            font-size: %1px;
        }
        QDoubleSpinBox {
            height: %11px;
            padding: %4px %5px;
            border: 1px solid #E2E8F0;
            border-radius: %6px;
            background-color: #FFFFFF;
            font-size: %1px;
        }
        QDoubleSpinBox:focus {
            border-color: #3B82F6;
            border-width: 2px;
            font-size: %1px;
        }
        QPushButton {
            height: %7px;
            padding: 0 %8px;
            border: none;
            border-radius: %6px;
            font-weight: 500;
            background-color: #3B82F6;
            color: #FFFFFF;
            min-width: 80px;
        }
        QPushButton:hover { background-color: #2563EB; }
        QPushButton:pressed { background-color: #1D4ED8; }
        QPushButton:disabled { background-color: #94A3B8; }
        QPushButton#secondary { background-color: #FFFFFF; color: #475569; border: 1px solid #E2E8F0; }
        QPushButton#secondary:hover { background-color: #F1F5F9; border-color: #CBD5E1; }
        QPushButton#danger { background-color: #EF4444; color: #FFFFFF; }
        QPushButton#danger:hover { background-color: #DC2626; }
        QTableWidget {
            border: 1px solid #E2E8F0;
            border-radius: %6px;
            background-color: #FFFFFF;
            gridline-color: #E2E8F0;
        }
        QTableWidget::item {
            padding: %9px %10px;
            border-bottom: 1px solid #F1F5F9;
            text-overflow: ellipsis;
            white-space: nowrap;
        }
        QTableWidget::item:selected { background-color: #DBEAFE; color: #1E40AF; }
        QTableWidget::item:hover { background-color: #F8FAFC; }
        QHeaderView::section {
            height: %3px;
            padding: 0 %10px;
            border: none;
            border-bottom: 2px solid #E2E8F0;
            background-color: #F8FAFC;
            color: #64748B;
            font-weight: 600;
            text-align: left;
        }
        QGroupBox {
            border: 1px solid #E2E8F0;
            border-radius: %6px;
            padding-top: 20px;
            font-weight: 600;
            color: #334155;
            background-color: #FFFFFF;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 16px;
            padding: 0 10px;
        }
        QMessageBox { background-color: #FFFFFF; }
        QMessageBox QLabel { color: #1E293B; font-size: %1px; }
        QDialogButtonBox QPushButton { height: %7px; padding: 0 %8px; min-width: 80px; }
        QScrollBar:vertical {
            width: 8px;
            background: transparent;
        }
        QScrollBar::handle:vertical {
            background: #CBD5E1;
            border-radius: 4px;
            min-height: 30px;
        }
        QScrollBar::handle:vertical:hover { background: #94A3B8; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    )").arg(baseSize).arg(borderRadius).arg(baseSize).arg(padding/2).arg(padding/2).arg(smallRadius).arg(buttonHeight).arg(padding).arg(margin).arg(padding/3).arg(inputHeight);
}

void StyleHelper::applyGlobalStyle()
{
    QApplication::setFont(generateFont());
    qApp->setStyleSheet(generateStylesheet());
}

// ============================================================
// LoginDialog
// ============================================================
LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("学生管理系统 - 登录");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumSize(450, 520);
    resize(500, 560);
    setSizeGripEnabled(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(40, 40, 40, 40);

    QLabel *titleLabel = new QLabel("学生管理系统", this);
    titleLabel->setStyleSheet("font-size: 28px; font-weight: 600; color: #1E293B;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel("欢迎登录", this);
    subtitleLabel->setStyleSheet("font-size: 16px; color: #64748B;");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(subtitleLabel);

    QWidget *formWidget = new QWidget(this);
    formWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    QVBoxLayout *formLayout = new QVBoxLayout(formWidget);
    formLayout->setSpacing(12);

    QLabel *userLabel = new QLabel("用户名", this);
    userLabel->setStyleSheet("font-weight: 500; color: #475569;");
    formLayout->addWidget(userLabel);

    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText("请输入用户名");
    m_usernameEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    formLayout->addWidget(m_usernameEdit);

    QLabel *passLabel = new QLabel("密码", this);
    passLabel->setStyleSheet("font-weight: 500; color: #475569;");
    formLayout->addWidget(passLabel);

    QWidget *passWidget = new QWidget(this);
    passWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *passLayout = new QHBoxLayout(passWidget);
    passLayout->setContentsMargins(0, 0, 0, 0);
    passLayout->setSpacing(8);

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setPlaceholderText("请输入密码");
    m_passwordEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    passLayout->addWidget(m_passwordEdit);

    m_showPasswordBtn = new QPushButton(this);
    m_showPasswordBtn->setObjectName("showPassword");
    m_showPasswordBtn->setFixedSize(28, 28);
    m_showPasswordBtn->setStyleSheet(R"(
        QPushButton#showPassword {
            border: none;
            background-color: transparent;
            font-size: 18px;
            padding: 0;
            color: #64748B;
            font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
        }
        QPushButton#showPassword:hover {
            background-color: #E2E8F0;
            border-radius: 4px;
            color: #3B82F6;
        }
    )");
    m_showPasswordBtn->setText("👁");
    m_isPasswordShowing = false;
    connect(m_showPasswordBtn, &QPushButton::clicked, this, [this]() {
        m_isPasswordShowing = !m_isPasswordShowing;
        m_passwordEdit->setEchoMode(m_isPasswordShowing ? QLineEdit::Normal : QLineEdit::Password);
        m_showPasswordBtn->setText(m_isPasswordShowing ? "👁‍🗨" : "👁");
        m_showPasswordBtn->setStyleSheet(m_isPasswordShowing ? R"(
            QPushButton#showPassword {
                border: none;
                background-color: transparent;
                font-size: 18px;
                padding: 0;
                color: #3B82F6;
                font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
            }
            QPushButton#showPassword:hover {
                background-color: #E2E8F0;
                border-radius: 4px;
                color: #2563EB;
            }
        )" : R"(
            QPushButton#showPassword {
                border: none;
                background-color: transparent;
                font-size: 18px;
                padding: 0;
                color: #64748B;
                font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
            }
            QPushButton#showPassword:hover {
                background-color: #E2E8F0;
                border-radius: 4px;
                color: #3B82F6;
            }
        )");
    });
    passLayout->addWidget(m_showPasswordBtn);

    formLayout->addWidget(passWidget);
    mainLayout->addWidget(formWidget);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet("color: #EF4444; font-size: 14px;");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_statusLabel);

    QWidget *btnWidget = new QWidget(this);
    QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);
    btnLayout->setSpacing(12);

    m_loginBtn = new QPushButton("登录", this);
    m_loginBtn->setObjectName("primary");
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(8);
    shadow->setColor(QColor(59, 130, 246, 40));
    shadow->setOffset(0, 2);
    m_loginBtn->setGraphicsEffect(shadow);

    m_registerBtn = new QPushButton("注册", this);
    m_registerBtn->setObjectName("secondary");

    m_cancelBtn = new QPushButton("取消", this);
    m_cancelBtn->setObjectName("secondary");

    btnLayout->addWidget(m_loginBtn);
    btnLayout->addWidget(m_registerBtn);
    btnLayout->addWidget(m_cancelBtn);
    mainLayout->addWidget(btnWidget);

    m_usernameEdit->setFocus();

    connect(m_loginBtn, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(m_cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_registerBtn, &QPushButton::clicked, this, &LoginDialog::onRegisterClicked);
    connect(m_usernameEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
}
QString LoginDialog::getUsername() const
{
    return m_usernameEdit->text().trimmed();
}

QString LoginDialog::getPasswordHash() const
{
    QString password = m_passwordEdit->text();
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5);
    return hash.toHex();
}

void LoginDialog::onLoginClicked()
{
    QString username = getUsername();
    QString password = m_passwordEdit->text();

    if (username.isEmpty()) {
        m_statusLabel->setText("请输入用户名");
        m_usernameEdit->setFocus();
        return;
    }

    if (password.isEmpty()) {
        m_statusLabel->setText("请输入密码");
        m_passwordEdit->setFocus();
        return;
    }

    QString passwordHash = getPasswordHash();

    // 直接验证登录，避免先 getUser 造成信息泄露和双重请求
    QJsonObject resp = NetworkManager::instance().sendRequest("validateLogin",
        QJsonObject{{"username", username}, {"password_hash", passwordHash}});
    if (resp.value("code").toInt() != 0) {
        // 验证失败，检查是否因锁定导致
        User u = UserDao::selectByUsername(username);
        if (u.id != 0) {
            int remainSec = UserDao::getLockRemainingSeconds(u.id);
            if (remainSec > 0) {
                showLockDialog(remainSec);
                m_passwordEdit->clear();
                m_passwordEdit->setFocus();
                m_statusLabel->clear();
                return;
            }
        }
        m_statusLabel->setText("用户名或密码错误");
        m_passwordEdit->clear();
        m_passwordEdit->setFocus();
        return;
    }

    m_statusLabel->setText("");
    accept();
}

void LoginDialog::onRegisterClicked()
{
    RegisterDialog registerDlg(this);
    if (registerDlg.exec() == QDialog::Accepted) {
        m_statusLabel->setText("注册成功，请登录");
        m_statusLabel->setStyleSheet("color: #22C55E; font-size: 14px;");
    }
}

void LoginDialog::showLockDialog(int remainSec)
{
    if (remainSec <= 0) return;

    QDialog dialog(this);
    dialog.setWindowTitle("账号已锁定");
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
    dialog.setModal(true);
    dialog.setMinimumSize(400, 220);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(32, 32, 32, 32);

    QLabel *iconLabel = new QLabel("🔒", &dialog);
    iconLabel->setStyleSheet("font-size: 48px;");
    iconLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(iconLabel);

    QLabel *titleLabel = new QLabel("账号已锁定", &dialog);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: 600; color: #1E293B;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QLabel *descLabel = new QLabel("密码输入错误次数过多，请等待解锁", &dialog);
    descLabel->setStyleSheet("font-size: 14px; color: #64748B;");
    descLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(descLabel);

    QLabel *countdownLabel = new QLabel(&dialog);
    countdownLabel->setStyleSheet("font-size: 18px; font-weight: 600; color: #EF4444;");
    countdownLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(countdownLabel);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, &dialog);
    QPushButton *okBtn = buttonBox->button(QDialogButtonBox::Ok);
    okBtn->setText("确定");
    okBtn->setObjectName("secondary");
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

    QTimer timer;
    int remaining = remainSec;

    auto updateLabel = [&]() {
        int minutes = remaining / 60;
        int seconds = remaining % 60;
        countdownLabel->setText(QString("%1 分 %2 秒")
                                .arg(minutes).arg(seconds, 2, 10, QChar('0')));
        if (--remaining <= 0) {
            timer.stop();
            dialog.accept();
        }
    };

    updateLabel();
    connect(&timer, &QTimer::timeout, this, updateLabel);
    timer.start(1000);

    dialog.exec();

    if (timer.isActive()) timer.stop();
}

// ============================================================
// RegisterDialog
// ============================================================
RegisterDialog::RegisterDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("学生管理系统 - 注册");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumSize(520, 480);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(32, 32, 32, 32);

    QLabel *titleLabel = new QLabel("学生注册", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: 600; color: #1E293B;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QWidget *formWidget = new QWidget(this);
    QFormLayout *formLayout = new QFormLayout(formWidget);
    formLayout->setSpacing(12);
    formLayout->setLabelAlignment(Qt::AlignLeft);
    formLayout->setFormAlignment(Qt::AlignLeft);

    m_usernameEdit = new QLineEdit(this);
    m_usernameEdit->setPlaceholderText("请输入用户名（至少3个字符）");

    QWidget *passWidget = new QWidget(this);
    QHBoxLayout *passLayout = new QHBoxLayout(passWidget);
    passLayout->setContentsMargins(0, 0, 0, 0);
    passLayout->setSpacing(8);

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setPlaceholderText("请输入密码（至少6个字符）");
    passLayout->addWidget(m_passwordEdit);

    m_showPasswordBtn = new QPushButton(this);
    m_showPasswordBtn->setObjectName("showPassword");
    m_showPasswordBtn->setFixedSize(28, 28);
    m_showPasswordBtn->setStyleSheet(R"(
        QPushButton#showPassword {
            border: none;
            background-color: transparent;
            font-size: 18px;
            padding: 0;
            color: #64748B;
            font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
        }
        QPushButton#showPassword:hover {
            background-color: #E2E8F0;
            border-radius: 4px;
            color: #3B82F6;
        }
    )");
    m_showPasswordBtn->setText("👁");
    m_isPasswordShowing = false;
    connect(m_showPasswordBtn, &QPushButton::clicked, this, [this]() {
        m_isPasswordShowing = !m_isPasswordShowing;
        m_passwordEdit->setEchoMode(m_isPasswordShowing ? QLineEdit::Normal : QLineEdit::Password);
        m_showPasswordBtn->setText(m_isPasswordShowing ? "👁‍🗨" : "👁");
        m_showPasswordBtn->setStyleSheet(m_isPasswordShowing ? R"(
            QPushButton#showPassword {
                border: none;
                background-color: transparent;
                font-size: 18px;
                padding: 0;
                color: #3B82F6;
                font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
            }
            QPushButton#showPassword:hover {
                background-color: #E2E8F0;
                border-radius: 4px;
                color: #2563EB;
            }
        )" : R"(
            QPushButton#showPassword {
                border: none;
                background-color: transparent;
                font-size: 18px;
                padding: 0;
                color: #64748B;
                font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
            }
            QPushButton#showPassword:hover {
                background-color: #E2E8F0;
                border-radius: 4px;
                color: #3B82F6;
            }
        )");
    });
    passLayout->addWidget(m_showPasswordBtn);

    QWidget *confirmWidget = new QWidget(this);
    QHBoxLayout *confirmLayout = new QHBoxLayout(confirmWidget);
    confirmLayout->setContentsMargins(0, 0, 0, 0);
    confirmLayout->setSpacing(8);

    m_confirmEdit = new QLineEdit(this);
    m_confirmEdit->setEchoMode(QLineEdit::Password);
    m_confirmEdit->setPlaceholderText("请再次输入密码");
    confirmLayout->addWidget(m_confirmEdit);

    m_showConfirmBtn = new QPushButton(this);
    m_showConfirmBtn->setObjectName("showPassword");
    m_showConfirmBtn->setFixedSize(28, 28);
    m_showConfirmBtn->setStyleSheet(R"(
        QPushButton#showPassword {
            border: none;
            background-color: transparent;
            font-size: 18px;
            padding: 0;
            color: #64748B;
            font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
        }
        QPushButton#showPassword:hover {
            background-color: #E2E8F0;
            border-radius: 4px;
            color: #3B82F6;
        }
    )");
    m_showConfirmBtn->setText("👁");
    m_isConfirmShowing = false;
    connect(m_showConfirmBtn, &QPushButton::clicked, this, [this]() {
        m_isConfirmShowing = !m_isConfirmShowing;
        m_confirmEdit->setEchoMode(m_isConfirmShowing ? QLineEdit::Normal : QLineEdit::Password);
        m_showConfirmBtn->setText(m_isConfirmShowing ? "👁‍🗨" : "👁");
        m_showConfirmBtn->setStyleSheet(m_isConfirmShowing ? R"(
            QPushButton#showPassword {
                border: none;
                background-color: transparent;
                font-size: 18px;
                padding: 0;
                color: #3B82F6;
                font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
            }
            QPushButton#showPassword:hover {
                background-color: #E2E8F0;
                border-radius: 4px;
                color: #2563EB;
            }
        )" : R"(
            QPushButton#showPassword {
                border: none;
                background-color: transparent;
                font-size: 18px;
                padding: 0;
                color: #64748B;
                font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
            }
            QPushButton#showPassword:hover {
                background-color: #E2E8F0;
                border-radius: 4px;
                color: #3B82F6;
            }
        )");
    });
    confirmLayout->addWidget(m_showConfirmBtn);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("请输入真实姓名");

    m_majorCombo = new QComboBox(this);
    m_majorCombo->addItem("计算机科学与技术", "01");
    m_majorCombo->addItem("软件工程", "02");
    m_majorCombo->addItem("人工智能", "03");
    m_majorCombo->addItem("数据科学", "04");
    m_majorCombo->addItem("信息管理", "05");

    m_classCombo = new QComboBox(this);
    m_classCombo->addItem("1班", "01");
    m_classCombo->addItem("2班", "02");
    m_classCombo->addItem("3班", "03");
    m_classCombo->addItem("4班", "04");

    m_genderCombo = new QComboBox(this);
    m_genderCombo->addItems({"男", "女"});

    m_birthEdit = new QDateEdit(this);
    m_birthEdit->setDate(QDate(2000, 1, 1));
    m_birthEdit->setCalendarPopup(true);
    m_birthEdit->setDisplayFormat("yyyy/MM/dd");

    formLayout->addRow("用户名:", m_usernameEdit);
    formLayout->addRow("密码:", passWidget);
    formLayout->addRow("确认密码:", confirmWidget);
    formLayout->addRow("真实姓名:", m_nameEdit);
    formLayout->addRow("专业:", m_majorCombo);
    formLayout->addRow("班级:", m_classCombo);
    formLayout->addRow("性别:", m_genderCombo);
    formLayout->addRow("出生日期:", m_birthEdit);

    mainLayout->addWidget(formWidget);

    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet("color: #EF4444; font-size: 14px;");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_statusLabel);

    QWidget *btnWidget = new QWidget(this);
    QHBoxLayout *btnLayout = new QHBoxLayout(btnWidget);
    btnLayout->setSpacing(12);

    m_registerBtn = new QPushButton("注册", this);
    m_registerBtn->setObjectName("primary");
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(8);
    shadow->setColor(QColor(59, 130, 246, 40));
    shadow->setOffset(0, 2);
    m_registerBtn->setGraphicsEffect(shadow);

    m_cancelBtn = new QPushButton("取消", this);
    m_cancelBtn->setObjectName("secondary");

    btnLayout->addWidget(m_registerBtn);
    btnLayout->addWidget(m_cancelBtn);
    mainLayout->addWidget(btnWidget);

    connect(m_registerBtn, &QPushButton::clicked, this, &RegisterDialog::onRegisterClicked);
    connect(m_cancelBtn, &QPushButton::clicked, this, &RegisterDialog::onCancelClicked);

    connect(m_usernameEdit, &QLineEdit::returnPressed, this, &RegisterDialog::onRegisterClicked);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &RegisterDialog::onRegisterClicked);
    connect(m_confirmEdit, &QLineEdit::returnPressed, this, &RegisterDialog::onRegisterClicked);
    connect(m_nameEdit, &QLineEdit::returnPressed, this, &RegisterDialog::onRegisterClicked);
    connect(m_classCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) { m_classCombo->clearFocus(); });
}

void RegisterDialog::onRegisterClicked()
{
    QString username = m_usernameEdit->text().trimmed();
    QString password = m_passwordEdit->text();
    QString confirm = m_confirmEdit->text();
    QString name = m_nameEdit->text().trimmed();
    QString major = m_majorCombo->currentText();
    QString className = m_classCombo->currentText();
    QString gender = m_genderCombo->currentText();
    QDate birthDate = m_birthEdit->date();

    if (username.isEmpty()) { m_statusLabel->setText("请输入用户名"); return; }
    if (username.length() < 3) { m_statusLabel->setText("用户名至少3个字符"); return; }
    if (password.isEmpty()) { m_statusLabel->setText("请输入密码"); return; }
    if (password.length() < 6) { m_statusLabel->setText("密码至少6个字符"); return; }
    if (password != confirm) { m_statusLabel->setText("两次密码输入不一致"); return; }
    if (name.isEmpty()) { m_statusLabel->setText("请输入真实姓名"); return; }

    User existing = UserDao::selectByUsername(username);
    if (existing.id != 0) { m_statusLabel->setText("用户名已被注册"); return; }

    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5);
    QString passwordHash = hash.toHex();

    User user;
    user.username = username;
    user.passwordHash = passwordHash;
    user.role = "student";
    if (!UserDao::insert(user)) { m_statusLabel->setText("注册失败，请稍后重试"); return; }

    User newUser = UserDao::selectByUsername(username);
    if (newUser.id == 0) { m_statusLabel->setText("注册失败，无法获取用户ID"); return; }

    QString enrollmentYear = QString::number(QDate::currentDate().year());
    QString majorCode = m_majorCombo->currentData().toString();
    QString classCode = m_classCombo->currentData().toString();

    QList<Student> allStudents = StudentDao::selectAll();
    int count = 0;
    for (const Student &s : allStudents) {
        if (s.studentId.startsWith(enrollmentYear + majorCode + classCode)) {
            count++;
        }
    }
    QString seqCode = QString("%1").arg(count + 1, 2, 10, QChar('0'));
    QString studentId = enrollmentYear + majorCode + classCode + seqCode;

    Student student;
    student.studentId = studentId;
    student.name = name;
    student.major = major;
    student.className = className;
    student.gender = gender;
    student.birthDate = birthDate;
    student.userId = newUser.id;

    int newStuId = StudentDao::insert(student);
    if (newStuId <= 0) { m_statusLabel->setText("注册失败，请联系管理员"); return; }

    QMessageBox::information(this, "注册成功", QString("用户 %1 注册成功！\n学号：%2\n请使用该账号登录。").arg(username).arg(studentId));
    accept();
}

void RegisterDialog::onCancelClicked()
{
    reject();
}

// ============================================================
// MainWidget
// ============================================================
MainWidget::MainWidget(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("学生管理系统");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumSize(1200, 600);
    resize(1500, 700);

    // 检测服务器关闭，弹出确认框后退出程序
    connect(&NetworkManager::instance(), &NetworkManager::serverShutdown, this, [this](const QString &msg) {
        QMessageBox::StandardButton btn = QMessageBox::information(this, "服务器已关闭", msg, QMessageBox::Ok);
        if (btn == QMessageBox::Ok) {
            QApplication::quit();
        }
    });
}

MainWidget::~MainWidget() {}

void MainWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier) {
        if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) { onZoomIn(); return; }
        else if (event->key() == Qt::Key_Minus) { onZoomOut(); return; }
        else if (event->key() == Qt::Key_0) { onZoomReset(); return; }
    }
    QWidget::keyPressEvent(event);
}

void MainWidget::onZoomIn()
{
    double current = StyleHelper::instance().scaleFactor();
    applyZoom(current + 0.1);
}

void MainWidget::onZoomOut()
{
    double current = StyleHelper::instance().scaleFactor();
    applyZoom(current - 0.1);
}

void MainWidget::onZoomReset()
{
    applyZoom(1.0);
}

void MainWidget::applyZoom(double factor)
{
    StyleHelper::instance().setScaleFactor(factor);
    StyleHelper::instance().applyGlobalStyle();
    adjustSize();
}

void MainWidget::setCurrentUser(int userId, const QString &username, const QString &role)
{
    // 保存当前窗口尺寸，避免重建布局时窗口缩小
    QSize savedSize = isVisible() ? size() : QSize(1500, 700);

    m_currentUserId = userId;
    m_currentUsername = username;
    m_currentRole = role;

    setWindowTitle(QString("学生管理系统 - 当前用户: %1 (%2)")
                   .arg(username)
                   .arg(role == "admin" ? "管理员" : "学生"));

    if (role == "admin") { createAdminUI(); }
    else { createStudentUI(); }

    // 恢复窗口尺寸
    resize(savedSize);
}

void MainWidget::createAdminUI()
{
    clearLayout(layout());

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(24, 20, 24, 20);

    QHBoxLayout *topRow = new QHBoxLayout;
    topRow->setSpacing(12);

    QWidget *spacer1 = new QWidget(this);
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    topRow->addWidget(spacer1);

    QLabel *welcomeLabel = new QLabel(QString("👤 管理员: %1").arg(m_currentUsername), this);
    welcomeLabel->setStyleSheet("font-weight: 600; color: #1E293B; font-size: 18px;");

    QWidget *spacer2 = new QWidget(this);
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    topRow->addWidget(spacer2);

    QPushButton *btnZoomIn = new QPushButton("🔍 放大", this);
    btnZoomIn->setObjectName("secondary");
    btnZoomIn->setToolTip("Ctrl + +");

    QPushButton *btnZoomOut = new QPushButton("🔍 缩小", this);
    btnZoomOut->setObjectName("secondary");
    btnZoomOut->setToolTip("Ctrl + -");

    QPushButton *btnZoomReset = new QPushButton("重置", this);
    btnZoomReset->setObjectName("secondary");
    btnZoomReset->setToolTip("Ctrl + 0");

    topRow->addWidget(welcomeLabel);
    topRow->addWidget(btnZoomIn);
    topRow->addWidget(btnZoomOut);
    topRow->addWidget(btnZoomReset);
    mainLayout->addLayout(topRow);

    QTableWidget *table = new QTableWidget(this);
    table->setColumnCount(8);
    table->setHorizontalHeaderLabels({"ID", "学号", "姓名", "专业", "班级", "性别", "出生日期", "用户名"});
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    table->setColumnWidth(0, 50);
    table->setColumnWidth(1, 130);
    table->setColumnWidth(2, 120);
    table->setColumnWidth(3, 120);
    table->setColumnWidth(4, 100);
    table->setColumnWidth(5, 50);
    table->setColumnWidth(6, 110);
    table->setColumnWidth(7, 100);
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setVisible(false);
    table->setSelectionBehavior(QTableWidget::SelectRows);
    table->setAlternatingRowColors(true);
    table->setSelectionMode(QAbstractItemView::MultiSelection);
    table->setShowGrid(true);
    table->setSortingEnabled(false);
    mainLayout->addWidget(table, 1);

    QWidget *btnWidget = new QWidget(this);
    QVBoxLayout *btnVLayout = new QVBoxLayout(btnWidget);
    btnVLayout->setSpacing(10);
    btnVLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *btnRow1 = new QHBoxLayout;
    btnRow1->setSpacing(10);

    QPushButton *btnAdd = new QPushButton("➕ 新增学生", this);
    btnAdd->setObjectName("primary");
    QGraphicsDropShadowEffect *shadowAdd = new QGraphicsDropShadowEffect(this);
    shadowAdd->setBlurRadius(8);
    shadowAdd->setColor(QColor(59, 130, 246, 40));
    shadowAdd->setOffset(0, 2);
    btnAdd->setGraphicsEffect(shadowAdd);

    QPushButton *btnDelete = new QPushButton("🗑 删除选中", this);
    btnDelete->setObjectName("danger");

    QPushButton *btnRefresh = new QPushButton("🔄 刷新列表", this);
    btnRefresh->setObjectName("secondary");

    QPushButton *btnSave = new QPushButton("💾 保存修改", this);
    btnSave->setObjectName("primary");

    QPushButton *btnExport = new QPushButton("📤 导出CSV", this);
    btnExport->setObjectName("secondary");

    QPushButton *btnImport = new QPushButton("📥 导入CSV", this);
    btnImport->setObjectName("secondary");

    btnRow1->addWidget(btnAdd, 1);
    btnRow1->addWidget(btnDelete, 1);
    btnRow1->addWidget(btnRefresh, 1);
    btnRow1->addWidget(btnSave, 1);
    btnRow1->addWidget(btnExport, 1);
    btnRow1->addWidget(btnImport, 1);
    btnVLayout->addLayout(btnRow1);

    QHBoxLayout *btnRow2 = new QHBoxLayout;
    btnRow2->setSpacing(10);
    btnRow2->setContentsMargins(0, 0, 0, 0);

    QPushButton *btnStats = new QPushButton("📊 班级统计", this);
    btnStats->setObjectName("secondary");

    QPushButton *btnViewScore = new QPushButton("📋 查看成绩", this);
    btnViewScore->setObjectName("secondary");

    QPushButton *btnAddScore = new QPushButton("✏️ 登记成绩", this);
    btnAddScore->setObjectName("primary");

    QPushButton *btnChangePwd = new QPushButton("🔑 修改密码", this);
    btnChangePwd->setObjectName("secondary");

    QPushButton *btnChangeUsername = new QPushButton("✏️ 修改用户名", this);
    btnChangeUsername->setObjectName("secondary");

    QPushButton *btnResetPwd = new QPushButton("🔄 重置密码", this);
    btnResetPwd->setObjectName("primary");

    btnRow2->addWidget(btnStats, 1);
    btnRow2->addWidget(btnViewScore, 1);
    btnRow2->addWidget(btnAddScore, 1);
    btnRow2->addWidget(btnChangePwd, 1);
    btnRow2->addWidget(btnChangeUsername, 1);
    btnRow2->addWidget(btnResetPwd, 1);
    btnVLayout->addLayout(btnRow2);

    mainLayout->addWidget(btnWidget);

    setLayout(mainLayout);

    // ========== 信号槽 ==========
    connect(btnZoomIn, &QPushButton::clicked, this, &MainWidget::onZoomIn);
    connect(btnZoomOut, &QPushButton::clicked, this, &MainWidget::onZoomOut);
    connect(btnZoomReset, &QPushButton::clicked, this, &MainWidget::onZoomReset);

    connect(btnRefresh, &QPushButton::clicked, this, [table]() {
        QList<Student> list = StudentDao::selectAll();
        table->setRowCount(list.size());
        // 缓存已查询过的用户名，避免重复网络请求
        QMap<int, QString> usernameCache;
        for (int i = 0; i < list.size(); ++i) {
            const Student &stu = list[i];
            QTableWidgetItem *idItem = new QTableWidgetItem(QString::number(stu.id));
            idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
            table->setItem(i, 0, idItem);

            QTableWidgetItem *studentIdItem = new QTableWidgetItem(stu.studentId);
            studentIdItem->setFlags(studentIdItem->flags() & ~Qt::ItemIsEditable);
            table->setItem(i, 1, studentIdItem);

            table->setItem(i, 2, new QTableWidgetItem(stu.name));
            table->setItem(i, 3, new QTableWidgetItem(stu.major));
            table->setItem(i, 4, new QTableWidgetItem(stu.className));
            table->setItem(i, 5, new QTableWidgetItem(stu.gender));
            table->setItem(i, 6, new QTableWidgetItem(stu.birthDate.toString("yyyy/MM/dd")));

            QString username;
            if (stu.userId > 0) {
                if (usernameCache.contains(stu.userId)) {
                    username = usernameCache[stu.userId];
                } else {
                    User u = UserDao::selectById(stu.userId);
                    username = u.username;
                    usernameCache[stu.userId] = username;
                }
            }
            QTableWidgetItem *usernameItem = new QTableWidgetItem(username);
            usernameItem->setFlags(usernameItem->flags() & ~Qt::ItemIsEditable);
            usernameItem->setData(Qt::UserRole, stu.userId);
            table->setItem(i, 7, usernameItem);
        }
        qDebug() << "刷新列表，共" << list.size() << "条";
    });

    connect(btnSave, &QPushButton::clicked, this, [table, btnRefresh]() {
        int rowCount = table->rowCount();
        if (rowCount == 0) { QMessageBox::information(nullptr, "提示", "表格为空，无需保存"); return; }

        int successCount = 0, failCount = 0;
        QString errorMsg;

        for (int i = 0; i < rowCount; ++i) {
            int id = table->item(i, 0)->text().toInt();
            QString studentId = table->item(i, 1)->text().trimmed();
            QString name = table->item(i, 2)->text().trimmed();
            QString major = table->item(i, 3)->text().trimmed();
            QString className = table->item(i, 4)->text().trimmed();
            QString gender = table->item(i, 5)->text().trimmed();
            QString birthStr = table->item(i, 6)->text().trimmed();

            if (name.isEmpty()) { errorMsg += QString("第 %1 行姓名为空，跳过\n").arg(i + 1); failCount++; continue; }

            QDate birthDate;
            if (!birthStr.isEmpty()) {
                birthDate = QDate::fromString(birthStr, "yyyy-MM-dd");
                if (!birthDate.isValid()) {
                    birthDate = QDate::fromString(birthStr, "yyyy/MM/dd");
                }
            }
            if (!birthDate.isValid() && !birthStr.isEmpty()) {
                errorMsg += QString("第 %1 行日期格式错误，跳过\n").arg(i + 1); failCount++; continue;
            }

            // 检查学号是否被其他学生占用
            if (!studentId.isEmpty()) {
                Student existing = StudentDao::selectById(id);
                if (existing.studentId != studentId) {
                    // 学号被修改了，检查新学号是否已被其他学生使用
                    QList<Student> allStu = StudentDao::selectAll();
                    bool conflict = false;
                    for (const Student &stu : allStu) {
                        if (stu.id != id && stu.studentId == studentId) {
                            conflict = true;
                            break;
                        }
                    }
                    if (conflict) {
                        errorMsg += QString("第 %1 行学号\"%2\"已被其他学生使用，跳过\n").arg(i + 1).arg(studentId);
                        failCount++; continue;
                    }
                }
            }

            Student s;
            s.id = id; s.studentId = studentId; s.name = name; s.major = major;
            s.className = className; s.gender = gender; s.birthDate = birthDate;
            s.userId = table->item(i, 7)->data(Qt::UserRole).toInt();

            if (StudentDao::update(s)) { successCount++; }
            else { errorMsg += QString("第 %1 行更新失败（ID: %2）\n").arg(i + 1).arg(id); failCount++; }
        }

        QString msg = QString("保存完成：成功 %1 条，失败 %2 条").arg(successCount).arg(failCount);
        if (!errorMsg.isEmpty()) { msg += "\n\n详情：\n" + errorMsg; }
        QMessageBox::information(nullptr, "保存结果", msg);
        btnRefresh->click();
    });

    connect(btnAdd, &QPushButton::clicked, this, [btnRefresh, table]() {
        // 完整的新增学生对话框（已包含眼睛按钮）
        QDialog dialog;
        dialog.setWindowTitle("新增学生（含登录账号）");
        dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
        dialog.setMinimumSize(550, 500);

        QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
        mainLayout->setSpacing(12);
        mainLayout->setContentsMargins(24, 24, 24, 24);

        QFormLayout *formLayout = new QFormLayout;
        formLayout->setSpacing(10);

        QLineEdit *userEdit = new QLineEdit(&dialog);
        userEdit->setPlaceholderText("请输入用户名");

        QWidget *passWidget = new QWidget(&dialog);
        QHBoxLayout *passLayout = new QHBoxLayout(passWidget);
        passLayout->setContentsMargins(0, 0, 0, 0);
        passLayout->setSpacing(8);
        QLineEdit *passEdit = new QLineEdit(&dialog);
        passEdit->setEchoMode(QLineEdit::Password);
        passEdit->setPlaceholderText("请输入密码");
        QPushButton *showPassBtn = new QPushButton(&dialog);
        showPassBtn->setObjectName("showPassword");
        showPassBtn->setFixedSize(28, 28);
        showPassBtn->setStyleSheet(R"(
            QPushButton#showPassword {
                border: none;
                background-color: transparent;
                font-size: 18px;
                padding: 0;
                color: #64748B;
                font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
            }
            QPushButton#showPassword:hover {
                background-color: #E2E8F0;
                border-radius: 4px;
                color: #3B82F6;
            }
        )");
        showPassBtn->setText("👁");
        showPassBtn->setProperty("isShowing", false);
        connect(showPassBtn, &QPushButton::clicked, [passEdit, showPassBtn]() {
            bool isShowing = showPassBtn->property("isShowing").toBool();
            isShowing = !isShowing;
            showPassBtn->setProperty("isShowing", isShowing);
            passEdit->setEchoMode(isShowing ? QLineEdit::Normal : QLineEdit::Password);
            showPassBtn->setText(isShowing ? "👁‍🗨" : "👁");
            showPassBtn->setStyleSheet(isShowing ? R"(
                QPushButton#showPassword {
                    border: none;
                    background-color: transparent;
                    font-size: 18px;
                    padding: 0;
                    color: #3B82F6;
                    font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
                }
                QPushButton#showPassword:hover {
                    background-color: #E2E8F0;
                    border-radius: 4px;
                    color: #2563EB;
                }
            )" : R"(
                QPushButton#showPassword {
                    border: none;
                    background-color: transparent;
                    font-size: 18px;
                    padding: 0;
                    color: #64748B;
                    font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
                }
                QPushButton#showPassword:hover {
                    background-color: #E2E8F0;
                    border-radius: 4px;
                    color: #3B82F6;
                }
            )");
        });
        passLayout->addWidget(passEdit);
        passLayout->addWidget(showPassBtn);

        QWidget *confirmWidget = new QWidget(&dialog);
        QHBoxLayout *confirmLayout = new QHBoxLayout(confirmWidget);
        confirmLayout->setContentsMargins(0, 0, 0, 0);
        confirmLayout->setSpacing(8);
        QLineEdit *confirmEdit = new QLineEdit(&dialog);
        confirmEdit->setEchoMode(QLineEdit::Password);
        confirmEdit->setPlaceholderText("请确认密码");
        QPushButton *showConfirmBtn = new QPushButton(&dialog);
        showConfirmBtn->setObjectName("showPassword");
        showConfirmBtn->setFixedSize(28, 28);
        showConfirmBtn->setStyleSheet(R"(
            QPushButton#showPassword {
                border: none;
                background-color: transparent;
                font-size: 18px;
                padding: 0;
                color: #64748B;
                font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
            }
            QPushButton#showPassword:hover {
                background-color: #E2E8F0;
                border-radius: 4px;
                color: #3B82F6;
            }
        )");
        showConfirmBtn->setText("👁");
        showConfirmBtn->setProperty("isShowing", false);
        connect(showConfirmBtn, &QPushButton::clicked, [confirmEdit, showConfirmBtn]() {
            bool isShowing = showConfirmBtn->property("isShowing").toBool();
            isShowing = !isShowing;
            showConfirmBtn->setProperty("isShowing", isShowing);
            confirmEdit->setEchoMode(isShowing ? QLineEdit::Normal : QLineEdit::Password);
            showConfirmBtn->setText(isShowing ? "👁‍🗨" : "👁");
            showConfirmBtn->setStyleSheet(isShowing ? R"(
                QPushButton#showPassword {
                    border: none;
                    background-color: transparent;
                    font-size: 18px;
                    padding: 0;
                    color: #3B82F6;
                    font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
                }
                QPushButton#showPassword:hover {
                    background-color: #E2E8F0;
                    border-radius: 4px;
                    color: #2563EB;
                }
            )" : R"(
                QPushButton#showPassword {
                    border: none;
                    background-color: transparent;
                    font-size: 18px;
                    padding: 0;
                    color: #64748B;
                    font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
                }
                QPushButton#showPassword:hover {
                    background-color: #E2E8F0;
                    border-radius: 4px;
                    color: #3B82F6;
                }
            )");
        });
        confirmLayout->addWidget(confirmEdit);
        confirmLayout->addWidget(showConfirmBtn);

        QLineEdit *nameEdit = new QLineEdit(&dialog);
        nameEdit->setPlaceholderText("请输入真实姓名");

        QComboBox *majorCombo = new QComboBox(&dialog);
        majorCombo->addItem("计算机科学与技术", "01");
        majorCombo->addItem("软件工程", "02");
        majorCombo->addItem("人工智能", "03");
        majorCombo->addItem("数据科学", "04");
        majorCombo->addItem("信息管理", "05");

        QComboBox *classCombo = new QComboBox(&dialog);
        classCombo->addItem("1班", "01");
        classCombo->addItem("2班", "02");
        classCombo->addItem("3班", "03");
        classCombo->addItem("4班", "04");

        QComboBox *genderCombo = new QComboBox(&dialog);
        genderCombo->addItems({"男", "女"});

        QDateEdit *birthEdit = new QDateEdit(&dialog);
        birthEdit->setDate(QDate(2000, 1, 1));
        birthEdit->setCalendarPopup(true);
        birthEdit->setDisplayFormat("yyyy/MM/dd");

        QLineEdit *studentIdEdit = new QLineEdit(&dialog);
        studentIdEdit->setReadOnly(true);
        studentIdEdit->setStyleSheet("background-color: #F1F5F9; color: #64748B;");
        studentIdEdit->setPlaceholderText("学号将自动生成");

        formLayout->addRow("用户名:", userEdit);
        formLayout->addRow("密码:", passWidget);
        formLayout->addRow("确认密码:", confirmWidget);
        formLayout->addRow("真实姓名:", nameEdit);
        formLayout->addRow("专业:", majorCombo);
        formLayout->addRow("班级:", classCombo);
        formLayout->addRow("性别:", genderCombo);
        formLayout->addRow("出生日期:", birthEdit);
        formLayout->addRow("学号:", studentIdEdit);

        mainLayout->addLayout(formLayout);

        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
        mainLayout->addWidget(buttonBox);

        QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        auto updateStudentId = [&]() {
            QString enrollmentYear = QString::number(QDate::currentDate().year());
            QString majorCode = majorCombo->currentData().toString();
            QString classCode = classCombo->currentData().toString();

            QList<Student> allStudents = StudentDao::selectAll();
            int count = 0;
            for (const Student &s : allStudents) {
                if (s.studentId.startsWith(enrollmentYear + majorCode + classCode)) {
                    count++;
                }
            }
            QString seqCode = QString("%1").arg(count + 1, 2, 10, QChar('0'));
            studentIdEdit->setText(enrollmentYear + majorCode + classCode + seqCode);
        };

        connect(majorCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), updateStudentId);
        connect(classCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), updateStudentId);

        updateStudentId();

        if (dialog.exec() != QDialog::Accepted) return;

        QString username = userEdit->text().trimmed();
        QString password = passEdit->text();
        QString confirm = confirmEdit->text();
        QString name = nameEdit->text().trimmed();
        QString major = majorCombo->currentText();
        QString className = classCombo->currentText();
        QString gender = genderCombo->currentText();
        QDate birthDate = birthEdit->date();
        QString studentId = studentIdEdit->text();

        if (username.isEmpty() || username.length() < 3) { QMessageBox::warning(nullptr, "提示", "用户名至少3个字符"); return; }
        if (password.isEmpty() || password.length() < 6) { QMessageBox::warning(nullptr, "提示", "密码至少6个字符"); return; }
        if (password != confirm) { QMessageBox::warning(nullptr, "提示", "两次密码输入不一致"); return; }
        if (name.isEmpty()) { QMessageBox::warning(nullptr, "提示", "请输入真实姓名"); return; }

        User existing = UserDao::selectByUsername(username);
        if (existing.id != 0) { QMessageBox::warning(nullptr, "提示", "用户名已被注册"); return; }

        QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5);
        User newUser;
        newUser.username = username;
        newUser.passwordHash = hash.toHex();
        newUser.role = "student";
        if (!UserDao::insert(newUser)) { QMessageBox::warning(nullptr, "失败", "创建用户失败"); return; }

        User inserted = UserDao::selectByUsername(username);
        if (inserted.id == 0) { QMessageBox::warning(nullptr, "失败", "无法获取用户ID"); return; }

        Student s;
        s.studentId = studentId;
        s.name = name;
        s.major = major;
        s.className = className;
        s.gender = gender;
        s.birthDate = birthDate;
        s.userId = inserted.id;

        if (StudentDao::insert(s) > 0) {
            QMessageBox::information(nullptr, "成功", QString("新增学生成功！\n学号：%1\n账号：%2\n密码：%3").arg(studentId).arg(username).arg(password));
            btnRefresh->click();
        } else {
            if (UserDao::deleteById(inserted.id)) {
                QMessageBox::warning(nullptr, "失败", "新增学生失败，已自动清理创建的账号");
            } else {
                QMessageBox::warning(nullptr, "失败", "新增学生失败，且清理账号失败（请手动清理）");
            }
        }
    });

    connect(btnDelete, &QPushButton::clicked, this, [btnRefresh, table]() {
        QList<int> selectedRows;
        for (auto index : table->selectionModel()->selectedRows()) {
            selectedRows.append(index.row());
        }
        if (selectedRows.isEmpty()) {
            QMessageBox::warning(nullptr, "提示", "请先选中要删除的学生（可多选）");
            return;
        }
        if (QMessageBox::question(nullptr, "确认",
                                  QString("确定要删除选中的 %1 名学生吗？").arg(selectedRows.size()))
            != QMessageBox::Yes) {
            return;
        }
        int success = 0;
        for (int row : selectedRows) {
            int id = table->item(row, 0)->text().toInt();
            // 先删除关联成绩，再删除学生
            QList<Score> scores = ScoreDao::selectByStudentId(id);
            for (const Score &s : scores) {
                ScoreDao::deleteById(s.id);
            }
            if (StudentDao::deleteById(id)) success++;
        }
        QMessageBox::information(nullptr, "完成",
                                 QString("成功删除 %1 条，失败 %2 条")
                                 .arg(success).arg(selectedRows.size() - success));
        btnRefresh->click();
    });

    connect(btnExport, &QPushButton::clicked, this, []() {
        QString defaultName = FileExporter::generateDefaultFileName();
        QString path = QFileDialog::getSaveFileName(nullptr, "导出CSV", defaultName, "CSV文件 (*.csv)");
        if (path.isEmpty()) return;
        if (FileExporter::exportAllToCsv(path)) {
            QMessageBox::information(nullptr, "成功", "导出成功:\n" + path);
        }
    });

    connect(btnImport, &QPushButton::clicked, this, [btnRefresh]() {
        QString path = QFileDialog::getOpenFileName(nullptr, "选择CSV文件", QDir::homePath(), "*.csv");
        if (path.isEmpty()) return;

        ImportResult result = FileExporter::importStudentsFromCsv(path);

        QString msg = QString("导入完成：成功处理 %1 条学生数据").arg(result.studentCount);
        if (result.scoreCount > 0) {
            msg += QString("，导入 %1 条成绩").arg(result.scoreCount);
        }
        if (result.userCreated > 0) {
            msg += QString("\n\n自动创建了 %1 个账号，默认密码均为: 123456").arg(result.userCreated);
            int showCount = qMin(result.createdAccounts.size(), 20);
            for (int i = 0; i < showCount; ++i) {
                msg += "\n" + result.createdAccounts[i];
            }
            if (result.createdAccounts.size() > 20) {
                msg += QString("\n... 共 %1 条，详情请查看控制台输出").arg(result.createdAccounts.size());
            }
            msg += "\n请提醒学生使用默认密码登录后尽快修改密码。";
        }

        if (!result.failedRows.isEmpty()) {
            msg += QString("\n\n❌ 失败 %1 条：\n").arg(result.failedRows.size());
            for (const QString &err : result.failedRows) {
                msg += "  " + err + "\n";
            }
        }

        QDialog detailDialog;
        detailDialog.setWindowTitle("导入结果");
        detailDialog.setWindowFlags(detailDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
        detailDialog.setMinimumSize(700, 500);
        detailDialog.resize(800, 600);
        QVBoxLayout *dlgLayout = new QVBoxLayout(&detailDialog);
        dlgLayout->setSpacing(16);
        dlgLayout->setContentsMargins(24, 24, 24, 24);

        QLabel *dlgTitle = new QLabel("📋 导入结果", &detailDialog);
        dlgTitle->setStyleSheet("font-size: 20px; font-weight: 600; color: #1E293B;");
        dlgTitle->setAlignment(Qt::AlignCenter);
        dlgLayout->addWidget(dlgTitle);

        QTextEdit *textEdit = new QTextEdit(&detailDialog);
        textEdit->setPlainText(msg);
        textEdit->setReadOnly(true);
        textEdit->setStyleSheet(R"(
            QTextEdit {
                font-size: 14px;
                color: #334155;
                background-color: #F8FAFC;
                border: 1px solid #E2E8F0;
                border-radius: 8px;
                padding: 16px;
            }
        )");
        dlgLayout->addWidget(textEdit, 1);
        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, &detailDialog);
        QPushButton *okBtn = buttonBox->button(QDialogButtonBox::Ok);
        if (okBtn) {
            okBtn->setText("确定");
            okBtn->setMinimumWidth(120);
        }
        dlgLayout->addWidget(buttonBox);
        QObject::connect(buttonBox, &QDialogButtonBox::accepted, &detailDialog, &QDialog::accept);
        detailDialog.exec();

        btnRefresh->click();
    });

    // ========== 班级统计（增加筛选功能） ==========
    connect(btnStats, &QPushButton::clicked, this, []() {
        QDialog dialog;
        dialog.setWindowTitle("统计查询");
        dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
        dialog.setMinimumSize(800, 600);
        dialog.resize(900, 650);

        QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
        mainLayout->setSpacing(24);
        mainLayout->setContentsMargins(32, 32, 32, 32);

        QLabel *titleLabel = new QLabel("统计查询", &dialog);
        titleLabel->setStyleSheet("font-size: 18px; font-weight: 600; color: #1E293B;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);

        QWidget *btnWidget = new QWidget(&dialog);
        QGridLayout *btnLayout = new QGridLayout(btnWidget);
        btnLayout->setSpacing(16);

        QPushButton *btnClassAvg = new QPushButton("班级科目平均分查询", &dialog);
        btnClassAvg->setObjectName("primary");
        btnClassAvg->setMinimumHeight(50);
        btnLayout->addWidget(btnClassAvg, 0, 0);

        QPushButton *btnGradeCompare = new QPushButton("年级班级对比分析", &dialog);
        btnGradeCompare->setObjectName("primary");
        btnGradeCompare->setMinimumHeight(50);
        btnLayout->addWidget(btnGradeCompare, 0, 1);

        QPushButton *btnSubjectDetail = new QPushButton("科目详情查询", &dialog);
        btnSubjectDetail->setObjectName("primary");
        btnSubjectDetail->setMinimumHeight(50);
        btnLayout->addWidget(btnSubjectDetail, 1, 0);

        QPushButton *btnClassRank = new QPushButton("班级成绩排名", &dialog);
        btnClassRank->setObjectName("primary");
        btnClassRank->setMinimumHeight(50);
        btnLayout->addWidget(btnClassRank, 1, 1);

        mainLayout->addWidget(btnWidget);

        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal, &dialog);
        mainLayout->addWidget(buttonBox);
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        QMap<QString, QMap<QString, QStringList>> gradeMajorClasses = BusinessLogic::getGradeMajorClasses();
        QStringList allSubjects = BusinessLogic::getAllSubjects();

        QStringList gradeList = gradeMajorClasses.keys();
        std::sort(gradeList.begin(), gradeList.end(), [](const QString &a, const QString &b) {
            QStringList order = {"大一", "大二", "大三", "大四", "研究生"};
            return order.indexOf(a) < order.indexOf(b);
        });

        QObject::connect(btnClassAvg, &QPushButton::clicked, [&]() {
            QDialog subDialog;
            subDialog.setWindowFlags(subDialog.windowFlags() | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);
            subDialog.setWindowTitle("班级科目平均分查询");
            subDialog.setWindowFlags(subDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
            subDialog.setMinimumSize(800, 500);
            subDialog.resize(900, 600);

            QVBoxLayout *subLayout = new QVBoxLayout(&subDialog);
            subLayout->setSpacing(16);
            subLayout->setContentsMargins(24, 24, 24, 24);

            QWidget *filterWidget = new QWidget(&subDialog);
            QHBoxLayout *filterLayout = new QHBoxLayout(filterWidget);
            filterLayout->setSpacing(16);

            QComboBox *gradeCombo = new QComboBox(&subDialog);
            gradeCombo->addItem("全部");
            for (const QString &g : gradeList) gradeCombo->addItem(g);
            filterLayout->addWidget(new QLabel("年级:"));
            filterLayout->addWidget(gradeCombo);

            QComboBox *majorCombo = new QComboBox(&subDialog);
            majorCombo->addItem("全部");
            filterLayout->addWidget(new QLabel("专业:"));
            filterLayout->addWidget(majorCombo);

            QComboBox *classCombo = new QComboBox(&subDialog);
            classCombo->addItem("全部");
            filterLayout->addWidget(new QLabel("班级:"));
            filterLayout->addWidget(classCombo);

            QComboBox *subjectCombo = new QComboBox(&subDialog);
            //subjectCombo->addItem("全部");
            for (const QString &s : allSubjects) subjectCombo->addItem(s);
            filterLayout->addWidget(new QLabel("科目:"));
            filterLayout->addWidget(subjectCombo);

            QPushButton *queryBtn = new QPushButton("查询", &subDialog);
            queryBtn->setObjectName("primary");
            filterLayout->addWidget(queryBtn);

            subLayout->addWidget(filterWidget, 0);   // 0 表示不拉伸，保持固定高度

            QTableWidget *table = new QTableWidget(&subDialog);
            table->setStyleSheet(R"(
                QTableWidget { font-size: 14px; gridline-color: #E2E8F0; border: 1px solid #E2E8F0; border-radius: 8px; }
                QTableWidget::item { padding: 8px 12px; }
                QHeaderView::section { background-color: #F1F5F9; padding: 10px 12px; font-weight: 600; color: #475569; font-size: 14px; }
            )");
            table->verticalHeader()->setVisible(false);
            table->setAlternatingRowColors(true);
            subLayout->addWidget(table, 1);          // 1 表示拉伸，占用剩余空间

            QObject::connect(gradeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [&]() {
                majorCombo->clear();
                //majorCombo->addItem("全部");
                QString grade = gradeCombo->currentText();
                if (grade != "全部" && gradeMajorClasses.contains(grade)) {
                    for (const QString &m : gradeMajorClasses[grade].keys()) majorCombo->addItem(m);
                } else {
                    for (const QString &g : gradeList) {
                        for (const QString &m : gradeMajorClasses[g].keys()) {
                            if (majorCombo->findText(m) == -1) majorCombo->addItem(m);
                        }
                    }
                }
                classCombo->clear();
                classCombo->addItem("全部");
            });

            QObject::connect(majorCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [&]() {
                classCombo->clear();
                classCombo->addItem("全部");
                QString grade = gradeCombo->currentText();
                QString major = majorCombo->currentText();
                if (major != "全部") {
                    if (grade != "全部" && gradeMajorClasses.contains(grade) && gradeMajorClasses[grade].contains(major)) {
                        for (const QString &c : gradeMajorClasses[grade][major]) classCombo->addItem(c);
                    } else {
                        for (const QString &g : gradeList) {
                            if (gradeMajorClasses[g].contains(major)) {
                                for (const QString &c : gradeMajorClasses[g][major]) {
                                    if (classCombo->findText(c) == -1) classCombo->addItem(c);
                                }
                            }
                        }
                    }
                }
            });

            QObject::connect(queryBtn, &QPushButton::clicked, [&]() {
                QString selectedMajor = majorCombo->currentText();
                if (selectedMajor.isEmpty()) {
                    QMessageBox::warning(&subDialog, "提示", "请选择专业");
                    return;
                }
                table->clearContents();
                table->setRowCount(0);
                table->setColumnCount(0);

                QString selectedGrade = gradeCombo->currentText();
                //QString selectedMajor = majorCombo->currentText();
                QString selectedClass = classCombo->currentText();
                QString selectedSubject = subjectCombo->currentText();

                QStringList headers = {"年级", "专业", "班级"};
                QStringList displaySubjects = (selectedSubject == "全部") ? allSubjects : QStringList{selectedSubject};
                for (const QString &s : displaySubjects) headers << s;

                table->setColumnCount(headers.size());
                table->setHorizontalHeaderLabels(headers);
                table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
                table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

                int row = 0;
                for (const QString &grade : gradeList) {
                    if (selectedGrade != "全部" && grade != selectedGrade) continue;
                    QMap<QString, QStringList> majorClasses = gradeMajorClasses[grade];
                    for (const QString &major : majorClasses.keys()) {
                        if (selectedMajor != "全部" && major != selectedMajor) continue;
                        QStringList classes = majorClasses[major];
                        std::sort(classes.begin(), classes.end());
                        for (const QString &className : classes) {
                            if (selectedClass != "全部" && className != selectedClass) continue;
                            table->insertRow(row);
                            table->setItem(row, 0, new QTableWidgetItem(grade));
                            table->setItem(row, 1, new QTableWidgetItem(major));
                            table->setItem(row, 2, new QTableWidgetItem(className));
                            int col = 3;
                            for (const QString &subject : displaySubjects) {
                                double avg = BusinessLogic::calcClassSubjectAverage(className, subject);
                                QString avgStr = (avg < 0) ? "-" : QString::number(avg, 'f', 2);
                                QTableWidgetItem *item = new QTableWidgetItem(avgStr);
                                item->setTextAlignment(Qt::AlignCenter);
                                table->setItem(row, col, item);
                                col++;
                            }
                            row++;
                        }
                    }
                }

                // 允许换行，不显示省略号
                table->setWordWrap(true);
                table->setTextElideMode(Qt::ElideNone);

                // 表头内边距，增加可读性
                table->setStyleSheet("QHeaderView::section { padding: 0 15px; }");

                // 所有列均匀拉伸填满表格
                table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                // 确保所有列都参与拉伸（不单独拉伸最后一列）
                table->horizontalHeader()->setStretchLastSection(false);

                // 设置每列的最小宽度（例如 100 像素），防止列太窄导致表头截断
                table->horizontalHeader()->setMinimumSectionSize(100);


                if (row == 0) {
                    table->insertRow(0);
                    table->setColumnCount(1);
                    table->setHorizontalHeaderLabels({"暂无数据"});
                    QTableWidgetItem *item = new QTableWidgetItem("没有找到符合条件的数据");
                    item->setTextAlignment(Qt::AlignCenter);
                    table->setItem(0, 0, item);
                }
            });

            subDialog.exec();
        });

        QObject::connect(btnGradeCompare, &QPushButton::clicked, [&]() {
            QDialog subDialog;
            subDialog.setWindowFlags(subDialog.windowFlags() | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);
            subDialog.setWindowTitle("年级班级对比分析");
            subDialog.setWindowFlags(subDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
            subDialog.setMinimumSize(800, 500);
            subDialog.resize(1000, 650);

            QVBoxLayout *subLayout = new QVBoxLayout(&subDialog);
            subLayout->setSpacing(16);
            subLayout->setContentsMargins(24, 24, 24, 24);

            QWidget *filterWidget = new QWidget(&subDialog);
            QHBoxLayout *filterLayout = new QHBoxLayout(filterWidget);
            filterLayout->setSpacing(16);

            QComboBox *subjectCombo = new QComboBox(&subDialog);
            //subjectCombo->addItem("全部");
            for (const QString &s : allSubjects) subjectCombo->addItem(s);
            filterLayout->addWidget(new QLabel("科目:"));
            filterLayout->addWidget(subjectCombo);

            QPushButton *queryBtn = new QPushButton("查询", &subDialog);
            queryBtn->setObjectName("primary");
            filterLayout->addWidget(queryBtn);

            subLayout->addWidget(filterWidget, 0);   // 0 表示不拉伸，保持固定高度

            QTableWidget *table = new QTableWidget(&subDialog);
            table->setStyleSheet(R"(
                QTableWidget { font-size: 14px; gridline-color: #E2E8F0; border: 1px solid #E2E8F0; border-radius: 8px; }
                QTableWidget::item { padding: 8px 12px; }
                QHeaderView::section { background-color: #F1F5F9; padding: 10px 12px; font-weight: 600; color: #475569; font-size: 14px; }
            )");
            table->verticalHeader()->setVisible(false);
            table->setAlternatingRowColors(true);
            subLayout->addWidget(table, 1);          // 1 表示拉伸，占用剩余空间

            QObject::connect(queryBtn, &QPushButton::clicked, [&]() {
                QString selectedSubject = subjectCombo->currentText();
                    if (selectedSubject.isEmpty()) {
                        QMessageBox::warning(&subDialog, "提示", "请选择科目");
                        return;
                    }
                table->clearContents();
                table->setRowCount(0);
                table->setColumnCount(0);



                QStringList headers = {"年级", "专业", "班级"};
                QStringList displaySubjects = (selectedSubject == "全部") ? allSubjects : QStringList{selectedSubject};
                for (const QString &s : displaySubjects) {
                    headers << s + "-均分";
                    headers << s + "-最高分";
                }

                table->setColumnCount(headers.size());
                table->setHorizontalHeaderLabels(headers);
                table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
                table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
                int row = 0;
                for (const QString &grade : gradeList) {
                    QMap<QString, QStringList> majorClasses = gradeMajorClasses[grade];
                    for (const QString &major : majorClasses.keys()) {
                        QStringList classes = majorClasses[major];
                        std::sort(classes.begin(), classes.end());
                        for (const QString &className : classes) {
                            table->insertRow(row);
                            table->setItem(row, 0, new QTableWidgetItem(grade));
                            table->setItem(row, 1, new QTableWidgetItem(major));
                            table->setItem(row, 2, new QTableWidgetItem(className));
                            int col = 3;
                            for (const QString &subject : displaySubjects) {
                                double avg = BusinessLogic::calcClassSubjectAverage(className, subject);
                                double max = BusinessLogic::calcClassSubjectMax(className, subject);
                                QString avgStr = (avg < 0) ? "-" : QString::number(avg, 'f', 2);
                                QString maxStr = (max < 0) ? "-" : QString::number(max, 'f', 2);
                                QTableWidgetItem *avgItem = new QTableWidgetItem(avgStr);
                                avgItem->setTextAlignment(Qt::AlignCenter);
                                QTableWidgetItem *maxItem = new QTableWidgetItem(maxStr);
                                maxItem->setTextAlignment(Qt::AlignCenter);
                                if (max >= 85) maxItem->setForeground(QColor("#10B981"));
                                else if (max >= 60) maxItem->setForeground(QColor("#3B82F6"));
                                else maxItem->setForeground(QColor("#EF4444"));
                                table->setItem(row, col++, avgItem);
                                table->setItem(row, col++, maxItem);
                            }
                            row++;
                        }
                    }
                }

                // 允许换行，不显示省略号
                table->setWordWrap(true);
                table->setTextElideMode(Qt::ElideNone);

                // 表头内边距，增加可读性
                table->setStyleSheet("QHeaderView::section { padding: 0 15px; }");

                // 所有列均匀拉伸填满表格
                table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                // 确保所有列都参与拉伸（不单独拉伸最后一列）
                table->horizontalHeader()->setStretchLastSection(false);

                // 设置每列的最小宽度（例如 100 像素），防止列太窄导致表头截断
                table->horizontalHeader()->setMinimumSectionSize(100);

                if (row == 0) {
                    table->insertRow(0);
                    table->setColumnCount(1);
                    table->setHorizontalHeaderLabels({"暂无数据"});
                    QTableWidgetItem *item = new QTableWidgetItem("没有找到符合条件的数据");
                    item->setTextAlignment(Qt::AlignCenter);
                    table->setItem(0, 0, item);
                }
            });

            subDialog.exec();
        });

        QObject::connect(btnSubjectDetail, &QPushButton::clicked, [&]() {
            QDialog subDialog;
            subDialog.setWindowFlags(subDialog.windowFlags() | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);
            subDialog.setWindowTitle("科目详情查询");
            subDialog.setWindowFlags(subDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
            subDialog.setMinimumSize(800, 500);
            subDialog.resize(900, 600);

            QVBoxLayout *subLayout = new QVBoxLayout(&subDialog);
            subLayout->setSpacing(16);
            subLayout->setContentsMargins(24, 24, 24, 24);

            QWidget *filterWidget = new QWidget(&subDialog);
            QHBoxLayout *filterLayout = new QHBoxLayout(filterWidget);
            filterLayout->setSpacing(16);

            QComboBox *subjectCombo = new QComboBox(&subDialog);
            for (const QString &s : allSubjects) subjectCombo->addItem(s);
            filterLayout->addWidget(new QLabel("科目:"));
            filterLayout->addWidget(subjectCombo);

            QPushButton *queryBtn = new QPushButton("查询", &subDialog);
            queryBtn->setObjectName("primary");
            filterLayout->addWidget(queryBtn);

            subLayout->addWidget(filterWidget, 0);   // 0 表示不拉伸，保持固定高度

            QTableWidget *table = new QTableWidget(&subDialog);
            table->setStyleSheet(R"(
                QTableWidget { font-size: 14px; gridline-color: #E2E8F0; border: 1px solid #E2E8F0; border-radius: 8px; }
                QTableWidget::item { padding: 8px 12px; }
                QHeaderView::section { background-color: #F1F5F9; padding: 10px 12px; font-weight: 600; color: #475569; font-size: 14px; }
            )");
            table->verticalHeader()->setVisible(false);
            table->setAlternatingRowColors(true);
            subLayout->addWidget(table, 1);          // 1 表示拉伸，占用剩余空间

            QObject::connect(queryBtn, &QPushButton::clicked, [&]() {
                table->clearContents();
                table->setRowCount(0);
                table->setColumnCount(0);

                QString subject = subjectCombo->currentText();
                QList<Student> allStudents = StudentDao::selectAll();

                QMap<QString, QList<double>> classScores;
                for (const Student &student : allStudents) {
                    QList<Score> scores = ScoreDao::selectByStudentId(student.id);
                    for (const Score &s : scores) {
                        if (s.subject == subject) {
                            classScores[student.className].append(s.score);
                        }
                    }
                }

                QStringList headers = {"班级", "最高分", "最低分", "平均分", "人数"};
                table->setColumnCount(headers.size());
                table->setHorizontalHeaderLabels(headers);
                table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
                table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

                for (const QString &className : classScores.keys()) {
                    QList<double> scoresList = classScores[className];
                    double maxScore = *std::max_element(scoresList.begin(), scoresList.end());
                    double minScore = *std::min_element(scoresList.begin(), scoresList.end());
                    double avgScore = std::accumulate(scoresList.begin(), scoresList.end(), 0.0) / scoresList.size();

                    int row = table->rowCount();
                    table->insertRow(row);
                    table->setItem(row, 0, new QTableWidgetItem(className));
                    table->setItem(row, 1, new QTableWidgetItem(QString::number(maxScore, 'f', 2)));
                    table->setItem(row, 2, new QTableWidgetItem(QString::number(minScore, 'f', 2)));
                    table->setItem(row, 3, new QTableWidgetItem(QString::number(avgScore, 'f', 2)));
                    table->setItem(row, 4, new QTableWidgetItem(QString::number(scoresList.size())));
                }
                // 数据填充完成后（row 或 table->rowCount() 已确定）

                // 允许换行，不显示省略号
                table->setWordWrap(true);
                table->setTextElideMode(Qt::ElideNone);

                // 表头内边距，增加可读性
                table->setStyleSheet("QHeaderView::section { padding: 0 15px; }");

                // 所有列均匀拉伸填满表格
                table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                // 确保所有列都参与拉伸（不单独拉伸最后一列）
                table->horizontalHeader()->setStretchLastSection(false);

                // 设置每列的最小宽度（例如 100 像素），防止列太窄导致表头截断
                table->horizontalHeader()->setMinimumSectionSize(100);

                if (table->rowCount() == 0) {
                    table->insertRow(0);
                    table->setColumnCount(1);
                    table->setHorizontalHeaderLabels({"暂无数据"});
                    QTableWidgetItem *item = new QTableWidgetItem("该科目暂无成绩数据");
                    item->setTextAlignment(Qt::AlignCenter);
                    table->setItem(0, 0, item);
                }
            });

            subDialog.exec();
        });

        QObject::connect(btnClassRank, &QPushButton::clicked, [&]() {
            QDialog subDialog;
            subDialog.setWindowFlags(subDialog.windowFlags() | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);
            subDialog.setWindowTitle("班级成绩排名");
            subDialog.setWindowFlags(subDialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
            subDialog.setMinimumSize(800, 500);
            subDialog.resize(900, 600);

            QVBoxLayout *subLayout = new QVBoxLayout(&subDialog);
            subLayout->setSpacing(16);
            subLayout->setContentsMargins(24, 24, 24, 24);

            QWidget *filterWidget = new QWidget(&subDialog);
            QHBoxLayout *filterLayout = new QHBoxLayout(filterWidget);
            filterLayout->setSpacing(16);

            QComboBox *classCombo = new QComboBox(&subDialog);
            classCombo->addItem("全部");
            for (const QString &g : gradeList) {
                for (const QString &m : gradeMajorClasses[g].keys()) {
                    for (const QString &c : gradeMajorClasses[g][m]) {
                        if (classCombo->findText(c) == -1) classCombo->addItem(c);
                    }
                }
            }
            filterLayout->addWidget(new QLabel("班级:"));
            filterLayout->addWidget(classCombo);

            QComboBox *subjectCombo = new QComboBox(&subDialog);
            subjectCombo->addItem("全部");
            for (const QString &s : allSubjects) subjectCombo->addItem(s);
            filterLayout->addWidget(new QLabel("科目:"));
            filterLayout->addWidget(subjectCombo);

            QPushButton *queryBtn = new QPushButton("查询", &subDialog);
            queryBtn->setObjectName("primary");
            filterLayout->addWidget(queryBtn);

            subLayout->addWidget(filterWidget, 0);   // 0 表示不拉伸，保持固定高度

            QTableWidget *table = new QTableWidget(&subDialog);
            table->setStyleSheet(R"(
                QTableWidget { font-size: 14px; gridline-color: #E2E8F0; border: 1px solid #E2E8F0; border-radius: 8px; }
                QTableWidget::item { padding: 8px 12px; }
                QHeaderView::section { background-color: #F1F5F9; padding: 10px 12px; font-weight: 600; color: #475569; font-size: 14px; }
            )");
            table->verticalHeader()->setVisible(false);
            table->setAlternatingRowColors(true);
            subLayout->addWidget(table, 1);          // 1 表示拉伸，占用剩余空间

            QObject::connect(queryBtn, &QPushButton::clicked, [&]() {
                table->clearContents();
                table->setRowCount(0);
                table->setColumnCount(0);

                QString className = classCombo->currentText();
                QString subject = subjectCombo->currentText();
                QList<Student> allStudents = StudentDao::selectAll();

                QList<std::tuple<QString, QString, double>> rankedScores;
                for (const Student &student : allStudents) {
                    if (className != "全部" && student.className != className) continue;
                    QList<Score> scores = ScoreDao::selectByStudentId(student.id);
                    for (const Score &s : scores) {
                        if (subject == "全部" || s.subject == subject) {
                            rankedScores.append(std::make_tuple(student.name, s.subject, s.score));
                        }
                    }
                }

                std::sort(rankedScores.begin(), rankedScores.end(), [](const std::tuple<QString, QString, double> &a, const std::tuple<QString, QString, double> &b) {
                    return std::get<2>(a) > std::get<2>(b);
                });

                QStringList headers = {"排名", "学生姓名", "科目", "成绩"};
                table->setColumnCount(headers.size());
                table->setHorizontalHeaderLabels(headers);
                table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
                table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
                for (int i = 0; i < rankedScores.size(); i++) {
                    table->insertRow(i);
                    table->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
                    table->setItem(i, 1, new QTableWidgetItem(std::get<0>(rankedScores[i])));
                    table->setItem(i, 2, new QTableWidgetItem(std::get<1>(rankedScores[i])));
                    QTableWidgetItem *scoreItem = new QTableWidgetItem(QString::number(std::get<2>(rankedScores[i]), 'f', 2));
                    scoreItem->setTextAlignment(Qt::AlignCenter);
                    table->setItem(i, 3, scoreItem);
                }

                // 允许换行，不显示省略号
                table->setWordWrap(true);
                table->setTextElideMode(Qt::ElideNone);

                // 表头内边距，增加可读性
                table->setStyleSheet("QHeaderView::section { padding: 0 15px; }");

                // 所有列均匀拉伸填满表格
                table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                // 确保所有列都参与拉伸（不单独拉伸最后一列）
                table->horizontalHeader()->setStretchLastSection(false);

                // 设置每列的最小宽度（例如 100 像素），防止列太窄导致表头截断
                table->horizontalHeader()->setMinimumSectionSize(100);

                if (table->rowCount() == 0) {
                    table->insertRow(0);
                    table->setColumnCount(1);
                    table->setHorizontalHeaderLabels({"暂无数据"});
                    QTableWidgetItem *item = new QTableWidgetItem("没有找到符合条件的数据");
                    item->setTextAlignment(Qt::AlignCenter);
                    table->setItem(0, 0, item);
                }
            });

            subDialog.exec();
        });

        dialog.exec();
    });

    connect(btnViewScore, &QPushButton::clicked, this, [table]() {
        int row = table->currentRow();
        if (row < 0) { QMessageBox::warning(nullptr, "提示", "请先选中一名学生"); return; }
        int studentId = table->item(row, 0)->text().toInt();
        QString studentName = table->item(row, 2)->text();

        QList<Score> scores = ScoreDao::selectByStudentId(studentId);
        if (scores.isEmpty()) { QMessageBox::information(nullptr, "成绩", studentName + " 暂无成绩记录"); return; }

        QString msg = QString("学生：%1\n\n成绩列表：\n").arg(studentName);
        double sum = 0;
        for (const Score &s : scores) { msg += QString("  %1: %2\n").arg(s.subject).arg(s.score); sum += s.score; }
        msg += QString("\n平均分：%1").arg(sum / scores.size(), 0, 'f', 2);

        // 使用对话框而非 QMessageBox，避免成绩过多时显示不全
        QDialog dlg;
        dlg.setWindowTitle("成绩详情 - " + studentName);
        dlg.setWindowFlags(dlg.windowFlags() & ~Qt::WindowContextHelpButtonHint);
        dlg.setMinimumSize(400, 300);
        QVBoxLayout *dlgLayout = new QVBoxLayout(&dlg);
        QTextEdit *textEdit = new QTextEdit(&dlg);
        textEdit->setPlainText(msg);
        textEdit->setReadOnly(true);
        dlgLayout->addWidget(textEdit);
        QDialogButtonBox *btnBox = new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, &dlg);
        dlgLayout->addWidget(btnBox);
        QObject::connect(btnBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
        dlg.exec();
    });

    connect(btnAddScore, &QPushButton::clicked, this, [table]() {
        int row = table->currentRow();
        if (row < 0) { QMessageBox::warning(nullptr, "提示", "请先选中一名学生"); return; }
        int studentId = table->item(row, 0)->text().toInt();
        QString studentName = table->item(row, 2)->text();
        QString major = table->item(row, 3)->text();
        QString studentIdStr = table->item(row, 1)->text();

        QString grade = BusinessLogic::getGradeByStudentId(studentIdStr);
        QStringList subjects = BusinessLogic::getSubjectsByMajorAndGrade(major, grade);

        QDialog dialog;
        dialog.setWindowTitle("登记成绩 - " + studentName);
        dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
        dialog.setMinimumSize(480, 280);

        QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
        mainLayout->setSpacing(16);
        mainLayout->setContentsMargins(24, 24, 24, 24);

        QLabel *infoLabel = new QLabel(QString("学生：%1 | 专业：%2 | 年级：%3").arg(studentName).arg(major).arg(grade), &dialog);
        infoLabel->setStyleSheet("font-size: 14px; color: #64748B; font-weight: 500;");
        mainLayout->addWidget(infoLabel);

        QFormLayout *formLayout = new QFormLayout;
        formLayout->setSpacing(12);

        QComboBox *subjectCombo = new QComboBox(&dialog);
        subjectCombo->addItems(subjects);
        subjectCombo->setMinimumWidth(200);
        QDoubleSpinBox *scoreSpin = new QDoubleSpinBox(&dialog);
        scoreSpin->setRange(0, 100);
        scoreSpin->setSingleStep(0.5);
        scoreSpin->setValue(60.0);
        scoreSpin->setMinimumWidth(120);

        formLayout->addRow("科目:", subjectCombo);
        formLayout->addRow("成绩:", scoreSpin);

        mainLayout->addLayout(formLayout);

        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
        mainLayout->addWidget(buttonBox);

        QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            QString subject = subjectCombo->currentText();
            double score = scoreSpin->value();
            Score s;
            s.studentId = studentId; s.subject = subject; s.score = score; s.examDate = QDate::currentDate();
            if (ScoreDao::insert(s)) { QMessageBox::information(nullptr, "成功", "成绩登记成功"); }
            else { QMessageBox::warning(nullptr, "失败", "成绩登记失败"); }
        }
    });

    connect(btnChangePwd, &QPushButton::clicked, this, [table, btnRefresh]() {
        int row = table->currentRow();
        if (row < 0) { QMessageBox::warning(nullptr, "提示", "请先选中一名学生"); return; }
        int userId = table->item(row, 7)->data(Qt::UserRole).toInt();
        if (userId <= 0) { QMessageBox::warning(nullptr, "提示", "该学生未关联账号，无法修改密码"); return; }
        QString studentName = table->item(row, 2)->text();

        QDialog dialog;
        dialog.setWindowTitle("修改密码 - " + studentName);
        dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
        dialog.setMinimumSize(400, 220);

        QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
        mainLayout->setSpacing(12);
        mainLayout->setContentsMargins(24, 24, 24, 24);

        QFormLayout *formLayout = new QFormLayout;
        formLayout->setSpacing(10);

        QWidget *newPassWidget = new QWidget(&dialog);
        QHBoxLayout *newPassLayout = new QHBoxLayout(newPassWidget);
        newPassLayout->setContentsMargins(0, 0, 0, 0);
        newPassLayout->setSpacing(8);
        QLineEdit *newPassEdit = new QLineEdit(&dialog);
        newPassEdit->setEchoMode(QLineEdit::Password);
        newPassEdit->setPlaceholderText("请输入新密码");
        QPushButton *showNewPassBtn = new QPushButton(&dialog);
        showNewPassBtn->setObjectName("showPassword");
        showNewPassBtn->setFixedSize(28, 28);
        showNewPassBtn->setStyleSheet(R"(
            QPushButton#showPassword {
                border: none;
                background-color: transparent;
                font-size: 18px;
                padding: 0;
                color: #64748B;
                font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
            }
            QPushButton#showPassword:hover {
                background-color: #E2E8F0;
                border-radius: 4px;
                color: #3B82F6;
            }
        )");
        showNewPassBtn->setText("👁");
        showNewPassBtn->setProperty("isShowing", false);
        connect(showNewPassBtn, &QPushButton::clicked, [newPassEdit, showNewPassBtn]() {
            bool isShowing = showNewPassBtn->property("isShowing").toBool();
            isShowing = !isShowing;
            showNewPassBtn->setProperty("isShowing", isShowing);
            newPassEdit->setEchoMode(isShowing ? QLineEdit::Normal : QLineEdit::Password);
            showNewPassBtn->setText(isShowing ? "👁‍🗨" : "👁");
            showNewPassBtn->setStyleSheet(isShowing ? R"(
                QPushButton#showPassword {
                    border: none;
                    background-color: transparent;
                    font-size: 18px;
                    padding: 0;
                    color: #3B82F6;
                    font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
                }
                QPushButton#showPassword:hover {
                    background-color: #E2E8F0;
                    border-radius: 4px;
                    color: #2563EB;
                }
            )" : R"(
                QPushButton#showPassword {
                    border: none;
                    background-color: transparent;
                    font-size: 18px;
                    padding: 0;
                    color: #64748B;
                    font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
                }
                QPushButton#showPassword:hover {
                    background-color: #E2E8F0;
                    border-radius: 4px;
                    color: #3B82F6;
                }
            )");
        });
        newPassLayout->addWidget(newPassEdit);
        newPassLayout->addWidget(showNewPassBtn);

        QWidget *confirmWidget = new QWidget(&dialog);
        QHBoxLayout *confirmLayout = new QHBoxLayout(confirmWidget);
        confirmLayout->setContentsMargins(0, 0, 0, 0);
        confirmLayout->setSpacing(8);
        QLineEdit *confirmEdit = new QLineEdit(&dialog);
        confirmEdit->setEchoMode(QLineEdit::Password);
        confirmEdit->setPlaceholderText("请确认新密码");
        QPushButton *showConfirmBtn = new QPushButton(&dialog);
        showConfirmBtn->setObjectName("showPassword");
        showConfirmBtn->setFixedSize(28, 28);
        showConfirmBtn->setStyleSheet(R"(
            QPushButton#showPassword {
                border: none;
                background-color: transparent;
                font-size: 18px;
                padding: 0;
                color: #64748B;
                font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
            }
            QPushButton#showPassword:hover {
                background-color: #E2E8F0;
                border-radius: 4px;
                color: #3B82F6;
            }
        )");
        showConfirmBtn->setText("👁");
        showConfirmBtn->setProperty("isShowing", false);
        connect(showConfirmBtn, &QPushButton::clicked, [confirmEdit, showConfirmBtn]() {
            bool isShowing = showConfirmBtn->property("isShowing").toBool();
            isShowing = !isShowing;
            showConfirmBtn->setProperty("isShowing", isShowing);
            confirmEdit->setEchoMode(isShowing ? QLineEdit::Normal : QLineEdit::Password);
            showConfirmBtn->setText(isShowing ? "👁‍🗨" : "👁");
            showConfirmBtn->setStyleSheet(isShowing ? R"(
                QPushButton#showPassword {
                    border: none;
                    background-color: transparent;
                    font-size: 18px;
                    padding: 0;
                    color: #3B82F6;
                    font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
                }
                QPushButton#showPassword:hover {
                    background-color: #E2E8F0;
                    border-radius: 4px;
                    color: #2563EB;
                }
            )" : R"(
                QPushButton#showPassword {
                    border: none;
                    background-color: transparent;
                    font-size: 18px;
                    padding: 0;
                    color: #64748B;
                    font-family: "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji", sans-serif;
                }
                QPushButton#showPassword:hover {
                    background-color: #E2E8F0;
                    border-radius: 4px;
                    color: #3B82F6;
                }
            )");
        });
        confirmLayout->addWidget(confirmEdit);
        confirmLayout->addWidget(showConfirmBtn);

        formLayout->addRow("新密码:", newPassWidget);
        formLayout->addRow("确认密码:", confirmWidget);

        mainLayout->addLayout(formLayout);

        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
        mainLayout->addWidget(buttonBox);

        QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() != QDialog::Accepted) return;

        QString newPwd = newPassEdit->text();
        QString confirm = confirmEdit->text();

        if (newPwd.isEmpty() || newPwd.length() < 6) { QMessageBox::warning(nullptr, "提示", "密码至少6个字符"); return; }
        if (newPwd != confirm) { QMessageBox::warning(nullptr, "提示", "两次密码输入不一致"); return; }

        QByteArray hash = QCryptographicHash::hash(newPwd.toUtf8(), QCryptographicHash::Md5);
        if (UserDao::updatePassword(userId, hash.toHex())) { QMessageBox::information(nullptr, "成功", "密码修改成功"); btnRefresh->click(); }
        else { QMessageBox::warning(nullptr, "失败", "密码修改失败"); }
    });

    connect(btnChangeUsername, &QPushButton::clicked, this, [table, btnRefresh]() {
        int row = table->currentRow();
        if (row < 0) { QMessageBox::warning(nullptr, "提示", "请先选中一名学生"); return; }
        int userId = table->item(row, 7)->data(Qt::UserRole).toInt();
        if (userId <= 0) { QMessageBox::warning(nullptr, "提示", "该学生未关联账号，无法修改用户名"); return; }
        QString studentName = table->item(row, 2)->text();

        User currentUser = UserDao::selectById(userId);
        if (currentUser.role != "student") { QMessageBox::warning(nullptr, "提示", "该账号不是学生账号，无法修改用户名"); return; }

        QDialog dialog;
        dialog.setWindowTitle("修改用户名 - " + studentName);
        dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);
        dialog.setMinimumSize(400, 180);

        QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
        mainLayout->setSpacing(12);
        mainLayout->setContentsMargins(24, 24, 24, 24);

        QFormLayout *formLayout = new QFormLayout;
        formLayout->setSpacing(10);

        QLineEdit *usernameEdit = new QLineEdit(&dialog);
        usernameEdit->setText(currentUser.username);

        formLayout->addRow("新用户名:", usernameEdit);

        mainLayout->addLayout(formLayout);

        QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
        mainLayout->addWidget(buttonBox);

        QObject::connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() != QDialog::Accepted) return;

        QString newUsername = usernameEdit->text().trimmed();

        if (newUsername.isEmpty()) { QMessageBox::warning(nullptr, "提示", "用户名不能为空"); return; }
        if (newUsername.length() < 3) { QMessageBox::warning(nullptr, "提示", "用户名至少3个字符"); return; }
        if (UserDao::existsUsername(newUsername, userId)) { QMessageBox::warning(nullptr, "提示", "用户名已被其他用户使用"); return; }

        if (UserDao::updateUsername(userId, newUsername)) { QMessageBox::information(nullptr, "成功", "用户名修改成功"); btnRefresh->click(); }
        else { QMessageBox::warning(nullptr, "失败", "用户名修改失败"); }
    });

    // 重置密码（修正用户ID获取）
    connect(btnResetPwd, &QPushButton::clicked, this, [table, btnRefresh]() {
        int row = table->currentRow();
        if (row < 0) {
            QMessageBox::warning(nullptr, "提示", "请先选中一名学生");
            return;
        }
        int userId = table->item(row, 7)->data(Qt::UserRole).toInt();
        if (userId <= 0) {
            QMessageBox::warning(nullptr, "提示", "该学生未关联账号，无法重置密码");
            return;
        }
        QString studentName = table->item(row, 2)->text();

        if (QMessageBox::question(nullptr, "确认重置",
                                  QString("确定要重置学生 \"%1\" 的密码吗？\n新密码将随机生成。").arg(studentName))
            != QMessageBox::Yes) {
            return;
        }

        const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*";
        QString newPwd;
        qsrand(QDateTime::currentDateTime().toMSecsSinceEpoch());
        for (int i = 0; i < 10; ++i) {
            newPwd += chars.at(qrand() % chars.length());
        }

        QByteArray hash = QCryptographicHash::hash(newPwd.toUtf8(), QCryptographicHash::Md5);
        if (UserDao::updatePassword(userId, hash.toHex())) {
            QMessageBox::information(nullptr, "重置成功",
                                     QString("学生 \"%1\" 的新密码为：\n\n%2\n\n请妥善保管并通知学生。")
                                     .arg(studentName).arg(newPwd));
            btnRefresh->click();
        } else {
            QMessageBox::warning(nullptr, "失败", "密码重置失败，请稍后重试");
        }
    });

    btnRefresh->click();
}

void MainWidget::createStudentUI()
{
    clearLayout(layout());

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(24, 20, 24, 20);

    QHBoxLayout *topRow = new QHBoxLayout;
    topRow->setSpacing(12);

    QWidget *spacer1 = new QWidget(this);
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    topRow->addWidget(spacer1);

    QLabel *welcomeLabel = new QLabel(QString("👤 学生: %1").arg(m_currentUsername), this);
    welcomeLabel->setStyleSheet("font-weight: 600; color: #1E293B; font-size: 18px;");

    QWidget *spacer2 = new QWidget(this);
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    topRow->addWidget(spacer2);

    QPushButton *btnZoomIn = new QPushButton("🔍 放大", this);
    btnZoomIn->setObjectName("secondary");
    btnZoomIn->setToolTip("Ctrl + +");

    QPushButton *btnZoomOut = new QPushButton("🔍 缩小", this);
    btnZoomOut->setObjectName("secondary");
    btnZoomOut->setToolTip("Ctrl + -");

    QPushButton *btnZoomReset = new QPushButton("重置", this);
    btnZoomReset->setObjectName("secondary");
    btnZoomReset->setToolTip("Ctrl + 0");

    topRow->addWidget(welcomeLabel);
    topRow->addWidget(btnZoomIn);
    topRow->addWidget(btnZoomOut);
    topRow->addWidget(btnZoomReset);
    mainLayout->addLayout(topRow);

    QGroupBox *infoBox = new QGroupBox("📋 我的信息", this);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoBox);
    infoLayout->setSpacing(12);
    infoLayout->setContentsMargins(24, 24, 24, 24);

    QLabel *studentIdLabel = new QLabel(this);
    QLabel *nameLabel = new QLabel(this);
    QLabel *majorLabel = new QLabel(this);
    QLabel *classLabel = new QLabel(this);
    QLabel *genderLabel = new QLabel(this);
    QLabel *birthLabel = new QLabel(this);
    QLabel *scoreLabel = new QLabel(this);
    QLabel *avgLabel = new QLabel(this);

    QList<QLabel*> labels = {studentIdLabel, nameLabel, majorLabel, classLabel, genderLabel, birthLabel, scoreLabel, avgLabel};
    for (QLabel *l : labels) {
        l->setStyleSheet("font-size: 16px; color: #1E293B; padding: 4px 0;");
    }

    // 成绩标签设为可换行+可伸缩，避免文字挤在一起
    scoreLabel->setWordWrap(true);
    scoreLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    avgLabel->setWordWrap(true);
    avgLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    infoLayout->addWidget(studentIdLabel);
    infoLayout->addWidget(nameLabel);
    infoLayout->addWidget(majorLabel);
    infoLayout->addWidget(classLabel);
    infoLayout->addWidget(genderLabel);
    infoLayout->addWidget(birthLabel);
    infoLayout->addWidget(scoreLabel);
    infoLayout->addWidget(avgLabel);

    // 用 QScrollArea 包裹 infoBox，使内容超出时可滚动
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { border: none; background: transparent; }");
    scrollArea->setWidget(infoBox);
    mainLayout->addWidget(scrollArea, 1);

    QPushButton *btnRefresh = new QPushButton("🔄 刷新我的信息", this);
    btnRefresh->setObjectName("primary");
    btnRefresh->setMinimumHeight(44);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(8);
    shadow->setColor(QColor(59, 130, 246, 40));
    shadow->setOffset(0, 2);
    btnRefresh->setGraphicsEffect(shadow);
    mainLayout->addWidget(btnRefresh);

    setLayout(mainLayout);

    connect(btnZoomIn, &QPushButton::clicked, this, &MainWidget::onZoomIn);
    connect(btnZoomOut, &QPushButton::clicked, this, &MainWidget::onZoomOut);
    connect(btnZoomReset, &QPushButton::clicked, this, &MainWidget::onZoomReset);

    connect(btnRefresh, &QPushButton::clicked, this, [=]() {
        Student s = StudentDao::selectByUserId(m_currentUserId);
        if (s.id == 0) { nameLabel->setText("⚠️ 未找到学生档案，请联系管理员"); return; }

        studentIdLabel->setText("学号: " + s.studentId);
        nameLabel->setText("姓名: " + s.name);
        majorLabel->setText("专业: " + s.major);
        classLabel->setText("班级: " + s.className);
        genderLabel->setText("性别: " + s.gender);
        birthLabel->setText("出生日期: " + s.birthDate.toString("yyyy/MM/dd"));

        QList<Score> scores = ScoreDao::selectByStudentId(s.id);
        if (scores.isEmpty()) { scoreLabel->setText("成绩: 暂无"); avgLabel->setText("平均分: 暂无"); }
        else {
            QString scoreText;
            double sum = 0;
            for (const Score &sc : scores) {
                scoreText += sc.subject + ": " + QString::number(sc.score) + "\n";
                sum += sc.score;
            }
            scoreLabel->setText("成绩:\n" + scoreText);
            avgLabel->setText("平均分: " + QString::number(sum / scores.size(), 'f', 2));
        }
    });

    btnRefresh->click();
}

void MainWidget::clearLayout(QLayout *layout)
{
    if (!layout) return;
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget()) { delete item->widget(); }
        delete item;
    }
}
