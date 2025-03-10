#include "AbilitySystem.hpp"
#include <iostream>

// Ability class implementation
Ability::Ability(const std::string &name_, const AbilityType &type_, SuperChessGame &game_, Logger &log_)
    : name(name_), type(type_), game(game_), log(log_)
{
}

Ability::Ability(const std::string &name_, const AbilityType &type_, SuperChessGame &game_, Logger &log_, int cooldown_, int activation_turn_)
    : name(name_), type(type_), game(game_), log(log_), cooldown(cooldown_), activation_turn(activation_turn_)
{
}

std::string Ability::GetName() const
{
    return name;
}

AbilityType Ability::GetType() const
{
    return type;
}

bool Ability::Effect(const SuperPiece &piece)
{
    std::cout << "Normal effect" << std::endl;
    return true;
}

void Ability::Modify(U64 &move, Square piece)
{
    std::cout << "Normal modify" << std::endl;
}

bool Ability::OnCapture(SuperPiece &piece, Square to_capture, Square from_sq, std::vector<Square> &keys_to_remove)
{
    std::cout << "Normal on capture" << std::endl;
    return false;
}

void Ability::OnGameStart(SuperPiece &piece)
{
    std::cout << "Normal on game start" << std::endl;
}

Square Ability::GetInputSquare(const std::string &prompt) const
{
    std::string input_str;
    Square sq;
    std::cout << prompt;

    std::cin >> input_str;
    std::cout << std::endl;
    auto it = SqStrMap.find(input_str);
    if (it != SqStrMap.end())
        sq = it->second;
    else
    {
        return Square::invalid;
    }
    return sq;
}

int Ability::GetCooldownTracker() const
{
    return log.CalculateTurnDiff(turn_last_used_ability);
}

std::unique_ptr<Ability> Ability::Clone() const
{
    return nullptr; // Base implementation
}

// SuperPiece implementation
SuperPiece::SuperPiece(std::vector<std::unique_ptr<Ability>> &abilities_, SuperPieceInfo info_, Square sq_, Color color_)
    : info(info_), sq(sq_), color(color_)
{
    AddAbilities(abilities_);
}

bool SuperPiece::UseAbility(Tier key)
{
    auto it = abilities.find(key);
    if (it != abilities.end())
    {
        return it->second->Effect(*this);
    }

    return false;
}

void SuperPiece::ModifyMove(U64 &move)
{
    for (const auto &pair : abilities)
    {
        if (pair.second->GetType() == AbilityType::move_modifer)
            pair.second->Modify(move, sq);
    }
}

bool SuperPiece::OnCaptureEffects(Square to_capture, Square from_sq, std::vector<Square> &keys_to_remove)
{
    bool success = false;

    for (const auto &pair : abilities)
    {
        if ((pair.second->GetType() == AbilityType::on_capture) && pair.second->OnCapture(*this, to_capture, from_sq, keys_to_remove))
        {
            success = true;
        }
    }
    return success;
}

void SuperPiece::OnGameStartEffects()
{
    for (const auto &pair : abilities)
    {
        if (pair.second->GetType() == AbilityType::on_game_start)
        {
            pair.second->OnGameStart(*this);
        }
    }
}

void SuperPiece::UpdateSquare(Square to_sq)
{
    sq = to_sq;
}

void SuperPiece::UpdateColor(Color new_color)
{
    color = new_color;
}

void SuperPiece::UpdateTier(Tier t)
{
    info.second = t;
}

void SuperPiece::AddAbilities(std::vector<std::unique_ptr<Ability>> &abilities_)
{
    for (auto &a : abilities_)
    {
        abilities[info.second] = std::move(a);
    }
}

SuperPieceInfo SuperPiece::GetInfo() const
{
    return info;
}

Square SuperPiece::GetSquare() const
{
    return sq;
}

Color SuperPiece::GetColor() const
{
    return color;
}

Piece SuperPiece::GetPieceType() const
{
    return info.first;
}

Tier SuperPiece::GetTier() const
{
    return info.second;
}

std::vector<std::string> SuperPiece::GetAbilityNames() const
{
    std::vector<std::string> names;
    for (const auto &pair : abilities)
    {
        names.push_back(pair.second->GetName());
    }
    return names;
}

std::vector<std::string> SuperPiece::GetAbilityNames(AbilityType type) const
{
    std::vector<std::string> names;
    for (const auto &pair : abilities)
    {
        if (pair.second->GetType() == type)
            names.push_back(pair.second->GetName());
    }
    return names;
}

int SuperPiece::GetNumberOfAbilities() const
{
    return abilities.size();
}

// AbilityLibrary implementation
AbilityLibrary::AbilityLibrary(SuperChessGame &game_, Logger &log_)
    : game(game_), log(log_)
{
    // In a real implementation, we would add abilities here
    // For now, just provide a stub implementation
}

std::unique_ptr<Ability> AbilityLibrary::GetAbility(const SuperPieceInfo &key)
{
    auto it = lib.find(key);
    if (it != lib.end())
        return it->second->Clone();

    return nullptr;
}

Tier AbilityLibrary::GetMaxTier(const Piece p_type) const
{
    return MAX_TIERS[p_type];
}

// SuperChessGame implementation
SuperChessGame::SuperChessGame()
    : ChessGame()
{
    al = std::make_shared<AbilityLibrary>(*this, log);
}

SuperChessGame::SuperChessGame(const SuperPieceInfo &white_info, const SuperPieceInfo &black_info)
    : ChessGame()
{
    al = std::make_shared<AbilityLibrary>(*this, log);

    InitSuperPieces(white_info, black_info);
    InitGameStartEffects();
}

bool SuperChessGame::RemovePiece(Square square)
{
    U64 p = 1ULL << square;
    Piece p_type = GetPieceType(p);
    if ((p & PieceTypeArray[King]))
    {
        std::cout << "WARNING: You are removing a king" << std::endl;
    }

    UnSuper(square);

    if (board & p)
    {
        AddToGraveyard(GetColor(p), square, p_type);
    }

    if (!ChessGame::RemovePiece(square))
        return false;

    super_pieces.erase(square);
    UpdateBoard();
    return true;
}

bool SuperChessGame::AddSuperPiece(SuperPieceInfo info, Square square, Color color, bool conversion)
{
    U64 p = 1ULL << square;
    if (!conversion && !ChessGame::AddPiece(square, color, info.first))
        return false;
    if (conversion && !(board & p)) // square doesn't contain piece to convert
        return false;
    if (conversion && (GetPieceType(p) != info.first)) // converting type and current type are not the same
        return false;

    CapTier(info.second, info.first);
    std::vector<std::unique_ptr<Ability>> v;
    MakeAbilityVector(v, info);
    super_pieces[square] = std::make_shared<SuperPiece>(v, info, square, color);

    return true;
}

bool SuperChessGame::UnSuper(Square square)
{
    auto it = super_pieces.find(square);
    if (it != super_pieces.end())
    {
        super_pieces.erase(square);
        return true;
    }
    return false;
}

bool SuperChessGame::IsSuperPiece(const Square &key) const
{
    return super_pieces.find(key) != super_pieces.end();
}

U64 SuperChessGame::GetBoardOf(Piece piece, Color color)
{
    return (color == white) ? WhitePiecesArray[piece] : BlackPiecesArray[piece];
}

U64 SuperChessGame::GetBoardOf(Color color) const
{
    if (color == white)
        return WhitePieces;
    return BlackPieces;
}

bool SuperChessGame::InCheck(Color color) const
{
    // Check if the king exists first
    if (AllColorPiecesArray[color][King] == 0)
    {
        // King doesn't exist, technically not in check but may be a win for opponent
        return false;
    }

    return ChessGame::InCheck(ChessGame::GetBoard(), color, static_cast<U64>(1ULL << GetSquare(AllColorPiecesArray[color][King])));
}

bool SuperChessGame::IsWin(Color color) const
{
    std::cout << "\n==== CHECKMATE DETECTION START ====\n";
    std::cout << "IsWin check for " << (color == white ? "white" : "black") << std::endl;

    // First check if the opponent king is missing (which is a win condition in SuperChessGame)
    Color opponent = (color == white) ? black : white;
    U64 kingBitboard = (opponent == white) ? WhitePiecesArray[King] : BlackPiecesArray[King];

    // If king is captured, it's a win
    if (kingBitboard == 0)
    {
        std::cout << "Win by king capture: " << (opponent == white ? "white" : "black") << " king is missing" << std::endl;
        return true;
    }

    // Before proceeding, check if our king exists
    U64 ownKingBitboard = (color == white) ? WhitePiecesArray[King] : BlackPiecesArray[King];
    if (ownKingBitboard == 0)
    {
        std::cout << "Cannot check for win - own king is missing" << std::endl;
        std::cout << "==== CHECKMATE DETECTION END (OWN KING MISSING) ====\n";
        return false;
    }

    // Otherwise check for traditional checkmate

    // First check if opponent is in check
    bool isInCheck = InCheck(opponent);
    std::cout << (opponent == white ? "White" : "Black") << " in check: " << (isInCheck ? "yes" : "no") << std::endl;

    if (!isInCheck)
    {
        std::cout << "==== CHECKMATE DETECTION END (NOT IN CHECK) ====\n";
        return false; // Not in check, so definitely not checkmate
    }

    // Find the king square
    Square kingSquare = static_cast<Square>(get_LSB(kingBitboard));

    // Try all possible moves for the king first
    U64 kingMoves = 0;

    // Get legal king moves using our improved method
    if (opponent == white)
    {
        kingMoves = GetWhiteKingMoves();
    }
    else
    {
        kingMoves = GetBlackKingMoves();
    }

    std::cout << "King at " << kingSquare << " has " << count_bits(kingMoves) << " possible moves" << std::endl;

    // If king has any legal moves, it's not checkmate
    if (kingMoves)
    {
        std::cout << "==== CHECKMATE DETECTION END (KING CAN MOVE) ====\n";
        return false;
    }

    std::cout << "King has no legal moves, checking if other pieces can save it...\n";

    // If king has no moves, we need to check if any other piece can block or capture the checking piece

    // First, identify the attacking piece(s)
    // Find which opponent pieces are giving check
    U64 attackers = 0;
    U64 attackerBitboard = 0;

    // Color of the player giving check (the attacker)
    Color checkerColor = color;

    // Check if any opponent pieces are checking the king
    for (int pieceType = 0; pieceType < 6; pieceType++)
    {
        U64 piecesBitboard = (checkerColor == white) ? WhitePiecesArray[pieceType] : BlackPiecesArray[pieceType];
        U64 tempPieces = piecesBitboard;

        while (tempPieces)
        {
            Square sq = static_cast<Square>(get_LSB(tempPieces));
            U64 attacks = ChessGame::GetAttacks(sq, board, pieceType);

            // Check if this piece is attacking the king
            if (attacks & kingBitboard)
            {
                attackers++;
                attackerBitboard |= (1ULL << sq);
                std::cout << "Piece type " << pieceType << " at " << sq << " is giving check" << std::endl;
            }

            tempPieces &= tempPieces - 1; // Clear LSB
        }
    }

    // If more than one piece is giving check, we can only escape by moving the king
    // Since we already checked that the king has no moves, it's checkmate
    if (attackers > 1)
    {
        std::cout << "Multiple attackers, king has no moves - checkmate" << std::endl;
        std::cout << "==== CHECKMATE DETECTION END (MULTIPLE ATTACKERS) ====\n";
        return true;
    }

    // If only one piece is giving check, see if it can be captured or blocked
    if (attackers == 1)
    {
        // Find the square of the checking piece
        Square attackerSquare = static_cast<Square>(get_LSB(attackerBitboard));
        std::cout << "Single attacker at square " << attackerSquare << std::endl;

        // 1. Check if the checking piece can be captured by any other piece

        // Get all pieces of the opponent (the player in check)
        U64 defenderPieces = (opponent == white) ? WhitePieces : BlackPieces;
        defenderPieces &= ~kingBitboard; // Remove king from defenders

        // Debug dump the defender pieces
        std::cout << "Defender pieces bitboard: " << std::hex << defenderPieces << std::dec << std::endl;

        U64 tempDefenders = defenderPieces;
        while (tempDefenders)
        {
            Square sq = static_cast<Square>(get_LSB(tempDefenders));
            std::cout << "Defender at square " << sq;

            // Determine piece type
            for (int i = 0; i < 6; i++)
            {
                if ((1ULL << sq) & ((opponent == white) ? WhitePiecesArray[i] : BlackPiecesArray[i]))
                {
                    std::cout << " is a " << ((opponent == white) ? "white " : "black ") << i << std::endl;
                    break;
                }
            }

            tempDefenders &= tempDefenders - 1;
        }

        while (defenderPieces)
        {
            Square defenderSquare = static_cast<Square>(get_LSB(defenderPieces));
            // Skip the king, we already checked king moves
            if (defenderSquare != kingSquare)
            {
                int defenderType = -1;

                // Find piece type
                for (int i = 0; i < 6; i++)
                {
                    if ((1ULL << defenderSquare) & ((opponent == white) ? WhitePiecesArray[i] : BlackPiecesArray[i]))
                    {
                        defenderType = i;
                        break;
                    }
                }

                std::cout << "Checking if defender at " << defenderSquare << " (type " << defenderType << ") can capture attacker" << std::endl;

                if (defenderType != -1)
                {
                    // Get moves for this piece using our improved method
                    U64 defenderAttacks = GetAttacks(defenderSquare);
                    std::cout << "Defender attacks bitboard: " << std::hex << defenderAttacks << std::dec << std::endl;
                    std::cout << "Attacker bitboard: " << std::hex << attackerBitboard << std::dec << std::endl;

                    // Check if the defender can capture the attacker
                    if (defenderAttacks & attackerBitboard)
                    {
                        std::cout << "Checking piece at " << attackerSquare << " can be captured by piece at " << defenderSquare << std::endl;
                        std::cout << "==== CHECKMATE DETECTION END (ATTACKER CAN BE CAPTURED) ====\n";
                        return false; // Not checkmate, the attacker can be captured
                    }
                    else
                    {
                        std::cout << "Defender at " << defenderSquare << " cannot capture attacker" << std::endl;
                    }
                }
            }

            defenderPieces &= defenderPieces - 1; // Clear LSB
        }

        // 2. For sliding pieces (Bishop, Rook, Queen), check if the check can be blocked
        // Get the piece type of the attacker
        int attackerType = -1;
        for (int i = 0; i < 6; i++)
        {
            if ((1ULL << attackerSquare) & ((color == white) ? WhitePiecesArray[i] : BlackPiecesArray[i]))
            {
                attackerType = i;
                break;
            }
        }

        std::cout << "Attacker type: " << attackerType << std::endl;

        // Only sliding pieces can be blocked (Bishop, Rook, Queen)
        if (attackerType == Bishop || attackerType == Rook || attackerType == Queen)
        {
            std::cout << "Attacker is a sliding piece, checking for blocking" << std::endl;

            // Determine the ray between the attacker and the king
            U64 blockingSquares = 0;

            // Generate the ray from attacker to king
            int attackerRow = attackerSquare / 8;
            int attackerCol = attackerSquare % 8;
            int kingRow = kingSquare / 8;
            int kingCol = kingSquare % 8;

            std::cout << "Attacker at row " << attackerRow << ", col " << attackerCol << std::endl;
            std::cout << "King at row " << kingRow << ", col " << kingCol << std::endl;

            // Determine direction (diagonal, horizontal, vertical)
            int rowStep = 0;
            int colStep = 0;

            if (attackerRow < kingRow)
                rowStep = 1;
            else if (attackerRow > kingRow)
                rowStep = -1;

            if (attackerCol < kingCol)
                colStep = 1;
            else if (attackerCol > kingCol)
                colStep = -1;

            std::cout << "Ray direction: rowStep=" << rowStep << ", colStep=" << colStep << std::endl;

            // Generate blocking squares (squares between attacker and king)
            int row = attackerRow + rowStep;
            int col = attackerCol + colStep;

            while (row != kingRow || col != kingCol)
            {
                Square blockingSquare = static_cast<Square>(row * 8 + col);
                std::cout << "Potential blocking square: " << blockingSquare << " (row " << row << ", col " << col << ")" << std::endl;
                blockingSquares |= (1ULL << blockingSquare);
                row += rowStep;
                col += colStep;
            }

            std::cout << "Blocking squares bitboard: " << std::hex << blockingSquares << std::dec << std::endl;

            // Check if any defender piece can move to a blocking square
            defenderPieces = (opponent == white) ? WhitePieces : BlackPieces;
            defenderPieces &= ~kingBitboard; // Remove king from defenders

            while (defenderPieces)
            {
                Square defenderSquare = static_cast<Square>(get_LSB(defenderPieces));

                // Skip the king, we already checked king moves
                if (defenderSquare != kingSquare)
                {
                    int defenderType = -1;

                    // Find piece type
                    for (int i = 0; i < 6; i++)
                    {
                        if ((1ULL << defenderSquare) & ((opponent == white) ? WhitePiecesArray[i] : BlackPiecesArray[i]))
                        {
                            defenderType = i;
                            break;
                        }
                    }

                    std::cout << "Checking if defender at " << defenderSquare << " (type " << defenderType << ") can block the check" << std::endl;

                    if (defenderType != -1)
                    {
                        // Get moves for this piece
                        U64 defenderAttacks = GetAttacks(defenderSquare);
                        std::cout << "Defender attacks bitboard: " << std::hex << defenderAttacks << std::dec << std::endl;

                        // Check if the defender can block any square
                        U64 possibleBlocks = defenderAttacks & blockingSquares;
                        if (possibleBlocks)
                        {
                            std::cout << "Check can be blocked by piece at " << defenderSquare << std::endl;

                            // Debug: show which squares can be blocked
                            U64 tempBlocks = possibleBlocks;
                            while (tempBlocks)
                            {
                                Square blockSq = static_cast<Square>(get_LSB(tempBlocks));
                                std::cout << "  Can block at square " << blockSq << std::endl;
                                tempBlocks &= tempBlocks - 1;
                            }

                            std::cout << "==== CHECKMATE DETECTION END (CHECK CAN BE BLOCKED) ====\n";
                            return false; // Not checkmate, the check can be blocked
                        }
                        else
                        {
                            std::cout << "Defender at " << defenderSquare << " cannot block the check" << std::endl;
                        }
                    }
                }

                defenderPieces &= defenderPieces - 1; // Clear LSB
            }
        }
        else
        {
            std::cout << "Attacker is not a sliding piece, cannot be blocked" << std::endl;
        }
    }

    // If we get here, no piece has any legal moves that escape check
    std::cout << "Checkmate: " << (opponent == white ? "White" : "Black") << " has no legal moves to escape check" << std::endl;
    std::cout << "==== CHECKMATE DETECTION END (CHECKMATE CONFIRMED) ====\n";
    return true;
}

void SuperChessGame::InitSuperPieces(const SuperPieceInfo &white_info, const SuperPieceInfo &black_info)
{
    // Basic implementation
    U64 whitePieces = GetBoardOf(white_info.first, white);
    U64 blackPieces = GetBoardOf(black_info.first, black);

    // Add super pieces for each matching piece on the board
    for (int i = 0; i < 64; i++)
    {
        if ((whitePieces >> i) & 1)
            AddSuperPiece(white_info, (Square)i, white, true);
        if ((blackPieces >> i) & 1)
            AddSuperPiece(black_info, (Square)i, black, true);
    }
}

void SuperChessGame::InitGameStartEffects()
{
    for (auto &pair : super_pieces)
    {
        pair.second->OnGameStartEffects();
    }
}

void SuperChessGame::CapTier(Tier &t, Piece p_type) const
{
    if (t > al->GetMaxTier(p_type))
    {
        t = al->GetMaxTier(p_type);
    }
}

void SuperChessGame::MakeAbilityVector(std::vector<std::unique_ptr<Ability>> &v, SuperPieceInfo info)
{
    // Just a simple stub
    // In a real implementation, we would get abilities from the ability library
}

U64 SuperChessGame::GetAttacks(Square square_) const
{
    U64 attacks, piece = 0ULL;
    set_bit(piece, square_);
    if (!(piece & board))
        throw std::invalid_argument("No piece on square");

    int which_function = -1;
    for (size_t i = 0; i < PieceTypeArray.size(); i++)
    {
        if (piece & PieceTypeArray[i])
            which_function = i;
    }

    // Get basic attacks
    attacks = ChessGame::GetAttacks(square_, board, which_function);

    if (which_function == King)
    {
        std::cout << "Initial king moves for square " << square_ << ": " << std::hex << attacks << std::dec << std::endl;
    }

    // Super piece modifications
    const auto it = super_pieces.find(square_);
    if (it != super_pieces.end())
    {
        // Add any attack modifications from abilities
        it->second->ModifyMove(attacks);
    }

    // Special handling for kings - filter moves that would put king in check
    if (which_function == King)
    {
        // Determine if this is white or black king
        bool isWhiteKing = piece & WhitePiecesArray[King];
        std::cout << "Processing " << (isWhiteKing ? "WHITE" : "BLACK") << " king at square " << square_ << std::endl;

        // Get all squares attacked by opponent
        U64 attackedSquares = 0;

        if (isWhiteKing)
        {
            // Calculate all squares attacked by black pieces
            // We need to create a local copy of the board to modify temporarily
            U64 localBoard = board;
            localBoard &= ~piece; // Temporarily remove king to see through its position

            // Pawns (special handling for diagonal attacks)
            U64 blackPawns = BlackPiecesArray[Pawn];
            U64 pawnAttacks = 0;
            while (blackPawns)
            {
                Square pawnSq = static_cast<Square>(get_LSB(blackPawns));
                int pawnRow = pawnSq / 8;
                int pawnCol = pawnSq % 8;

                // Pawns attack diagonally forward (from black's perspective, that's down the board)
                if (pawnRow > 0) // Not on first rank
                {
                    if (pawnCol > 0)
                    {                                          // Not on a-file
                        pawnAttacks |= (1ULL << (pawnSq - 9)); // Diagonal left (from black's perspective)
                    }
                    if (pawnCol < 7)
                    {                                          // Not on h-file
                        pawnAttacks |= (1ULL << (pawnSq - 7)); // Diagonal right (from black's perspective)
                    }
                }

                blackPawns &= blackPawns - 1; // Clear LSB
            }
            attackedSquares |= pawnAttacks;
            std::cout << "  Black pawn attacks: " << std::hex << pawnAttacks << std::dec << std::endl;

            // Knights
            U64 blackKnights = BlackPiecesArray[Knight];
            U64 knightAttacks = 0;
            while (blackKnights)
            {
                Square knightSq = static_cast<Square>(get_LSB(blackKnights));
                U64 attacks = ChessGame::GetAttacks(knightSq, localBoard, Knight);
                knightAttacks |= attacks;
                blackKnights &= blackKnights - 1; // Clear LSB
            }
            attackedSquares |= knightAttacks;
            std::cout << "  Black knight attacks: " << std::hex << knightAttacks << std::dec << std::endl;

            // Bishops
            U64 blackBishops = BlackPiecesArray[Bishop];
            U64 bishopAttacks = 0;
            while (blackBishops)
            {
                Square bishopSq = static_cast<Square>(get_LSB(blackBishops));
                U64 attacks = ChessGame::GetAttacks(bishopSq, localBoard, Bishop);
                bishopAttacks |= attacks;
                blackBishops &= blackBishops - 1; // Clear LSB
            }
            attackedSquares |= bishopAttacks;
            std::cout << "  Black bishop attacks: " << std::hex << bishopAttacks << std::dec << std::endl;

            // Rooks
            U64 blackRooks = BlackPiecesArray[Rook];
            U64 rookAttacks = 0;
            while (blackRooks)
            {
                Square rookSq = static_cast<Square>(get_LSB(blackRooks));
                U64 attacks = ChessGame::GetAttacks(rookSq, localBoard, Rook);
                rookAttacks |= attacks;
                blackRooks &= blackRooks - 1; // Clear LSB
            }
            attackedSquares |= rookAttacks;
            std::cout << "  Black rook attacks: " << std::hex << rookAttacks << std::dec << std::endl;

            // Queens
            U64 blackQueens = BlackPiecesArray[Queen];
            U64 queenAttacks = 0;
            while (blackQueens)
            {
                Square queenSq = static_cast<Square>(get_LSB(blackQueens));
                U64 attacks = ChessGame::GetAttacks(queenSq, localBoard, Queen);
                std::cout << "  Queen at " << queenSq << " attacks: " << std::hex << attacks << std::dec << std::endl;
                queenAttacks |= attacks;
                blackQueens &= blackQueens - 1; // Clear LSB
            }
            attackedSquares |= queenAttacks;
            std::cout << "  All black queen attacks: " << std::hex << queenAttacks << std::dec << std::endl;

            // Black king
            U64 blackKing = BlackPiecesArray[King];
            U64 kingAttacks = 0;
            if (blackKing)
            {
                Square kingPos = static_cast<Square>(get_LSB(blackKing));
                kingAttacks = ChessGame::GetAttacks(kingPos, localBoard, King);
                attackedSquares |= kingAttacks;
            }
            std::cout << "  Black king attacks: " << std::hex << kingAttacks << std::dec << std::endl;
        }
        else // Black king
        {
            // Calculate all squares attacked by white pieces
            // We need to create a local copy of the board to modify temporarily
            U64 localBoard = board;
            localBoard &= ~piece; // Temporarily remove king to see through its position

            // Pawns (special handling for diagonal attacks)
            U64 whitePawns = WhitePiecesArray[Pawn];
            U64 pawnAttacks = 0;
            while (whitePawns)
            {
                Square pawnSq = static_cast<Square>(get_LSB(whitePawns));
                int pawnRow = pawnSq / 8;
                int pawnCol = pawnSq % 8;

                // Pawns attack diagonally forward (from white's perspective, that's up the board)
                if (pawnRow < 7) // Not on eighth rank
                {
                    if (pawnCol > 0)
                    {                                          // Not on a-file
                        pawnAttacks |= (1ULL << (pawnSq + 7)); // Diagonal left (from white's perspective)
                    }
                    if (pawnCol < 7)
                    {                                          // Not on h-file
                        pawnAttacks |= (1ULL << (pawnSq + 9)); // Diagonal right (from white's perspective)
                    }
                }

                whitePawns &= whitePawns - 1; // Clear LSB
            }
            attackedSquares |= pawnAttacks;
            std::cout << "  White pawn attacks: " << std::hex << pawnAttacks << std::dec << std::endl;

            // Knights
            U64 whiteKnights = WhitePiecesArray[Knight];
            U64 knightAttacks = 0;
            while (whiteKnights)
            {
                Square knightSq = static_cast<Square>(get_LSB(whiteKnights));
                U64 attacks = ChessGame::GetAttacks(knightSq, localBoard, Knight);
                knightAttacks |= attacks;
                whiteKnights &= whiteKnights - 1; // Clear LSB
            }
            attackedSquares |= knightAttacks;
            std::cout << "  White knight attacks: " << std::hex << knightAttacks << std::dec << std::endl;

            // Bishops
            U64 whiteBishops = WhitePiecesArray[Bishop];
            U64 bishopAttacks = 0;
            while (whiteBishops)
            {
                Square bishopSq = static_cast<Square>(get_LSB(whiteBishops));
                U64 attacks = ChessGame::GetAttacks(bishopSq, localBoard, Bishop);
                bishopAttacks |= attacks;
                whiteBishops &= whiteBishops - 1; // Clear LSB
            }
            attackedSquares |= bishopAttacks;
            std::cout << "  White bishop attacks: " << std::hex << bishopAttacks << std::dec << std::endl;

            // Rooks
            U64 whiteRooks = WhitePiecesArray[Rook];
            U64 rookAttacks = 0;
            while (whiteRooks)
            {
                Square rookSq = static_cast<Square>(get_LSB(whiteRooks));
                U64 attacks = ChessGame::GetAttacks(rookSq, localBoard, Rook);
                rookAttacks |= attacks;
                whiteRooks &= whiteRooks - 1; // Clear LSB
            }
            attackedSquares |= rookAttacks;
            std::cout << "  White rook attacks: " << std::hex << rookAttacks << std::dec << std::endl;

            // Queens
            U64 whiteQueens = WhitePiecesArray[Queen];
            U64 queenAttacks = 0;
            while (whiteQueens)
            {
                Square queenSq = static_cast<Square>(get_LSB(whiteQueens));
                U64 attacks = ChessGame::GetAttacks(queenSq, localBoard, Queen);
                std::cout << "  Queen at " << queenSq << " attacks: " << std::hex << attacks << std::dec << std::endl;
                queenAttacks |= attacks;
                whiteQueens &= whiteQueens - 1; // Clear LSB
            }
            attackedSquares |= queenAttacks;
            std::cout << "  All white queen attacks: " << std::hex << queenAttacks << std::dec << std::endl;

            // White king
            U64 whiteKing = WhitePiecesArray[King];
            U64 kingAttacks = 0;
            if (whiteKing)
            {
                Square kingPos = static_cast<Square>(get_LSB(whiteKing));
                kingAttacks = ChessGame::GetAttacks(kingPos, localBoard, King);
                attackedSquares |= kingAttacks;
            }
            std::cout << "  White king attacks: " << std::hex << kingAttacks << std::dec << std::endl;
        }

        std::cout << "All squares attacked by opponent: " << std::hex << attackedSquares << std::dec << std::endl;
        std::cout << "Before filtering, king moves: " << std::hex << attacks << std::dec << std::endl;

        // Remove squares that are under attack from legal king moves
        attacks &= ~attackedSquares;

        // Remove moves to squares occupied by pieces of the same color
        U64 ownPieces = isWhiteKing ? GetBoardOf(white) : GetBoardOf(black);
        std::cout << "Own pieces: " << std::hex << ownPieces << std::dec << std::endl;
        attacks &= ~ownPieces;

        std::cout << "King at " << square_ << " after filtering, has moves: " << std::hex << attacks << std::dec << std::endl;

        // Debug: show which squares the king can move to
        U64 tempMoves = attacks;
        while (tempMoves)
        {
            Square targetSq = static_cast<Square>(get_LSB(tempMoves));
            std::cout << "King can legally move to square: " << targetSq << std::endl;
            tempMoves &= tempMoves - 1;
        }
    }

    return attacks;
}

std::vector<Action> SuperChessGame::Move(Square from_sq, Square to_sq)
{
    std::vector<Action> actions;
    try
    {
        // Basic implementation
        U64 from = 1ULL << from_sq;
        U64 to = 1ULL << to_sq;

        // Validate move
        if (!(from & board) || from_sq == to_sq)
            return actions;

        // Execute move
        Color color = GetColor(from);
        Piece fromPiece = GetPieceType(from);
        Piece toPiece = GetPieceType(to);

        // Add a debug print to see if we're reaching this point
        std::cout << "Executing move from " << SquareStrings[from_sq] << " to " << SquareStrings[to_sq] << std::endl;

        ExecuteMove(color, from_sq, to_sq, fromPiece, toPiece);

        // if capturing
        if ((board & to) != 0)
            actions.push_back(Action::Capture);
        else
            actions.push_back(Action::Move);

        // Check if opponent is in check
        Color oppositeColor = (color == white) ? black : white;

        // Verify both kings exist before calling InCheck
        bool whiteKingExists = WhitePiecesArray[King] != 0;
        bool blackKingExists = BlackPiecesArray[King] != 0;

        if (!whiteKingExists || !blackKingExists)
        {
            // One of the kings is missing - handle appropriately
            if (!whiteKingExists)
            {
                std::cout << "White king is missing - Black wins!" << std::endl;
                actions.push_back(Action::Checkmate);
            }
            else if (!blackKingExists)
            {
                std::cout << "Black king is missing - White wins!" << std::endl;
                actions.push_back(Action::Checkmate);
            }
        }
        else
        {
            // Both kings exist, safe to check for check/checkmate
            bool opponentInCheck = InCheck(oppositeColor);
            if (opponentInCheck)
            {
                std::cout << "Opponent is in check!" << std::endl;
                actions.push_back(Action::Check);

                try
                {
                    // Try all three checkmate detection methods and only declare checkmate if all agree
                    bool isWinResult = IsWin(color);
                    bool isDefiniteCheckmateResult = IsDefiniteCheckmate(color);
                    bool isDirectCheckmateResult = IsDirectCheckmate(color);

                    std::cout << "Checkmate detection results: IsWin=" << isWinResult
                              << ", IsDefiniteCheckmate=" << isDefiniteCheckmateResult
                              << ", IsDirectCheckmate=" << isDirectCheckmateResult << std::endl;

                    // Only declare checkmate if all methods agree
                    if (isWinResult && isDefiniteCheckmateResult && isDirectCheckmateResult)
                    {
                        std::cout << "Checkmate confirmed! " << (color == white ? "White" : "Black") << " wins!" << std::endl;
                        actions.push_back(Action::Checkmate);
                    }
                    else
                    {
                        std::cout << "Check, but NOT checkmate." << std::endl;
                    }
                }
                catch (const std::exception &e)
                {
                    std::cout << "Exception during checkmate detection: " << e.what() << std::endl;
                    std::cout << "Check, but checkmate detection failed." << std::endl;
                }
            }
        }

        // Critical: Update the board state to reflect changes
        UpdateBoard();

        // Add extra debug print to confirm we updated
        std::cout << "Board updated after move" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cout << "Exception during move execution: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cout << "Unknown exception during move execution" << std::endl;
    }

    return actions;
}

void SuperChessGame::ExecuteMove(Color color, Square from_sq, Square to_sq, Piece from_piece, Piece to_piece)
{
    std::cout << "ExecuteMove called: " << SquareStrings[from_sq] << " to " << SquareStrings[to_sq] << std::endl;

    // Print the board state before the move
    std::cout << "Board state BEFORE move:" << std::endl;
    std::cout << "White pieces: " << WhitePieces << std::endl;
    std::cout << "Black pieces: " << BlackPieces << std::endl;
    std::cout << "From square bit: " << (1ULL << from_sq) << std::endl;
    std::cout << "To square bit: " << (1ULL << to_sq) << std::endl;

    // Save the super piece from the source square before any operations
    bool hasSuperPieceAtFromSq = IsSuperPiece(from_sq);
    std::shared_ptr<SuperPiece> superPieceToMove = nullptr;

    if (hasSuperPieceAtFromSq)
    {
        superPieceToMove = super_pieces[from_sq];
    }

    // First handle captures before executing the move
    bool captureExists = (board & (1ULL << to_sq)) != 0;
    if (captureExists)
    {
        std::cout << "Capture detected at " << SquareStrings[to_sq] << std::endl;
        // Add to graveyard if there's a piece being captured
        AddToGraveyard(color, to_sq, to_piece);

        // If there's a super piece at the target, remove it
        if (IsSuperPiece(to_sq))
        {
            std::cout << "Removing super piece at " << SquareStrings[to_sq] << std::endl;
            super_pieces.erase(to_sq);
        }
    }

    // Call base class implementation to move the pieces on the regular board
    std::cout << "Calling ChessGame::ExecuteMove" << std::endl;
    ChessGame::ExecuteMove(color, from_sq, to_sq, from_piece, to_piece);

    // If we had a super piece at the source square, move it to the destination
    if (hasSuperPieceAtFromSq && superPieceToMove != nullptr)
    {
        std::cout << "Moving super piece from " << SquareStrings[from_sq] << " to " << SquareStrings[to_sq] << std::endl;
        // Move the super piece from from_sq to to_sq
        super_pieces[to_sq] = superPieceToMove;
        super_pieces.erase(from_sq);

        // Update the square in the super piece object
        super_pieces[to_sq]->UpdateSquare(to_sq);
    }

    // Explicitly update the board after the move
    std::cout << "Calling UpdateBoard()" << std::endl;
    UpdateBoard();

    // Print the board state after the move
    std::cout << "Board state AFTER move:" << std::endl;
    std::cout << "White pieces: " << WhitePieces << std::endl;
    std::cout << "Black pieces: " << BlackPieces << std::endl;

    std::cout << "ExecuteMove completed" << std::endl;
}

// graveyard functions
void SuperChessGame::AddToGraveyard(Color color, Square sq, Piece piece)
{
    SuperPieceInfo info = std::make_pair(piece, Tier::not_superpiece);
    if (IsSuperPiece(sq))
    {
        info.second = super_pieces[sq]->GetInfo().second;
    }

    auto key = std::make_pair(color, info);

    if (graveyard.find(key) != graveyard.end())
        graveyard[key]++;
    else
        graveyard[key] = 1;
}

void SuperChessGame::RemoveFromGraveYard(const std::pair<Color, SuperPieceInfo> &key)
{
    auto it = graveyard.find(key);
    if (it != graveyard.end() && it->second >= 0)
    {
        it->second--;
    }
}

bool SuperChessGame::PieceInGraveyard(Color color, Piece piece)
{
    SuperPieceInfo info;
    for (const auto &p : graveyard)
    {
        if (p.first.second.first == piece)
            return true;
    }
    return false;
}

std::vector<SuperPieceInfo> SuperChessGame::GetPiecesInGraveyard(Color color) const
{
    std::vector<SuperPieceInfo> pieces;
    for (const auto &p : graveyard)
        if (p.first.first == color && p.second > 0)
        {
            pieces.push_back(p.first.second);
        }
    return pieces;
}

// Simpler implementation that better handles line attacks
U64 SuperChessGame::GetWhiteKingMoves() const
{
    try
    {
        // Find the white king
        U64 kingBitboard = WhitePiecesArray[King];
        if (kingBitboard == 0)
        {
            std::cout << "White king not found on board" << std::endl;
            return 0; // No king = no moves
        }

        // Get the king's square
        Square kingSquare = static_cast<Square>(get_LSB(kingBitboard));
        std::cout << "White king found at square " << kingSquare << std::endl;

        // Get all possible moves for the king (ignoring check)
        U64 possibleMoves = GetAttacks(kingSquare);
        std::cout << "Raw possible moves bitboard: " << std::hex << possibleMoves << std::dec << std::endl;

        // Remove moves to squares occupied by own pieces
        possibleMoves &= ~GetBoardOf(white);
        std::cout << "After removing own pieces: " << std::hex << possibleMoves << std::dec << std::endl;

        // Now check for each potential move if it would leave the king in check
        U64 legalMoves = 0;
        U64 attackedSquares = 0; // Squares under attack by black

        // Get all squares attacked by black pieces
        // Start with pawns (special handling)
        U64 blackPawns = BlackPiecesArray[Pawn];
        while (blackPawns)
        {
            Square pawnSq = static_cast<Square>(get_LSB(blackPawns));
            int pawnRow = pawnSq / 8;
            int pawnCol = pawnSq % 8;

            // Pawns attack diagonally forward
            if (pawnRow > 0)
            {
                if (pawnCol > 0)
                { // Can attack to the left
                    attackedSquares |= (1ULL << (pawnSq - 9));
                }
                if (pawnCol < 7)
                { // Can attack to the right
                    attackedSquares |= (1ULL << (pawnSq - 7));
                }
            }

            blackPawns &= blackPawns - 1; // Clear LSB
        }

        // Handle knights
        U64 blackKnights = BlackPiecesArray[Knight];
        while (blackKnights)
        {
            Square knightSq = static_cast<Square>(get_LSB(blackKnights));
            attackedSquares |= ChessGame::GetAttacks(knightSq, board, Knight);
            blackKnights &= blackKnights - 1; // Clear LSB
        }

        // Handle black king
        U64 blackKing = BlackPiecesArray[King];
        if (blackKing)
        {
            Square kingPos = static_cast<Square>(get_LSB(blackKing));
            attackedSquares |= ChessGame::GetAttacks(kingPos, board, King);
        }

        // For sliding pieces (bishop, rook, queen), we need special handling
        // Bishops
        U64 blackBishops = BlackPiecesArray[Bishop];
        while (blackBishops)
        {
            Square bishopSq = static_cast<Square>(get_LSB(blackBishops));
            attackedSquares |= ChessGame::GetAttacks(bishopSq, board, Bishop);
            blackBishops &= blackBishops - 1;
        }

        // Rooks
        U64 blackRooks = BlackPiecesArray[Rook];
        while (blackRooks)
        {
            Square rookSq = static_cast<Square>(get_LSB(blackRooks));
            attackedSquares |= ChessGame::GetAttacks(rookSq, board, Rook);
            blackRooks &= blackRooks - 1;
        }

        // Queens (most important for line attacks)
        U64 blackQueens = BlackPiecesArray[Queen];
        while (blackQueens)
        {
            Square queenSq = static_cast<Square>(get_LSB(blackQueens));
            U64 queenAttacks = ChessGame::GetAttacks(queenSq, board, Queen);
            std::cout << "Queen at " << queenSq << " attacks: " << std::hex << queenAttacks << std::dec << std::endl;
            attackedSquares |= queenAttacks;
            blackQueens &= blackQueens - 1;
        }

        std::cout << "All black attacks bitboard: " << std::hex << attackedSquares << std::dec << std::endl;

        // King cannot move to any attacked square
        legalMoves = possibleMoves & ~attackedSquares;

        std::cout << "White king has " << count_bits(possibleMoves) << " possible moves and "
                  << count_bits(legalMoves) << " legal moves" << std::endl;

        // Debug: print each legal move for verification
        U64 temp = legalMoves;
        while (temp)
        {
            Square sq = static_cast<Square>(get_LSB(temp));
            std::cout << "Legal move to square: " << sq << std::endl;
            temp &= temp - 1;
        }

        return legalMoves;
    }
    catch (const std::exception &e)
    {
        std::cout << "Error in GetWhiteKingMoves: " << e.what() << std::endl;
        return 0; // Return 0 (no moves) on error
    }
}

// Similar simplified implementation for black king
U64 SuperChessGame::GetBlackKingMoves() const
{
    try
    {
        // Find the black king
        U64 kingBitboard = BlackPiecesArray[King];
        if (kingBitboard == 0)
        {
            std::cout << "Black king not found on board" << std::endl;
            return 0; // No king = no moves
        }

        // Get the king's square
        Square kingSquare = static_cast<Square>(get_LSB(kingBitboard));
        std::cout << "Black king found at square " << kingSquare << std::endl;

        // Get all possible moves for the king (ignoring check)
        U64 possibleMoves = GetAttacks(kingSquare);
        std::cout << "Raw possible moves bitboard: " << std::hex << possibleMoves << std::dec << std::endl;

        // Remove moves to squares occupied by own pieces
        possibleMoves &= ~GetBoardOf(black);
        std::cout << "After removing own pieces: " << std::hex << possibleMoves << std::dec << std::endl;

        // Now check for each potential move if it would leave the king in check
        U64 legalMoves = 0;
        U64 attackedSquares = 0; // Squares under attack by white

        // Get all squares attacked by white pieces
        // Start with pawns (special handling)
        U64 whitePawns = WhitePiecesArray[Pawn];
        while (whitePawns)
        {
            Square pawnSq = static_cast<Square>(get_LSB(whitePawns));
            int pawnRow = pawnSq / 8;
            int pawnCol = pawnSq % 8;

            // Pawns attack diagonally forward
            if (pawnRow < 7)
            {
                if (pawnCol > 0)
                { // Can attack to the left
                    attackedSquares |= (1ULL << (pawnSq + 7));
                }
                if (pawnCol < 7)
                { // Can attack to the right
                    attackedSquares |= (1ULL << (pawnSq + 9));
                }
            }

            whitePawns &= whitePawns - 1; // Clear LSB
        }

        // Handle knights
        U64 whiteKnights = WhitePiecesArray[Knight];
        while (whiteKnights)
        {
            Square knightSq = static_cast<Square>(get_LSB(whiteKnights));
            attackedSquares |= ChessGame::GetAttacks(knightSq, board, Knight);
            whiteKnights &= whiteKnights - 1; // Clear LSB
        }

        // Handle white king
        U64 whiteKing = WhitePiecesArray[King];
        if (whiteKing)
        {
            Square kingPos = static_cast<Square>(get_LSB(whiteKing));
            attackedSquares |= ChessGame::GetAttacks(kingPos, board, King);
        }

        // For sliding pieces (bishop, rook, queen), we need special handling
        // Bishops
        U64 whiteBishops = WhitePiecesArray[Bishop];
        while (whiteBishops)
        {
            Square bishopSq = static_cast<Square>(get_LSB(whiteBishops));
            attackedSquares |= ChessGame::GetAttacks(bishopSq, board, Bishop);
            whiteBishops &= whiteBishops - 1;
        }

        // Rooks
        U64 whiteRooks = WhitePiecesArray[Rook];
        while (whiteRooks)
        {
            Square rookSq = static_cast<Square>(get_LSB(whiteRooks));
            attackedSquares |= ChessGame::GetAttacks(rookSq, board, Rook);
            whiteRooks &= whiteRooks - 1;
        }

        // Queens (most important for line attacks)
        U64 whiteQueens = WhitePiecesArray[Queen];
        while (whiteQueens)
        {
            Square queenSq = static_cast<Square>(get_LSB(whiteQueens));
            U64 queenAttacks = ChessGame::GetAttacks(queenSq, board, Queen);
            std::cout << "Queen at " << queenSq << " attacks: " << std::hex << queenAttacks << std::dec << std::endl;
            attackedSquares |= queenAttacks;
            whiteQueens &= whiteQueens - 1;
        }

        std::cout << "All white attacks bitboard: " << std::hex << attackedSquares << std::dec << std::endl;

        // King cannot move to any attacked square
        legalMoves = possibleMoves & ~attackedSquares;

        std::cout << "Black king has " << count_bits(possibleMoves) << " possible moves and "
                  << count_bits(legalMoves) << " legal moves" << std::endl;

        // Debug: print each legal move for verification
        U64 temp = legalMoves;
        while (temp)
        {
            Square sq = static_cast<Square>(get_LSB(temp));
            std::cout << "Legal move to square: " << sq << std::endl;
            temp &= temp - 1;
        }

        return legalMoves;
    }
    catch (const std::exception &e)
    {
        std::cout << "Error in GetBlackKingMoves: " << e.what() << std::endl;
        return 0; // Return 0 (no moves) on error
    }
}

// Add these helper functions to get the king positions
Square SuperChessGame::GetWhiteKingPosition() const
{
    U64 kingBitboard = WhitePiecesArray[King];
    if (kingBitboard == 0)
    {
        return static_cast<Square>(64); // Invalid square to indicate no king
    }
    return static_cast<Square>(get_LSB(kingBitboard));
}

Square SuperChessGame::GetBlackKingPosition() const
{
    U64 kingBitboard = BlackPiecesArray[King];
    if (kingBitboard == 0)
    {
        return static_cast<Square>(64); // Invalid square to indicate no king
    }
    return static_cast<Square>(get_LSB(kingBitboard));
}

// Add this method after the IsWin method
bool SuperChessGame::IsDefiniteCheckmate(Color color) const
{
    // This is a simplified and robust checkmate detector that only returns true
    // when we're absolutely certain it's checkmate.

    // First, identify the player who might be in checkmate
    Color defender = (color == white) ? black : white;

    // Check if the king exists
    U64 kingBitboard = (defender == white) ? WhitePiecesArray[King] : BlackPiecesArray[King];
    if (kingBitboard == 0)
    {
        // King captured = automatic win
        return true;
    }

    // Also check if our own king exists
    U64 ownKingBitboard = (color == white) ? WhitePiecesArray[King] : BlackPiecesArray[King];
    if (ownKingBitboard == 0)
    {
        // Our king is missing, cannot declare checkmate
        return false;
    }

    // Check if the king is in check
    if (!InCheck(defender))
    {
        // Not in check, definitely not checkmate
        return false;
    }

    // Shortcut - if king has any legal moves, it's not checkmate
    Square kingSquare = static_cast<Square>(get_LSB(kingBitboard));
    U64 kingMoves = (defender == white) ? GetWhiteKingMoves() : GetBlackKingMoves();
    if (kingMoves != 0)
    {
        // King can move, not checkmate
        return false;
    }

    // Get all defender pieces (excluding king) and check if any can move
    U64 defenderPieces = (defender == white) ? WhitePieces : BlackPieces;
    defenderPieces &= ~kingBitboard; // Remove king

    // For each defender piece, check if it has ANY legal moves
    U64 tempDefenders = defenderPieces;
    while (tempDefenders)
    {
        Square sq = static_cast<Square>(get_LSB(tempDefenders));

        // Get all possible moves for this piece
        U64 moves;
        try
        {
            moves = GetAttacks(sq);
            if (moves != 0)
            {
                // Any piece with any legal move means we're not in checkmate
                return false;
            }
        }
        catch (const std::exception &e)
        {
            // Skip this piece if there's an error
        }

        tempDefenders &= tempDefenders - 1; // Clear LSB
    }

    // If we get here, the king is in check and neither it nor any other piece can move
    return true;
}

// Add this method after the IsDefiniteCheckmate method
bool SuperChessGame::CanPieceBlockOrCapture(Square pieceSquare, Square attackerSquare) const
{
    // Get the attacks for the piece
    U64 pieceAttacks = GetAttacks(pieceSquare);

    // Check if the piece can directly capture the attacker
    if (pieceAttacks & (1ULL << attackerSquare))
    {
        std::cout << "Piece at " << pieceSquare << " can directly capture attacker at " << attackerSquare << std::endl;
        return true;
    }

    // Get the piece type of the attacker
    int attackerType = -1;
    U64 attackerBit = 1ULL << attackerSquare;

    for (int i = 0; i < 6; i++)
    {
        if (attackerBit & WhitePiecesArray[i] || attackerBit & BlackPiecesArray[i])
        {
            attackerType = i;
            break;
        }
    }

    // Only sliding pieces can be blocked
    if (attackerType != Bishop && attackerType != Rook && attackerType != Queen)
    {
        return false;
    }

    // Find the king of the same color as the piece
    Color pieceColor = (pieceSquare & WhitePieces) ? white : black;
    U64 kingBitboard = (pieceColor == white) ? WhitePiecesArray[King] : BlackPiecesArray[King];
    Square kingSquare = static_cast<Square>(get_LSB(kingBitboard));

    // Calculate the ray between attacker and king
    int attackerRow = attackerSquare / 8;
    int attackerCol = attackerSquare % 8;
    int kingRow = kingSquare / 8;
    int kingCol = kingSquare % 8;

    // Determine direction
    int rowStep = 0;
    int colStep = 0;

    if (attackerRow < kingRow)
        rowStep = 1;
    else if (attackerRow > kingRow)
        rowStep = -1;

    if (attackerCol < kingCol)
        colStep = 1;
    else if (attackerCol > kingCol)
        colStep = -1;

    // Generate blocking squares
    U64 blockingSquares = 0;
    int row = attackerRow + rowStep;
    int col = attackerCol + colStep;

    while (row != kingRow || col != kingCol)
    {
        blockingSquares |= (1ULL << (row * 8 + col));
        row += rowStep;
        col += colStep;
    }

    // Check if the piece can block any square in the ray
    if (pieceAttacks & blockingSquares)
    {
        std::cout << "Piece at " << pieceSquare << " can block the attack" << std::endl;
        return true;
    }

    return false;
}

// Add this method after the CanPieceBlockOrCapture method
bool SuperChessGame::IsDirectCheckmate(Color color) const
{
    std::cout << "\n==== DIRECT CHECKMATE CHECK START ====\n";

    // First, identify the player who might be in checkmate
    Color defender = (color == white) ? black : white;

    // Check if the king exists
    U64 kingBitboard = (defender == white) ? WhitePiecesArray[King] : BlackPiecesArray[King];
    if (kingBitboard == 0)
    {
        std::cout << "King captured = automatic win\n";
        std::cout << "==== DIRECT CHECKMATE CHECK END (KING CAPTURED) ====\n";
        return true;
    }

    // Also check if our own king exists
    U64 ownKingBitboard = (color == white) ? WhitePiecesArray[King] : BlackPiecesArray[King];
    if (ownKingBitboard == 0)
    {
        std::cout << "Cannot check for checkmate - own king is missing\n";
        std::cout << "==== DIRECT CHECKMATE CHECK END (OWN KING MISSING) ====\n";
        return false;
    }

    // Check if the king is in check
    if (!InCheck(defender))
    {
        std::cout << "Not in check, definitely not checkmate\n";
        std::cout << "==== DIRECT CHECKMATE CHECK END (NOT IN CHECK) ====\n";
        return false;
    }

    // Find the king square
    Square kingSquare = static_cast<Square>(get_LSB(kingBitboard));

    // Check if the king has any legal moves
    U64 kingMoves = (defender == white) ? GetWhiteKingMoves() : GetBlackKingMoves();
    if (kingMoves != 0)
    {
        std::cout << "King can move, not checkmate\n";
        std::cout << "==== DIRECT CHECKMATE CHECK END (KING CAN MOVE) ====\n";
        return false;
    }

    // Find which pieces are attacking the king
    std::vector<Square> attackers;
    Color attackerColor = (defender == white) ? black : white;

    // Check each piece of the attacker's color
    for (int pieceType = 0; pieceType < 6; pieceType++)
    {
        U64 pieces = (attackerColor == white) ? WhitePiecesArray[pieceType] : BlackPiecesArray[pieceType];
        U64 tempPieces = pieces;

        while (tempPieces)
        {
            Square sq = static_cast<Square>(get_LSB(tempPieces));
            U64 attacks = ChessGame::GetAttacks(sq, board, pieceType);

            // Check if this piece is attacking the king
            if (attacks & kingBitboard)
            {
                attackers.push_back(sq);
                std::cout << "Piece at " << sq << " is attacking the king\n";
            }

            tempPieces &= tempPieces - 1; // Clear LSB
        }
    }

    // If more than one piece is giving check, the king must move
    // Since we already checked that the king has no moves, it's checkmate
    if (attackers.size() > 1)
    {
        std::cout << "Multiple attackers and king can't move = checkmate\n";
        std::cout << "==== DIRECT CHECKMATE CHECK END (MULTIPLE ATTACKERS) ====\n";
        return true;
    }

    // If only one piece is giving check, see if any defender piece can block or capture it
    if (attackers.size() == 1)
    {
        Square attackerSquare = attackers[0];

        // Get all pieces of the defender's color (excluding the king)
        U64 defenderPieces = (defender == white) ? WhitePieces : BlackPieces;
        defenderPieces &= ~kingBitboard; // Remove king

        // Check each defender piece
        U64 tempDefenders = defenderPieces;
        while (tempDefenders)
        {
            Square defenderSquare = static_cast<Square>(get_LSB(tempDefenders));

            // Check if this piece can block or capture the attacker
            if (CanPieceBlockOrCapture(defenderSquare, attackerSquare))
            {
                std::cout << "Piece at " << defenderSquare << " can block or capture the attacker\n";
                std::cout << "==== DIRECT CHECKMATE CHECK END (CAN BLOCK OR CAPTURE) ====\n";
                return false;
            }

            tempDefenders &= tempDefenders - 1; // Clear LSB
        }
    }

    // If we get here, the king is in check, can't move, and no other piece can block or capture
    std::cout << "Checkmate confirmed: king in check, can't move, and no piece can block or capture\n";
    std::cout << "==== DIRECT CHECKMATE CHECK END (CHECKMATE CONFIRMED) ====\n";
    return true;
}