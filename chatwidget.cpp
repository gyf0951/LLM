#include "ChatWidget.h"
#include <QPainter>
#include <QFontMetrics>
#include <QScrollBar>

ChatDelegate::ChatDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

void ChatDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                         const QModelIndex &index) const
{
    painter->save();

    QString text = index.data(Qt::DisplayRole).toString();
    bool isMe    = index.data(Qt::UserRole).toBool();

    QFontMetrics fm(option.font);
    // 列表项的可用宽度 (减去一点边距)
    int itemWidth = option.rect.width() - 20;
    // 气泡的最大宽度 (例如70%)
    int maxWidth = itemWidth * 0.7;

    // <-- FIX 1: 使用 QRect(..., 0) 来表示无限高度，而不是 1000
    QRect textRect = fm.boundingRect(QRect(0, 0, maxWidth, 0),
                                     Qt::TextWordWrap, text);

    if (isMe) {
        // 用户发送的消息
        int bubbleWidth  = textRect.width() + 2 * m_paddingH;
        int bubbleHeight = textRect.height() + 2 * m_paddingV;

        // 应用 m_marginV
        QPoint bubbleTopRight = option.rect.topRight() - QPoint(10, 0);
        bubbleTopRight.setY(bubbleTopRight.y() + m_marginV); // <-- FIX 2

        QRect bubbleRect(0, 0, bubbleWidth, bubbleHeight);
        bubbleRect.moveTopRight(bubbleTopRight);

        // 绘制气泡
        QColor bubbleColor("#95EC69");
        painter->setBrush(bubbleColor);
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(bubbleRect, 10, 10);

        // 绘制文字（居中在气泡里）
        int offsetY = (bubbleRect.height() - textRect.height()) / 2;
        QRect textArea(bubbleRect.left() + m_paddingH,
                       bubbleRect.top() + offsetY,
                       textRect.width(),
                       textRect.height());

        painter->setPen(Qt::black);
        painter->drawText(textArea, Qt::TextWordWrap | Qt::AlignLeft, text);

    } else {
        // AI 返回的消息
        // <-- FIX 2: 应用 m_marginV
        QRect textArea(option.rect.left() + 10,
                       option.rect.top() + m_marginV,
                       textRect.width(),
                       textRect.height());

        painter->setPen(Qt::black);
        painter->drawText(textArea, Qt::TextWordWrap | Qt::AlignLeft, text);
    }

    painter->restore();
}

QSize ChatDelegate::sizeHint(const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    QString text = index.data(Qt::DisplayRole).toString();
    QFontMetrics fm(option.font);

    int itemWidth = option.rect.width() - 20;
    int maxWidth = itemWidth * 0.7;

    // <-- FIX 1: 使用 QRect(..., 0) 来表示无限高度
    QRect textRect = fm.boundingRect(QRect(0, 0, maxWidth, 0),
                                     Qt::TextWordWrap, text);

    int finalHeight;
    bool isMe = index.data(Qt::UserRole).toBool();

    if (isMe) {
        // 气泡高度 = 文字高度 + 上下内边距
        finalHeight = textRect.height() + 2 * m_paddingV;
    } else {
        // 纯文本高度
        finalHeight = textRect.height();
    }

    // <-- FIX 3: 最终高度 = 内容高度 + 上下外边距
    finalHeight += (2 * m_marginV);

    // 返回 QListWidget 应该分配的完整大小
    return QSize(option.rect.width(), finalHeight);
}


ChatWidget::ChatWidget(QWidget *parent)
    : QListWidget(parent)
{
    // 样式表可以简化，背景色等在父窗口设置
    setStyleSheet("QListWidget { border: none; }");
    setItemDelegate(new ChatDelegate(this));
    setSelectionMode(QAbstractItemView::NoSelection);
    setFocusPolicy(Qt::NoFocus);

    // <-- FIX 4: 确保内容变化时自动调整大小
    setResizeMode(QListWidget::Adjust);
}

void ChatWidget::addMessage(const QString &text, bool isMe)
{
    bool autoScroll = isAtBottom();

    QListWidgetItem *item = new QListWidgetItem();
    item->setData(Qt::DisplayRole, text);
    item->setData(Qt::UserRole, isMe);  // 用来区分是我还是对方
    addItem(item);



    if (autoScroll) {
        delayedScrollToBottom(); // <-- 使用延迟滚动
    }

}

void ChatWidget::appendToLastMessage(const QString &textFragment, bool isMe)
{
    bool autoScroll = isAtBottom();

    QListWidgetItem *lastItem = (count() > 0) ? item(count() - 1) : nullptr;

    // 检查最后一条消息是否存在，并且发送者是 AI (isMe == false)
    if (lastItem && lastItem->data(Qt::UserRole).toBool() == isMe)
    {
        // 1. 追加文本
        QString currentText = lastItem->text();
        currentText += textFragment;
        // 使用 setData 而不是 setText，确保模型知道数据已更改
        lastItem->setData(Qt::DisplayRole, currentText);

        // 2. *** 这是解决“半行字”问题的关键 ***
        //    强制使 QListWidget 丢弃这个 item 的尺寸缓存
        //    这样它就会被迫下次重绘时调用 delegate 的 sizeHint()
        lastItem->setSizeHint(QSize(-1, -1));

        // (可选) 确保该项在视图中是更新的
        // update(indexFromItem(lastItem));
    }
    else
    {
        // 如果没有上一条，或者上一条是用户发的，则新建一个 item
        addMessage(textFragment, isMe);
        return;
    }

    if (autoScroll) {
        delayedScrollToBottom(); // <-- 使用延迟滚动
    }
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
