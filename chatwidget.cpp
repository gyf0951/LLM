#include "ChatWidget.h"
#include <QTextBrowser>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QDebug>
#include <QResizeEvent>


ChatWidget::ChatWidget(QWidget *parent)
    : QListWidget(parent)
    ,m_lastAiItem(nullptr)
{
    // 样式表可以简化，背景色等在父窗口设置
    setStyleSheet("QListWidget { border: none; background: transparent; }");

    setSelectionMode(QAbstractItemView::NoSelection);
    setFocusPolicy(Qt::NoFocus);

    setResizeMode(QListWidget::Adjust);
    setSpacing(10);

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

void ChatWidget::clear()
{
    m_lastAiMessage.clear();
    m_lastAiItem = nullptr;
    QListWidget::clear(); // 调用基类的 clear
}

void ChatWidget::addMessage(const QString &text, bool isMe)
{
    bool autoScroll = isAtBottom();

    // 1. 创建文本浏览器 (用于显示 Markdown)
    QTextBrowser *browser = new QTextBrowser();
    browser->setReadOnly(true);
    browser->setOpenExternalLinks(true);
    browser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);   // 禁用内部滚动条
    browser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用内部滚动条

    // **关键**: 继承你在 widget.cpp 中设置的字体
    browser->setFont(this->font());

    if (isMe) {
        browser->setPlainText(text); // 用户消息显示为纯文本
    } else {
        m_lastAiMessage = text; // 存储AI消息
        browser->setMarkdown(text); // AI 消息渲染为 Markdown
    }

    // 2. 创建一个容器 QWidget 和一个布局
    QWidget *container = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0); // 容器0边距

    // 3. 根据 isMe 设置样式表和对齐方式
    if (isMe) {
        // 用户: 气泡绿色, 靠右
        browser->setStyleSheet(
            "QTextBrowser {"
            "  background-color: #F7F7F8;"
            "  border-radius: 10px;"
            "  padding: 0px;"
            "}"
            );
        layout->addStretch(1);     // 弹簧在左
        layout->addWidget(browser, 0, Qt::AlignVCenter);
    } else {
        browser->setStyleSheet(
            "QTextBrowser {"
            "  background-color: transparent;" // <-- 修改：背景透明
            "  border: none;"                // <-- 修改：无边框
            "  padding: 8px;"                // <-- 保留: 内边距，使其与用户消息对齐
            "}"
            );
        layout->addWidget(browser); // 控件在左
        layout->addStretch(1);     // 弹簧在右
    }
    container->setLayout(layout);


    // 4. 创建 QListWidgetItem
    QListWidgetItem *item = new QListWidgetItem();
    if (!isMe) {
        m_lastAiItem = item; // 保存对最后 AI item 的引用
    }

    // ** 解决动态高度的核心 **
    // 当 browser 的内容改变时 (例如 setMarkdown)，我们调用槽函数更新 item 高度
    connect(browser->document(), &QTextDocument::contentsChanged, this, [=](){
        updateItemHeight(item);
    });

    // 5. 添加到 ListWidget
    this->addItem(item);
    this->setItemWidget(item, container); // <-- 使用 setItemWidget

    // 6. 立即更新一次高度
    updateItemHeight(item);

    // 7. 滚动
    if (autoScroll) {
        delayedScrollToBottom();
    }

}

void ChatWidget::appendToLastMessage(const QString &textFragment, bool isMe)
{
    // 如果是用户消息, 或是第一条 AI 消息, 则调用 addMessage
    if (isMe || m_lastAiItem == nullptr) {
        addMessage(textFragment, isMe);
        return;
    }

    bool autoScroll = isAtBottom();

    // 1. 找到最后一条 AI 消息的 QListWidgetItem
    QWidget *container = this->itemWidget(m_lastAiItem);
    if (!container) return;

    // 2. 从容器中找到 QTextBrowser
    QTextBrowser *browser = container->findChild<QTextBrowser*>();
    if (!browser) return;

    // 3. 追加文本
    m_lastAiMessage += textFragment;
    browser->setMarkdown(m_lastAiMessage); // 重新设置 *全部* Markdown 文本

    // 4. 滚动
    // (高度更新会由 contentsChanged 信号自动触发)
    if (autoScroll) {
        delayedScrollToBottom();
    }
}

// 新的槽函数：更新 item 高度
void ChatWidget::updateItemHeight(QListWidgetItem* item)
{
    if (!item) return;
    QWidget *container = this->itemWidget(item);
    if (!container) return;
    QTextBrowser *browser = container->findChild<QTextBrowser*>();
    if (!browser) return;

    // 1. 获取 ListWidget 的可用宽度 (viewport 宽度减去一点边距)
    int viewportWidth = this->viewport()->width() - 20;
    if (viewportWidth <= 0) return; // 窗口还没显示

    // 2. 定义气泡的最大宽度 (例如 70%)
    int maxWidth = viewportWidth * 0.7;

    // 3. 获取 document
    QTextDocument *doc = browser->document();

    // 4. *** 关键: 计算理想宽度 ***
    //    先设置 textWidth 为 -1 (无限宽), 获取内容的 "idealWidth"
    doc->setTextWidth(-1);
    int idealWidth = doc->idealWidth();

    // 5. 确定最终宽度
    //    取 "理想宽度" 和 "最大宽度" 中的较小值
    int finalWidth = qMin(idealWidth, maxWidth);

    // 6. 告诉 document 按这个最终宽度来换行
    doc->setTextWidth(finalWidth);
    QSize docSize = doc->size().toSize(); // 获取换行后的文档大小

    // 7. 获取 QSS 中设置的 padding (8px * 2)
    int hPadding = 16;
    int vPadding = 16;

    // 8. *** 关键: 固定 browser 的大小 ***
    //    这能解决 "用户消息过长" 和 "AI 消息过短" 的所有问题
    browser->setFixedSize(docSize.width() + hPadding, docSize.height() + vPadding);

    // 9. 设置 QListWidgetItem 的 SizeHint (高度是关键)
    item->setSizeHint(QSize(viewportWidth, docSize.height() + vPadding));
}


void ChatWidget::resizeEvent(QResizeEvent *event)
{
    // 1. 调用基类
    QListWidget::resizeEvent(event);

    // 2. 延迟更新所有 item 的高度
    //    使用 QTimer::singleShot(0, ...) 确保在布局都稳定后再执行
    QTimer::singleShot(0, this, [this](){
        for(int i = 0; i < this->count(); ++i) {
            QListWidgetItem *item = this->item(i);
            // 重新调用我们的计算函数
            updateItemHeight(item);
        }
    });
}

bool ChatWidget::isAtBottom() const
{
    QScrollBar *sb = verticalScrollBar();
    if (!sb) return false;
    return sb->value() == sb->maximum();
}

void ChatWidget::delayedScrollToBottom()
{
    // 使用 QTimer::singleShot 来延迟滚动
    // 0 毫秒意味着"尽快，但在处理完当前事件队列之后"
    QTimer::singleShot(0, this, [this](){
        QScrollBar *sb = verticalScrollBar();
        if (sb) {
            sb->setValue(sb->maximum());
        }
    });
}
