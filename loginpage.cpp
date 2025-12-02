#include "loginpage.h"
#include "ui_loginpage.h"


using namespace QKeychain;


loginPage::loginPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::loginPage)
{
    ui->setupUi(this);

    settings = new QSettings("MyCompany", "MyApp", this);

    // 设置无边框窗口（去掉标题栏、最大化、最小化按钮）
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);

    setAttribute(Qt::WA_TranslucentBackground);  // 允许透明
    setStyleSheet("background-color: white; border-radius: 15px;");

    // 添加阴影效果
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);       // 模糊半径
    shadow->setOffset(0, 0);         // 阴影偏移
    shadow->setColor(QColor(0, 0, 0, 150)); // 阴影颜色
    this->setGraphicsEffect(shadow);

    ui->label_5->setVisible(false);

    // 绑定关闭按钮
    connect(ui->closeButton, &QPushButton::clicked, qApp, &QCoreApplication::quit);

    // 登录按钮逻辑
        connect(ui->loginButton, &QPushButton::clicked, this,&loginPage::Loginaccount);
        connect(ui->passwordEdit, &QLineEdit::returnPressed, this, &loginPage::Loginaccount);

    //记住密码
        loadSavedCredentials();  // 加载已保存的凭据
        connect(ui->checkBox,&QCheckBox::stateChanged,this,&loginPage::on_checkBox_stateChanged);
}

loginPage::~loginPage()
{
    delete ui;
}


// 记录鼠标点击位置
void loginPage::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

// 鼠标拖动事件（实现窗口拖动）
void loginPage::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - m_dragPos);
        event->accept();
    }
}

// 圆角绘制
void loginPage::paintEvent(QPaintEvent *event)
{
    QPainterPath path;
    path.addRoundedRect(rect(), 10, 10); // 半径 15，可调

    QRegion maskRegion(path.toFillPolygon().toPolygon());
    setMask(maskRegion);

    QWidget::paintEvent(event);
}



void loginPage::Loginaccount()
{
    account = ui->accountEdit->text().trimmed();
    QString password = ui->passwordEdit->text();

    ui->label_5->setVisible(false);

    // 手机号验证
    QRegularExpression regex("^1[3-9]\\d{9}$");
    if (!regex.match(account).hasMatch()) {
        ui->label_5->setVisible(true);
        ui->accountEdit->setStyleSheet("QLineEdit {"
                                       "    background-color: rgb(247, 249, 250);"
                                       "    border-radius: 25px;"
                                       "    border: 1px solid red;"   // 改成红色边框
                                       "    padding: 10px;"
                                       "}"
                                       "QLineEdit QScrollBar { width: 0; height: 0; }");
        return;
    }
    else{
        ui->accountEdit->setStyleSheet("QLineEdit {"
                                       "    background-color: rgb(247, 249, 250);"
                                       "    border-radius: 25px;"
                                       "    border: 1px solid #999;"
                                       "    padding: 10px;"
                                       "}"
                                       "QLineEdit QScrollBar { width: 0; height: 0; }");
    }


    if (password != "Nxjk@0951") {
        QMessageBox::warning(this, "登录失败", "密码错误！");
        return;
    }

    // 登录成功后的处理
    if(ui->checkBox->isChecked())
        saveCredentials();
    else
        clearCredentials();

    emit loginSuccess(); // 登录成功，发信号
}


//记住密码
void loginPage::on_checkBox_stateChanged(int state)
{
    if (state == Qt::Unchecked) {
        clearCredentials();
    }
}

//加载数据
void loginPage::loadSavedCredentials()
{
    QString savedAccount = settings->value("login/account", "").toString();

    bool remember = settings->value("login/remember", false).toBool();

    if (!savedAccount.isEmpty()) {
        ui->accountEdit->setText(savedAccount);
    }
    if (remember) {
        ReadPasswordJob *job = new ReadPasswordJob("MyApp", this);
        job->setKey(savedAccount);
        connect(job, &Job::finished, this, [=]() {
            if (job->error()) {
                qDebug() << "读取密码失败:" << job->errorString();
                return;
            }
            ui->passwordEdit->setText(job->textData());
            ui->checkBox->setChecked(true);
        });
        job->start();
    }

}

//保存数据
void loginPage::saveCredentials()
{

    QString account = ui->accountEdit->text().trimmed();
    QString password = ui->passwordEdit->text();

    settings->setValue("login/account", account);
    settings->setValue("login/remember", true);

    // 保存密码到系统安全存储
    WritePasswordJob *job = new WritePasswordJob("MyApp", this);
    job->setKey(account);
    job->setTextData(password);
    connect(job, &Job::finished, this, [=]() {
        if (job->error()) {
            qDebug() << "保存密码失败:" << job->errorString();
        }
    });
    job->start();
}


void loginPage::clearCredentials()
{
    QString account = settings->value("login/account", "").toString();
    settings->remove("login/account");
    settings->setValue("login/remember", false);

    // 删除安全存储中的密码
    DeletePasswordJob *job = new DeletePasswordJob("MyApp", this);
    job->setKey(account);
    connect(job, &Job::finished, this, [=]() {
        if (job->error()) {
            qDebug() << "删除密码失败:" << job->errorString();
        }
    });
    job->start();
}

