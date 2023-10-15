#pragma once
#include "SuperChessGame.hpp"
#include "AbilityLibrary.cpp"
#include "SuperPiece.cpp"
SuperChessGame::SuperChessGame(const SuperPieceInfo &white_info, const SuperPieceInfo &black_info)
{
    // InitSuperPieces(white, black);
    al = std::make_shared<AbilityLibrary>(this);

    // AddSuperPiece(white_info, Square::b3, Color::white);
    // ConvertToSuperPiece(white_info, Square::e1);
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

bool SuperChessGame::AddSuperPiecesofType(SuperPieceInfo info, Color color)
{
    return false;
}

bool SuperChessGame::RemovePiece(Square square)
{
    if (!ChessGame::RemovePiece(square))
        return false;

    // remove if superpiece
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

std::vector<Action> SuperChessGame::Move(Square from_sq, Square to_sq)
{
    // for passive abilities later
    return ChessGame::Move(from_sq, to_sq);
}

void SuperChessGame::ExecuteMove(Color color, Square from_sq, Square to_sq, Piece from_piece, Piece to_piece)
{
    // If piece is removed, update Graveyard
    if (board & (1ULL << to_sq))
    {
        SuperPieceInfo info = std::make_pair(to_piece, Tier::not_superpiece);
        if (IsSuperPiece(to_sq))
        {
            info.second = super_pieces[to_sq]->GetInfo().second;
        }
        if (graveyard.find(std::make_pair(GetColor(to_piece), info)) != graveyard.end())
        {
            graveyard[std::make_pair(GetColor(to_piece), info)]++;
        }
        else
        {
            graveyard[std::make_pair(GetColor(to_piece), info)] = 1;
        }
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

// utility
bool SuperChessGame::IsSuperPiece(const Square &key) const
{
    auto it = super_pieces.find(key);
    if (it != super_pieces.end())
        return true;

    return false;
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
    // to remove
    v.push_back(al->GetAbility(info));

    // to add
    //  for(int i = 0; i < info.second; i++)
    //  {
    //      info.second = static_cast<Tier>(i);
    //      v.push_back(al->GetAbility(info));
    //  }
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
void SuperChessGame::Do(Square sq, Tier t)
{
    super_pieces[sq]->UseAbility(t);
}