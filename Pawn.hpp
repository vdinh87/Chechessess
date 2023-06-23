#pragma once
#include "Piece.hpp"


class Pawn: public Piece{
 Pawn(std::string color) : Piece("Pawn", color) {};
};