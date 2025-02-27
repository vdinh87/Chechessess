#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "ChessEngine/Definitions.hpp"
#include "DraggableLabel.h"
#include "ChessEngine/RecursiveChessGame.hpp"
#include "ChessEngine/Magics.hpp"
#include <QMap>
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QGraphicsBlurEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QGraphicsOpacityEffect>
#include <QPointer>

class CustomRecursiveChessGame : public RecursiveChessGame
{
private:
    MainWindow *mainWindow;
    bool inSubGame;
    std::shared_ptr<CustomRecursiveChessGame> activeSubGame;
    std::vector<U64> savedWhitePieces;
    std::vector<U64> savedBlackPieces;

public:
    CustomRecursiveChessGame(MainWindow *window = nullptr)
        : RecursiveChessGame(), mainWindow(window), inSubGame(false), activeSubGame(nullptr) {}

    Piece PromoteInput(Square from_sq, Square to_sq, Color color, Piece to_piece) override
    {
        if (mainWindow)
        {
            return mainWindow->handlePawnPromotion();
        }
        return Queen; // Default if no window is set
    }

    bool IsInSubGame() const { return inSubGame; }

    CustomRecursiveChessGame *GetActiveGame()
    {
        return inSubGame && activeSubGame ? activeSubGame.get() : this;
    }

    void StartSubGame(Square from, Square to, Color attacker)
    {
        // Save current game state
        savedWhitePieces = WhitePiecesArray;
        savedBlackPieces = BlackPiecesArray;

        // Create new sub-game
        activeSubGame = std::make_shared<CustomRecursiveChessGame>(mainWindow);
        activeSubGame->parent_game = this;
        activeSubGame->capture_from = from;
        activeSubGame->capture_to = to;
        activeSubGame->attacker_color = attacker;
        inSubGame = true;
    }

    void EndSubGame()
    {
        if (inSubGame && activeSubGame)
        {
            // Check if there was a winner and handle piece removal
            Color winner = activeSubGame->GetWinner();
            if (winner != static_cast<Color>(-1)) // If there was a winner
            {
                if (winner == activeSubGame->attacker_color)
                {
                    // Attacker won - remove defending piece and move attacking piece
                    RemovePiece(activeSubGame->capture_to);
                    ExecuteMove(activeSubGame->attacker_color,
                                activeSubGame->capture_from,
                                activeSubGame->capture_to,
                                GetPieceType(1ULL << activeSubGame->capture_from),
                                GetPieceType(1ULL << activeSubGame->capture_to));
                }
                else
                {
                    // Defender won - remove attacking piece
                    RemovePiece(activeSubGame->capture_from);
                }
            }

            // Clean up sub-game
            activeSubGame = nullptr;
            inSubGame = false;
            UpdateBoard();
        }
    }
};

static CustomRecursiveChessGame cg(nullptr);    // Make it static to avoid multiple definition
static std::vector<DraggableLabel *> allLabels; // Make it static
static Square dragSourceSquare = invalid;       // Make it static
static QDialog *subGameDialog = nullptr;        // Dialog for sub-games

void updateLabelsFromBitboard(uint64_t bitboard, std::vector<DraggableLabel *> &draggableLabels);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cg = CustomRecursiveChessGame(this); // Initialize with this window

    allLabels = {ui->a1, ui->b1, ui->c1, ui->d1, ui->e1, ui->f1, ui->g1, ui->h1,
                 ui->a2, ui->b2, ui->c2, ui->d2, ui->e2, ui->f2, ui->g2, ui->h2,
                 ui->a3, ui->b3, ui->c3, ui->d3, ui->e3, ui->f3, ui->g3, ui->h3,
                 ui->a4, ui->b4, ui->c4, ui->d4, ui->e4, ui->f4, ui->g4, ui->h4,
                 ui->a5, ui->b5, ui->c5, ui->d5, ui->e5, ui->f5, ui->g5, ui->h5,
                 ui->a6, ui->b6, ui->c6, ui->d6, ui->e6, ui->f6, ui->g6, ui->h6,
                 ui->a7, ui->b7, ui->c7, ui->d7, ui->e7, ui->f7, ui->g7, ui->h7,
                 ui->a8, ui->b8, ui->c8, ui->d8, ui->e8, ui->f8, ui->g8, ui->h8};

    for (DraggableLabel *label : allLabels)
    {
        connect(label, &DraggableLabel::dragStarted, this, &MainWindow::handleDragStarted);
        connect(label, &DraggableLabel::dragEntered, this, &MainWindow::handleDragEntered);
        connect(label, &DraggableLabel::dropOccurred, this, &MainWindow::handleDrop);
        label->setDraggable(true);
    }
}

void updateLabelsFromBitboard(uint64_t bitboard, std::vector<DraggableLabel *> &draggableLabels)
{
    if (draggableLabels.empty())
    {
        return;
    }

    for (int i = 0; i < 64 && i < draggableLabels.size(); i++)
    {
        DraggableLabel *label = draggableLabels[i];
        if (!label)
            continue;

        bool bit = get_bit(bitboard, i);
        if (bit)
        {
            // Store the original style if it's not already stored
            QString currentStyle = label->styleSheet();
            if (!currentStyle.contains("background-color"))
            {
                label->setProperty("originalStyle", currentStyle);
            }

            // Apply the original piece image with a semi-transparent green overlay
            QString newStyle = label->property("originalStyle").toString();
            if (newStyle.isEmpty() || newStyle.contains("blank.png"))
            {
                newStyle = "border-image: url(:/img/blank.png) 0 0 0 0 stretch stretch;";
            }
            newStyle += " background-color: rgba(0, 255, 0, 0.3);"; // Semi-transparent green
            label->setStyleSheet(newStyle);
        }
        else if (label->styleSheet().contains("background-color"))
        {
            // Restore the original style without the green overlay
            QVariant originalStyle = label->property("originalStyle");
            if (originalStyle.isValid())
            {
                label->setStyleSheet(originalStyle.toString());
            }
            else
            {
                label->setStyleSheet("border-image: url(:/img/blank.png) 0 0 0 0 stretch stretch;");
            }
        }
    }
}

void MainWindow::handleDragStarted(QString objectName)
{
    // Clear any existing highlights and restore original styles
    for (DraggableLabel *label : allLabels)
    {
        if (label->styleSheet().contains("background-color"))
        {
            QVariant originalStyle = label->property("originalStyle");
            if (originalStyle.isValid())
            {
                label->setStyleSheet(originalStyle.toString());
            }
            else
            {
                label->setStyleSheet("border-image: url(:/img/blank.png) 0 0 0 0 stretch stretch;");
            }
        }
    }

    // Convert chess notation to square index
    int file = objectName[0].toLatin1() - 'a';
    int rank = objectName[1].digitValue() - 1;
    dragSourceSquare = static_cast<Square>(rank * 8 + file);

    // Get and show valid moves for this piece using the active game
    CustomRecursiveChessGame *activeGame = cg.GetActiveGame();
    if (activeGame)
    {
        try
        {
            U64 validMoves = activeGame->GetAttacks(dragSourceSquare);
            updateLabelsFromBitboard(validMoves, allLabels);
        }
        catch (const std::exception &e)
        {
            // If there's no piece at the square, just clear the dragSourceSquare
            dragSourceSquare = invalid;
        }
    }

    // Log the drag start
    ui->textEdit->append(QString("Selected piece at %1").arg(objectName));
}

void MainWindow::handleDragEntered(QString objectName)
{
    // Do nothing during drag - we keep the original valid moves highlighted
}

void MainWindow::handleDrop(QString targetSquareName)
{
    // Convert target square name to index
    int file = targetSquareName[0].toLatin1() - 'a';
    int rank = targetSquareName[1].digitValue() - 1;
    Square targetSquare = static_cast<Square>(rank * 8 + file);

    // Try to make the move
    if (dragSourceSquare != invalid)
    {
        CustomRecursiveChessGame *activeGame = cg.GetActiveGame();
        std::vector<Action> actions = activeGame->Move(dragSourceSquare, targetSquare);

        // If the move was successful (actions is not empty)
        if (!actions.empty())
        {
            // Check if this move triggers a sub-game
            if (std::find(actions.begin(), actions.end(), Action::Abilityes) != actions.end())
            {
                // Find the chessboard widget (it's the first QWidget child that contains our chess squares)
                QWidget *chessboard = nullptr;
                for (QObject *child : ui->centralwidget->children())
                {
                    if (QWidget *widget = qobject_cast<QWidget *>(child))
                    {
                        if (widget->findChild<DraggableLabel *>())
                        {
                            chessboard = widget;
                            break;
                        }
                    }
                }

                if (!chessboard)
                    return; // Safety check

                // Raise the chessboard to be on top of all other widgets
                chessboard->raise();

                // Create and setup the blur effect for the entire central widget
                QPointer<QGraphicsBlurEffect> blurEffect = new QGraphicsBlurEffect(ui->centralwidget);
                blurEffect->setBlurRadius(0);
                ui->centralwidget->setGraphicsEffect(blurEffect);

                // Store the initial geometry of the chessboard
                QRect originalGeom = chessboard->geometry();
                QPoint globalCenter = chessboard->mapToGlobal(chessboard->rect().center());
                QPoint localCenter = ui->centralwidget->mapFromGlobal(globalCenter);

                // Create animation group with safe pointer tracking
                QPointer<QParallelAnimationGroup> animGroup = new QParallelAnimationGroup(this);

                // Create blur animation for the entire UI
                QPropertyAnimation *blurAnimation = new QPropertyAnimation(blurEffect, "blurRadius", animGroup);
                blurAnimation->setDuration(1000);
                blurAnimation->setStartValue(0);
                blurAnimation->setEndValue(15);
                blurAnimation->setEasingCurve(QEasingCurve::InOutQuad);
                animGroup->addAnimation(blurAnimation);

                // Create zoom animation for the chessboard only
                QPropertyAnimation *scaleAnimation = new QPropertyAnimation(chessboard, "geometry", animGroup);
                scaleAnimation->setDuration(1000);
                QRect endGeom = originalGeom;

                // Calculate zoom with proper centering on the chessboard
                int newWidth = static_cast<int>(originalGeom.width() * 2.0);
                int newHeight = static_cast<int>(originalGeom.height() * 2.0);
                endGeom.setWidth(newWidth);
                endGeom.setHeight(newHeight);
                endGeom.moveCenter(localCenter);

                scaleAnimation->setStartValue(originalGeom);
                scaleAnimation->setEndValue(endGeom);
                scaleAnimation->setEasingCurve(QEasingCurve::InOutQuad);
                animGroup->addAnimation(scaleAnimation);

                // Capture necessary variables
                Square captureFrom = dragSourceSquare;
                Square captureTo = targetSquare;
                Color attackerColor = activeGame->GetColor(1ULL << dragSourceSquare);

                // Connect animation finished signal
                connect(animGroup, &QParallelAnimationGroup::finished, this,
                        [this, blurEffect, captureFrom, captureTo, attackerColor, originalGeom, chessboard]()
                        {
                            if (!chessboard)
                                return;

                            // Create dialog for the sub-game
                            QDialog *dialog = new QDialog(this);
                            dialog->setWindowTitle("Capture Resolution Game");
                            dialog->setModal(true);

                            QVBoxLayout *layout = new QVBoxLayout(dialog);
                            QLabel *infoLabel = new QLabel(
                                "A capture has been attempted! Play this game to determine the outcome.\n"
                                "If the attacker wins, the capture succeeds.\n"
                                "If the defender wins, the attacking piece is removed instead.",
                                dialog);
                            layout->addWidget(infoLabel);
                            dialog->setLayout(layout);

                            if (subGameDialog)
                            {
                                subGameDialog->deleteLater();
                            }
                            subGameDialog = dialog;

                            // Connect dialog's finished signal
                            connect(dialog, &QDialog::finished, this, [this, blurEffect, originalGeom, chessboard]()
                                    {
                                if (!chessboard) return;

                                // Lower the chessboard back to its original stack position
                                chessboard->lower();

                                // Create reverse animation group with safe pointer tracking
                                QPointer<QParallelAnimationGroup> reverseGroup = new QParallelAnimationGroup(this);

                                // Create reverse blur animation for the entire UI
                                QPropertyAnimation *reverseBlur = new QPropertyAnimation(blurEffect, "blurRadius", reverseGroup);
                                reverseBlur->setDuration(1000);
                                reverseBlur->setStartValue(15);
                                reverseBlur->setEndValue(0);
                                reverseBlur->setEasingCurve(QEasingCurve::InOutQuad);
                                reverseGroup->addAnimation(reverseBlur);

                                // Create reverse scale animation for the chessboard
                                QPropertyAnimation *reverseScale = new QPropertyAnimation(chessboard, "geometry", reverseGroup);
                                reverseScale->setDuration(1000);
                                reverseScale->setStartValue(chessboard->geometry());
                                reverseScale->setEndValue(originalGeom);
                                reverseScale->setEasingCurve(QEasingCurve::InOutQuad);
                                reverseGroup->addAnimation(reverseScale);

                                // Cleanup after reverse animation finishes
                                connect(reverseGroup, &QParallelAnimationGroup::finished, this, [this, blurEffect, reverseGroup]() {
                                    if (blurEffect) {
                                        ui->centralwidget->setGraphicsEffect(nullptr);
                                        blurEffect->deleteLater();
                                    }
                                    if (reverseGroup) {
                                        reverseGroup->deleteLater();
                                    }
                                    updateBoardFromGame();
                                });

                                reverseGroup->start();

                                if (subGameDialog) {
                                    subGameDialog->deleteLater();
                                    subGameDialog = nullptr;
                                } });

                            // Start the sub-game
                            cg.StartSubGame(captureFrom, captureTo, attackerColor);
                            updateBoardFromGame();
                            dialog->show();
                        });

                // Start the animations
                animGroup->start();
                return;
            }

            // Log the move and any special actions
            QString moveLog = QString("Move: %1 -> %2").arg(SquareStrings[dragSourceSquare].c_str()).arg(targetSquareName);
            if (!actions.empty())
            {
                moveLog += " (";
                for (size_t i = 0; i < actions.size(); ++i)
                {
                    if (i > 0)
                        moveLog += ", ";
                    moveLog += ActionStrings[actions[i]].c_str();
                }
                moveLog += ")";
            }
            ui->textEdit->append(moveLog);

            // Check for checkmate
            if (std::find(actions.begin(), actions.end(), Action::Checkmate) != actions.end())
            {
                if (!cg.IsInSubGame())
                {
                    // Main game ended
                    QString winner = activeGame->GetWinner() == white ? "White" : "Black";
                    ui->textEdit->append(QString("%1 has checkmated %2! Game Over!").arg(winner).arg(winner == "White" ? "Black" : "White"));
                    QMessageBox::information(this, "Game Over",
                                             QString("%1 Wins!").arg(winner));
                }
                else
                {
                    // Sub-game ended
                    QString result = activeGame->GetWinner() == activeGame->GetColor(1ULL << dragSourceSquare) ? "Capture succeeds!" : "Capture fails! Attacking piece is removed.";
                    ui->textEdit->append(result);

                    // Close the sub-game dialog safely
                    if (subGameDialog)
                    {
                        subGameDialog->accept();
                        subGameDialog = nullptr;
                    }

                    // End the sub-game and force a board update
                    cg.EndSubGame();
                }
            }
            else if (!cg.IsInSubGame() && activeGame->IsWin(white))
            {
                ui->textEdit->append("White has checkmated Black! Game Over!");
                QMessageBox::information(this, "Game Over", "White Wins!");
            }
            else if (!cg.IsInSubGame() && activeGame->IsWin(black))
            {
                ui->textEdit->append("Black has checkmated White! Game Over!");
                QMessageBox::information(this, "Game Over", "Black Wins!");
            }

            // Update the visual board state
            updateBoardFromGame();
        }

        // Clear highlights
        for (DraggableLabel *label : allLabels)
        {
            if (label->styleSheet().contains("background-color"))
            {
                QVariant originalStyle = label->property("originalStyle");
                if (originalStyle.isValid())
                {
                    label->setStyleSheet(originalStyle.toString());
                }
            }
        }
        dragSourceSquare = invalid;
    }
}

Piece MainWindow::handlePawnPromotion()
{
    QStringList items;
    items << "Queen" << "Rook" << "Bishop" << "Knight";

    bool ok;
    QString item = QInputDialog::getItem(this, "Pawn Promotion",
                                         "Choose piece to promote to:", items, 0, false, &ok);

    if (ok && !item.isEmpty())
    {
        if (item == "Queen")
            return Queen;
        if (item == "Rook")
            return Rook;
        if (item == "Bishop")
            return Bishop;
        if (item == "Knight")
            return Knight;
    }

    return Queen; // Default to Queen if dialog is cancelled
}

void MainWindow::updateBoardFromGame()
{
    // Get the active game instance
    CustomRecursiveChessGame *activeGame = cg.GetActiveGame();

    // Update each square based on the current game state
    for (int i = 0; i < 64; i++)
    {
        Square sq = static_cast<Square>(i);
        DraggableLabel *label = allLabels[i];
        U64 piece = 1ULL << i;

        if (!(piece & activeGame->GetBoard()))
        {
            // Empty square
            label->setStyleSheet("border-image: url(:/img/blank.png) 0 0 0 0 stretch stretch;");
            label->setProperty("originalStyle", QVariant());
            continue;
        }

        // Get piece type and color
        Color color = activeGame->GetColor(piece);
        Piece pieceType = activeGame->GetPieceType(piece);

        // Construct image path
        QString pieceName;
        switch (pieceType)
        {
        case Pawn:
            pieceName = "pawn";
            break;
        case Knight:
            pieceName = "knight";
            break;
        case Bishop:
            pieceName = "bishop";
            break;
        case Rook:
            pieceName = "rook";
            break;
        case Queen:
            pieceName = "queen";
            break;
        case King:
            pieceName = "king";
            break;
        default:
            continue;
        }

        QString colorName = (color == white) ? "white" : "black";
        QString imagePath = QString(":/img/%1_%2.png").arg(colorName).arg(pieceName);
        QString newStyle = QString("border-image: url(%1) 0 0 0 0 stretch stretch;").arg(imagePath);

        label->setStyleSheet(newStyle);
        label->setProperty("originalStyle", newStyle);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
