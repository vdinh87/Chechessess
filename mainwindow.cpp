#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "ChessEngine/Definitions.hpp"
#include "DraggableLabel.h"
#include "ChessEngine/ChessGame.hpp"
#include "ChessEngine/Magics.hpp"
#include <QMap>

class CustomChessGame : public ChessGame
{
private:
    MainWindow *mainWindow;

public:
    CustomChessGame(MainWindow *window = nullptr) : ChessGame(), mainWindow(window) {}

    Piece PromoteInput(Square from_sq, Square to_sq, Color color, Piece to_piece) override
    {
        if (mainWindow)
        {
            return mainWindow->handlePawnPromotion();
        }
        return Queen; // Default if no window is set
    }
};

static CustomChessGame cg(nullptr);             // Make it static to avoid multiple definition
static std::vector<DraggableLabel *> allLabels; // Make it static
static Square dragSourceSquare = invalid;       // Make it static

void updateLabelsFromBitboard(uint64_t bitboard, std::vector<DraggableLabel *> &draggableLabels);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cg = CustomChessGame(this); // Initialize with this window

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
    for (int i = 0; i < 64; i++)
    {
        DraggableLabel *label = draggableLabels[i];
        bool bit = get_bit(bitboard, i);
        if (bit)
        {
            // If there's a piece on this square, preserve its style and add green highlight
            QString currentStyle = label->styleSheet();
            if (!currentStyle.contains("blank.png") && !currentStyle.contains("green_hue.png"))
            {
                // Store the original piece style
                label->setProperty("originalStyle", currentStyle);
            }
            label->setStyleSheet("border-image: url(:/img/green_hue.png) 0 0 0 0 stretch stretch;");
        }
        else if (label->styleSheet().contains("green_hue.png"))
        {
            // Restore the original style if it exists, otherwise set to blank
            QVariant originalStyle = label->property("originalStyle");
            if (originalStyle.isValid())
            {
                label->setStyleSheet(originalStyle.toString());
                label->setProperty("originalStyle", QVariant()); // Clear the stored style
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
        if (label->styleSheet().contains("green_hue.png"))
        {
            QVariant originalStyle = label->property("originalStyle");
            if (originalStyle.isValid())
            {
                label->setStyleSheet(originalStyle.toString());
                label->setProperty("originalStyle", QVariant());
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

    // Get and show valid moves for this piece
    U64 validMoves = cg.GetAttacks(dragSourceSquare);
    updateLabelsFromBitboard(validMoves, allLabels);

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
        std::vector<Action> actions = cg.Move(dragSourceSquare, targetSquare);

        // If the move was successful (actions is not empty)
        if (!actions.empty())
        {
            // Get the source and target labels
            DraggableLabel *sourceLabel = allLabels[dragSourceSquare];
            DraggableLabel *targetLabel = allLabels[targetSquare];

            // Store the original piece style before the move
            QString originalPieceStyle = sourceLabel->property("originalStyle").toString();
            if (originalPieceStyle.isEmpty())
            {
                originalPieceStyle = sourceLabel->styleSheet();
            }

            // Check if this was a promotion
            bool wasPromotion = false;
            for (const Action &action : actions)
            {
                if (action == Action::Promotion)
                {
                    wasPromotion = true;
                    break;
                }
            }

            // Clear the source square
            sourceLabel->setStyleSheet("border-image: url(:/img/blank.png) 0 0 0 0 stretch stretch;");
            sourceLabel->setProperty("originalStyle", QVariant());

            if (wasPromotion)
            {
                U64 piece = 1ULL << targetSquare;
                Color color = cg.GetColor(piece);
                Piece pieceType = cg.GetPieceType(piece);

                // Construct the appropriate image path based on piece type and color
                QString pieceName;
                switch (pieceType)
                {
                case Queen:
                    pieceName = "queen";
                    break;
                case Rook:
                    pieceName = "rook";
                    break;
                case Bishop:
                    pieceName = "bishop";
                    break;
                case Knight:
                    pieceName = "knight";
                    break;
                default:
                    pieceName = "pawn";
                    break;
                }
                QString colorName = (color == white) ? "white" : "black";
                QString imagePath = QString(":/img/%1_%2.png").arg(colorName).arg(pieceName);
                QString newStyle = QString("border-image: url(%1) 0 0 0 0 stretch stretch;").arg(imagePath);
                targetLabel->setStyleSheet(newStyle);
                targetLabel->setProperty("originalStyle", newStyle);
            }
            else
            {
                // Normal move - transfer the original piece style
                targetLabel->setStyleSheet(originalPieceStyle);
                targetLabel->setProperty("originalStyle", originalPieceStyle);
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

            // Check for checkmate using the chess engine's IsWin method
            if (cg.IsWin(white))
            {
                QMessageBox::information(this, "Game Over", "White Wins!");
            }
            else if (cg.IsWin(black))
            {
                QMessageBox::information(this, "Game Over", "Black Wins!");
            }
        }

        // Clear highlights
        for (DraggableLabel *label : allLabels)
        {
            if (label->styleSheet().contains("green_hue.png"))
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

MainWindow::~MainWindow()
{
    delete ui;
}
