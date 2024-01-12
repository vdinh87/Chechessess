#ifndef DraggableLabel_H
#define DraggableLabel_H

#include <QLabel>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

class DraggableLabel : public QLabel {
public:
    DraggableLabel();
    DraggableLabel(QWidget *parent = 0) : QLabel(parent) {
        setAcceptDrops(true);
    }

    void setLabels(QList<QLabel*>& labels) {
        this->labels_attack_locations = labels;
    }



protected:
    QList<QLabel*> labels_attack_locations;


    void mousePressEvent(QMouseEvent *event) override {


        if (event->button() == Qt::LeftButton) {
            QDrag *Drag = new QDrag(this);
            QMimeData *mimeData = new QMimeData;

            //we would probably make stuff green here.

            // Here we could add any data we want to the mime data object
            mimeData->setText(this->styleSheet());
            Drag->setMimeData(mimeData);

            QPixmap pixmap(this->size());
            this->render(&pixmap);
            Drag->setPixmap(pixmap);

            Drag->exec();
        }
        this->setStyleSheet("    border-image: url(:/img/blank.png) 0 0 0 0 stretch stretch;\\n");
    }

    void dragEnterEvent(QDragEnterEvent *event) override {
        if (event->mimeData()->hasText()) {
            event->acceptProposedAction();
        }
    }

    void dropEvent(QDropEvent *event) override {
        this->setStyleSheet(event->mimeData()->text());
    }
};


#endif // DraggableLabel_H
