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
#include <QMessageBox>
#include <QPushButton>
#include <QApplication>
#include <QDebug>
#include <QSizePolicy>

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

    // Helper methods to safely access king positions and moves
    Square GetWhiteKingPosition() const
    {
        return static_cast<Square>(get_ls1b_index(WhitePiecesArray[King]));
    }

    Square GetBlackKingPosition() const
    {
        return static_cast<Square>(get_ls1b_index(BlackPiecesArray[King]));
    }

    uint64_t GetWhiteKingMoves() const
    {
        try
        {
            Square whiteKingPos = GetWhiteKingPosition();
            return GetAttacks(whiteKingPos);
        }
        catch (...)
        {
            return 0;
        }
    }

    uint64_t GetBlackKingMoves() const
    {
        try
        {
            Square blackKingPos = GetBlackKingPosition();
            return GetAttacks(blackKingPos);
        }
        catch (...)
        {
            return 0;
        }
    }

    // Debug helper methods
    void LogPiecePositions() const
    {
        qDebug() << "White pieces:";
        for (int i = 0; i < 6; i++)
        {
            qDebug() << "  Type" << i << "bitboard:" << QString::number(WhitePiecesArray[i], 16);
        }
        qDebug() << "Black pieces:";
        for (int i = 0; i < 6; i++)
        {
            qDebug() << "  Type" << i << "bitboard:" << QString::number(BlackPiecesArray[i], 16);
        }
    }

    // Add NewGame method
    void NewGame()
    {
        // Reset to starting position
        WhitePiecesArray = {
            0x000000000000FF00ULL, // Pawns
            0x0000000000000042ULL, // Knights
            0x0000000000000024ULL, // Bishops
            0x0000000000000081ULL, // Rooks
            0x0000000000000008ULL, // Queen
            0x0000000000000010ULL  // King
        };

        BlackPiecesArray = {
            0x00FF000000000000ULL, // Pawns
            0x4200000000000000ULL, // Knights
            0x2400000000000000ULL, // Bishops
            0x8100000000000000ULL, // Rooks
            0x0800000000000000ULL, // Queen
            0x1000000000000000ULL  // King
        };

        UpdateBoard();
        inSubGame = false;
        activeSubGame = nullptr;
    }

    // Add helper methods for free move mode
    void MovePiece(Square from, Square to)
    {
        // Don't do anything if trying to move to the same square
        if (from == to)
            return;

        // Get the piece at the source square
        U64 sourceBit = 1ULL << from;
        U64 targetBit = 1ULL << to;

        // If there's no piece at the source, do nothing
        if (!(GetBoard() & sourceBit))
            return;

        // Remove any existing piece at the target
        RemovePiece(to);

        // Get the piece type and color
        Color pieceColor = GetColor(sourceBit);
        Piece pieceType = GetPieceType(sourceBit);

        // Remove from source
        RemovePiece(from);

        // Add to target based on color
        if (pieceColor == white)
        {
            WhitePiecesArray[pieceType] |= targetBit;
        }
        else
        {
            BlackPiecesArray[pieceType] |= targetBit;
        }

        // Update the board
        UpdateBoard();
    }

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
                    // Add debug logging for checkmate state
                    qDebug() << "Checking for checkmate after sub-game";
                    LogPiecePositions();
                    
                    // Check if kings are present and their positions
                    try {
                        qDebug() << "White king position:" << GetWhiteKingPosition();
                        qDebug() << "Black king position:" << GetBlackKingPosition();
                    } catch (...) {
                        qDebug() << "Error getting king positions";
                    }
                    
                    // Perform a recursive check to validate king mobility
                    bool whiteInCheckmate = IsWin(white);
                    bool blackInCheckmate = IsWin(black);
                    
                    // Extra verification that kings have legal moves
                    uint64_t whiteKingMoves = GetWhiteKingMoves();
                    uint64_t blackKingMoves = GetBlackKingMoves();
                    
                    qDebug() << "White king has moves:" << whiteKingMoves;
                    qDebug() << "Black king has moves:" << blackKingMoves;
                    
                    if (whiteInCheckmate && mainWindowPtr)
                    {
                        qDebug() << "White is in checkmate!";
                        mainWindowPtr->showGameOver(true);
                    }
                    else if (blackInCheckmate && mainWindowPtr)
                    {
                        qDebug() << "Black is in checkmate!";
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

// Remove the static cg variable - we'll use the member variable only
// static CustomRecursiveChessGame cg(nullptr);
// static std::vector<DraggableLabel *> allLabels; // Make it static
static Square dragSourceSquare = invalid; // Make it static
static QDialog *subGameDialog = nullptr;  // Dialog for sub-games

// Constants for visual appearance
const float PIECE_SCALE_FACTOR = 0.8; // Scale factor for chess pieces - moderate size with good clarity

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

    // In free move mode, highlight all squares except the source
    if (freeMoveMode)
    {
        for (int i = 0; i < allLabels.size(); i++)
        {
            // Skip the source square
            if (i == index)
                continue;

            DraggableLabel *label = allLabels[i];
            if (!label)
                continue;

            // Determine square color
            int r = i / 8;
            int c = i % 8;
            bool isDark = (r + c) % 2 != 0;

            // Use appropriate highlighting color based on square color
            if (isDark)
            {
                label->setStyleSheet("background-color: #7BAF6B;"); // Darker green for dark squares
            }
            else
            {
                label->setStyleSheet("background-color: #AAD785;"); // Lighter green for light squares
            }
        }
    }
    else
    {
        // Standard mode: Get valid moves for the piece
        uint64_t moves = cg->get_moves_bitboard(square);

        // Highlight valid moves
        updateLabelsFromBitboard(moves, allLabels);
    }
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

    // In free move mode, all moves are valid
    bool isValidMove;
    if (freeMoveMode)
    {
        isValidMove = true;
    }
    else
    {
        // Check if this is a valid square for the current piece
        uint64_t validMoves = cg->get_moves_bitboard(dragSourceSquare);
        isValidMove = is_bit_set(validMoves, static_cast<int>(targetSquare));
    }

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
    qDebug() << "handleDrop called with source:" << (source ? source->objectName() : "null")
             << "target:" << (target ? target->objectName() : "null");

    if (!source || !target)
    {
        qDebug() << "Source or target is null, aborting drop";
        return;
    }

    // If source and target are the same, don't do anything
    if (source == target)
    {
        qDebug() << "Source and target are the same label, aborting drop";
        return;
    }

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
    {
        qDebug() << "dragSourceSquare is invalid, aborting drop";
        return;
    }

    // Get the target square
    int targetIndex = indexOf(allLabels, target);
    if (targetIndex == -1)
    {
        qDebug() << "Target index not found in allLabels, aborting drop";
        return;
    }

    int targetRow = targetIndex / 8;
    int targetCol = targetIndex % 8;

    // Convert to chess notation
    char targetFile = 'a' + targetCol;
    char targetRank = '1' + targetRow;
    Square targetSquare = square_from_string(std::string(1, targetFile) + std::string(1, targetRank));

    qDebug() << "Source square:" << static_cast<int>(dragSourceSquare)
             << "Target square:" << static_cast<int>(targetSquare);

    // Check if this is a valid move (only if not in free move mode)
    bool isValidMove = true; // Default to true for free move mode

    if (!freeMoveMode)
    {
        uint64_t validMoves = cg->get_moves_bitboard(dragSourceSquare);
        isValidMove = is_bit_set(validMoves, static_cast<int>(targetSquare));

        if (!isValidMove)
        {
            qDebug() << "Invalid move! Valid moves bitboard:" << validMoves;
            // Invalid move, reset drag source
            dragSourceSquare = invalid;
            return;
        }
    }
    else
    {
        qDebug() << "Free move mode enabled - allowing any move";
    }

    qDebug() << "Valid move detected, proceeding with move logic";

    // Check if this is a capture (there's already a piece at the target square)
    CustomRecursiveChessGame *activeGame = cg->GetActiveGame();
    bool isCapture = false;
    if (activeGame)
    {
        U64 squareBit = 1ULL << targetSquare;
        isCapture = (activeGame->GetBoard() & squareBit) != 0;
    }

    if (isCapture && !cg->IsInSubGame())
    {
        // Log the capture attempt
        QString moveText = QString("%1%2 attempting to capture %3%4")
                               .arg(QChar('a' + (dragSourceSquare % 8)))
                               .arg(dragSourceSquare / 8 + 1)
                               .arg(QChar('a' + (targetSquare % 8)))
                               .arg(targetSquare / 8 + 1);
        ui->textEdit->append(moveText);

        // Get the attacker's color
        U64 sourceBit = 1ULL << dragSourceSquare;
        Color attackerColor = activeGame->GetColor(sourceBit);

        // Create a zoom/blur effect for transition to sub-game
        QWidget *chessWidget = findChild<QWidget *>("chesswdg");
        if (chessWidget)
        {
            // Store original size
            QSize originalSize = chessWidget->size();
            chessWidget->setProperty("originalSize", QVariant(originalSize));

            // Raise the chess widget to the top of the stacking order
            // This ensures it appears above all other UI elements when zoomed
            chessWidget->raise();

            // Create blur effect
            QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect(this);
            blurEffect->setBlurRadius(0);
            chessWidget->setGraphicsEffect(blurEffect);

            // Create animation for blur
            QPropertyAnimation *blurAnimation = new QPropertyAnimation(blurEffect, "blurRadius");
            blurAnimation->setDuration(500);
            blurAnimation->setStartValue(0);
            blurAnimation->setEndValue(10);
            blurAnimation->setEasingCurve(QEasingCurve::InOutQuad);

            // Create animation for zoom
            QPropertyAnimation *zoomAnimation = new QPropertyAnimation(chessWidget, "geometry");
            zoomAnimation->setDuration(500);

            QRect startGeometry = chessWidget->geometry();
            QRect endGeometry = startGeometry;

            // Calculate the zoom effect to cover the entire window
            // Get the main window size
            QSize windowSize = size();
            int targetWidth = windowSize.width() * 1.5;   // 150% of window width
            int targetHeight = windowSize.height() * 1.5; // 150% of window height

            // Calculate the centered position (negative offsets to extend beyond window)
            int leftOffset = (windowSize.width() - targetWidth) / 2;
            int topOffset = (windowSize.height() - targetHeight) / 2;

            // Set the ending geometry to cover more than the entire window
            endGeometry.setLeft(leftOffset);
            endGeometry.setTop(topOffset);
            endGeometry.setWidth(targetWidth);
            endGeometry.setHeight(targetHeight);

            zoomAnimation->setStartValue(startGeometry);
            zoomAnimation->setEndValue(endGeometry);
            zoomAnimation->setEasingCurve(QEasingCurve::OutQuad);

            // Create a container for animations
            QParallelAnimationGroup *animGroup = new QParallelAnimationGroup(this);
            animGroup->addAnimation(blurAnimation);
            animGroup->addAnimation(zoomAnimation);

            // Connect animation finished signal to start the sub-game
            connect(animGroup, &QParallelAnimationGroup::finished, this, [this, fromSquare = dragSourceSquare, toSquare = targetSquare, attacker = attackerColor]()
                    {
                // Start the sub-game
                cg->StartSubGame(fromSquare, toSquare, attacker);
                
                // Update the board to show the new sub-game state
                updateBoardFromGame();
                
                // Add a delayed update here:
                QTimer::singleShot(50, this, [this]() {
                    forceUpdatePieceSizes(); // Force the piece sizes to be consistent
                });
                
                // Log the start of the sub-game
                ui->textEdit->append("Sub-game started for capture resolution");
                
                // Remove blur effect when sub-game begins
                QWidget* chessWidget = findChild<QWidget *>("chesswdg");
                if (chessWidget && chessWidget->graphicsEffect())
                {
                    chessWidget->setGraphicsEffect(nullptr);
                } });

            // Start the animation
            animGroup->start(QAbstractAnimation::DeleteWhenStopped);
        }
    }
    else if (cg->IsInSubGame())
    {
        // We're in a sub-game, make the move
        try
        {
            // Make the move in the game
            cg->make_move(dragSourceSquare, targetSquare);

            // Update the board to reflect the new state
            updateBoardFromGame();

            // Log the move
            QString moveText = QString("Sub-game: %1%2 to %3%4")
                                   .arg(QChar('a' + (dragSourceSquare % 8)))
                                   .arg(dragSourceSquare / 8 + 1)
                                   .arg(QChar('a' + (targetSquare % 8)))
                                   .arg(targetSquare / 8 + 1);
            ui->textEdit->append(moveText);

            // Check if sub-game is over (checkmate)
            CustomRecursiveChessGame *activeGame = cg->GetActiveGame();
            if (activeGame)
            {
                // More thorough check before declaring checkmate
                bool whiteKingHasMoves = false;
                bool blackKingHasMoves = false;

                try
                {
                    // Check if white king has moves using our safe helper methods
                    uint64_t whiteKingMoves = activeGame->GetWhiteKingMoves();
                    whiteKingHasMoves = whiteKingMoves != 0;
                    qDebug() << "White king at" << activeGame->GetWhiteKingPosition() << "has moves:" << whiteKingMoves;

                    // Check if black king has moves
                    uint64_t blackKingMoves = activeGame->GetBlackKingMoves();
                    blackKingHasMoves = blackKingMoves != 0;
                    qDebug() << "Black king at" << activeGame->GetBlackKingPosition() << "has moves:" << blackKingMoves;
                }
                catch (...)
                {
                    qDebug() << "Error checking king moves";
                }

                // Log more info about the checkmate state
                bool whiteInCheckmate = activeGame->IsWin(white);
                bool blackInCheckmate = activeGame->IsWin(black);

                qDebug() << "Checkmate detection: White in checkmate:" << whiteInCheckmate
                         << "White king has moves:" << whiteKingHasMoves
                         << "Black in checkmate:" << blackInCheckmate
                         << "Black king has moves:" << blackKingHasMoves;

                if (whiteInCheckmate && !blackKingHasMoves)
                {
                    ui->textEdit->append("White wins the sub-game!");

                    // Create transition effect back to main game
                    QWidget *chessWidget = findChild<QWidget *>("chesswdg");
                    if (chessWidget)
                    {
                        // Raise the chess widget to ensure it's above text boxes
                        chessWidget->raise();

                        // Create blur effect
                        QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect(this);
                        blurEffect->setBlurRadius(10);
                        chessWidget->setGraphicsEffect(blurEffect);

                        // Create animation for blur
                        QPropertyAnimation *blurAnimation = new QPropertyAnimation(blurEffect, "blurRadius");
                        blurAnimation->setDuration(500);
                        blurAnimation->setStartValue(10);
                        blurAnimation->setEndValue(0);
                        blurAnimation->setEasingCurve(QEasingCurve::InOutQuad);

                        // Create animation for zoom (return to original size)
                        QPropertyAnimation *zoomAnimation = new QPropertyAnimation(chessWidget, "geometry");
                        zoomAnimation->setDuration(500);

                        // We're zoomed in to nearly the full window size, so our current geometry is large
                        QRect currentGeometry = chessWidget->geometry();

                        // Get the original size from property if available
                        QRect originalGeometry;
                        QVariant originalSizeVar = chessWidget->property("originalSize");
                        if (originalSizeVar.isValid())
                        {
                            QSize originalSize = originalSizeVar.toSize();
                            // Calculate position to center the original sized widget
                            int centerX = currentGeometry.center().x() - (originalSize.width() / 2);
                            int centerY = currentGeometry.center().y() - (originalSize.height() / 2);
                            originalGeometry = QRect(centerX, centerY, originalSize.width(), originalSize.height());
                        }
                        else
                        {
                            // Fallback if no stored size - just shrink back to a more reasonable size
                            // Use 1/3 of the current size to create a dramatic effect
                            int width = currentGeometry.width() / 3;
                            int height = currentGeometry.height() / 3;
                            int left = currentGeometry.center().x() - (width / 2);
                            int top = currentGeometry.center().y() - (height / 2);
                            originalGeometry = QRect(left, top, width, height);
                        }

                        zoomAnimation->setStartValue(currentGeometry);
                        zoomAnimation->setEndValue(originalGeometry);
                        zoomAnimation->setEasingCurve(QEasingCurve::InQuad);

                        // Create a container for animations
                        QParallelAnimationGroup *animGroup = new QParallelAnimationGroup(this);
                        animGroup->addAnimation(blurAnimation);
                        animGroup->addAnimation(zoomAnimation);

                        // Connect animation finished signal
                        connect(animGroup, &QParallelAnimationGroup::finished, this, [this]()
                                {
                            // End the sub-game and return to main game
                            cg->EndSubGame();
                            
                            // Update the board to show the main game state
                            updateBoardFromGame();
                            
                            // Add a delayed update here:
                            QTimer::singleShot(50, this, [this]() {
                                forceUpdatePieceSizes(); // Force the piece sizes to be consistent
                            });
                            
                            // Remove blur effect
                            QWidget* chessWidget = findChild<QWidget *>("chesswdg");
                            if (chessWidget && chessWidget->graphicsEffect())
                            {
                                chessWidget->setGraphicsEffect(nullptr);
                            }
                            
                            // Log return to main game
                            ui->textEdit->append("Returned to main game");
                            
                            // Check for checkmate in main game
                            RecursiveChessGame* activeGame = cg->GetActiveGame();
                            if (activeGame->IsWin(white))
                            {
                                showGameOver(true);  // White wins
                            }
                            else if (activeGame->IsWin(black))
                            {
                                showGameOver(false); // Black wins
                            } });

                        // Start the animation
                        animGroup->start(QAbstractAnimation::DeleteWhenStopped);
                    }
                }
                else if (blackInCheckmate && !whiteKingHasMoves)
                {
                    ui->textEdit->append("Black wins the sub-game!");

                    // Create transition effect back to main game
                    QWidget *chessWidget = findChild<QWidget *>("chesswdg");
                    if (chessWidget)
                    {
                        // Raise the chess widget to ensure it's above text boxes
                        chessWidget->raise();

                        // Create blur effect
                        QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect(this);
                        blurEffect->setBlurRadius(10);
                        chessWidget->setGraphicsEffect(blurEffect);

                        // Create animation for blur
                        QPropertyAnimation *blurAnimation = new QPropertyAnimation(blurEffect, "blurRadius");
                        blurAnimation->setDuration(500);
                        blurAnimation->setStartValue(10);
                        blurAnimation->setEndValue(0);
                        blurAnimation->setEasingCurve(QEasingCurve::InOutQuad);

                        // Create animation for zoom (return to original size)
                        QPropertyAnimation *zoomAnimation = new QPropertyAnimation(chessWidget, "geometry");
                        zoomAnimation->setDuration(500);

                        // We're zoomed in to nearly the full window size, so our current geometry is large
                        QRect currentGeometry = chessWidget->geometry();

                        // Get the original size from property if available
                        QRect originalGeometry;
                        QVariant originalSizeVar = chessWidget->property("originalSize");
                        if (originalSizeVar.isValid())
                        {
                            QSize originalSize = originalSizeVar.toSize();
                            // Calculate position to center the original sized widget
                            int centerX = currentGeometry.center().x() - (originalSize.width() / 2);
                            int centerY = currentGeometry.center().y() - (originalSize.height() / 2);
                            originalGeometry = QRect(centerX, centerY, originalSize.width(), originalSize.height());
                        }
                        else
                        {
                            // Fallback if no stored size - just shrink back to a more reasonable size
                            // Use 1/3 of the current size to create a dramatic effect
                            int width = currentGeometry.width() / 3;
                            int height = currentGeometry.height() / 3;
                            int left = currentGeometry.center().x() - (width / 2);
                            int top = currentGeometry.center().y() - (height / 2);
                            originalGeometry = QRect(left, top, width, height);
                        }

                        zoomAnimation->setStartValue(currentGeometry);
                        zoomAnimation->setEndValue(originalGeometry);
                        zoomAnimation->setEasingCurve(QEasingCurve::InQuad);

                        // Create a container for animations
                        QParallelAnimationGroup *animGroup = new QParallelAnimationGroup(this);
                        animGroup->addAnimation(blurAnimation);
                        animGroup->addAnimation(zoomAnimation);

                        // Connect animation finished signal
                        connect(animGroup, &QParallelAnimationGroup::finished, this, [this]()
                                {
                            // End the sub-game and return to main game
                            cg->EndSubGame();
                            
                            // Update the board to show the main game state
                            updateBoardFromGame();
                            
                            // Add a delayed update here:
                            QTimer::singleShot(50, this, [this]() {
                                forceUpdatePieceSizes(); // Force the piece sizes to be consistent
                            });
                            
                            // Remove blur effect
                            QWidget* chessWidget = findChild<QWidget *>("chesswdg");
                            if (chessWidget && chessWidget->graphicsEffect())
                            {
                                chessWidget->setGraphicsEffect(nullptr);
                            }
                            
                            // Log return to main game
                            ui->textEdit->append("Returned to main game");
                            
                            // Check for checkmate in main game
                            RecursiveChessGame* activeGame = cg->GetActiveGame();
                            if (activeGame->IsWin(white))
                            {
                                showGameOver(true);  // White wins
                            }
                            else if (activeGame->IsWin(black))
                            {
                                showGameOver(false); // Black wins
                            } });

                        // Start the animation
                        animGroup->start(QAbstractAnimation::DeleteWhenStopped);
                    }
                }
            }
        }
        catch (const std::exception &e)
        {
            // Invalid move, log error
            ui->textEdit->append(QString("Invalid sub-game move: %1").arg(e.what()));
        }
    }
    else
    {
        // Regular move (not a capture, or we're already in a sub-game, or in free move mode)
        try
        {
            if (freeMoveMode)
            {
                // In free move mode, use our helper method to handle piece movement
                activeGame->MovePiece(dragSourceSquare, targetSquare);

                // Log the move
                QString moveText = QString("Free move: %1%2 to %3%4")
                                       .arg(QChar('a' + (dragSourceSquare % 8)))
                                       .arg(dragSourceSquare / 8 + 1)
                                       .arg(QChar('a' + (targetSquare % 8)))
                                       .arg(targetSquare / 8 + 1);
                ui->textEdit->append(moveText);
            }
            else
            {
                // Make the move in the game using normal rules
                cg->make_move(dragSourceSquare, targetSquare);

                // Log the move
                QString moveText = QString("%1%2 to %3%4")
                                       .arg(QChar('a' + (dragSourceSquare % 8)))
                                       .arg(dragSourceSquare / 8 + 1)
                                       .arg(QChar('a' + (targetSquare % 8)))
                                       .arg(targetSquare / 8 + 1);
                ui->textEdit->append(moveText);
            }

            // Update the board to reflect the new state
            updateBoardFromGame();

            // Add a delayed update for consistent sizing
            QTimer::singleShot(50, this, [this]()
                               {
                                   forceUpdatePieceSizes(); // Force the piece sizes to be consistent
                               });

            // Check for checkmate in the main game regardless of free move mode
            if (activeGame)
            {
                // Only check for checkmate in normal mode, not in free move mode
                if (!freeMoveMode)
                {
                    // More thorough check before declaring checkmate
                    bool whiteInCheckmate = false;
                    bool blackInCheckmate = false;
                    bool whiteKingHasMoves = false;
                    bool blackKingHasMoves = false;

                    try
                    {
                        // Check if kings have any valid moves using our safe helper methods
                        uint64_t whiteKingMoves = activeGame->GetWhiteKingMoves();
                        whiteKingHasMoves = whiteKingMoves != 0;

                        uint64_t blackKingMoves = activeGame->GetBlackKingMoves();
                        blackKingHasMoves = blackKingMoves != 0;

                        // Standard checkmate detection
                        whiteInCheckmate = activeGame->IsWin(white);
                        blackInCheckmate = activeGame->IsWin(black);

                        qDebug() << "Main game checkmate detection:"
                                 << "White in checkmate:" << whiteInCheckmate
                                 << "White king has moves:" << whiteKingHasMoves
                                 << "Black in checkmate:" << blackInCheckmate
                                 << "Black king has moves:" << blackKingHasMoves;
                    }
                    catch (const std::exception &e)
                    {
                        qDebug() << "Error in checkmate detection:" << e.what();
                    }

                    // Only declare checkmate if IsWin returns true
                    if (whiteInCheckmate)
                    {
                        showGameOver(true);
                    }
                    else if (blackInCheckmate)
                    {
                        showGameOver(false);
                    }
                }
            }
        }
        catch (const std::exception &e)
        {
            // Invalid move, log error
            ui->textEdit->append(QString("Invalid move: %1").arg(e.what()));
        }
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
    qDebug() << "updateBoardFromGame called, allLabels.size():" << allLabels.size();
    if (allLabels.empty())
    {
        qDebug() << "ERROR: No labels available to update";
        return;
    }

    CustomRecursiveChessGame *activeGame = cg->GetActiveGame();
    if (!activeGame)
    {
        qDebug() << "ERROR: No active game available";
        return;
    }

    // Find the chessboard widget to get its size
    QWidget *chesswdg = findChild<QWidget *>("chesswdg");
    if (!chesswdg)
    {
        qDebug() << "ERROR: Could not find chesswdg widget";
        return;
    }

    int boardSize = qMin(chesswdg->width(), chesswdg->height());
    int squareSize = boardSize / 8;
    qDebug() << "Board size:" << boardSize << "Square size:" << squareSize;

    // For each square on the board
    for (int i = 0; i < 64 && i < allLabels.size(); i++)
    {
        DraggableLabel *label = allLabels[i];
        if (!label)
        {
            qDebug() << "ERROR: Null label at index" << i;
            continue;
        }

        // Determine square color (dark or light)
        int row = i / 8;
        int col = i % 8;
        bool isDark = (row + col) % 2 != 0;
        QString squareColor = isDark ? "#B58863" : "#F0D9B5";

        // Get parent square
        QFrame *square = nullptr;
        if (i < allFrames.size())
            square = allFrames[i];

        if (square)
        {
            square->setStyleSheet(QString("background-color: %1;").arg(squareColor));
        }

        // Convert index to Square enum
        char file = 'a' + col;
        char rank = '1' + row;
        std::string squareStr = std::string(1, file) + std::string(1, rank);
        Square square_enum = square_from_string(squareStr);

        // Check if there's a piece at this square
        U64 squareBit = 1ULL << square_enum;
        bool hasPiece = (activeGame->GetBoard() & squareBit) != 0;

        if (!hasPiece)
        {
            // Empty square
            qDebug() << "Empty square at" << squareStr.c_str() << "index" << i;
            label->clear();
            label->setPixmap(QPixmap());
            QString baseStyle = QString("background-color: transparent;");
            label->setStyleSheet(baseStyle);
            label->setProperty("originalStyle", baseStyle);

            // Reset any margins
            label->setContentsMargins(0, 0, 0, 0);
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

            QString imagePath = QString(":/img/%1_%2.png").arg(colorStr, pieceStr);
            QPixmap piecePixmap(imagePath);

            if (piecePixmap.isNull())
            {
                qDebug() << "ERROR: Failed to load piece image:" << imagePath;
                continue;
            }

            qDebug() << "Setting piece at" << squareStr.c_str() << "to" << colorStr << pieceStr;

            // Use scale factor to size the pieces
            int pieceSize = squareSize * PIECE_SCALE_FACTOR;
            QPixmap scaledPixmap = piecePixmap.scaled(pieceSize, pieceSize,
                                                      Qt::KeepAspectRatio,
                                                      Qt::SmoothTransformation);

            // Set transparent background for the label
            QString baseStyle = QString("background-color: transparent;");
            label->setStyleSheet(baseStyle);
            label->setProperty("originalStyle", baseStyle);

            // Set the pixmap for the piece
            label->setPixmap(scaledPixmap);
            label->setAlignment(Qt::AlignCenter);

            // Add margins to make pieces appear smaller within squares
            int margin = squareSize / 12; // Smaller margin (1/12 of square size)
            label->setContentsMargins(margin, margin, margin, margin);

            // Enable dragging for the piece
            label->setDraggable(true);
        }
    }

    qDebug() << "Board update completed";
}

void MainWindow::showGameOver(bool isWhiteWinner)
{
    // Create a message box to display the winner
    QMessageBox msgBox;
    msgBox.setWindowTitle("Game Over");

    // Set the text based on who won
    if (isWhiteWinner)
    {
        msgBox.setText("White wins by checkmate!");
    }
    else
    {
        msgBox.setText("Black wins by checkmate!");
    }

    // Add stylesheets for better appearance
    msgBox.setStyleSheet("QMessageBox { background-color: #f0f0f0; border: 1px solid #cccccc; }"
                         "QMessageBox QLabel { font-size: 16px; font-weight: bold; color: #333333; }"
                         "QPushButton { background-color: #4CAF50; color: white; "
                         "border-radius: 5px; padding: 8px 16px; font-weight: bold; }"
                         "QPushButton:hover { background-color: #45a049; }");

    // Add buttons for starting a new game or quitting
    msgBox.addButton("New Game", QMessageBox::AcceptRole);
    QPushButton *quitButton = msgBox.addButton("Quit", QMessageBox::RejectRole);

    // Show the message box and get the result
    int result = msgBox.exec();

    // Handle the button clicks
    if (msgBox.clickedButton() == quitButton)
    {
        // Quit the application
        QApplication::quit();
    }
    else
    {
        // Start a new game
        on_actionNew_Game_triggered();
    }
}

void MainWindow::on_actionNew_Game_triggered()
{
    // Reset the chess game to its initial state
    cg->NewGame();

    // Clear any highlight and reset the drag source
    dragSourceSquare = invalid;

    // Update the board to show the new game state
    updateBoardFromGame();

    // Log the new game start
    ui->textEdit->append("New game started");
}

void MainWindow::forceUpdatePieceSizes()
{
    qDebug() << "forceUpdatePieceSizes called, allLabels.size():" << allLabels.size();
    if (allLabels.empty())
    {
        qDebug() << "No labels to update sizes for";
        return;
    }

    // Find the chessboard widget to get its size
    QWidget *chesswdg = findChild<QWidget *>("chesswdg");
    if (!chesswdg)
    {
        qDebug() << "ERROR: Could not find chesswdg widget";
        return;
    }

    // Calculate the board size and square size
    int boardSize = qMin(chesswdg->width(), chesswdg->height());
    int squareSize = boardSize / 8;

    qDebug() << "Board size:" << boardSize << "Square size:" << squareSize;

    // Update all piece sizes
    int updatedCount = 0;
    for (int i = 0; i < allLabels.size(); i++)
    {
        DraggableLabel *label = allLabels[i];
        if (!label || label->pixmap().isNull())
            continue;

        // Get the piece image
        QPixmap originalPixmap = label->pixmap();

        // Use the scale factor for consistent sizing
        int pieceSize = squareSize * PIECE_SCALE_FACTOR;

        // Only resize if necessary (avoid unnecessary operations)
        if (originalPixmap.width() != pieceSize || originalPixmap.height() != pieceSize)
        {
            // Scale to the correct size
            QPixmap scaledPixmap = originalPixmap.scaled(pieceSize, pieceSize,
                                                         Qt::KeepAspectRatio,
                                                         Qt::SmoothTransformation);

            // Update the label
            label->setPixmap(scaledPixmap);
            label->setAlignment(Qt::AlignCenter);

            // Add margins to make pieces appear smaller within squares
            int margin = squareSize / 12; // Smaller margin (1/12 of square size)
            label->setContentsMargins(margin, margin, margin, margin);

            updatedCount++;
        }
    }

    qDebug() << "Updated sizes for" << updatedCount << "pieces";
}

void MainWindow::initializeUI()
{
    qDebug() << "Initializing UI...";

    // Create and setup the UI
    ui->setupUi(this);

    // Initialize the chessboard - REMOVE THE BACKGROUND IMAGE to avoid showing two boards
    ui->chesswdg->setStyleSheet("QWidget#chesswdg { background-color: white; }");
    ui->chesswdg->setObjectName("chesswdg"); // Ensure the object name is set

    // Clear any existing grid layout that might have been created by the UI file
    QLayout *existingLayout = ui->chesswdg->layout();
    if (existingLayout)
    {
        qDebug() << "Clearing existing layout from UI file";
        QLayoutItem *item;
        while ((item = existingLayout->takeAt(0)) != nullptr)
        {
            if (item->widget())
            {
                item->widget()->deleteLater();
            }
            delete item;
        }
        delete existingLayout;
    }

    // Layout initialization
    QWidget *chesswdg = findChild<QWidget *>("chesswdg");
    if (chesswdg)
    {
        qDebug() << "Found chesswdg, setting up chessboard grid";

        // Set a fixed size policy for the chess widget to ensure it occupies proper space
        chesswdg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        chesswdg->setMinimumSize(600, 600);

        QGridLayout *grid = new QGridLayout(chesswdg);
        grid->setSpacing(0);
        grid->setContentsMargins(0, 0, 0, 0);

        // Make the grid layout expand to fill available space
        for (int i = 0; i < 8; i++)
        {
            grid->setRowStretch(i, 1);
            grid->setColumnStretch(i, 1);
        }

        // Create chessboard squares and labels
        for (int r = 0; r < 8; r++)
        {
            for (int c = 0; c < 8; c++)
            {
                int idx = r * 8 + c;
                QFrame *square = new QFrame(chesswdg);
                DraggableLabel *label = new DraggableLabel(square); // Pass square as parent

                // Set fixed size policy for squares to ensure they are uniform
                square->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                square->setMinimumSize(40, 40);

                // Configure square
                square->setObjectName(QString("square%1").arg(idx));

                // Set alternating colors for the checkerboard pattern
                if ((r + c) % 2 == 0)
                {
                    square->setStyleSheet("background-color: #F0D9B5;"); // Light square
                }
                else
                {
                    square->setStyleSheet("background-color: #B58863;"); // Dark square
                }

                // Configure label
                label->setObjectName(QString("label%1").arg(idx));
                label->setAlignment(Qt::AlignCenter);
                label->setContentsMargins(0, 0, 0, 0);

                // Create layout for the square
                QVBoxLayout *layout = new QVBoxLayout(square);
                layout->setContentsMargins(0, 0, 0, 0);
                layout->addWidget(label);

                // Add to grid and to the list of all labels
                grid->addWidget(square, r, c);
                allLabels.push_back(label);
                allFrames.push_back(square);

                qDebug() << "Created square" << square->objectName() << "with label" << label->objectName();
            }
        }

        qDebug() << "Chessboard created with" << allLabels.size() << "labels";
    }
    else
    {
        qDebug() << "ERROR: Could not find chesswdg widget!";
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    qDebug() << "MainWindow constructor started";

    // Initialize the UI
    initializeUI();

    qDebug() << "UI initialized, allLabels size:" << allLabels.size();

    // Set up any other connections needed
    cg = new CustomRecursiveChessGame(this); // Initialize with this window as pointer
    qDebug() << "CustomRecursiveChessGame created";

    // Connect signals/slots for all the labels
    for (auto label : allLabels)
    {
        if (!label)
        {
            qDebug() << "WARNING: Null label found in allLabels";
            continue;
        }

        connect(label, &DraggableLabel::dragStarted, this, &MainWindow::handleDragStarted);
        connect(label, &DraggableLabel::dragEntered, this, &MainWindow::handleDragEntered);
        connect(label, &DraggableLabel::dropOccurred, this, &MainWindow::handleDrop);

        label->setDraggable(true);
        label->setMainWindow(this);

        qDebug() << "Connected label:" << label->objectName() << "draggable set to true";
    }

    // Connect free move mode button
    connect(ui->freeMoveButton, &QPushButton::toggled, this, [this](bool checked)
            {
        freeMoveMode = checked;
        QString modeMessage = checked ? "Free Move Mode: ENABLED" : "Free Move Mode: DISABLED";
        ui->textEdit->append(modeMessage);
        qDebug() << modeMessage; });

    // Initialize the game engine
    cg->NewGame();
    qDebug() << "Game initialized";

    // Update the board with the initial positions
    updateBoardFromGame();
    qDebug() << "Board updated from game";

    // Set window size to ensure the board is properly displayed
    resize(900, 700);

    // Apply the larger piece size immediately after initialization
    QTimer::singleShot(100, this, [this]()
                       {
        QWidget *chesswdg = findChild<QWidget *>("chesswdg");
        if (chesswdg) {
            qDebug() << "Forcing resize on chessboard. Size:" << chesswdg->size();
            // Ensure the widget has been properly sized
            chesswdg->updateGeometry();
            // Force an update after the layout has settled
            forceUpdatePieceSizes();
        } });

    qDebug() << "MainWindow constructor completed";
}

MainWindow::~MainWindow()
{
    delete ui;
    delete cg; // Add this
}
