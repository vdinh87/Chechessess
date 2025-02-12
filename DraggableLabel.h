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

    void mousePressEvent(QMouseEvent *event) override
    {
        if (!draggable)
            return;
        if (event->button() == Qt::LeftButton)
        {
            dragStartPosition = event->pos();
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (!draggable)
            return;
        if (!(event->buttons() & Qt::LeftButton))
            return;

        if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
            return;

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData;

        // Store the original style and position
        mimeData->setText(this->styleSheet());
        drag->setMimeData(mimeData);

        QPixmap pixmap(this->size());
        this->render(&pixmap);
        drag->setPixmap(pixmap);

        Qt::DropAction dropAction = drag->exec();

        if (dropAction != Qt::IgnoreAction && drag->target() != this)
        {
            this->setStyleSheet("border-image: url(:/img/blank.png) 0 0 0 0 stretch stretch;\\n");
            this->draggable = false;
        }
    }

    void dragEnterEvent(QDragEnterEvent *event) override
    {
        if (event->mimeData()->hasText())
        {
            emit dragEntered(this->objectName());
            event->acceptProposedAction();
        }
    }

    void dropEvent(QDropEvent *event) override
    {
        if (event->source() != this)
        {
            this->setStyleSheet(event->mimeData()->text());
        }
    }

signals:
    void dragEntered(QString objectName);
};

#endif // DraggableLabel_H
