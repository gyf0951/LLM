#include "llmchat.h"
#include "ui_llmchat.h"
#include "welcomepage.h"

LLMChat::LLMChat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LLMChat)
    , m_welcomePage(nullptr)
{
    ui->setupUi(this);

    qDebug() << "toolButton_8 pointer:" << ui->toolButton_8;

    // 连接信号
    // connect(ui->toolButton_8, &QToolButton::clicked,
    //                          this, &LLMChat::on_toolButton_8_clicked);


    // connect(ui->toolButton_10, &QToolButton::clicked,
    //         this, &LLMChat::on_toolButton_10_clicked);

    // qDebug() << "Signal connection result:" << connected;

    // if (!connected) {
    //     qDebug() << "ERROR: Failed to connect toolButton_8 clicked signal!";
    // }
}



LLMChat::~LLMChat()
{
    delete ui;
}

void LLMChat::setWelcomePage(welcomepage* page)
{
    m_welcomePage = page;
}


void LLMChat::on_toolButton_8_clicked()
{
    qDebug() << "=== on_toolButton_8_clicked called ===";

    // 创建模型配置
    ModelConfig DeepSeekr1Config;
    DeepSeekr1Config.id = "deepseek-r1";
    DeepSeekr1Config.name = "deepseek-r1";
    DeepSeekr1Config.displayName = "deepseek-r1";
    DeepSeekr1Config.description = "deepseek-r1 大模型";
    DeepSeekr1Config.themeColor = "#FF5722";
    DeepSeekr1Config.imagePath = "E:\\QtProject\\LLM\\pic\\deepseek-r1_logo-removebg-preview.png";
    DeepSeekr1Config.apiKey = "application-d3efc597919f1e4d1a1873160de4d2a1";

    qDebug() << "Model config created:" << DeepSeekr1Config.name;
    qDebug() << "Image path:" << DeepSeekr1Config.imagePath;
    qDebug() << "m_welcomePage pointer:" << m_welcomePage;

    // 使用 setModelConfig 方法更新欢迎页
    if (m_welcomePage) {
        qDebug() << "Calling setModelConfig...";
        m_welcomePage->setModelConfig(DeepSeekr1Config);
        qDebug() << "setModelConfig called successfully";

        qDebug() << "Emitting switchToWelcomePage signal...";
        // 发出信号切换到欢迎页
        emit switchToWelcomePage();
        qDebug() << "Signal emitted";
    } else {
        qDebug() << "ERROR: m_welcomePage is null!";
        qDebug() << "Did you call setWelcomePage()?";
    }

    qDebug() << "=== on_toolButton_8_clicked finished ===";
}



void LLMChat::on_toolButton_10_clicked()
{
    qDebug() << "=== on_toolButton_10_clicked called ===";

    // 创建模型配置
    ModelConfig DeepSeekr1Config;
    DeepSeekr1Config.id = "deepseek-v3";
    DeepSeekr1Config.name = "deepseek-v3";
    DeepSeekr1Config.displayName = "deepseek-v3";
    DeepSeekr1Config.description = "deepseek-v3 大模型";
    DeepSeekr1Config.themeColor = "#FF5722";
    DeepSeekr1Config.imagePath = "E:\\QtProject\\LLM\\pic\\deepseek-v3_logo-removebg-preview.png";
    DeepSeekr1Config.apiKey ="application-67faa7877ddf2059413280febc71a644";

    qDebug() << "Model config created:" << DeepSeekr1Config.name;
    qDebug() << "Image path:" << DeepSeekr1Config.imagePath;
    qDebug() << "m_welcomePage pointer:" << m_welcomePage;

    // 使用 setModelConfig 方法更新欢迎页
    if (m_welcomePage) {
        qDebug() << "Calling setModelConfig...";
        m_welcomePage->setModelConfig(DeepSeekr1Config);
        qDebug() << "setModelConfig called successfully";

        qDebug() << "Emitting switchToWelcomePage signal...";
        // 发出信号切换到欢迎页
        emit switchToWelcomePage();
        qDebug() << "Signal emitted";
    } else {
        qDebug() << "ERROR: m_welcomePage is null!";
        qDebug() << "Did you call setWelcomePage()?";
    }

    qDebug() << "=== on_toolButton_10_clicked finished ===";
}


void LLMChat::on_toolButton_11_clicked()
{
    qDebug() << "=== on_toolButton_11_clicked called ===";

    // 创建模型配置
    ModelConfig DeepSeekr1Config;
    DeepSeekr1Config.id = "Kimi k2";
    DeepSeekr1Config.name = "Kimi k2";
    DeepSeekr1Config.displayName = "Kimi k2";
    DeepSeekr1Config.description = "Kimi k2";
    DeepSeekr1Config.themeColor = "#FF5722";
    DeepSeekr1Config.imagePath = "E:\\QtProject\\LLM\\pic\\kimi-logo-removebg-preview(1).png";
    DeepSeekr1Config.apiKey ="application-2020ea46782e88b90afb2e588f396af4";


    qDebug() << "Model config created:" << DeepSeekr1Config.name;
    qDebug() << "Image path:" << DeepSeekr1Config.imagePath;
    qDebug() << "m_welcomePage pointer:" << m_welcomePage;

    // 使用 setModelConfig 方法更新欢迎页
    if (m_welcomePage) {
        qDebug() << "Calling setModelConfig...";
        m_welcomePage->setModelConfig(DeepSeekr1Config);
        qDebug() << "setModelConfig called successfully";

        qDebug() << "Emitting switchToWelcomePage signal...";
        // 发出信号切换到欢迎页
        emit switchToWelcomePage();
        qDebug() << "Signal emitted";
    } else {
        qDebug() << "ERROR: m_welcomePage is null!";
        qDebug() << "Did you call setWelcomePage()?";
    }

    qDebug() << "=== on_toolButton_11_clicked finished ===";
}


void LLMChat::on_toolButton_15_clicked()
{
    qDebug() << "=== on_toolButton_15_clicked called ===";

    // 创建模型配置
    ModelConfig DeepSeekr1Config;
    DeepSeekr1Config.id = "Qwen-Plus";
    DeepSeekr1Config.name = "Qwen-Plus";
    DeepSeekr1Config.displayName = "Qwen-Plus";
    DeepSeekr1Config.description = "Qwen-Plus";
    DeepSeekr1Config.themeColor = "#FF5722";
    DeepSeekr1Config.imagePath = "E:\\QtProject\\LLM\\pic\\qwne-plus-logo-removebg-preview.png";
    DeepSeekr1Config.apiKey = "application-9211aba0dc6878930f01f62e20e882cc";

    qDebug() << "Model config created:" << DeepSeekr1Config.name;
    qDebug() << "Image path:" << DeepSeekr1Config.imagePath;
    qDebug() << "m_welcomePage pointer:" << m_welcomePage;

    // 使用 setModelConfig 方法更新欢迎页
    if (m_welcomePage) {
        qDebug() << "Calling setModelConfig...";
        m_welcomePage->setModelConfig(DeepSeekr1Config);
        qDebug() << "setModelConfig called successfully";

        qDebug() << "Emitting switchToWelcomePage signal...";
        // 发出信号切换到欢迎页
        emit switchToWelcomePage();
        qDebug() << "Signal emitted";
    } else {
        qDebug() << "ERROR: m_welcomePage is null!";
        qDebug() << "Did you call setWelcomePage()?";
    }

    qDebug() << "=== on_toolButton_15_clicked finished ===";
}


void LLMChat::on_toolButton_16_clicked()
{
    qDebug() << "=== on_toolButton_16_clicked called ===";

    // 创建模型配置
    ModelConfig DeepSeekr1Config;
    DeepSeekr1Config.id = "Qwen-Flash";
    DeepSeekr1Config.name = "Qwen-Flash";
    DeepSeekr1Config.displayName = "Qwen-Flash";
    DeepSeekr1Config.description = "Qwen-Flash";
    DeepSeekr1Config.themeColor = "#FF5722";
    DeepSeekr1Config.imagePath = "E:\\QtProject\\LLM\\pic\\qwen-flash-removebg-preview.png";
    DeepSeekr1Config.apiKey ="application-5ee531f103171536f7f17c47ce7db60a";

    qDebug() << "Model config created:" << DeepSeekr1Config.name;
    qDebug() << "Image path:" << DeepSeekr1Config.imagePath;
    qDebug() << "m_welcomePage pointer:" << m_welcomePage;

    // 使用 setModelConfig 方法更新欢迎页
    if (m_welcomePage) {
        qDebug() << "Calling setModelConfig...";
        m_welcomePage->setModelConfig(DeepSeekr1Config);
        qDebug() << "setModelConfig called successfully";

        qDebug() << "Emitting switchToWelcomePage signal...";
        // 发出信号切换到欢迎页
        emit switchToWelcomePage();
        qDebug() << "Signal emitted";
    } else {
        qDebug() << "ERROR: m_welcomePage is null!";
        qDebug() << "Did you call setWelcomePage()?";
    }

    qDebug() << "=== on_toolButton_16_clicked finished ===";
}


void LLMChat::on_toolButton_17_clicked()
{
    qDebug() << "=== on_toolButton_17_clicked called ===";

    // 创建模型配置
    ModelConfig DeepSeekr1Config;
    DeepSeekr1Config.id = "Qwen-Max";
    DeepSeekr1Config.name = "Qwen-Max";
    DeepSeekr1Config.displayName = "Qwen-Max";
    DeepSeekr1Config.description = "Qwen-Maxh";
    DeepSeekr1Config.themeColor = "#FF5722";
    DeepSeekr1Config.imagePath = "E:\\QtProject\\LLM\\pic\\qwen-max-removebg-preview.png";
    DeepSeekr1Config.apiKey ="application-a5bd862c386743bc29977022c0786873";

    qDebug() << "Model config created:" << DeepSeekr1Config.name;
    qDebug() << "Image path:" << DeepSeekr1Config.imagePath;
    qDebug() << "m_welcomePage pointer:" << m_welcomePage;

    // 使用 setModelConfig 方法更新欢迎页
    if (m_welcomePage) {
        qDebug() << "Calling setModelConfig...";
        m_welcomePage->setModelConfig(DeepSeekr1Config);
        qDebug() << "setModelConfig called successfully";

        qDebug() << "Emitting switchToWelcomePage signal...";
        // 发出信号切换到欢迎页
        emit switchToWelcomePage();
        qDebug() << "Signal emitted";
    } else {
        qDebug() << "ERROR: m_welcomePage is null!";
        qDebug() << "Did you call setWelcomePage()?";
    }

    qDebug() << "=== on_toolButton_17_clicked finished ===";
}

