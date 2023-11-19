#pragma once
#include "SuperChessGame.hpp"
#include "AbilityLibrary.cpp"
#include "SuperPiece.cpp"

SuperChessGame::SuperChessGame() //testing purposes only
{
    // InitSuperPieces(white, black);
    al = std::make_shared<AbilityLibrary>(*this, log);
}

SuperChessGame::SuperChessGame(const SuperPieceInfo &white_info, const SuperPieceInfo &black_info)
{
    // InitSuperPieces(white, black);
    al = std::make_shared<AbilityLibrary>(*this, log);

    ConvertSuperPiecesofType(white_info, Color::white);
    ConvertSuperPiecesofType(black_info, Color::black);
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

bool SuperChessGame::RemovePiece(Square square)
{
    U64 p = 1ULL << square;
    Piece p_type = GetPieceType(p);
    if ((p & PieceTypeArray[King]))
    {
        std::cout << "You may not remove the King from the Board!!\n";
        return false;
    }
    
    if (board & p)
        AddToGraveyard(GetColor(p_type), square, p_type);

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
    MakeAbilityVector(v, info);

    std::pair<Color, Piece> key = std::make_pair(color, info.first);
    std::vector<Square> start_loc = StartingSquares[key];
    for(int i = 0; i < start_loc.size(); i++)
    {
        Square sq = start_loc[i];
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

//move functions
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
    
    //super piece modify
    const auto it = super_pieces.find(square_);
    if( it != super_pieces.end() )
        it->second->ModifyMove(attacks);

    attacks = FilterTeam(attacks, piece);
    attacks = FilterCheck(attacks, piece);
    if (GetPieceType(piece) == King)
        attacks = attacks & FilterLegalKingMoves(attacks, piece);
    else
        attacks = attacks & FilterPin(attacks, piece);

    return attacks;
}

std::vector<Action> SuperChessGame::Move(Square from_sq, Square to_sq)
{
    // for passive abilities later
    return ChessGame::Move(from_sq, to_sq);
}

void SuperChessGame::ExecuteMove(Color color, Square from_sq, Square to_sq, Piece from_piece, Piece to_piece)
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

// init
void SuperChessGame::InitSuperPieces(const SuperPieceInfo &white, const SuperPieceInfo &black)
{
}

U64 SuperChessGame::GetBoardOf(Piece piece, Color color)
{
    return (color == white) ? WhitePiecesArray[piece] : BlackPiecesArray[piece];
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
     
    for(int i = 0; i <= info.second; i++)
    {
        v.push_back(al->GetAbility( {info.first, static_cast<Tier>(i)} ));
    }
}

void SuperChessGame::Swap(Square from, Square to)
{
    SuperPieceInfo from_info, to_info;
    Color from_color = GetColor(1ULL << from), to_color = GetColor(1ULL << to);

    //Using superpiece info to store even non-superpieces.
    from_info = IsSuperPiece(from) ? super_pieces[from]->GetInfo() : std::make_pair(GetPieceType(1ULL << from), not_superpiece);
    to_info = IsSuperPiece(to) ? super_pieces[to]->GetInfo() : std::make_pair(GetPieceType(1ULL << to), not_superpiece);
    
    RemovePiece(from);
    RemovePiece(to);
    //Using addsuperpiece if it's a superpiece, and add piece if it's not.
    to_info.second != not_superpiece ? AddSuperPiece(to_info, from, to_color, false) : AddPiece(from, to_color, to_info.first);
    from_info.second != not_superpiece ? AddSuperPiece(from_info, to, from_color, false) : AddPiece(to, from_color, from_info.first);
}

void SuperChessGame::SwapSuperPieces(Square from, Square to)
{
    auto it1 = super_pieces.find(from);
    auto it2 = super_pieces.find(to);
    auto end = super_pieces.end();

    if(it1 != end && it2 != end) {
        std::swap(it1->second, it2->second);
    }
    else if(it1 != end) {
        super_pieces.emplace(std::make_pair(to, std::move(it1->second)));
        super_pieces.erase(from);
    }
    else if(it2 != end) {
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

    auto key = std::make_pair(GetColor(piece), info);

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
            pieces.push_back(p.first.second);

    return pieces;
}

// misc
bool SuperChessGame::UseAbility(Square sq, Tier t)
{
    return super_pieces[sq]->UseAbility(t);
}

//to be removed
int SuperChessGame::GetNumAbilities(Square sq)
{
    return super_pieces[sq]->GetNumberOfAbilities();
}

void SuperChessGame::PrintAbilityNames(Square sq)
{
    auto names = super_pieces[sq]->GetAbilityNames();
    for(int i = 0; i < names.size(); i++)
    {
        std::cout << "(" << i << ")" << " " << names[i] << " ";
    }
    std::cout << std::endl;
}