#ifndef DRAG_H
#define DRAG_H

#include <QLabel>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

class Drag : public QLabel {
public:
    Drag();
    Drag(QWidget *parent = 0) : QLabel(parent) {
        setAcceptDrops(true);
    }

protected:
    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            QDrag *drag = new QDrag(this);
            QMimeData *mimeData = new QMimeData;

            // Here we could add any data we want to the mime data object
            mimeData->setText(this->text());
            drag->setMimeData(mimeData);

            QPixmap pixmap(this->size());
            this->render(&pixmap);
            drag->setPixmap(pixmap);

            drag->exec();
        }
    }

    void dragEnterEvent(QDragEnterEvent *event) override {
        if (event->mimeData()->hasText()) {
            event->acceptProposedAction();
        }
    }

    void dropEvent(QDropEvent *event) override {
        this->setText(event->mimeData()->text());
    }
};


#endif // DRAG_H
