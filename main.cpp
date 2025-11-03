#include "widget.h"
#include "welcomepage.h"
#include "loginpage.h"
#include "llmchat.h"
#include "Chat.h"

#include <QCoreApplication>
#include <QApplication>
#include <QStackedWidget>
#include <QPushButton>
#include <QDebug>
#include <QLabel>
#include<QMessageBox>
#include <QLineEdit>
#include<agent.h>
#include<QToolButton>

int main(int argc, char *argv[])
{



    QApplication::setHighDpiScaleFactorRoundingPolicy(
        Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");

    QApplication a(argc, argv);



    loginPage login;
    login.show();

    //登录成功
    QObject::connect(&login, &loginPage::loginSuccess,[&]{
        login.close();

        // 创建主窗口（有边框）
        QStackedWidget *stackedWidget = new QStackedWidget;
        QEventLoop loop;

        welcomepage *welcomePage = new welcomepage();

        Chat *chat_model_key = new Chat();
        Widget *chatPage = new Widget(chat_model_key);

        Agent *agent = new Agent();

        LLMChat *llm = new LLMChat();



        llm->setWelcomePage(welcomePage);

        chat_model_key->getProfile();

        QObject::connect(welcomePage, &welcomepage::modelConfigChanged,
                         chat_model_key, [=](const ModelConfig& config) {
                            chat_model_key->setModelConfig(config);
                         });


        stackedWidget->addWidget(welcomePage); // index 0  首页
        stackedWidget->addWidget(chatPage);    // index 1  聊天页
        stackedWidget->addWidget(agent);       // index 2 智能体页
        stackedWidget->addWidget(llm);        // index 3 大模型选择页

        stackedWidget->setCurrentIndex(0);

        stackedWidget->setWindowTitle("交小科");
        QIcon con("E:\\QtProject\\LLM\\pic\\20250815090415.png");
        stackedWidget->setWindowIcon(con);
        stackedWidget->show();

        // 首页 -> 聊天页
        QPushButton *btn = welcomePage->findChild<QPushButton*>("pushButton");
        QObject::connect(btn, &QPushButton::clicked, stackedWidget, [=]() {
            stackedWidget->setCurrentIndex(1);
        });

        // 首页 -> 智能体页
        QToolButton *btnAgent = welcomePage->findChild<QToolButton*>("toolButton_2");
        if (btnAgent)
        {
            QObject::connect(btnAgent, &QToolButton::clicked, stackedWidget, [=]() {
                stackedWidget->setCurrentIndex(2);
            });
        }

        // 智能体页 -> 返回首页
        QToolButton *btnHome = agent->findChild<QToolButton*>("toolButton_3");
        if (btnHome)
        {
            QObject::connect(btnHome, &QToolButton::clicked, stackedWidget, [=]() {
                stackedWidget->setCurrentIndex(0);
            });
        }


        // 聊天页 -> 智能体页
        QToolButton *btnAgent_2 = chatPage->findChild<QToolButton*>("toolButton_2");
        if(btnAgent_2)
        {
            QObject::connect(btnAgent_2, &QToolButton::clicked, stackedWidget, [=]() {
                stackedWidget->setCurrentIndex(2);
            });
        }

        //聊天页 -> 首页
        QToolButton *btnHome_2 = chatPage->findChild<QToolButton*>("toolButton_3");
        if(btnHome_2)
        {
             QObject::connect(btnHome_2, &QToolButton::clicked, stackedWidget, [=]() {
                 stackedWidget->setCurrentIndex(0);
             });
        }

        //首页-> 大模型页
        QToolButton *btnhome = welcomePage->findChild<QToolButton*>("toolButton");
        if(btnhome)
        {
            QObject::connect(btnhome, &QToolButton::clicked, stackedWidget, [=]() {
                stackedWidget->setCurrentIndex(3);
            });
        }

        // 大模型页 -> 首页
        QToolButton *btnllm = llm->findChild<QToolButton*>("toolButton_3");
        if(btnllm)
        {
            QObject::connect(btnllm, &QToolButton::clicked, stackedWidget, [=]() {
                stackedWidget->setCurrentIndex(0);
            });
        }

        // 聊天页 -> 大模型页
        QToolButton *btnchat = chatPage->findChild<QToolButton*>("toolButton");
        if(btnchat)
        {
            QObject::connect(btnchat, &QToolButton::clicked, stackedWidget, [=]() {
                stackedWidget->setCurrentIndex(3);
            });
        }

        // 智能体页 -> 大模型页
        QToolButton *btnagent = agent->findChild<QToolButton*>("toolButton");
        if(btnagent)
        {
            qDebug()<<"hhh";
            QObject::connect(btnagent, &QToolButton::clicked, stackedWidget, [=]() {
                stackedWidget->setCurrentIndex(3);
            });
        }

         // 大模型页 -> 智能体页
        QToolButton *btnllm_2 = llm->findChild<QToolButton*>("toolButton_2");
        if(btnllm_2)
        {
            QObject::connect(btnllm_2, &QToolButton::clicked, stackedWidget, [=]() {
                stackedWidget->setCurrentIndex(2);
            });
        }

        //点击r1模型切换页面
        QObject::connect(llm,&LLMChat::switchToWelcomePage,welcomePage, [=](){
            stackedWidget->setCurrentIndex(0);
        });

        QObject::connect(welcomePage, &welcomepage::sendMessageToChatPage,
                         chatPage, [=](const QString &msg) {
                             chatPage->receiveMessageFromWelcome(msg);
                             stackedWidget->setCurrentIndex(1);
                         });
    });

   // // 创建页面
   //  welcomepage *welcomePage = new welcomepage();
   //  Widget *chatPage = new Widget();
   //  loginPage *login = new loginPage();


   // // 创建stackedWidget
   //  QStackedWidget *stackedWidget = new QStackedWidget();
   //  stackedWidget->addWidget(login) ;       // index 0
   //  stackedWidget->addWidget(welcomePage); // index 1
   //  stackedWidget->addWidget(chatPage);    // index 2


   //  stackedWidget->setFixedSize(login->size());


   // // 默认显示欢迎页
   //  stackedWidget->setCurrentIndex(0);

   // // 页面切换时调整窗口大小
   //  QObject::connect(stackedWidget, &QStackedWidget::currentChanged,stackedWidget,
   //                   [=](int index) {
   //                       QWidget *page = stackedWidget->widget(index);
   //                       if (page) {
   //                           QSize pageSize = page->size();
   //                           // 先调整大小
   //                           stackedWidget->resize(pageSize);
   //                           // 再强制限制窗口大小，避免自动拉伸
   //                           stackedWidget->setMinimumSize(pageSize);
   //                           stackedWidget->setMaximumSize(pageSize);
   //                       }
   //                   });



   //  QPushButton *loginBtn = login->findChild<QPushButton*>("pushButton");
   //  QLineEdit *accountEdit = login->findChild<QLineEdit*>("lineEdit_2");
   //  QLineEdit *passwordEdit = login->findChild<QLineEdit*>("lineEdit");
   //  QLabel *accountErrorLabel = login->findChild<QLabel*>("label_5");
   //  accountErrorLabel->setVisible(false);

   //  QObject::connect(loginBtn, &QPushButton::clicked, [=]() {
   //      // 账号密码验证
   //      QString account = accountEdit->text().trimmed();
   //      QString password = passwordEdit->text();

   //       accountErrorLabel->setVisible(false);

   //      // 手机号正则
   //      QRegularExpression regex("^1[3-9]\\d{9}$");
   //      bool accountValid = regex.match(account).hasMatch();

   //      if (!accountValid){
   //          accountErrorLabel->setVisible(true);
   //          accountEdit->setStyleSheet("QLineEdit {"
   //                                     "    background-color: rgb(247, 249, 250);"
   //                                     "    border-radius: 25px;"
   //                                     "    border: 1px solid red;"   // 改成红色边框
   //                                     "    padding: 10px;"
   //                                     "}"
   //                                     "QLineEdit QScrollBar { width: 0; height: 0; }");
   //          return;
   //      }

   //      if (password != "Nxjk@0951"){
   //          QMessageBox::warning(login, QStringLiteral("登录失败"),  QStringLiteral("密码错误！"));
   //          return;
   //      }

   //      stackedWidget->setCurrentIndex(1); // 登录成功进入欢迎页
   //  });

   //  QPushButton *btn = welcomePage->findChild<QPushButton*>("pushButton");
   //  QObject::connect(btn, &QPushButton::clicked,stackedWidget, [=]() {
   //      stackedWidget->setCurrentIndex(2);
   //  });
   //  QObject::connect(welcomePage, &welcomepage::sendMessageToChatPage,
   //                   chatPage, [=](const QString &msg){

   //      chatPage->receiveMessageFromWelcome(msg);

   //      stackedWidget->setCurrentIndex(2);
   //  });


   //  stackedWidget->setWindowTitle("交小科");
   //  QIcon con("E:\\QtProject\\LLM\\pic\\20250815090415.png");
   //  stackedWidget->setWindowIcon(con);


   //  //stackedWidget->resize(login->size());
   //  stackedWidget->adjustSize();

   //  stackedWidget->show();

    return a.exec();
}
