#pragma once
#include "Magics.cpp"

class ChessGame
{
protected:
    // board
    std::vector<U64> WhitePiecesArray;
    std::vector<U64> BlackPiecesArray;
    std::vector<U64> AllColorPiecesArray;
    std::vector<U64> PieceTypeArray;
    U64 WhitePieces;
    U64 BlackPieces;
    U64 board;

    //for castling
    ChessMove prevMove;

    // Init magics
    void InitMagics() const;
    U64 Mask(Piece piece, Color color, const U64 &other_board) const;

    // get attacks
    U64 GetKnightAttacks(Square square, const U64 occupancy_) const;
    U64 GetBishopAttacks(Square square, const U64 occupancy_) const;
    U64 GetRookAttacks(Square square, const U64 occupancy_) const;
    U64 GetQueenAttacks(Square square, const U64 occupancy_) const;
    U64 GetKingAttacks(Square square, const U64 occupancy_) const;
    U64 GetPawnAttacks(Square square, const U64 occupancy_) const;
    U64 GetAttacks(Square square_, const U64 occupancy_, int which_function = -1) const;

    // checking
    U64 InCheck(const U64 &occupancy_, Color color_of_king, int offset) const;
    U64 InCheck(const U64 &occupancy_, Color color_of_king, const U64 &king) const;

    // filter functions
    U64 FilterTeam(const U64 &moveset, const U64 &piece) const;
    U64 FilterCheck(const U64 &moveset, const U64 &piece) const;
    U64 FilterPin(const U64 &moveset, const U64 &piece) const;
    U64 FilterLegalKingMoves(const U64 &moveset, const U64 &piece) const;

    // Utility functions
    void UpdatePrevMove(Square from, Square to, Piece type);
    Color GetColor(U64 piece) const;
    bool IsSlider(const U64 board_) const;
    Action RegMove(Color color, Square from_sq, Square to_sq, Piece from_piece, Piece to_piece);
    void UpdateBoard();

    // Special move functions
    U64 GetCastling(Color color) const;
    Action Castle(Square from_sq, Square to_sq, U64 valid_moves);
    U64 GetEnPassant(Square square, const U64 occupancy_, Color color) const;
    bool EnPassant(Square square, Piece type, Color color) const;
    Action Promote(Square from_sq, Square to_sq, Color color, Piece to_piece);

public:
    ChessGame();
    ~ChessGame() = default;

    //public getters
    U64 GetPiecesOfColor(Color color) const;
    U64 GetBoard() const;

    // check if win
    bool IsWin(Color color) const;

    // returns movelist
    U64 GetAttacks(Square square_) const;

    //moves pieces
    virtual std::vector<Action> Move(Square from_sq, Square to_sq);

    //Returns which type of piece it is
    Piece GetPieceType(U64 unknown_piece) const;

    void PrintBoard() const;
};