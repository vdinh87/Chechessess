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
#include <QPixmap>
#include <algorithm>

class CustomRecursiveChessGame : public RecursiveChessGame
{
private:
    MainWindow *mainWindow;
    bool inSubGame;
    std::shared_ptr<CustomRecursiveChessGame> activeSubGame;
    std::vector<U64> savedWhitePieces;
    std::vector<U64> savedBlackPieces;
    Piece attackingPieceType;

public:
    CustomRecursiveChessGame(MainWindow *window = nullptr)
        : RecursiveChessGame(), mainWindow(window), inSubGame(false), activeSubGame(nullptr), attackingPieceType(Pawn) {}

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

        // Store the attacking piece's type before creating sub-game
        attackingPieceType = GetPieceType(1ULL << from);

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
                                attackingPieceType,  // Use the stored piece type
                                attackingPieceType); // Same type for both from and to
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

            // Add a delayed check as a fallback with a shorter delay
            if (mainWindow)
            {
                QTimer::singleShot(250, mainWindow, [this, mainWindowPtr = mainWindow]()
                                   {
                    if (IsWin(white) && mainWindowPtr)
                    {
                        mainWindowPtr->showGameOver(true);
                    }
                    else if (IsWin(black) && mainWindowPtr)
                    {
                        mainWindowPtr->showGameOver(false);
                    } });
            }
        }
    }

    uint64_t get_moves_bitboard(Square square)
    {
        uint64_t moves = 0;
        CustomRecursiveChessGame *activeGame = GetActiveGame();

        if (!activeGame)
            return 0;

        // Get the attacks for the piece at this square
        try
        {
            moves = activeGame->GetAttacks(square);
        }
        catch (const std::exception &)
        {
            // No piece at this square
            return 0;
        }

        return moves;
    }

    void make_move(Square from, Square to)
    {
        CustomRecursiveChessGame *activeGame = GetActiveGame();
        if (!activeGame)
            return;

        activeGame->Move(from, to);
    }
};

static CustomRecursiveChessGame cg(nullptr);    // Make it static to avoid multiple definition
static std::vector<DraggableLabel *> allLabels; // Make it static
static Square dragSourceSquare = invalid;       // Make it static
static QDialog *subGameDialog = nullptr;        // Dialog for sub-games

// Helper function to find the index of an element in a vector (replacement for indexOf)
template <typename T>
int indexOf(const std::vector<T> &vec, const T &element)
{
    auto it = std::find(vec.begin(), vec.end(), element);
    if (it != vec.end())
    {
        return std::distance(vec.begin(), it);
    }
    return -1;
}

// Helper function to convert chess notation to a Square enum value
Square square_from_string(const std::string &str)
{
    if (str.length() != 2)
        return invalid;

    char file = str[0];
    char rank = str[1];

    if (file < 'a' || file > 'h' || rank < '1' || rank > '8')
        return invalid;

    int fileIdx = file - 'a';
    int rankIdx = rank - '1';

    return static_cast<Square>(rankIdx * 8 + fileIdx);
}

// Custom bit checking function to avoid macro conflicts
bool is_bit_set(uint64_t bitboard, int square)
{
    return (bitboard & (1ULL << square)) != 0;
}

// Get piece type helpers (using Piece enum directly)
Piece get_piece_type(Piece piece)
{
    // Clear the color bit (lower bits are piece type)
    return static_cast<Piece>(piece & 7); // 7 = 0b111 to get just the piece type bits
}

Color get_color(Piece piece)
{
    // The color is the high bit
    return (piece & 8) ? black : white;
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

        // Determine square color (dark or light)
        int row = i / 8;
        int col = i % 8;
        bool isDark = (row + col) % 2 != 0;
        QString squareColor = isDark ? "#B58863" : "#F0D9B5";

        bool bit = is_bit_set(bitboard, i);
        if (bit)
        {
            // Store the original style
            if (!label->property("originalStyle").isValid())
            {
                label->setProperty("originalStyle", label->styleSheet());
            }

            // Save the original pixmap if not already saved
            if (!label->property("originalPixmap").isValid() && !label->pixmap().isNull())
            {
                label->setProperty("originalPixmap", QVariant::fromValue(label->pixmap()));
            }

            // Highlight the square with green color
            // Use a slightly transparent green on top of the square color
            QString highlightStyle;
            if (isDark)
            {
                // Slightly lighter green for dark squares
                highlightStyle = "background-color: #7BAF6B;"; // Greenish color for dark squares
            }
            else
            {
                // Regular green for light squares
                highlightStyle = "background-color: #AAD785;"; // Greenish color for light squares
            }

            label->setStyleSheet(highlightStyle);
        }
        else if (label->styleSheet().contains("#7BAF6B") || label->styleSheet().contains("#AAD785"))
        {
            // Restore the original style
            QVariant originalStyle = label->property("originalStyle");
            if (originalStyle.isValid())
            {
                label->setStyleSheet(originalStyle.toString());
            }
            else
            {
                // If no original style is saved, set the default square color
                label->setStyleSheet(QString("background-color: %1;").arg(squareColor));
            }
        }
    }
}

void MainWindow::handleDragStarted(DraggableLabel *source)
{
    if (!source)
        return;

    // Clear previous highlights
    for (DraggableLabel *label : allLabels)
    {
        if (!label)
            continue;

        if (label->styleSheet().contains("#7BAF6B") || label->styleSheet().contains("#AAD785"))
        {
            // Restore original style
            QVariant originalStyle = label->property("originalStyle");
            if (originalStyle.isValid())
            {
                label->setStyleSheet(originalStyle.toString());
            }
            else
            {
                // Determine square color if original style not available
                int idx = indexOf(allLabels, label);
                if (idx >= 0 && idx < 64)
                {
                    int row = idx / 8;
                    int col = idx % 8;
                    bool isDark = (row + col) % 2 != 0;
                    QString squareColor = isDark ? "#B58863" : "#F0D9B5";
                    label->setStyleSheet(QString("background-color: %1;").arg(squareColor));
                }
            }
        }
    }

    // Find the square the drag started from
    int index = indexOf(allLabels, source);
    if (index == -1)
        return;

    int row = index / 8;
    int col = index % 8;

    // Convert to chess notation
    char file = 'a' + col;
    char rank = '1' + row;
    Square square = square_from_string(std::string(1, file) + std::string(1, rank));

    // Store the dragging square
    dragSourceSquare = square;

    // Get valid moves for the piece
    uint64_t moves = cg.get_moves_bitboard(square);

    // Highlight valid moves
    updateLabelsFromBitboard(moves, allLabels);
}

void MainWindow::handleDragEntered(DraggableLabel *target)
{
    if (!target || dragSourceSquare == invalid)
        return;

    // Find the index of the target label
    int index = indexOf(allLabels, target);
    if (index == -1)
        return;

    int row = index / 8;
    int col = index % 8;

    // Convert to chess notation
    char file = 'a' + col;
    char rank = '1' + row;
    Square targetSquare = square_from_string(std::string(1, file) + std::string(1, rank));

    // Check if this is a valid square for the current piece
    uint64_t validMoves = cg.get_moves_bitboard(dragSourceSquare);
    bool isValidMove = is_bit_set(validMoves, static_cast<int>(targetSquare));

    // Optionally highlight the target square to show if it's a valid move
    if (isValidMove)
    {
        // Determine if it's a dark or light square
        bool isDark = (row + col) % 2 != 0;

        // Use appropriate highlighting color based on square color
        if (isDark)
        {
            target->setStyleSheet("background-color: #7BAF6B;"); // Darker green for dark squares
        }
        else
        {
            target->setStyleSheet("background-color: #AAD785;"); // Lighter green for light squares
        }
    }
}

void MainWindow::handleDrop(DraggableLabel *source, DraggableLabel *target)
{
    if (!source || !target)
        return;

    // Clear all highlighted squares
    for (DraggableLabel *label : allLabels)
    {
        if (!label)
            continue;

        if (label->styleSheet().contains("#7BAF6B") || label->styleSheet().contains("#AAD785"))
        {
            // Restore original style
            QVariant originalStyle = label->property("originalStyle");
            if (originalStyle.isValid())
            {
                label->setStyleSheet(originalStyle.toString());
            }
            else
            {
                // Determine square color if original style not available
                int idx = indexOf(allLabels, label);
                if (idx >= 0 && idx < 64)
                {
                    int row = idx / 8;
                    int col = idx % 8;
                    bool isDark = (row + col) % 2 != 0;
                    QString squareColor = isDark ? "#B58863" : "#F0D9B5";
                    label->setStyleSheet(QString("background-color: %1;").arg(squareColor));
                }
            }
        }
    }

    if (dragSourceSquare == invalid)
        return;

    // Get the target square
    int targetIndex = indexOf(allLabels, target);
    if (targetIndex == -1)
        return;

    int targetRow = targetIndex / 8;
    int targetCol = targetIndex % 8;

    // Convert to chess notation
    char targetFile = 'a' + targetCol;
    char targetRank = '1' + targetRow;
    Square targetSquare = square_from_string(std::string(1, targetFile) + std::string(1, targetRank));

    // Try to make the move
    try
    {
        // Make the move in the game
        cg.make_move(dragSourceSquare, targetSquare);

        // Update the board to reflect the new state
        updateBoardFromGame();

        // Log the move
        QString moveText = QString("%1%2 to %3%4")
                               .arg(QChar('a' + (dragSourceSquare % 8)))
                               .arg(dragSourceSquare / 8 + 1)
                               .arg(QChar('a' + (targetSquare % 8)))
                               .arg(targetSquare / 8 + 1);
        ui->textEdit->append(moveText);
    }
    catch (const std::exception &e)
    {
        // Invalid move, log error
        ui->textEdit->append(QString("Invalid move: %1").arg(e.what()));
    }

    // Reset the drag source
    dragSourceSquare = invalid;
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
    if (allLabels.empty())
        return;

    CustomRecursiveChessGame *activeGame = cg.GetActiveGame();
    if (!activeGame)
        return;

    // For each square on the board
    for (int i = 0; i < 64 && i < allLabels.size(); i++)
    {
        DraggableLabel *label = allLabels[i];
        if (!label)
            continue;

        // Determine square color (dark or light)
        int row = i / 8;
        int col = i % 8;
        bool isDark = (row + col) % 2 != 0;
        QString squareColor = isDark ? "#B58863" : "#F0D9B5";

        // Convert index to Square enum
        char file = 'a' + col;
        char rank = '1' + row;
        std::string squareStr = std::string(1, file) + std::string(1, rank);
        Square square = square_from_string(squareStr);

        // Check if there's a piece at this square
        U64 squareBit = 1ULL << square;
        bool hasPiece = (activeGame->GetBoard() & squareBit) != 0;

        if (!hasPiece)
        {
            // Empty square
            label->setPixmap(QPixmap());
            QString baseStyle = QString("background-color: %1;").arg(squareColor);
            label->setStyleSheet(baseStyle);
            label->setProperty("originalStyle", baseStyle);
        }
        else
        {
            // Get piece color and type
            Color pieceColor = activeGame->GetColor(squareBit);
            Piece piece = activeGame->GetPieceType(squareBit);

            // Construct image path
            QString colorStr = (pieceColor == Color::white) ? "white" : "black";
            QString pieceStr;

            switch (piece)
            {
            case Pawn:
                pieceStr = "pawn";
                break;
            case Knight:
                pieceStr = "knight";
                break;
            case Bishop:
                pieceStr = "bishop";
                break;
            case Rook:
                pieceStr = "rook";
                break;
            case Queen:
                pieceStr = "queen";
                break;
            case King:
                pieceStr = "king";
                break;
            default:
                pieceStr = "unknown";
                break;
            }

            // Load the piece image
            QString imagePath = QString(":/img/%1_%2.png").arg(colorStr, pieceStr);
            QPixmap piecePixmap(imagePath);

            if (!piecePixmap.isNull())
            {
                // Get the label size, use a reasonable default if it's too small
                int labelWidth = label->width();
                int labelHeight = label->height();

                // If labels haven't been properly sized yet, use a reasonable default
                if (labelWidth < 20 || labelHeight < 20)
                {
                    // Use parent widget's size if available to guess the square size
                    if (label->parentWidget())
                    {
                        int boardSize = qMin(label->parentWidget()->width(), label->parentWidget()->height());
                        labelWidth = labelHeight = boardSize / 8;
                    }
                    else
                    {
                        // Default to a reasonable size if no parent is available
                        labelWidth = labelHeight = 40;
                    }
                }

                // Scale the pixmap to 85% of the label's size
                int size = qMin(labelWidth, labelHeight) * 0.85;
                QPixmap scaledPixmap = piecePixmap.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                // Set the background color for the square
                QString baseStyle = QString("background-color: %1;").arg(squareColor);
                label->setStyleSheet(baseStyle);
                label->setProperty("originalStyle", baseStyle);

                // Set the pixmap for the piece
                label->setPixmap(scaledPixmap);
                label->setAlignment(Qt::AlignCenter);
            }
        }
    }
}

void MainWindow::showGameOver(bool isWhiteWinner)
{
    QString winner = isWhiteWinner ? "White" : "Black";
    QString loser = isWhiteWinner ? "Black" : "White";
    ui->textEdit->append(QString("%1 has checkmated %2! Game Over!").arg(winner).arg(loser));
    QMessageBox::information(this, "Game Over", QString("%1 Wins!").arg(winner));
}

void MainWindow::on_actionNew_Game_triggered()
{
    // Reset the chess game to its initial state
    cg = CustomRecursiveChessGame(this);

    // Clear any highlight and reset the drag source
    dragSourceSquare = invalid;

    // Update the board to show the new game state
    updateBoardFromGame();

    // Log the new game start
    ui->textEdit->append("New game started");
}

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

    // Find the chessboard widget
    QWidget *chesswdg = findChild<QWidget *>("chesswdg");
    if (chesswdg)
    {
        // Instead of setting background on the chesswdg, clear any background
        chesswdg->setStyleSheet("");
    }

    // Apply checkerboard pattern directly to the labels
    for (int i = 0; i < 64 && i < allLabels.size(); i++)
    {
        DraggableLabel *label = allLabels[i];
        if (!label)
            continue;

        // Apply alignment settings
        label->setAlignment(Qt::AlignCenter);

        // Set content margins to ensure proper centering
        label->setContentsMargins(2, 2, 2, 2);

        // Clear any pixmap
        label->setPixmap(QPixmap());

        // Set the checkerboard pattern
        int row = i / 8;
        int col = i % 8;
        bool isDark = (row + col) % 2 != 0;

        // Use actual colors for the squares instead of the background image
        if (isDark)
        {
            label->setStyleSheet("background-color: #B58863;"); // Dark square
            label->setProperty("originalStyle", "background-color: #B58863;");
        }
        else
        {
            label->setStyleSheet("background-color: #F0D9B5;"); // Light square
            label->setProperty("originalStyle", "background-color: #F0D9B5;");
        }

        // Enable scaling for the pixmap
        label->setScaledContents(false); // We'll handle this manually in updateBoardFromGame

        connect(label, &DraggableLabel::dragStarted, this, &MainWindow::handleDragStarted);
        connect(label, &DraggableLabel::dragEntered, this, &MainWindow::handleDragEntered);
        connect(label, &DraggableLabel::dropOccurred, this, &MainWindow::handleDrop);
        label->setDraggable(true);
    }

    // Update the board for the first time
    updateBoardFromGame();

    // Schedule another update after the layout has been calculated
    QTimer::singleShot(100, this, &MainWindow::updateBoardFromGame);
}

MainWindow::~MainWindow()
{
    delete ui;
}
