#ifndef DraggableLabel_H
#define DraggableLabel_H

#include <QLabel>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QApplication>
#include <vector>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>

// Forward declaration to avoid circular dependency
class MainWindow;

class DraggableLabel : public QLabel
{
    Q_OBJECT
public:
    DraggableLabel() : QLabel(nullptr)
    {
        setAcceptDrops(true);
        setAlignment(Qt::AlignCenter);
        setScaledContents(true);
        setContentsMargins(2, 2, 2, 2);
        mainWindow = nullptr;
        setObjectName("DraggableLabel"); // Default name for debugging
        qDebug() << "Created DraggableLabel with default constructor";
    }

    DraggableLabel(QWidget *parent = 0) : QLabel(parent)
    {
        setAcceptDrops(true);
        setAlignment(Qt::AlignCenter);
        setScaledContents(true);
        setContentsMargins(2, 2, 2, 2);
        mainWindow = nullptr;
        qDebug() << "Created DraggableLabel with parent:" << (parent ? parent->objectName() : "null");
    }

    void setDraggable(bool can_drag)
    {
        draggable = can_drag;
        qDebug() << "Setting draggable to" << can_drag << "for" << objectName();
    }

    void setMainWindow(MainWindow *window)
    {
        mainWindow = window;
        qDebug() << "MainWindow set for" << objectName();
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
    MainWindow *mainWindow = nullptr;

    void mousePressEvent(QMouseEvent *event) override
    {
        qDebug() << "Mouse press event on label" << objectName()
                 << "draggable:" << draggable
                 << "has pixmap:" << !pixmap().isNull()
                 << "at position:" << event->pos();

        if (!draggable)
            return;

        if (event->button() == Qt::LeftButton)
        {
            dragStartPosition = event->pos();
            originalStyle = this->styleSheet();
            qDebug() << "Left button pressed, stored drag start position" << dragStartPosition;
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        if (!draggable)
        {
            qDebug() << "Mouse move ignored - not draggable:" << objectName();
            return;
        }

        if (!(event->buttons() & Qt::LeftButton))
        {
            qDebug() << "Mouse move ignored - not left button:" << objectName();
            return;
        }

        if (this->pixmap().isNull())
        {
            qDebug() << "Mouse move ignored - no pixmap (no piece):" << objectName();
            return;
        }

        int distance = (event->pos() - dragStartPosition).manhattanLength();
        qDebug() << "Mouse move distance:" << distance << "Min required:" << QApplication::startDragDistance();

        if (distance < QApplication::startDragDistance())
        {
            qDebug() << "Mouse move ignored - not enough distance:" << objectName();
            return;
        }

        qDebug() << "Starting drag from" << objectName() << "with pixmap size:" << pixmap().size();
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

        qDebug() << "Executing drag operation with pixmap size:" << pixmap.size();

        // Don't clear the source square yet - wait for successful drop
        Qt::DropAction dropAction = drag->exec();

        qDebug() << "Drag completed with action:" << dropAction;

        if (dropAction == Qt::IgnoreAction)
        {
            // If the drop was cancelled, make sure we keep the original piece
            qDebug() << "Drop ignored, restoring original style";
            this->setStyleSheet(originalStyle);
        }
    }

    void dragEnterEvent(QDragEnterEvent *event) override
    {
        qDebug() << "Drag enter event on" << objectName()
                 << "formats:" << event->mimeData()->formats();

        if (event->mimeData()->hasFormat("application/x-style"))
        {
            qDebug() << "Valid mime data, accepting drag and emitting dragEntered signal";
            emit dragEntered(this);
            event->acceptProposedAction();
        }
        else
        {
            qDebug() << "Invalid mime data in dragEnterEvent, rejecting";
            event->ignore();
        }
    }

    void dropEvent(QDropEvent *event) override
    {
        qDebug() << "Drop event on" << objectName()
                 << "formats:" << event->mimeData()->formats();

        if (event->mimeData()->hasFormat("application/x-style"))
        {
            QString style = QString::fromUtf8(event->mimeData()->data("application/x-style"));
            QString sourceSquare = QString::fromUtf8(event->mimeData()->data("application/x-source"));

            qDebug() << "Valid drop from" << sourceSquare << "to" << objectName()
                     << "with style:" << style.left(30) + "...";

            // Accept the drop event
            event->acceptProposedAction();

            // Get the source from the parent window
            QWidget *topLevel = this->window();
            if (topLevel)
            {
                qDebug() << "Finding source label" << sourceSquare << "in window" << topLevel->objectName();
                DraggableLabel *sourceLabel = topLevel->findChild<DraggableLabel *>(sourceSquare);
                if (sourceLabel)
                {
                    qDebug() << "Found source label" << sourceLabel->objectName() << ", emitting dropOccurred signal";
                    emit dropOccurred(sourceLabel, this);
                }
                else
                {
                    qDebug() << "ERROR: Source label not found:" << sourceSquare;
                }
            }
            else
            {
                qDebug() << "ERROR: No top level window found";
            }
        }
        else
        {
            qDebug() << "Invalid mime data in dropEvent, rejecting";
            event->ignore();
        }
    }

signals:
    void dragStarted(DraggableLabel *source);
    void dragEntered(DraggableLabel *target);
    void dropOccurred(DraggableLabel *source, DraggableLabel *target);
};

#endif // DraggableLabel_H
