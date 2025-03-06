#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>
#include <QDialog>
#include "DraggableLabel.h"
#include "ChessEngine/RecursiveChessGame.hpp"

// Forward declaration of the CustomRecursiveChessGame class
class CustomRecursiveChessGame;

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
    void updateBoardFromGame();
    void forceUpdatePieceSizes();
    void initializeUI();
    void appendToLog(const QString &message);

    // Add a toggle for free move mode
    void toggleFreeMoveMode() { freeMoveMode = !freeMoveMode; }
    bool isFreeMoveMode() const { return freeMoveMode; }

private:
    Ui::MainWindow *ui;
    std::vector<QFrame *> allFrames;
    std::vector<DraggableLabel *> allLabels;
    // Use a pointer instead of a direct instance
    CustomRecursiveChessGame *cg;

    // Add free move mode flag
    bool freeMoveMode = false;

public slots:
    void handleDragStarted(DraggableLabel *source);
    void handleDragEntered(DraggableLabel *source);
    void handleDrop(DraggableLabel *source, DraggableLabel *target);
    void on_actionNew_Game_triggered();
};
#endif // MAINWINDOW_H
