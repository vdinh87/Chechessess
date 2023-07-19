#pragma once
#include "Magics.cpp"

class ChessGame
{
private:
    // white
    U64 WhitePawns;
    U64 WhiteKnights;
    U64 WhiteBishops;
    U64 WhiteRooks;
    U64 WhiteQueens;
    U64 WhiteKings;
    // black
    U64 BlackPawns;
    U64 BlackKnights;
    U64 BlackBishops;
    U64 BlackRooks;
    U64 BlackQueens;
    U64 BlackKings;
    //board
    U64 WhitePieces;
    U64 BlackPieces;
    U64 board;

    //Innit magics
    void InitMagics() const;
    //masking functions
    bool PawnMask   (const U64& otherBoard, Color color) const;
    bool KnightMask (const U64& otherBoard, Color color) const;
    bool BishopMask (const U64& otherBoard, Color color) const;
    bool RookMask   (const U64& otherBoard, Color color) const;
    bool QueenMask  (const U64& otherBoard, Color color) const;
    bool KingMask   (const U64& otherBoard, Color color) const;

    //get attacks
    U64 GetPawnAttacks(U64 position_, const U64 occupancy_) const;
    U64 GetKnightAttacks(U64 position_, const U64 occupancy_) const;
    U64 GetBishopAttacks(U64 position_, const U64 occupancy_) const;
    U64 GetRookAttacks(U64 position_, const U64 occupancy_) const;
    U64 GetQueenAttacks(U64 position_, const U64 occupancy_) const;
    U64 GetKingAttacks(U64 position_, const U64 occupancy_) const;

    //filter functions
    U64 FilterAllMoves(const U64& otherBoard, Color color) const;
    U64 FilterCheck(const U64& otherBoard, Color color) const;
    U64 FilterTeam(const U64& otherBoard, Color color) const;
    
    //Utility functions
    Color GetColor(U64 piece) const;

    void UpdateBoard();
public:
    ChessGame(/* args */);
    ~ChessGame() = default;
    U64 GetAttacks(Square square_, const U64 occupancy_) const;
    void PrintBoard() const;
};