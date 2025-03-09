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

U64 SuperChessGame::GetBoardOf(Color color)
{
    return (color == white) ? WhitePieces : BlackPieces;
}

bool SuperChessGame::InCheck(Color color) const
{
    return ChessGame::InCheck(ChessGame::GetBoard(), color, static_cast<U64>(1ULL << GetSquare(AllColorPiecesArray[color][King])));
}

bool SuperChessGame::IsWin(Color color) const
{
    // For super chess game, check if the opponent king is missing
    Color opponent = (color == white) ? black : white;
    U64 kingBitboard = (opponent == white) ? WhitePiecesArray[King] : BlackPiecesArray[King];
    return kingBitboard == 0;
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

    attacks = ChessGame::GetAttacks(square_, board, which_function);

    // Super piece modifications
    const auto it = super_pieces.find(square_);
    if (it != super_pieces.end())
        it->second->ModifyMove(attacks);

    // Filter out illegal moves
    attacks = FilterTeam(attacks, piece);

    // Only apply check filtering in actual game play, not in free move mode
    // Since we don't have access to the free move flag here, we'll make it conditional
    // in the main UI instead

    // We'll leave this commented out for now to debug the issue
    // attacks = FilterCheck(attacks, piece);

    if (GetPieceType(piece) == King)
    {
        // For kings, filter out moves that would put it in check
        // Again, leave this commented out for debugging
        // attacks = attacks & FilterLegalKingMoves(attacks, piece);
    }
    else
    {
        // For other pieces, filter out moves that would leave the king in check (pins)
        // attacks = attacks & FilterPin(attacks, piece);
    }

    return attacks;
}

std::vector<Action> SuperChessGame::Move(Square from_sq, Square to_sq)
{
    std::vector<Action> actions;
    // Basic implementation
    U64 from = 1ULL << from_sq;
    U64 to = 1ULL << to_sq;

    // Validate move
    if (!(from & board) || from_sq == to_sq)
        return actions;

    // For debugging purposes, let's skip the strict validation for now
    // U64 validMoves = GetAttacks(from_sq);
    // if (!((validMoves >> to_sq) & 1ULL)) {
    //     return actions;  // Return empty actions if the move isn't valid
    // }

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
    if (ChessGame::InCheck(board, oppositeColor, 0))
        actions.push_back(Action::Check);

    // Critical: Update the board state to reflect changes
    UpdateBoard();

    // Add extra debug print to confirm we updated
    std::cout << "Board updated after move" << std::endl;

    // Check for checkmate
    if (IsWin(white) || IsWin(black))
        actions.push_back(Action::Checkmate);

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

    // If the piece we're moving is a super piece, update its position
    if (IsSuperPiece(from_sq))
    {
        std::cout << "Moving super piece from " << SquareStrings[from_sq] << " to " << SquareStrings[to_sq] << std::endl;
        // Move the super piece from from_sq to to_sq
        super_pieces[to_sq] = std::move(super_pieces[from_sq]);
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