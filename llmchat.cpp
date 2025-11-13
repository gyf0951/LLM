#include "llmchat.h"
#include "ui_llmchat.h"
#include<QStyle>
#include<QTimer>
#include<QAction>
#include <QKeyEvent>
#include <QClipboard>
#include <QMimeData>

LLMChat::LLMChat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LLMChat)
{
    ui->setupUi(this);


    //增加搜索图标
    QAction *searchAction = new QAction(this);
    searchAction->setIcon(QIcon(":/pic/img/66@2x.png"));
    ui->lineEdit->addAction(searchAction, QLineEdit::LeadingPosition);

    //初始化模型配置
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

    setModelConfig(DeepSeekr1Config);



    //默认选择button10
    m_selectedButton = ui->toolButton_10;
    m_selectedButton->setProperty("selected", true);
    m_selectedButton->style()->unpolish(m_selectedButton);
    m_selectedButton->style()->polish(m_selectedButton);


    QTimer::singleShot(0, this, [this]() {
        // 在事件循环启动后调用，保证控件布局正确
        resizeEvent(nullptr);
    });

    //发送按钮使能
    ui->pushButton->setEnabled(false);
    ui->pushButton->setIcon(QIcon());
    ui->pushButton->setStyleSheet(
        "QPushButton {"
        "    border-image: url(\"E:/QtProject/LLM/pic/img/切图 72@2x.png\");"
        "    border-radius: 19px;"
        "}"
        );
    connect(ui->textEdit, &QTextEdit::textChanged, this, [this] {
        if(ui->textEdit->document()->isEmpty()){
            ui->pushButton->setEnabled(false);
            ui->pushButton->setStyleSheet(
                "QPushButton {"
                "    border-image: url(\"E:/QtProject/LLM/pic/img/切图 72@2x.png\");"
                "    border-radius: 19px;"
                "}"
                );
        }
        else{
            ui->pushButton->setEnabled(true);
            ui->pushButton->setStyleSheet(
                "QPushButton {"
                "    border-image: url(\"E:/QtProject/LLM/pic/img/切图 77@2x.png\");"
                "    border-radius: 19px;"
                "}"
                );
        }
    });

    ui->textEdit->installEventFilter(this);
    ui->textEdit->setFocus();
}



LLMChat::~LLMChat()
{
    delete ui;
}

//设置模型参数
void LLMChat::setModelConfig(const ModelConfig& config)
{
    m_currentModelConfig = config;
    updateChatUI();

    if (!config.imagePath.isEmpty())
        addModelImageToHeader(config.imagePath);
}


//设置按钮位置
void LLMChat::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event); // 先调用父类的处理

    int margin = 10; // 发送按钮距离边界的间隔
    ui->pushButton->move(
        ui->textEdit->x() + ui->textEdit->width() - ui->pushButton->width() - margin,
        ui->textEdit->y() + ui->textEdit->height() - ui->pushButton->height() - margin
        );
    int margin_2 = 20; //按钮2与发送按钮间隔
    ui->pushButton_2->move(
        ui->pushButton->x()  - ui->pushButton_2->width() - margin_2,
        ui->pushButton->y()
        );

    int margin_3 = 11;  //按钮3与发送按钮间隔
    ui->pushButton_3->move(
        ui->pushButton_2->x()  - ui->pushButton_3->width() - margin_3,
        ui->pushButton_2->y()
        );

    /* ui->pushButton_4->move(
        ui->pushButton_3->x()  - ui->pushButton_4->width() - margin_3,
        ui->pushButton_3->y()
        ); */

    //设置frame线条位置
    int  spacing = 15;
    ui->frame_5->move(
        ui->pushButton->geometry().center().x() - ui->frame->width() - spacing,
        ui->pushButton->geometry().center().y() - ui->frame->height()/2
        );
}

bool LLMChat::eventFilter(QObject *obj, QEvent *ev)
{
    if(obj == ui->textEdit && ev->type() == QEvent::KeyPress)
    {
        auto *ke = static_cast<QKeyEvent *>(ev);
        int key = ke->key();
        Qt::KeyboardModifiers mods = ke->modifiers();

        // 普通 Enter / Return 且没有 Shift
        if ((key == Qt::Key_Return || key == Qt::Key_Enter) &&
            !(mods & Qt::ShiftModifier))
        {
            on_pushButton_clicked();
            return true;
        }

        //统一复制过来的格式
        if  (ke->matches(QKeySequence::Paste))
        {
            const QMimeData *mime = QGuiApplication::clipboard()->mimeData();
            if (mime && mime->hasText())
            {
                QTextCursor cursor = ui->textEdit->textCursor();
                QTextCharFormat fmt;
                fmt.setFontFamilies({ "Microsoft YaHei" });
                fmt.setFontPointSize(9);
                cursor.insertText(mime->text(), fmt);
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, ev);
}


//设置模型图片
void LLMChat::addModelImageToHeader(const QString& imagePath)
{
    removeModelImageFromHeader();

    m_modelImageLabel = new QLabel(this);
    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) return;

    m_modelImageLabel->setPixmap(pixmap);
    m_modelImageLabel->setFixedSize(200, 80);
    m_modelImageLabel->setScaledContents(true);

    QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(ui->horizontalFrame->layout());
    if (layout) {
        int label2Index = layout->indexOf(ui->label_2);
        if (label2Index >= 0) {
            layout->insertWidget(label2Index + 1, m_modelImageLabel);
            layout->insertSpacing(label2Index + 2, 5);
        }
    }
}


//按钮切换选中状态
void LLMChat::setSelectedButton(QToolButton *button)
{
    if (m_selectedButton == button)
        return;

    // 1. 取消上一个选中状态
    if (m_selectedButton) {
        m_selectedButton->setProperty("selected", false);
        m_selectedButton->style()->unpolish(m_selectedButton);
        m_selectedButton->style()->polish(m_selectedButton);
    }

    // 2. 设置新的选中按钮
    m_selectedButton = button;
    m_selectedButton->setProperty("selected", true);
    m_selectedButton->style()->unpolish(m_selectedButton);
    m_selectedButton->style()->polish(m_selectedButton);
}


//删除图片
void LLMChat::removeModelImageFromHeader()
{
    if (m_modelImageLabel) {
        QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(ui->horizontalFrame->layout());
        if (layout) layout->removeWidget(m_modelImageLabel);
        delete m_modelImageLabel;
        m_modelImageLabel = nullptr;
    }
}

void LLMChat::updateChatUI()
{
    ui->pushButton->setToolTip(QString("给 %1 发送消息").arg(m_currentModelConfig.displayName));
    this->update();
}


void LLMChat::applyCurrentModel()
{
    if (m_selectedButton == ui->toolButton_10)
        on_toolButton_10_clicked();
    else if (m_selectedButton == ui->toolButton_11)
        on_toolButton_11_clicked();
    else if (m_selectedButton == ui->toolButton_15)
        on_toolButton_15_clicked();
    else if (m_selectedButton == ui->toolButton_16)
        on_toolButton_16_clicked();
    else if (m_selectedButton == ui->toolButton_17)
        on_toolButton_17_clicked();
    else if (m_selectedButton == ui->toolButton_12)
        on_toolButton_12_clicked();
}



void LLMChat::on_toolButton_10_clicked()
{
    qDebug() << "=== on_toolButton_10_clicked called ===";

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

    setModelConfig(DeepSeekr1Config);

    qDebug() << "=== on_toolButton_10_clicked finished ===";

    setSelectedButton(ui->toolButton_10);

    emit modelConfigChanged(DeepSeekr1Config); //  通知 Chat 使用新的模型
}



void LLMChat::on_toolButton_11_clicked()
{
    qDebug() << "=== on_toolButton_11_clicked called ===";

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
    setModelConfig(DeepSeekr1Config);


    qDebug() << "=== on_toolButton_11_clicked finished ===";

    setSelectedButton(ui->toolButton_11);

    emit modelConfigChanged(DeepSeekr1Config);
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
    setModelConfig(DeepSeekr1Config);


    qDebug() << "=== on_toolButton_15_clicked finished ===";

    setSelectedButton(ui->toolButton_15);

    emit modelConfigChanged(DeepSeekr1Config);
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
    setModelConfig(DeepSeekr1Config);


    qDebug() << "=== on_toolButton_16_clicked finished ===";

    setSelectedButton(ui->toolButton_16);

    emit modelConfigChanged(DeepSeekr1Config);
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
    setModelConfig(DeepSeekr1Config);


    qDebug() << "=== on_toolButton_17_clicked finished ===";

    setSelectedButton(ui->toolButton_17);

    emit modelConfigChanged(DeepSeekr1Config);
}


void LLMChat::on_toolButton_12_clicked()
{
        qDebug() << "=== on_toolButton_12_clicked called ===";

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

        setModelConfig(DeepSeekr1Config);


        qDebug() << "=== on_toolButton_12_clicked finished ===";

        setSelectedButton(ui->toolButton_12);
}

void LLMChat::on_toolButton_8_clicked()
{

}

void LLMChat::on_pushButton_clicked()
{
    QString msg = ui->textEdit->toPlainText().trimmed();
    if (msg.isEmpty()) return;

    emit sendMessageToChatPage(msg);  // 发射信号给聊天页
    ui->textEdit->clear();
}

