#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>
#include <QDialog>
#include "DraggableLabel.h"
#include "ChessEngine/RecursiveChessGame.hpp"

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
    void showGameOver(bool isWhiteWinner);

private:
    Ui::MainWindow *ui;
    void updateBoardFromGame(); // Helper to update the visual board state

public slots:
    void handleDragStarted(QString objectName);
    void handleDragEntered(QString objectName);
    void handleDrop(QString targetSquareName);
};
#endif // MAINWINDOW_H
