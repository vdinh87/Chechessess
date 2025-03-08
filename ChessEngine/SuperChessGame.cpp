#include "SuperChessGame.hpp"
#include "AbilityLibrary.hpp"

SuperChessGame::SuperChessGame() // testing purposes only
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

bool SuperChessGame::RemovePiece(Square square)
{
    std::cout << "Test: 3\n";

    U64 p = 1ULL << square;
    Piece p_type = GetPieceType(p);
    if ((p & PieceTypeArray[King]))
    {
        std::cout << "You may not remove the King from the Board!!\n";
        return false;
    }

    if (board & p)
        AddToGraveyard(GetColor(p), square, p_type);

    if (!ChessGame::RemovePiece(square))
        return false;

    if (IsSuperPiece(square))
        super_pieces.erase(square);

    UpdateBoard();

    return true;
}

bool SuperChessGame::ConvertPieceToSide(Square square, Color side)
{
    U64 p = 1ULL << square;
    if (!(p & board)) // no piece
        return false;

    if ((side == white && (WhitePieces & p)) || (side == black && (BlackPieces & p)))
    {
        std::cout << "Piece is already of color: " << ColorStrings[side] << std::endl;
        return false;
    }

    Piece p_type = GetPieceType(p);
    RemovePiece(square);
    AddPiece(square, side, p_type);
    if (IsSuperPiece(square))
        super_pieces[square]->UpdateColor(side);
    return true;
}

bool SuperChessGame::ConvertPiece(Piece from_piece, Color from_color, Square square, Piece to_piece)
{
    if (from_color == black)
    {
        clear_bit(BlackPiecesArray[from_piece], square);
        set_bit(BlackPiecesArray[to_piece], square);
        return true;
    }
    else if (from_color == white)
    {
        clear_bit(WhitePiecesArray[from_piece], square);
        set_bit(WhitePiecesArray[to_piece], square);
        return true;
    }
    return false;
}

bool SuperChessGame::ConvertToSuperPiece(SuperPieceInfo info, Square square)
{
    auto it = super_pieces.find(square);
    if (it != super_pieces.end())
        return false;
    return AddSuperPiece(info, square, GetColor(1ULL << square), true);
}

bool SuperChessGame::ConvertSuperPiecesofType(SuperPieceInfo info, Color color)
{
    CapTier(info.second, info.first);
    std::vector<std::unique_ptr<Ability>> v;

    std::pair<Color, Piece> key = std::make_pair(color, info.first);
    std::vector<Square> start_loc = StartingSquares[key];
    for (const auto &sq : start_loc)
    {
        MakeAbilityVector(v, info);
        super_pieces[sq] = std::make_shared<SuperPiece>(v, info, sq, color);
    }
    return true;
}

bool SuperChessGame::UpgradeSuperPieceTier(Square square, Tier to_tier)
{
    auto it = super_pieces.find(square);
    if (it == super_pieces.end())
        return false;

    CapTier(to_tier, GetPieceType(1ULL << square));
    it->second->UpdateTier(to_tier);
    std::vector<std::unique_ptr<Ability>> v;
    MakeAbilityVector(v, std::make_pair(GetPieceType(1ULL << square), to_tier));
    return true;
}

// move functions
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
    // super piece modify
    const auto it = super_pieces.find(square_);
    if (it != super_pieces.end())
        it->second->ModifyMove(attacks);

    attacks = FilterTeam(attacks, piece);
    attacks = FilterCheck(attacks, piece);
    if (GetPieceType(piece) == King)
        attacks = attacks & FilterLegalKingMoves(attacks, piece);
    else
        attacks = attacks & FilterPin(attacks, piece);
    return attacks;
}

U64 SuperChessGame::GetAttacks(Square square_, U64 team_filter) const
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

    // super piece modify
    const auto it = super_pieces.find(square_);
    if (it != super_pieces.end())
        it->second->ModifyMove(attacks);

    attacks = attacks & ~team_filter;
    attacks = FilterCheck(attacks, piece);
    if (GetPieceType(piece) == King)
        attacks = attacks & FilterLegalKingMoves(attacks, piece);
    else
        attacks = attacks & FilterPin(attacks, piece);

    return attacks;
}

Action SuperChessGame::Promote(Square from_sq, Square to_sq, Color from_color, Piece to_piece)
{
    if (!IsSuperPiece(from_sq))
        return ChessGame::Promote(from_sq, to_sq, from_color, to_piece);
    Piece promoting_to_piece = PromoteInput(from_sq, to_sq, from_color, to_piece);
    ExecuteMove(from_color, from_sq, to_sq, promoting_to_piece, to_piece);
    RemovePiece(from_sq);

    // piece tier of current tier - 1 static_cast<Tier>(static_cast<int>(super_pieces.at(from_sq)->GetTier()) - 1)
    SuperPieceInfo info = std::make_pair(promoting_to_piece, T1);
    ConvertToSuperPiece(info, to_sq);

    return Action::Promotion;
}

std::vector<Action> SuperChessGame::Move(Square from_sq, Square to_sq)
{
    std::vector<Action> actions;
    U64 from = 0ULL;
    U64 to = 0ULL;
    set_bit(from, from_sq);
    set_bit(to, to_sq);

    U64 ally_pieces = ChessGame::GetColor(from) == white ? WhitePieces : BlackPieces;

    // no piece on board or parameters are same square
    if (!(from & board) || (from_sq == to_sq) || (to & ally_pieces))
        return actions;

    Color from_color = ChessGame::GetColor(from);
    Piece from_piece = ChessGame::GetPieceType(from);
    Piece to_piece = ChessGame::GetPieceType(to);

    if (from_color == white)
    { // Castling Conditions
        if ((from_piece == King) && (ChessGame::GetCastling(from_color) != 0) &&
            ((to_sq == c1) || (to_sq == c8) || (to_sq == g1) || (to_sq == g8)))
        {                                                         // Castling
            U64 valid_moves = ChessGame::GetCastling(from_color); // does null check and InCheck
            ChessGame::ExecuteMove(from_color, from_sq, to_sq, from_piece, to_piece);
            // Checks which way
            if (to_sq == c1 && get_bit(valid_moves, c1))
            {
                ChessGame::ExecuteMove(from_color, a1, static_cast<Square>(static_cast<int>(to_sq) + 1), Rook, King);
            }
            else if (to_sq == g1 && get_bit(valid_moves, g1))
                ChessGame::ExecuteMove(from_color, h1, static_cast<Square>(static_cast<int>(to_sq) - 1), Rook, Rook);

            actions.push_back(Action::Castle);
        } // Promotion conditions
        else if (from_piece == Pawn && to_sq >= 56 && to_sq <= 63)
        { // Promotion
            actions.push_back(Promote(from_sq, to_sq, from_color, to_piece));
        } // En passant conditions
        else if (EnPassant(from_sq, from_piece, from_color))
        { // En passant
            ChessGame::ExecuteMove(from_color, from_sq, static_cast<Square>(static_cast<int>(GetPreviousMove().to) + 8), from_piece, to_piece);
            // clear_bit(BlackPiecesArray[to_piece], GetPreviousMove().to);
            ChessGame::RemovePiece(GetPreviousMove().to);
            actions.push_back(Action::Capture);
        }
        else // Normal Move
        {
            ChessGame::ExecuteMove(from_color, from_sq, to_sq, from_piece, to_piece);
            // if capturing
            if (board & (1ULL << to_sq))
                actions.push_back(Action::Capture);
            else
                actions.push_back(Action::Move);
        }
    }
    else if (from_color == black)
    { // Castling Conditions
        if ((from_piece == King) && (ChessGame::GetCastling(from_color) != 0) &&
            ((to_sq == c1) || (to_sq == c8) || (to_sq == g1) || (to_sq == g8)))
        {                                                         // Castling
            U64 valid_moves = ChessGame::GetCastling(from_color); // does null check and InCheck
            ChessGame::ExecuteMove(from_color, from_sq, to_sq, King, King);
            // Checks which way
            if (to_sq == c8 && get_bit(valid_moves, c8))
                ChessGame::ExecuteMove(from_color, a8, static_cast<Square>(static_cast<int>(to_sq) + 1), Rook, Rook);
            else if (to_sq == g8 && get_bit(valid_moves, g8))
                ChessGame::ExecuteMove(from_color, h8, static_cast<Square>(static_cast<int>(to_sq) - 1), Rook, Rook);

            actions.push_back(Action::Castle);
        }
        else if (from_piece == Pawn && to_sq >= 0 && to_sq <= 7)
        { // Promotion
            actions.push_back(Promote(from_sq, to_sq, from_color, to_piece));
        } // Enpassant conditoins
        else if (EnPassant(from_sq, from_piece, from_color))
        { // Enpassant
            ChessGame::ExecuteMove(from_color, from_sq, static_cast<Square>(static_cast<int>(GetPreviousMove().to) - 8), from_piece, to_piece);
            // clear_bit(WhitePiecesArray[to_piece], GetPreviousMove().to);
            ChessGame::RemovePiece(GetPreviousMove().to);
            actions.push_back(Capture);
        } // Normal move
        else
        {
            ChessGame::ExecuteMove(from_color, from_sq, to_sq, from_piece, to_piece);
            // if capturing
            if (board & (1ULL << to_sq))
                actions.push_back(Action::Capture);
            else
                actions.push_back(Action::Move);
        }
    }

    from_color = static_cast<Color>(!static_cast<bool>(from_color));
    if (ChessGame::InCheck(board, from_color, 0)) // const U64 &occupany_, Color color_of_king, int offset
        actions.push_back(Check);

    // after move
    ChessGame::UpdateBoard();

    if (IsWin(white) || IsWin(black))
        actions.push_back(Checkmate);
    return actions;
}

void SuperChessGame::ExecuteMove(Color color, Square from_sq, Square to_sq, Piece from_piece, Piece to_piece)
{
    bool do_normal_capture = true;
    // If piece is removed, check on capture effect
    std::vector<Square> keys;
    for (const auto &sp : super_pieces)
    {
        keys.push_back(sp.first);
    }

    std::vector<Square> keys_to_remove;

    if (board & (1ULL << to_sq))
    {
        for (const auto &k : keys)
        {
            std::cout << "Loop on: " << SquareStrings[k] << std::endl;
            auto it = super_pieces.find(k);
            if (it != super_pieces.end() && it->second->OnCaptureEffects(to_sq, from_sq, keys_to_remove))
            {
                std::cout << "In if statemnt\n";
                do_normal_capture = false;
            }
        }
    }

    for (const auto &k : keys_to_remove)
    {
        RemovePiece(k);
    }

    if (do_normal_capture)
    {
        // If piece is removed, add to Graveyard
        if (board & (1ULL << to_sq))
        {
            AddToGraveyard(color, to_sq, to_piece);
        }

        ChessGame::ExecuteMove(color, from_sq, to_sq, from_piece, to_piece);

        // remove to_piece
        if (IsSuperPiece(to_sq))
            super_pieces.erase(to_sq);
        // move from_piece
        if (IsSuperPiece(from_sq))
        {
            super_pieces.erase(to_sq);
            super_pieces[to_sq] = std::move(super_pieces[from_sq]);
            super_pieces.erase(from_sq);
            super_pieces[to_sq]->UpdateSquare(to_sq);
        }
    }
}

// init
void SuperChessGame::InitSuperPieces(const SuperPieceInfo &white_info, const SuperPieceInfo &black_info)
{
    // Initialize standard chess board first
    // White pieces - find the pieces of the specified type
    U64 whitePieces = GetBoardOf(white_info.first, white);
    while (whitePieces)
    {
        int sq_idx = get_LSB(whitePieces);
        Square sq = static_cast<Square>(sq_idx);

        // Create super piece
        SuperPieceInfo info = std::make_pair(white_info.first, white_info.second);
        AddSuperPiece(info, sq, white, true);

        // Clear the bit
        whitePieces &= ~(1ULL << sq_idx);
    }

    // Black pieces - find the pieces of the specified type
    U64 blackPieces = GetBoardOf(black_info.first, black);
    while (blackPieces)
    {
        int sq_idx = get_LSB(blackPieces);
        Square sq = static_cast<Square>(sq_idx);

        // Create super piece
        SuperPieceInfo info = std::make_pair(black_info.first, black_info.second);
        AddSuperPiece(info, sq, black, true);

        // Clear the bit
        blackPieces &= ~(1ULL << sq_idx);
    }
}

void SuperChessGame::InitGameStartEffects()
{
    for (auto &pair : super_pieces)
    {
        pair.second->OnGameStartEffects();
    }
}

U64 SuperChessGame::GetBoardOf(Piece piece, Color color)
{
    return (color == white) ? WhitePiecesArray[piece] : BlackPiecesArray[piece];
}

U64 SuperChessGame::GetBoardOf(Color color)
{
    return (color == white) ? WhitePieces : BlackPieces;
}

// utility
bool SuperChessGame::IsSuperPiece(const Square &key) const
{
    return super_pieces.count(key);
}

bool SuperChessGame::InCheck(Color color) const
{
    return ChessGame::InCheck(ChessGame::GetBoard(), color, static_cast<U64>(1ULL << GetSquare(AllColorPiecesArray[color][King]))); // it wouldn't take 1ull<<square unless it was assigned to a variable, or casted.
}

bool SuperChessGame::IsWin(Color color) const
{
    // For super chess game, check if the opponent king is captured
    Color opponent = (color == white) ? black : white;
    U64 kingBitboard = (opponent == white) ? WhitePiecesArray[King] : BlackPiecesArray[King];
    return kingBitboard == 0;
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
    CapTier(info.second, info.first);

    for (int i = 0; i <= static_cast<int>(info.second); i++)
    {
        v.push_back(al->GetAbility({info.first, static_cast<Tier>(i)}));
    }
}

void SuperChessGame::Swap(Square from, Square to)
{
    SuperPieceInfo from_info, to_info;
    Color from_color = GetColor(1ULL << from), to_color = GetColor(1ULL << to);

    // Using superpiece info to store even non-superpieces.
    from_info = IsSuperPiece(from) ? super_pieces[from]->GetInfo() : std::make_pair(GetPieceType(1ULL << from), not_superpiece);
    to_info = IsSuperPiece(to) ? super_pieces[to]->GetInfo() : std::make_pair(GetPieceType(1ULL << to), not_superpiece);

    RemovePiece(from);
    RemovePiece(to);
    // Using addsuperpiece if it's a superpiece, and add piece if it's not.
    to_info.second != not_superpiece ? AddSuperPiece(to_info, from, to_color, false) : AddPiece(from, to_color, to_info.first);
    from_info.second != not_superpiece ? AddSuperPiece(from_info, to, from_color, false) : AddPiece(to, from_color, from_info.first);
}

void SuperChessGame::SwapSuperPieces(Square from, Square to)
{
    auto it1 = super_pieces.find(from);
    auto it2 = super_pieces.find(to);
    auto end = super_pieces.end();

    if (it1 != end && it2 != end)
    {
        std::swap(it1->second, it2->second);
    }
    else if (it1 != end)
    {
        super_pieces.emplace(std::make_pair(to, std::move(it1->second)));
        super_pieces.erase(from);
    }
    else if (it2 != end)
    {
        super_pieces.emplace(std::make_pair(from, std::move(it2->second)));
        super_pieces.erase(to);
    }
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

// misc
bool SuperChessGame::UseAbility(Square sq, Tier t)
{
    return super_pieces[sq]->UseAbility(t);
}

// to be removed
int SuperChessGame::GetNumAbilities(Square sq)
{
    return super_pieces[sq]->GetNumberOfAbilities();
}

void SuperChessGame::PrintAbilityNames(Square sq)
{
    auto names = super_pieces[sq]->GetAbilityNames();
    for (int i = 0; i < names.size(); i++)
    {
        std::cout << "(" << i << ")" << " " << names[i] << " ";
    }
    std::cout << std::endl;
}

void SuperChessGame::PrintAbilityNames(Square sq, AbilityType type)
{
    auto all_names = super_pieces[sq]->GetAbilityNames();
    auto names = super_pieces[sq]->GetAbilityNames(type);
    for (int i = 0; i < all_names.size(); i++)
    {
        auto it = std::find(names.begin(), names.end(), all_names[i]);
        if (it != names.end())
        {
            std::cout << "(" << i << ")" << " " << all_names[i] << " ";
        }
    }
    std::cout << std::endl;
}