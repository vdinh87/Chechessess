#pragma once
#include <utility>

// Forward declarations
class SuperChessGame;
class SuperPiece;
class Ability;
class Logger;
class AbilityLibrary;

// Enums
enum class Color;
enum class Piece;
enum class Tier;
enum class AbilityType;

// Type aliases
using Square = int;
using U64 = unsigned long long;
using SuperPieceInfo = std::pair<Piece, Tier>;