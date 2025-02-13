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
            originalStyle = this->styleSheet();
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (!draggable)
            return;
        if (!(event->buttons() & Qt::LeftButton))
            return;
        if (this->styleSheet().contains("blank.png"))
            return;

        if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
            return;

        emit dragStarted(this->objectName());

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        // Store both the style and the source object name
        mimeData->setData("application/x-style", originalStyle.toUtf8());
        mimeData->setData("application/x-source", objectName().toUtf8());
        drag->setMimeData(mimeData);

        QPixmap pixmap(this->size());
        this->render(&pixmap);
        drag->setPixmap(pixmap);

        // Don't clear the source square yet - wait for successful drop
        Qt::DropAction dropAction = drag->exec();

        if (dropAction == Qt::IgnoreAction)
        {
            // If the drop was cancelled, make sure we keep the original piece
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
            QString sourceSquare = QString::fromUtf8(event->mimeData()->data("application/x-source"));

            // Only update styles after confirming the move with the game engine
            event->acceptProposedAction();
            emit dropOccurred(this->objectName());
        }
    }

signals:
    void dragStarted(QString objectName);
    void dragEntered(QString objectName);
    void dropOccurred(QString targetSquare);
};

#endif // DraggableLabel_H
