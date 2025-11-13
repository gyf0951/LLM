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

        static bool firstEnterLLM = true;  // 定义一个静态变量，用于标记是否是第一次进入 LLMChat



        chat_model_key->getProfile();




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

                ModelConfig defaultConfig;

                defaultConfig.id = "jxk";
                defaultConfig.name = "jxk";
                defaultConfig.displayName = "jxk";
                defaultConfig.description = "jxk";
                defaultConfig.themeColor = "#FF5722";
                defaultConfig.apiKey = "application-4a5b57a1f8e72c81b7fe26194a1243d0";

                emit llm->modelConfigChanged(defaultConfig);
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
                if (firstEnterLLM) {
                    //  第一次进入时默认 DeepSeek R1
                    QMetaObject::invokeMethod(llm, "on_toolButton_10_clicked", Qt::QueuedConnection);
                    firstEnterLLM = false;
                } else {
                    //  之后进入时应用上次选中的模型
                    llm->applyCurrentModel();
                }
            });
        }

        // 大模型页 -> 首页
        QToolButton *btnllm = llm->findChild<QToolButton*>("toolButton_3");
        if(btnllm)
        {
            QObject::connect(btnllm, &QToolButton::clicked, stackedWidget, [=]() {
                stackedWidget->setCurrentIndex(0);

                ModelConfig defaultConfig;

                defaultConfig.id = "jxk";
                defaultConfig.name = "jxk";
                defaultConfig.displayName = "jxk";
                defaultConfig.description = "jxk";
                defaultConfig.themeColor = "#FF5722";
                defaultConfig.apiKey = "application-4a5b57a1f8e72c81b7fe26194a1243d0";

                emit llm->modelConfigChanged(defaultConfig);
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

        QObject::connect(llm, &LLMChat::sendMessageToChatPage,
                         chatPage, [=](const QString &msg) {
                             chatPage->receiveMessageFromLLM(msg);
                             stackedWidget->setCurrentIndex(1);
                         });

        // LLMChat 模型切换 -> Chat 更新配置
        QObject::connect(llm, &LLMChat::modelConfigChanged,
                         chat_model_key, &Chat::setModelConfig);
    });




    return a.exec();
}
