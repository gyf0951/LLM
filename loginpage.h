#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>
#include <QMouseEvent>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include<QMessageBox>
#include <QPainterPath>
#include <QGraphicsDropShadowEffect>
#include <QSettings>
#include <qt6keychain/keychain.h>


namespace Ui {
class loginPage;
}

class loginPage : public QWidget
{
    Q_OBJECT

public:
    explicit loginPage(QWidget *parent = nullptr);
    ~loginPage();

    QString account; //存储用户账号

signals:
    void loginSuccess();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QPoint m_dragPos;

private slots:
    void Loginaccount();



    void on_checkBox_stateChanged(int state); //记住密码

private:
    Ui::loginPage *ui;

    QSettings *settings;

    void loadSavedCredentials(); // 加载保存的凭据
    void saveCredentials(); // 保存凭据
    void clearCredentials(); // 清除保存的凭据
    QString encryptPassword(const QString &password); // 加密密码
    bool verifyPassword(const QString &inputPassword, const QString &storedPassword); // 验证密码
};

#endif // LOGINPAGE_H
