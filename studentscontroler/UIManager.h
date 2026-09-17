#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <QWidget>
#include <QDialog>
#include <QString>
#include <QLayout>
#include <QKeyEvent>
#include <QFont>
#include <QCheckBox>

class QLineEdit;
class QPushButton;
class QLabel;
class QTableWidget;
class QComboBox;
class QDateEdit;

class StyleHelper
{
public:
    static StyleHelper& instance();
    void setScaleFactor(double scale);
    double scaleFactor() const;
    QString generateStylesheet() const;
    QFont generateFont() const;
    void applyGlobalStyle();

private:
    StyleHelper();
    StyleHelper(const StyleHelper&) = delete;
    StyleHelper& operator=(const StyleHelper&) = delete;
    double m_scaleFactor;
};

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    QString getUsername() const;
    QString getPasswordHash() const;

private slots:
    void onLoginClicked();
    void onRegisterClicked();

private:
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginBtn;
    QPushButton *m_cancelBtn;
    QLabel *m_statusLabel;
    QPushButton *m_registerBtn;
    QPushButton *m_showPasswordBtn;
    bool m_isPasswordShowing;

    void showLockDialog(int remainSec);
};

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);

private slots:
    void onRegisterClicked();
    void onCancelClicked();

private:
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QLineEdit *m_confirmEdit;
    QLineEdit *m_nameEdit;
    QComboBox *m_majorCombo;
    QComboBox *m_classCombo;
    QComboBox *m_genderCombo;
    QDateEdit *m_birthEdit;
    QPushButton *m_registerBtn;
    QPushButton *m_cancelBtn;
    QLabel *m_statusLabel;
    QPushButton *m_showPasswordBtn;
    QPushButton *m_showConfirmBtn;
    bool m_isPasswordShowing;
    bool m_isConfirmShowing;
};

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();
    void setCurrentUser(int userId, const QString &username, const QString &role);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onZoomIn();
    void onZoomOut();
    void onZoomReset();

private:
    int m_currentUserId;
    QString m_currentUsername;
    QString m_currentRole;

    void createAdminUI();
    void createStudentUI();
    void clearLayout(QLayout *layout);
    void applyZoom(double factor);
};

#endif // UIMANAGER_H
