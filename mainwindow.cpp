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
#include "ChessEngine/AbilitySystem.hpp"
#include <QThread>
#include <QInputDialog>
#include <QButtonGroup>
#include <QRadioButton>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QtGlobal>
#include <QDateTime>
#include <random>
#include <chrono>
#include <iterator>

class CustomSuperChessGame : public SuperChessGame
{
private:
    MainWindow *mainWindow;
    bool inSubGame;
    std::shared_ptr<CustomSuperChessGame> activeSubGame;
    std::vector<U64> savedWhitePieces;
    std::vector<U64> savedBlackPieces;
    Piece attackingPieceType;
    Piece defendingPieceType;

public:
    CustomSuperChessGame(MainWindow *window = nullptr)
        : SuperChessGame(), mainWindow(window), inSubGame(false), activeSubGame(nullptr), attackingPieceType(Pawn), defendingPieceType(Pawn) {}

    // Constructor for subgames with specific piece types
    CustomSuperChessGame(MainWindow *window, const SuperPieceInfo &white_info, const SuperPieceInfo &black_info)
        : SuperChessGame(white_info, black_info), mainWindow(window), inSubGame(false), activeSubGame(nullptr),
          attackingPieceType(Pawn), defendingPieceType(Pawn) {}

    // Helper methods for debugging
    QString pieceToString(Piece piece) const
    {
        switch (piece)
        {
        case Pawn:
            return "Pawn";
        case Knight:
            return "Knight";
        case Bishop:
            return "Bishop";
        case Rook:
            return "Rook";
        case Queen:
            return "Queen";
        case King:
            return "King";
        default:
            return "Unknown";
        }
    }

    QString squareToString(Square sq) const
    {
        if (sq >= 0 && sq < 64)
        {
            char file = 'a' + (sq % 8);
            char rank = '1' + (sq / 8);
            return QString("%1%2").arg(file).arg(rank);
        }
        return "??";
    }

    // UI integration methods - only keep methods that connect UI with engine

    // Get king position using engine data
    Square GetWhiteKingPosition() const
    {
        return static_cast<Square>(get_ls1b_index(this->WhitePiecesArray[King]));
    }

    Square GetBlackKingPosition() const
    {
        return static_cast<Square>(get_ls1b_index(this->BlackPiecesArray[King]));
    }

    // Re-add king moves methods but implement them using engine functionality
    uint64_t GetWhiteKingMoves() const
    {
        try
        {
            Square whiteKingPos = GetWhiteKingPosition();
            return this->GetAttacks(whiteKingPos);
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
            return this->GetAttacks(blackKingPos);
        }
        catch (...)
        {
            return 0;
        }
    }

    // Add helper method to get checking pieces for debugging
    uint64_t GetCheckingPieces(Color kingColor) const
    {
        try
        {
            Color targetColor = (kingColor == white) ? black : white;
            Square kingPos = (kingColor == white) ? GetWhiteKingPosition() : GetBlackKingPosition();
            return ChessGame::InCheck(this->GetBoard(), kingColor, 0);
        }
        catch (...)
        {
            return 0;
        }
    }

    // Helper function to get the piece type at a given square (for debugging)
    Piece GetPieceAtSquare(Square sq) const
    {
        U64 bit = 1ULL << sq;
        if (!(bit & board))
        {
            // Return an actual Piece type as default
            // Using Pawn as a placeholder since there's no "invalid" Piece value
            return Pawn;
        }
        return ChessGame::GetPieceType(bit);
    }

    // Display a bitboard in a human-readable form for debugging
    QString BitboardToString(U64 bitboard) const
    {
        QString result = "\n";
        for (int rank = 7; rank >= 0; rank--)
        {
            result += QString("%1 ").arg(rank + 1);
            for (int file = 0; file < 8; file++)
            {
                int sq = rank * 8 + file;
                result += (bitboard & (1ULL << sq)) ? "X " : ". ";
            }
            result += "\n";
        }
        result += "  a b c d e f g h\n";
        return result;
    }

    // UI debug method
    void LogPiecePositions() const
    {
        if (!mainWindow)
            return;

        auto formatBitboard = [](U64 bb) -> QString
        {
            return QString("0x%1").arg(bb, 16, 16, QChar('0'));
        };

        mainWindow->appendToLog("White: " + formatBitboard(this->WhitePieces));
        mainWindow->appendToLog("Black: " + formatBitboard(this->BlackPieces));
        mainWindow->appendToLog("Board: " + formatBitboard(this->board));

        for (size_t i = 0; i < this->WhitePiecesArray.size(); i++)
        {
            mainWindow->appendToLog(QString("White %1: %2").arg(i).arg(formatBitboard(this->WhitePiecesArray[i])));
        }

        for (size_t i = 0; i < this->BlackPiecesArray.size(); i++)
        {
            mainWindow->appendToLog(QString("Black %1: %2").arg(i).arg(formatBitboard(this->BlackPiecesArray[i])));
        }
    }

    void NewGame()
    {
        // Reset to starting position
        this->WhitePiecesArray = {
            0x000000000000FF00ULL, // Pawns
            0x0000000000000042ULL, // Knights
            0x0000000000000024ULL, // Bishops
            0x0000000000000081ULL, // Rooks
            0x0000000000000008ULL, // Queen
            0x0000000000000010ULL  // King
        };

        this->BlackPiecesArray = {
            0x00FF000000000000ULL, // Pawns
            0x4200000000000000ULL, // Knights
            0x2400000000000000ULL, // Bishops
            0x8100000000000000ULL, // Rooks
            0x0800000000000000ULL, // Queen
            0x1000000000000000ULL  // King
        };

        this->UpdateBoard();
        inSubGame = false;
        activeSubGame = nullptr;
    }

    void MovePiece(Square from, Square to)
    {
        qDebug() << "MovePiece called:" << from << "to" << to;
        U64 from_bit = 1ULL << from;
        U64 to_bit = 1ULL << to;

        // Get the active game that should be handling this move
        CustomSuperChessGame *activeGame = GetActiveGame();
        if (!activeGame)
        {
            qDebug() << "ERROR: No active game available";
            return;
        }

        // Don't allow moving to squares that have our own pieces
        if (!inSubGame)
        {
            Color color = this->GetColor(from_bit);
            U64 ownPieces = (color == white) ? this->WhitePieces : this->BlackPieces;
            if (to_bit & ownPieces)
            {
                if (mainWindow)
                    mainWindow->appendToLog("Can't capture your own pieces!");
                return;
            }
        }

        // We're in a subgame
        qDebug() << "Before Move call";
        // First, print the current state of the board for debugging
        qDebug() << "White pieces:" << activeGame->GetBoardOf(white);
        qDebug() << "Black pieces:" << activeGame->GetBoardOf(black);
        qDebug() << "From piece exists:" << ((activeGame->board & from_bit) != 0);

        std::vector<Action> actions = activeGame->Move(from, to);
        qDebug() << "After Move call, actions size:" << actions.size();

        // Print the state after the move
        qDebug() << "White pieces:" << activeGame->GetBoardOf(white);
        qDebug() << "Black pieces:" << activeGame->GetBoardOf(black);
        qDebug() << "To piece exists:" << ((activeGame->board & to_bit) != 0);

        if (actions.empty())
        {
            if (mainWindow)
                mainWindow->appendToLog("Invalid move!");
            return;
        }

        if (mainWindow)
        {
            QString fromStr = QString::fromStdString(square_to_string(from));
            QString toStr = QString::fromStdString(square_to_string(to));
            mainWindow->appendToLog(QString("Moving %1 to %2").arg(fromStr).arg(toStr));

            // Force UI update after move
            qDebug() << "Calling updateBoardFromGame from MovePiece";
            mainWindow->updateBoardFromGame();

            // Also force an immediate refresh of the board
            QTimer::singleShot(0, mainWindow, &MainWindow::forceUpdatePieceSizes);
        }
    }

    Piece PromoteInput(Square from_sq, Square to_sq, Color color, Piece to_piece) override
    {
        if (mainWindow)
        {
            return mainWindow->getPromotionPiece(color);
        }
        return Queen; // Default if no UI available
    }

    bool IsInSubGame() const { return inSubGame; }

    CustomSuperChessGame *GetActiveGame()
    {
        if (inSubGame && activeSubGame)
            return activeSubGame.get();
        return this;
    }

    // Sub-game handling with super pieces
    void StartSubGame(Square from, Square to, Color attacker)
    {
        // Save current game state
        savedWhitePieces = this->WhitePiecesArray;
        savedBlackPieces = this->BlackPiecesArray;

        // Store the attacking piece's type before creating sub-game
        U64 attackingPieceBit = 1ULL << from;
        U64 defendingPieceBit = 1ULL << to;

        attackingPieceType = this->GetPieceType(attackingPieceBit);
        defendingPieceType = this->GetPieceType(defendingPieceBit);

        // Create appropriate SuperPieceInfo for both pieces
        SuperPieceInfo attackerInfo = std::make_pair(attackingPieceType, Tier::T1);
        SuperPieceInfo defenderInfo = std::make_pair(defendingPieceType, Tier::T1);

        // Create new sub-game with super pieces
        activeSubGame = std::make_shared<CustomSuperChessGame>(mainWindow, attackerInfo, defenderInfo);

        // Make sure the board is initialized with a standard chess position
        activeSubGame->NewGame();

        // Add debug output to verify the subgame's initial state
        qDebug() << "SubGame initialized with standard position";
        qDebug() << "White pieces:" << activeSubGame->GetBoardOf(white);
        qDebug() << "Black pieces:" << activeSubGame->GetBoardOf(black);

        // Set subgame properties
        activeSubGame->capture_from = from;
        activeSubGame->capture_to = to;
        activeSubGame->attacker_color = attacker;
        inSubGame = true;

        // Log the start of the subgame
        if (mainWindow)
        {
            mainWindow->appendToLog("Super Chess game started for capture resolution");
            mainWindow->appendToLog(QString("Attacker: %1 (%2) at %3")
                                        .arg(pieceToString(attackingPieceType))
                                        .arg(attacker == white ? "White" : "Black")
                                        .arg(squareToString(from)));
            mainWindow->appendToLog(QString("Defender: %1 (%2) at %3")
                                        .arg(pieceToString(defendingPieceType))
                                        .arg(attacker == white ? "Black" : "White")
                                        .arg(squareToString(to)));

            // Update the ability description label on the right with both pieces' abilities
            QString abilityText;

            // Only show each piece type once if they're the same
            if (attackingPieceType == defendingPieceType)
            {
                abilityText = QString("<b>%1 Abilities:</b><br>%2")
                                  .arg(pieceToString(attackingPieceType))
                                  .arg(getAbilityDescription(attackingPieceType).replace("\n", "<br>"));
            }
            else
            {
                abilityText = QString("<b>%1 %2 Abilities:</b><br>%3<br><br><b>%4 %5 Abilities:</b><br>%6")
                                  .arg(attacker == white ? "White" : "Black")
                                  .arg(pieceToString(attackingPieceType))
                                  .arg(getAbilityDescription(attackingPieceType).replace("\n", "<br>"))
                                  .arg(attacker == white ? "Black" : "White")
                                  .arg(pieceToString(defendingPieceType))
                                  .arg(getAbilityDescription(defendingPieceType).replace("\n", "<br>"));
            }

            // Use the new helper method to update the ability description
            mainWindow->updateAbilityDescription(abilityText, true);

            // Show a special dialog for the super chess game
            showSuperChessDialog(attackingPieceType, defendingPieceType, attacker);

            // Force UI update to sync with the new game state
            mainWindow->updateBoardFromGame();

            // Force the chess board to immediately update
            QTimer::singleShot(0, mainWindow, &MainWindow::forceUpdatePieceSizes);
        }
    }

    void EndSubGame()
    {
        if (inSubGame && activeSubGame)
        {
            // Check if there was a winner and handle piece removal
            Color winner = white; // Default, will need to implement proper win checking for SuperChessGame

            // For now, use a simple check - if white king is missing, black won
            if (activeSubGame->GetBoardOf(King, white) == 0)
            {
                winner = black;
            }
            // If black king is missing, white won
            else if (activeSubGame->GetBoardOf(King, black) == 0)
            {
                winner = white;
            }

            // Determine if attacker won based on the initial setup
            Color attackerColor = (activeSubGame->GetBoardOf(attackingPieceType, white) != 0) ? white : black;

            if (winner == attackerColor)
            {
                // Attacker won - remove defending piece and move attacking piece
                RemovePiece(activeSubGame->capture_to);
                ExecuteMove(attackerColor,
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

            // Clean up sub-game
            activeSubGame = nullptr;
            inSubGame = false;
            this->UpdateBoard();

            if (mainWindow)
            {
                mainWindow->appendToLog("Super Chess game ended");

                // Reset the ability description label when subgame ends
                mainWindow->resetAbilityDescription();
            }
        }
    }

    uint64_t get_moves_bitboard(Square square)
    {
        if (!inSubGame)
        {
            // For the main game, get all valid moves for the piece at the given square
            return GetAttacks(square);
        }
        else
        {
            // We're in a sub-game, let the active sub-game handle it
            CustomSuperChessGame *activeGame = GetActiveGame();
            return activeGame->GetAttacks(square);
        }
    }

    void make_move(Square from, Square to)
    {
        MovePiece(from, to);
    }

    bool IsKingInCheck(bool isWhiteKing)
    {
        Color kingColor = isWhiteKing ? white : black;
        return InCheck(kingColor);
    }

    // Add IsWin method since it's used in many places
    bool IsWin(Color color) const
    {
        // For simplicity, we'll check if the king of the given color is not on the board
        Color kingColor = (color == white) ? black : white;
        U64 kingBitboard = (kingColor == white) ? this->WhitePiecesArray[King] : this->BlackPiecesArray[King];
        return kingBitboard == 0;
    }

    // Helper function for UI
    std::string square_to_string(Square sq)
    {
        if (sq >= 0 && sq < 64)
        {
            char file = 'a' + (sq % 8);
            char rank = '1' + (sq / 8);
            return std::string{file, rank};
        }
        return "??";
    }

    // For UI interactions
    Square capture_from;
    Square capture_to;
    Color attacker_color;

    // Display a dialog showing the super chess game and abilities
    void showSuperChessDialog(Piece attackerPiece, Piece defenderPiece, Color attackerColor)
    {
        if (!mainWindow)
            return;

        QDialog dialog(mainWindow);
        dialog.setWindowTitle("Super Chess Battle");
        dialog.setMinimumSize(400, 300);

        QVBoxLayout *layout = new QVBoxLayout(&dialog);

        // Title
        QLabel *titleLabel = new QLabel("Super Chess Battle!");
        titleLabel->setAlignment(Qt::AlignCenter);
        QFont titleFont = titleLabel->font();
        titleFont.setPointSize(16);
        titleFont.setBold(true);
        titleLabel->setFont(titleFont);
        layout->addWidget(titleLabel);

        // Description
        QLabel *descLabel = new QLabel("The pieces have transformed into Super Pieces with special abilities!");
        descLabel->setWordWrap(true);
        descLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(descLabel);

        // Check if both pieces are the same type
        if (attackerPiece == defenderPiece)
        {
            // If they're the same type, only show one frame with abilities
            QFrame *pieceFrame = new QFrame();
            pieceFrame->setFrameShape(QFrame::StyledPanel);
            QVBoxLayout *pieceLayout = new QVBoxLayout(pieceFrame);

            QLabel *pieceTitle = new QLabel(QString("%1 Abilities")
                                                .arg(pieceToString(attackerPiece)));
            pieceTitle->setAlignment(Qt::AlignCenter);
            QFont boldFont = pieceTitle->font();
            boldFont.setBold(true);
            pieceTitle->setFont(boldFont);
            pieceLayout->addWidget(pieceTitle);

            // Abilities
            QLabel *pieceAbilities = new QLabel(getAbilityDescription(attackerPiece));
            pieceAbilities->setWordWrap(true);
            pieceLayout->addWidget(pieceAbilities);

            layout->addWidget(pieceFrame);

            // Show which piece belongs to which player
            QLabel *playerInfo = new QLabel(QString("%1 %2 vs %3 %4")
                                                .arg(attackerColor == white ? "White" : "Black")
                                                .arg(pieceToString(attackerPiece))
                                                .arg(attackerColor == white ? "Black" : "White")
                                                .arg(pieceToString(defenderPiece)));
            playerInfo->setAlignment(Qt::AlignCenter);
            layout->addWidget(playerInfo);
        }
        else
        {
            // Attacker details
            QFrame *attackerFrame = new QFrame();
            attackerFrame->setFrameShape(QFrame::StyledPanel);
            QVBoxLayout *attackerLayout = new QVBoxLayout(attackerFrame);

            QLabel *attackerTitle = new QLabel(QString("%1 %2")
                                                   .arg(attackerColor == white ? "White" : "Black")
                                                   .arg(pieceToString(attackerPiece)));
            attackerTitle->setAlignment(Qt::AlignCenter);
            QFont boldFont = attackerTitle->font();
            boldFont.setBold(true);
            attackerTitle->setFont(boldFont);
            attackerLayout->addWidget(attackerTitle);

            // Attacker abilities
            QLabel *attackerAbilities = new QLabel(getAbilityDescription(attackerPiece));
            attackerAbilities->setWordWrap(true);
            attackerLayout->addWidget(attackerAbilities);

            layout->addWidget(attackerFrame);

            // Defender details
            QFrame *defenderFrame = new QFrame();
            defenderFrame->setFrameShape(QFrame::StyledPanel);
            QVBoxLayout *defenderLayout = new QVBoxLayout(defenderFrame);

            QLabel *defenderTitle = new QLabel(QString("%1 %2")
                                                   .arg(attackerColor == white ? "Black" : "White")
                                                   .arg(pieceToString(defenderPiece)));
            defenderTitle->setAlignment(Qt::AlignCenter);
            defenderTitle->setFont(boldFont);
            defenderLayout->addWidget(defenderTitle);

            // Defender abilities
            QLabel *defenderAbilities = new QLabel(getAbilityDescription(defenderPiece));
            defenderAbilities->setWordWrap(true);
            defenderLayout->addWidget(defenderAbilities);

            layout->addWidget(defenderFrame);
        }

        // Battle instructions
        QLabel *instructionsLabel = new QLabel("Play the mini-game to determine if the capture succeeds!");
        instructionsLabel->setWordWrap(true);
        instructionsLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(instructionsLabel);

        // OK button
        QPushButton *okButton = new QPushButton("Start Battle");
        layout->addWidget(okButton);

        QObject::connect(okButton, &QPushButton::clicked, &dialog, &QDialog::accept);

        dialog.setLayout(layout);
        dialog.exec();
    }

    // Return a description of abilities based on piece type
    QString getAbilityDescription(Piece piece) const
    {
        switch (piece)
        {
        case Pawn:
            return "• Two Tiles: Can move forward two squares in one turn.\n"
                   "• Convert: Can convert captured enemy pieces to become friendly pieces.\n"
                   "• Two Tiles Backwards: Can move backwards two squares in one turn.\n"
                   "• Convert Super: Can convert captured pieces into super pieces.";
        case Knight:
            return "• Big L: Extended jump movement in an L-shape pattern.\n"
                   "• Protection: Gains immunity to capture when adjacent to friendly pieces.\n"
                   "• Knight Tier 2: Special ability that enhances knight's attack potential.";
        case Bishop:
            return "• Swap: Can swap positions with another friendly piece.\n"
                   "• Take Cover: Can move behind friendly pieces for protection.\n"
                   "• Resurrect: Can bring back a captured piece from the graveyard.";
        case Rook:
            return "• Swap: Can swap positions with another friendly piece.\n"
                   "• Ram Buff: Gains attack power when aligned with other rooks.\n"
                   "• Rook Tier 2: Special ability that extends rook's control of the board.";
        case Queen:
            return "• Inspire: Boosts movement abilities of nearby friendly pieces.\n"
                   "• Kamikaze: Can sacrifice itself to eliminate multiple enemy pieces in one move.";
        case King:
            return "• Inspire: Enhances attack power of nearby friendly pieces.\n"
                   "• Turn Into Dead: Can transform enemy pieces to harmless state.\n"
                   "• Teleport: Can instantly move to any empty square on the board.\n"
                   "• Sniper Shot: Can attack pieces at a distance without moving.\n"
                   "• Convert: Can turn enemy pieces into friendly pieces.";
        default:
            return "No special abilities";
        }
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
    CustomSuperChessGame *activeGame = cg->GetActiveGame();
    bool isCapture = false;
    U64 squareBit = 1ULL << targetSquare;
    isCapture = (activeGame->GetBoardOf(white) | activeGame->GetBoardOf(black)) & squareBit;

    if (isCapture && !cg->IsInSubGame())
    {
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
            CustomSuperChessGame *activeGame = cg->GetActiveGame();
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
                        SuperChessGame* activeGame = cg->GetActiveGame();
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
                        SuperChessGame* activeGame = cg->GetActiveGame();
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

                        // Enhanced debugging - show positions and attack information
                        Square whiteKingPos = activeGame->GetWhiteKingPosition();
                        Square blackKingPos = activeGame->GetBlackKingPosition();

                        // Get checking pieces for each king
                        uint64_t whiteKingCheckers = activeGame->GetCheckingPieces(white);
                        uint64_t blackKingCheckers = activeGame->GetCheckingPieces(black);

                        qDebug() << "Enhanced checkmate debugging:";
                        qDebug() << "White king at" << QChar('a' + (whiteKingPos % 8)) << (whiteKingPos / 8 + 1)
                                 << "has" << __builtin_popcountll(whiteKingMoves) << "moves";
                        qDebug() << "Black king at" << QChar('a' + (blackKingPos % 8)) << (blackKingPos / 8 + 1)
                                 << "has" << __builtin_popcountll(blackKingMoves) << "moves";

                        // Visualize king's legal moves
                        qDebug() << "White king's legal moves:" << activeGame->BitboardToString(whiteKingMoves);
                        qDebug() << "Black king's legal moves:" << activeGame->BitboardToString(blackKingMoves);

                        qDebug() << "White king has" << __builtin_popcountll(whiteKingCheckers) << "checking pieces";
                        qDebug() << "Black king has" << __builtin_popcountll(blackKingCheckers) << "checking pieces";

                        // For each checking piece, show its type and position
                        uint64_t checkers = whiteKingCheckers;
                        while (checkers)
                        {
                            int checkerIndex = get_ls1b_index(checkers);
                            Square checkerSquare = static_cast<Square>(checkerIndex);
                            Piece checkerType = activeGame->GetPieceAtSquare(checkerSquare);

                            // Convert piece type to string manually
                            QString pieceTypeStr;
                            switch (checkerType)
                            {
                            case Pawn:
                                pieceTypeStr = "Pawn";
                                break;
                            case Knight:
                                pieceTypeStr = "Knight";
                                break;
                            case Bishop:
                                pieceTypeStr = "Bishop";
                                break;
                            case Rook:
                                pieceTypeStr = "Rook";
                                break;
                            case Queen:
                                pieceTypeStr = "Queen";
                                break;
                            case King:
                                pieceTypeStr = "King";
                                break;
                            default:
                                pieceTypeStr = "Unknown";
                                break;
                            }

                            qDebug() << "White king is checked by" << pieceTypeStr
                                     << "at" << QChar('a' + (checkerSquare % 8)) << (checkerSquare / 8 + 1);
                            checkers &= checkers - 1; // Remove the least significant bit
                        }

                        checkers = blackKingCheckers;
                        while (checkers)
                        {
                            int checkerIndex = get_ls1b_index(checkers);
                            Square checkerSquare = static_cast<Square>(checkerIndex);
                            Piece checkerType = activeGame->GetPieceAtSquare(checkerSquare);

                            // Convert piece type to string manually
                            QString pieceTypeStr;
                            switch (checkerType)
                            {
                            case Pawn:
                                pieceTypeStr = "Pawn";
                                break;
                            case Knight:
                                pieceTypeStr = "Knight";
                                break;
                            case Bishop:
                                pieceTypeStr = "Bishop";
                                break;
                            case Rook:
                                pieceTypeStr = "Rook";
                                break;
                            case Queen:
                                pieceTypeStr = "Queen";
                                break;
                            case King:
                                pieceTypeStr = "King";
                                break;
                            default:
                                pieceTypeStr = "Unknown";
                                break;
                            }

                            qDebug() << "Black king is checked by" << pieceTypeStr
                                     << "at" << QChar('a' + (checkerSquare % 8)) << (checkerSquare / 8 + 1);
                            checkers &= checkers - 1; // Remove the least significant bit
                        }

                        qDebug() << "Main game checkmate detection:"
                                 << "White in checkmate:" << whiteInCheckmate
                                 << "White king has moves:" << whiteKingHasMoves
                                 << "Black in checkmate:" << blackInCheckmate
                                 << "Black king has moves:" << blackKingHasMoves;

                        // If a king is in check but has moves, log the moves for debugging
                        if (__builtin_popcountll(whiteKingCheckers) > 0 && whiteKingHasMoves)
                        {
                            qDebug() << "White king is in check but has moves - possible bug in checkmate detection";
                        }
                        if (__builtin_popcountll(blackKingCheckers) > 0 && blackKingHasMoves)
                        {
                            qDebug() << "Black king is in check but has moves - possible bug in checkmate detection";
                        }
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

Piece MainWindow::getPromotionPiece(Color color)
{
    QDialog dialog(this);
    dialog.setWindowTitle("Pawn Promotion");
    dialog.setModal(true);

    QVBoxLayout layout(&dialog);
    layout.addWidget(new QLabel("Choose a piece to promote to:"));

    QButtonGroup group;
    QRadioButton *queenButton = new QRadioButton("Queen");
    QRadioButton *rookButton = new QRadioButton("Rook");
    QRadioButton *bishopButton = new QRadioButton("Bishop");
    QRadioButton *knightButton = new QRadioButton("Knight");

    queenButton->setChecked(true); // Default to queen

    group.addButton(queenButton);
    group.addButton(rookButton);
    group.addButton(bishopButton);
    group.addButton(knightButton);

    layout.addWidget(queenButton);
    layout.addWidget(rookButton);
    layout.addWidget(bishopButton);
    layout.addWidget(knightButton);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok, Qt::Horizontal, &dialog);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    layout.addWidget(&buttonBox);

    dialog.setLayout(&layout);
    dialog.exec();

    if (queenButton->isChecked())
        return Queen;
    if (rookButton->isChecked())
        return Rook;
    if (bishopButton->isChecked())
        return Bishop;
    if (knightButton->isChecked())
        return Knight;

    return Queen; // Default if somehow nothing was selected
}

void MainWindow::updateBoardFromGame()
{
    qDebug() << "updateBoardFromGame called, allLabels.size():" << allLabels.size();
    if (allLabels.empty())
    {
        qDebug() << "ERROR: No labels available to update";
        return;
    }

    CustomSuperChessGame *activeGame = cg->GetActiveGame();
    if (!activeGame)
    {
        qDebug() << "ERROR: No active game available";
        return;
    }

    // Debug output to show the current board state
    qDebug() << "White pieces bitboard:" << activeGame->GetBoardOf(white);
    qDebug() << "Black pieces bitboard:" << activeGame->GetBoardOf(black);

    // Print out an ASCII representation of the board for debugging
    qDebug() << "Current board state:";
    for (int row = 7; row >= 0; row--)
    {
        QString rowStr = QString::number(row + 1) + " ";
        for (int col = 0; col < 8; col++)
        {
            Square sq = static_cast<Square>(row * 8 + col);
            U64 squareBit = 1ULL << sq;

            if ((activeGame->GetBoardOf(white) & squareBit) != 0)
            {
                // White piece
                Piece p = activeGame->GetPieceType(squareBit);
                switch (p)
                {
                case Pawn:
                    rowStr += "P ";
                    break;
                case Knight:
                    rowStr += "N ";
                    break;
                case Bishop:
                    rowStr += "B ";
                    break;
                case Rook:
                    rowStr += "R ";
                    break;
                case Queen:
                    rowStr += "Q ";
                    break;
                case King:
                    rowStr += "K ";
                    break;
                default:
                    rowStr += "? ";
                    break;
                }
            }
            else if ((activeGame->GetBoardOf(black) & squareBit) != 0)
            {
                // Black piece
                Piece p = activeGame->GetPieceType(squareBit);
                switch (p)
                {
                case Pawn:
                    rowStr += "p ";
                    break;
                case Knight:
                    rowStr += "n ";
                    break;
                case Bishop:
                    rowStr += "b ";
                    break;
                case Rook:
                    rowStr += "r ";
                    break;
                case Queen:
                    rowStr += "q ";
                    break;
                case King:
                    rowStr += "k ";
                    break;
                default:
                    rowStr += "? ";
                    break;
                }
            }
            else
            {
                // Empty square
                rowStr += ". ";
            }
        }
        qDebug() << rowStr;
    }
    qDebug() << "  a b c d e f g h";

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

    // Force a board update
    chesswdg->update();

    // Temporarily block signals to avoid cascading updates
    bool wasBlocked = chesswdg->blockSignals(true);

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

        // Check if there's a piece at this square in the active game
        U64 squareBit = 1ULL << square_enum;
        bool hasPiece = (activeGame->GetBoardOf(white) | activeGame->GetBoardOf(black)) & squareBit;

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

            // Disable dragging for empty squares
            label->setDraggable(false);
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

    // Restore signal blocking state
    chesswdg->blockSignals(wasBlocked);

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
    cg = new CustomSuperChessGame(this);
    cg->NewGame();

    // Clear any highlight and reset the drag source
    dragSourceSquare = invalid;

    // Reset the ability description
    resetAbilityDescription();

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
    cg = new CustomSuperChessGame(this); // Initialize with this window as pointer
    cg->NewGame();                       // Initialize the game board
    qDebug() << "CustomSuperChessGame created";

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

void MainWindow::appendToLog(const QString &message)
{
    if (ui && ui->textEdit)
    {
        ui->textEdit->append(message);
    }
}

// Add implementation for the ability description update methods
void MainWindow::updateAbilityDescription(const QString &text, bool isHtml)
{
    if (ui && ui->label_67)
    {
        ui->label_67->setText(text);
        if (isHtml)
        {
            ui->label_67->setTextFormat(Qt::RichText);
        }
        else
        {
            ui->label_67->setTextFormat(Qt::AutoText);
        }
        ui->label_67->setWordWrap(true);
        ui->label_67->setMinimumHeight(200);
    }
}

void MainWindow::resetAbilityDescription()
{
    if (ui && ui->label_67)
    {
        ui->label_67->setText("Ability Description:");
        ui->label_67->setTextFormat(Qt::AutoText);
    }
}
