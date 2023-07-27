#pragma once
#include <cstdint>
#include <algorithm>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <cmath>
#include <stdexcept>

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

enum Square {
  a1, b1, c1, d1, e1, f1, g1, h1,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a8, b8, c8, d8, e8, f8, g8, h8
};

enum Piece { Pawn, Knight, Bishop, Rook, Queen, King };

// define sides
enum Color { white, black };

enum Direction { 
  NW, N, NE, E,
  SW, S, SE, W
};

U64 North(U64 & other_board)
{
  if( ~(other_board & RANK_8) )
      return other_board << 8;
  return 0ULL;
}

U64 NorthWest(U64 & other_board)
{
  if( ~(other_board & RANK_8) | ~(other_board & FILE_A) )
      return other_board << 7;
  return 0ULL;
}

U64 NorthEast(U64 & other_board)
{
  if( ~(other_board & RANK_8) | ~(other_board & FILE_H) )
      return other_board << 9;
  return 0ULL;
}

U64 East(U64 & other_board)
{
  if( ~(other_board & FILE_H) )
      return other_board << 1;
  return 0ULL;
}

U64 South(U64 & other_board)
{
  if( ~(other_board & RANK_1) )
      return other_board >> 8;
  return 0ULL;
}

U64 SouthWest(U64 & other_board)
{
  if( ~(other_board & RANK_1) | ~(other_board & FILE_A) )
      return other_board >> 9;
  return 0ULL;
}

U64 SouthEast(U64 & other_board)
{
  if( ~(other_board & RANK_1) | ~(other_board & FILE_H) )
      return other_board >> 7;
  return 0ULL;
}

U64 West(U64 & other_board)
{
  if( ~(other_board & FILE_A) )
      return other_board >> 1;
  return 0ULL;
}

U64 GoInDirection(Direction direction, U64 & other_board)
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

// inclusive to the to parameter
// Ex: a1 -> c1 = 2 units
double GetDistance(Square from, Square to) 
{
  double distance = std::abs( (double)(from - to) / 8);
  if( distance < 1 ) // if decimal
    distance = distance * 8;
  distance = std::round(distance);
  return distance;
}

U64 GetRay(U64 from, U64 to, int distance)
{
  U64 ray = from;
  //uses direction enum
  for( int dir = NW; dir <= W; dir++ ) 
  {   
    // form ray
    for( int steps = 0; steps < distance; steps++ )
      ray = ray | GoInDirection( (Direction)dir, ray );

    // Check if to_square is in ray
    if( to & ray )
      break;
    else
      ray = from;     
  }

  // Clean up ray
  ray = (ray & ~from) & (ray & ~to);
  return ray;
}
