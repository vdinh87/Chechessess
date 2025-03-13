#include "DraggableLabel.h"
#include <QDebug>
#include <QApplication>

// Constructor implementations
DraggableLabel::DraggableLabel() : QLabel(nullptr)
{
    setAcceptDrops(true);
    setAlignment(Qt::AlignCenter);
    setScaledContents(true);
    setContentsMargins(2, 2, 2, 2);
    mainWindow = nullptr;
    draggable = false;
    setObjectName("DraggableLabel"); // Default name for debugging
    qDebug() << "Created DraggableLabel with default constructor";
}

DraggableLabel::DraggableLabel(QWidget *parent) : QLabel(parent)
{
    setAcceptDrops(true);
    setAlignment(Qt::AlignCenter);
    setScaledContents(true);
    setContentsMargins(2, 2, 2, 2);
    mainWindow = nullptr;
    draggable = false;
    qDebug() << "Created DraggableLabel with parent:" << (parent ? parent->objectName() : "null");
}

void DraggableLabel::setDraggable(bool can_drag)
{
    draggable = can_drag;
    qDebug() << "Setting draggable to" << can_drag << "for" << objectName();
}

void DraggableLabel::setMainWindow(MainWindow *window)
{
    mainWindow = window;
    qDebug() << "MainWindow set for" << objectName();
}

void DraggableLabel::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "Mouse press event on label" << objectName()
             << "draggable:" << draggable
             << "has pixmap:" << !pixmap().isNull()
             << "at position:" << event->pos();

    if (!draggable)
    {
        qDebug() << "Ignoring mouse press on non-draggable label" << objectName();
        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        dragStartPosition = event->pos();
        originalStyle = this->styleSheet();
        qDebug() << "Left button pressed, stored drag start position" << dragStartPosition;
    }
}

void DraggableLabel::mouseMoveEvent(QMouseEvent *event)
{
    if (!draggable)
    {
        // Don't spam debug messages for every mouse move
        return;
    }

    if (!(event->buttons() & Qt::LeftButton))
    {
        return;
    }

    if (this->pixmap().isNull())
    {
        return;
    }

    int distance = (event->pos() - dragStartPosition).manhattanLength();

    if (distance < QApplication::startDragDistance())
    {
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

    // Use the actual chess piece pixmap for dragging
    QPixmap piecePixmap = this->pixmap();
    if (!piecePixmap.isNull())
    {
        drag->setPixmap(piecePixmap);
        // Center the piece under the cursor
        drag->setHotSpot(QPoint(piecePixmap.width() / 2, piecePixmap.height() / 2));
        qDebug() << "Using piece pixmap for drag:" << piecePixmap.size();
    }
    else
    {
        qDebug() << "WARNING: No pixmap available for drag";
    }

    qDebug() << "Executing drag operation for" << objectName();

    // Don't clear the source square yet - wait for successful drop
    Qt::DropAction dropAction = drag->exec();

    qDebug() << "Drag completed with action:" << dropAction;

    if (dropAction == Qt::IgnoreAction)
    {
        // If the drop was cancelled, make sure we keep the original piece
        qDebug() << "Drop ignored, restoring original style";
        this->setStyleSheet(originalStyle);

        // Also restore original margins if available
        QVariant originalMargins = this->property("originalMargins");
        if (originalMargins.isValid())
        {
            QMargins margins = originalMargins.value<QMargins>();
            this->setContentsMargins(margins);
        }

        // Reset any fixed size that might have been set
        this->setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    }
}

void DraggableLabel::dragEnterEvent(QDragEnterEvent *event)
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

void DraggableLabel::dropEvent(QDropEvent *event)
{
    qDebug() << "Drop event on" << objectName()
             << "formats:" << event->mimeData()->formats();

    if (event->mimeData()->hasFormat("application/x-style"))
    {
        QString style = QString::fromUtf8(event->mimeData()->data("application/x-style"));
        QString sourceSquare = QString::fromUtf8(event->mimeData()->data("application/x-source"));

        qDebug() << "Valid drop from" << sourceSquare << "to" << objectName();

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
