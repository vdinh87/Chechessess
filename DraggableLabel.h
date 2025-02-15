#ifndef DraggableLabel_H
#define DraggableLabel_H

#include <QLabel>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QApplication>

class DraggableLabel : public QLabel
{
    Q_OBJECT
public:
    DraggableLabel();
    DraggableLabel(QWidget *parent = 0) : QLabel(parent)
    {
        setAcceptDrops(true);
    }
    void setDraggable(bool can_drag)
    {
        draggable = can_drag;
    }
    void updateLabelsFromBitboard(uint64_t bitboard, std::vector<DraggableLabel *> &draggableLabels)
    {
        for (int i = 0; i < 64; i++)
        {
            DraggableLabel *label = draggableLabels[i];
            bool bit = (bitboard & (1ULL << i));
            if (bit)
            {
                label->setStyleSheet("border: 1px solid black;");
            }
        }
    }

protected:
    bool draggable = false;
    QPoint dragStartPosition;
    QString originalStyle;

    void mousePressEvent(QMouseEvent *event) override
    {
        if (!draggable)
            return;
        if (event->button() == Qt::LeftButton)
        {
            dragStartPosition = event->pos();
            originalStyle = this->styleSheet(); // Store the original style
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (!draggable)
            return;
        if (!(event->buttons() & Qt::LeftButton))
            return;

        // Don't start drag if the square is empty (has blank image)
        if (this->styleSheet().contains("blank.png"))
            return;

        if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
            return;

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        // Store both the style and the source object name
        mimeData->setData("application/x-style", originalStyle.toUtf8());
        mimeData->setData("application/x-source", objectName().toUtf8());
        drag->setMimeData(mimeData);

        QPixmap pixmap(this->size());
        this->render(&pixmap);
        drag->setPixmap(pixmap);

        // Clear the source square immediately
        this->setStyleSheet("border-image: url(:/img/blank.png) 0 0 0 0 stretch stretch;");

        Qt::DropAction dropAction = drag->exec();

        if (dropAction == Qt::IgnoreAction || drag->target() == this)
        {
            // If the drop was cancelled or invalid, restore the original piece
            this->setStyleSheet(originalStyle);
        }
    }

    void dragEnterEvent(QDragEnterEvent *event) override
    {
        if (event->mimeData()->hasFormat("application/x-style"))
        {
            emit dragEntered(this->objectName());
            event->acceptProposedAction();
        }
    }

    void dropEvent(QDropEvent *event) override
    {
        if (event->mimeData()->hasFormat("application/x-style"))
        {
            QString style = QString::fromUtf8(event->mimeData()->data("application/x-style"));
            this->setStyleSheet(style);
            event->acceptProposedAction();
        }
    }

signals:
    void dragEntered(QString objectName);
};

#endif // DraggableLabel_H
