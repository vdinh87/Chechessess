#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>
#include "DraggableLabel.h"
#include "ChessEngine/ChessGame.hpp"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Piece handlePawnPromotion();

private:
    Ui::MainWindow *ui;
    // void updateLabelsFromBitboard(uint64_t bitboard, std::vector<DraggableLabel*>& draggableLabels);

public slots:
    void handleDragStarted(QString objectName);
    void handleDragEntered(QString objectName);
    void handleDrop(QString targetSquareName);
};
#endif // MAINWINDOW_H
