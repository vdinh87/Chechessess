#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "ChessEngine/Definitions.hpp"
#include "DraggableLabel.h"
// #include "ChessEngine/ChessGame.cpp"

// ChessGame cg;
void updateLabelsFromBitboard(uint64_t bitboard, std::vector<DraggableLabel*>& draggableLabels);


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    std::vector<DraggableLabel*> draggableLabels;
    draggableLabels.insert(draggableLabels.end(), {
                                                      ui->a1, ui->b1, ui->c1, ui->d1, ui->e1, ui->f1, ui->g1, ui->h1,
                                                      ui->a2, ui->b2, ui->c2, ui->d2, ui->e2, ui->f2, ui->g2, ui->h2,
                                                      ui->a3, ui->b3, ui->c3, ui->d3, ui->e3, ui->f3, ui->g3, ui->h3,
                                                      ui->a4, ui->b4, ui->c4, ui->d4, ui->e4, ui->f4, ui->g4, ui->h4,
                                                      ui->a5, ui->b5, ui->c5, ui->d5, ui->e5, ui->f5, ui->g5, ui->h5,
                                                      ui->a6, ui->b6, ui->c6, ui->d6, ui->e6, ui->f6, ui->g6, ui->h6,
                                                      ui->a7, ui->b7, ui->c7, ui->d7, ui->e7, ui->f7, ui->g7, ui->h7,
                                                      ui->a8, ui->b8, ui->c8, ui->d8, ui->e8, ui->f8, ui->g8, ui->h8
                                                  });

    for (DraggableLabel* label : draggableLabels) {
        connect(label, &DraggableLabel::dragEntered, this, &MainWindow::handleDragEntered);
    }



    // updateLabelsFromBitboard(FILE_B, draggableLabels);

    //promote all of them
    //create vector of all of them by doing .append(ui-> label_18)
    draggableLabels[a1]->setDraggable(true);


    //labels
    // ui->label_54->setLabels()

}


void updateLabelsFromBitboard(uint64_t bitboard, std::vector<DraggableLabel*>& draggableLabels){
    for (int i = 0; i < 64; i++) {
        DraggableLabel *label = draggableLabels[i];

        bool bit = get_bit(bitboard, i);

        if (bit) {
            label->setStyleSheet("border-image: url(:/img/blank.png) 0 0 0 0 stretch stretch;\\n");
        }
    }
}


#include <QMessageBox>
/* Will Handle Dragging by getting attacks using the ObjectName
 * ObjectName is in the format of chess positions a1, a2, b2 etc, so we can use our hashmap of strings to squares.  */
void MainWindow::handleDragEntered(QString objectName){
    QMessageBox msgBox;
    msgBox.setText("The object name is " + objectName);
    msgBox.exec();


}


MainWindow::~MainWindow()
{
    delete ui;
}
