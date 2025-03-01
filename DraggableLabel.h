#ifndef DraggableLabel_H
#define DraggableLabel_H

#include <QLabel>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QApplication>
#include <vector>

class DraggableLabel : public QLabel
{
    Q_OBJECT
public:
    DraggableLabel();
    DraggableLabel(QWidget *parent = 0) : QLabel(parent)
    {
        setAcceptDrops(true);
        setAlignment(Qt::AlignCenter);
        setScaledContents(true);
        setContentsMargins(2, 2, 2, 2);
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
                // Save current style
                QString currentStyle = label->styleSheet();
                if (!currentStyle.contains("background-color"))
                {
                    label->setProperty("originalStyle", currentStyle);
                }

                // Add highlight
                currentStyle += " background-color: rgba(0, 255, 0, 0.3);";
                label->setStyleSheet(currentStyle);
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
        if (this->pixmap().isNull())
            return;

        if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
            return;

        emit dragStarted(this);

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        // Store both the style and the source pointer
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
            emit dragEntered(this);
            event->acceptProposedAction();
        }
    }

    void dropEvent(QDropEvent *event) override
    {
        if (event->mimeData()->hasFormat("application/x-style"))
        {
            QString style = QString::fromUtf8(event->mimeData()->data("application/x-style"));
            QString sourceSquare = QString::fromUtf8(event->mimeData()->data("application/x-source"));

            // Get the source label from the source square name
            // Only update styles after confirming the move with the game engine
            event->acceptProposedAction();

            // Get the source from the parent window
            QWidget *topLevel = this->window();
            if (topLevel)
            {
                DraggableLabel *sourceLabel = topLevel->findChild<DraggableLabel *>(sourceSquare);
                emit dropOccurred(sourceLabel, this);
            }
        }
    }

signals:
    void dragStarted(DraggableLabel *source);
    void dragEntered(DraggableLabel *target);
    void dropOccurred(DraggableLabel *source, DraggableLabel *target);
};

#endif // DraggableLabel_H
