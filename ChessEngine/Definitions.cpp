#include "Definitions.hpp"

std::vector<U64> WhiteSpawnLocations{
    0x000000000000FF00ULL,
    0x0000000000000042ULL,
    0x0000000000000024ULL,
    0x0000000000000081ULL,
    0x0000000000000008ULL,
    0x0000000000000010ULL};

std::vector<U64> BlackSpawnLocations{
    0x00FF000000000000ULL,
    0x4200000000000000ULL,
    0x2400000000000000ULL,
    0x8100000000000000ULL,
    0x0800000000000000ULL,
    0x1000000000000000ULL};

std::unordered_map<std::pair<Color, Piece>, std::vector<Square>, PairEnumHash> StartingSquares = {
    {{Color::white, Piece::Pawn}, {Square::a2, Square::b2, Square::c2, Square::d2, Square::e2, Square::f2, Square::g2, Square::h2}},
    {{Color::white, Piece::Knight}, {Square::b1, Square::g1}},
    {{Color::white, Piece::Bishop}, {Square::c1, Square::f1}},
    {{Color::white, Piece::Rook}, {Square::a1, Square::h1}},
    {{Color::white, Piece::Queen}, {Square::d1}},
    {{Color::white, Piece::King}, {Square::e1}},

    {{Color::black, Piece::Pawn}, {Square::a7, Square::b7, Square::c7, Square::d7, Square::e7, Square::f7, Square::g7, Square::h7}},
    {{Color::black, Piece::Knight}, {Square::b8, Square::g8}},
    {{Color::black, Piece::Bishop}, {Square::c8, Square::f8}},
    {{Color::black, Piece::Rook}, {Square::a8, Square::h8}},
    {{Color::black, Piece::Queen}, {Square::d8}},
    {{Color::black, Piece::King}, {Square::e8}},
};