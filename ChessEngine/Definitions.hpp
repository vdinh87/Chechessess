#pragma once
#include <cstdint>
#include <iomanip>
#include <algorithm>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <cmath>
#include <stdexcept>
#include <bitset>
#include <unordered_map>
#include <map>
#include <random>
#include <utility>
#include <memory>

typedef uint64_t U64;

#define FILE_A 0x0101010101010101ULL
#define FILE_B 0x0202020202020202ULL
#define FILE_C 0x0404040404040404ULL
#define FILE_D 0x0808080808080808ULL
#define FILE_E 0x1010101010101010ULL
#define FILE_F 0x2020202020202020ULL
#define FILE_G 0x4040404040404040ULL
#define FILE_H 0x8080808080808080ULL

#define RANK_1 0x00000000000000FFULL
#define RANK_2 0x000000000000FF00ULL
#define RANK_3 0x0000000000FF0000ULL
#define RANK_4 0x00000000FF000000ULL
#define RANK_5 0x000000FF00000000ULL
#define RANK_6 0x0000FF0000000000ULL
#define RANK_7 0x00FF000000000000ULL
#define RANK_8 0xFF00000000000000ULL

#define set_bit(b, i) ((b) |= (1ULL << i))
#define get_bit(b, i) ((b) & (1ULL << i))
#define clear_bit(b, i) ((b) &= ~(1ULL << i))
#define get_LSB(b) (__builtin_ctzll(b))
#define pop_bit(bitboard, square) (get_bit(bitboard, square) ? (bitboard ^= (1ULL << square)) : 0)

// First all the enums
enum Piece : unsigned char
{
  Pawn,
  Knight,
  Bishop,
  Rook,
  Queen,
  King
};

enum Color : unsigned char
{
  white,
  black
};

enum Square
{
  a1,
  b1,
  c1,
  d1,
  e1,
  f1,
  g1,
  h1,
  a2,
  b2,
  c2,
  d2,
  e2,
  f2,
  g2,
  h2,
  a3,
  b3,
  c3,
  d3,
  e3,
  f3,
  g3,
  h3,
  a4,
  b4,
  c4,
  d4,
  e4,
  f4,
  g4,
  h4,
  a5,
  b5,
  c5,
  d5,
  e5,
  f5,
  g5,
  h5,
  a6,
  b6,
  c6,
  d6,
  e6,
  f6,
  g6,
  h6,
  a7,
  b7,
  c7,
  d7,
  e7,
  f7,
  g7,
  h7,
  a8,
  b8,
  c8,
  d8,
  e8,
  f8,
  g8,
  h8,
  invalid,
};

static const std::string SquareStrings[] = {
    "a1",
    "b1",
    "c1",
    "d1",
    "e1",
    "f1",
    "g1",
    "h1",
    "a2",
    "b2",
    "c2",
    "d2",
    "e2",
    "f2",
    "g2",
    "h2",
    "a3",
    "b3",
    "c3",
    "d3",
    "e3",
    "f3",
    "g3",
    "h3",
    "a4",
    "b4",
    "c4",
    "d4",
    "e4",
    "f4",
    "g4",
    "h4",
    "a5",
    "b5",
    "c5",
    "d5",
    "e5",
    "f5",
    "g5",
    "h5",
    "a6",
    "b6",
    "c6",
    "d6",
    "e6",
    "f6",
    "g6",
    "h6",
    "a7",
    "b7",
    "c7",
    "d7",
    "e7",
    "f7",
    "g7",
    "h7",
    "a8",
    "b8",
    "c8",
    "d8",
    "e8",
    "f8",
    "g8",
    "h8",
};

// Then the hash struct needed for the map
struct PairEnumHash
{
  template <typename Enum1, typename Enum2>
  std::size_t operator()(std::pair<Enum1, Enum2> t) const
  {
    return static_cast<std::size_t>(t.first) * 2 + static_cast<std::size_t>(t.second);
  }
};

// Now the extern declarations that use the hash
extern std::vector<U64> WhiteSpawnLocations;
extern std::vector<U64> BlackSpawnLocations;
extern std::unordered_map<std::pair<Color, Piece>, std::vector<Square>, PairEnumHash> StartingSquares;

static std::unordered_map<std::string, Square> const SqStrMap = {
    {"a1", Square::a1}, {"a2", Square::a2}, {"a3", Square::a3}, {"a4", Square::a4}, {"a5", Square::a5}, {"a6", Square::a6}, {"a7", Square::a7}, {"a8", Square::a8}, {"b1", Square::b1}, {"b2", Square::b2}, {"b3", Square::b3}, {"b4", Square::b4}, {"b5", Square::b5}, {"b6", Square::b6}, {"b7", Square::b7}, {"b8", Square::b8}, {"c1", Square::c1}, {"c2", Square::c2}, {"c3", Square::c3}, {"c4", Square::c4}, {"c5", Square::c5}, {"c6", Square::c6}, {"c7", Square::c7}, {"c8", Square::c8}, {"d1", Square::d1}, {"d2", Square::d2}, {"d3", Square::d3}, {"d4", Square::d4}, {"d5", Square::d5}, {"d6", Square::d6}, {"d7", Square::d7}, {"d8", Square::d8}, {"e1", Square::e1}, {"e2", Square::e2}, {"e3", Square::e3}, {"e4", Square::e4}, {"e5", Square::e5}, {"e6", Square::e6}, {"e7", Square::e7}, {"e8", Square::e8}, {"f1", Square::f1}, {"f2", Square::f2}, {"f3", Square::f3}, {"f4", Square::f4}, {"f5", Square::f5}, {"f6", Square::f6}, {"f7", Square::f7}, {"f8", Square::f8}, {"g1", Square::g1}, {"g2", Square::g2}, {"g3", Square::g3}, {"g4", Square::g4}, {"g5", Square::g5}, {"g6", Square::g6}, {"g7", Square::g7}, {"g8", Square::g8}, {"h1", Square::h1}, {"h2", Square::h2}, {"h3", Square::h3}, {"h4", Square::h4}, {"h5", Square::h5}, {"h6", Square::h6}, {"h7", Square::h7}, {"h8", Square::h8}};

static const std::string PieceStrings[] = {
    "P",
    "K",
    "B",
    "R",
    "Q",
    "K",
};

const std::string ColorStrings[] = {"White", "Black"};

enum Direction : unsigned char
{
  NW,
  N,
  NE,
  E,
  SW,
  S,
  SE,
  W
};

enum Tier : unsigned char
{
  T0,
  T1,
  T2,
  T3,
  T4,
  not_superpiece
};
const std::string TierStrings[] = {"T0", "T1", "T2", "T3", "T4"};
enum AbilityType : unsigned char
{
  active,
  move_modifer,
  on_capture,
  on_game_start
};

enum Action : unsigned char
{
  Capture,
  Check,
  Castle,
  Checkmate,
  Promotion,
  Move,
  Abilityes
};
const std::string ActionStrings[] = {"Capture", "Check", "Castle", "Checkmate", "Promotion", "Move", "Ability"};

struct ChessMove
{
  Square from = Square::invalid;
  Square to = Square::invalid;
  Piece type;
  std::vector<Action> actions;
  bool is_pawn; // To be replaced
};

struct EnumClassComparer
{
  template <typename T>
  bool operator()(T lhs, T rhs) const
  {
    return static_cast<int>(lhs) < static_cast<int>(rhs);
  }
};

using SuperPieceInfo = std::pair<Piece, Tier>;
struct PairInsidePairEnumHash
{
  template <typename Enum, typename Pair>
  std::size_t operator()(std::pair<Enum, Pair> t) const
  {
    return static_cast<std::size_t>(t.first) * 2 + static_cast<std::size_t>(t.second.first);
  }
};

inline U64 North(U64 &other_board)
{
  if (~(other_board & RANK_8))
    return other_board << 8;
  return 0ULL;
}

inline U64 NorthWest(U64 &other_board)
{
  if (!(other_board & RANK_8) && !(other_board & FILE_A))
    return other_board << 7;
  return 0ULL;
}

inline U64 NorthEast(U64 &other_board)
{
  if (!(other_board & RANK_8) && !(other_board & FILE_H))
    return other_board << 9;
  return 0ULL;
}

inline U64 East(U64 &other_board)
{
  if (!(other_board & FILE_H))
    return other_board << 1;
  return 0ULL;
}

inline U64 South(U64 &other_board)
{
  if (!(other_board & RANK_1))
    return other_board >> 8;
  return 0ULL;
}

inline U64 SouthWest(U64 &other_board)
{
  if (!(other_board & RANK_1) && !(other_board & FILE_A))
    return other_board >> 9;
  return 0ULL;
}

inline U64 SouthEast(U64 &other_board)
{
  if (!(other_board & RANK_1) && !(other_board & FILE_H))
    return other_board >> 7;
  return 0ULL;
}

inline U64 West(U64 &other_board)
{
  if (!(other_board & FILE_A))
    return other_board >> 1;
  return 0ULL;
}

inline U64 GoInDirection(Direction direction, U64 &other_board)
{
  U64 board = 0ULL;
  switch (direction)
  {
  case NW:
    board = NorthWest(other_board);
    break;
  case N:
    board = North(other_board);
    break;
  case NE:
    board = NorthEast(other_board);
    break;
  case E:
    board = East(other_board);
    break;
  case SW:
    board = SouthWest(other_board);
    break;
  case S:
    board = South(other_board);
    break;
  case SE:
    board = SouthEast(other_board);
    break;
  case W:
    board = West(other_board);
    break;
  default:
    break;
  }
  return board;
}

inline double GetDistance(Square from_sq, Square to_sq)
{
  double distance = std::abs((double)(from_sq - to_sq) / 8);
  if (distance < 1) // if decimal
    distance = distance * 8;
  distance = std::round(distance);
  return distance;
}

inline U64 GetRay(Square from_sq, Square to_sq)
{
  U64 from = 0ULL;
  U64 to = 0ULL;
  set_bit(from, from_sq);
  set_bit(to, to_sq);
  int distance = GetDistance(from_sq, to_sq);
  U64 ray = from;
  for (int dir = NW; dir <= W; dir++)
  {
    for (int steps = 0; steps < distance; steps++)
      ray = ray | GoInDirection((Direction)dir, ray);

    if (to & ray)
      break;
    else
      ray = from;
  }
  ray = (ray & ~from) & (ray & ~to);
  return ray;
}

inline U64 GetRay(Square from_sq, Direction dir, U64 occupancy = 0ULL)
{
  U64 from = 1ULL << from_sq;
  U64 ray = from;

  while (GoInDirection(dir, from) != 0ULL)
  {
    from = GoInDirection(dir, from);
    ray |= from;

    if (from & occupancy)
      break;
  }

  ray &= ~(1ULL << from_sq);
  return ray;
}

inline U64 GetFirstCollision(Square from_sq, Direction dir, U64 occupancy)
{
  U64 collision = 1ULL << from_sq;

  while (GoInDirection(dir, collision) != 0ULL)
  {
    collision = GoInDirection(dir, collision);

    if (collision & occupancy)
      return collision;
  }

  return 0ULL;
}

inline Square GetSquare(const U64 &bitboard)
{
  return static_cast<Square>(get_LSB(bitboard));
}

inline void PrintGoard(U64 board)
{
  std::string boardString;
  U64 square;
  std::cout << "Chess Board: " << std::endl;
  for (int i = 63; i >= 0; i--)
  {
    square = get_bit(board, i);
    if (square)
      boardString += "1 ";
    else
      boardString += "0 ";

    if ((i % 8) == 0)
    {
      boardString += " " + std::to_string(i / 8 + 1);
      std::reverse(boardString.begin(), boardString.end());
      std::cout << boardString << std::endl;
      boardString.clear();
    }
  }
  std::cout << "   a b c d e f g h\n"
            << std::endl;
}