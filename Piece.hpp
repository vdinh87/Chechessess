#pragma once
#include <cstdint>
#include <algorithm>
#include <string>
typedef uint64_t U64;

#define FILE_A 0x0101010101010101ULL
#define FILE_B 0x0202020202020202ULL
#define FILE_C 0x0404040404040404ULL
#define FILE_D 0x0808080808080808ULL
#define FILE_E 0x1010101010101010ULL
#define FILE_F 0x2020202020202020ULL
#define FILE_G 0x4040404040404040ULL
#define FILE_H 0x8080808080808080ULL

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

// define sides
enum Color { white, black };
class Piece
{
private:
  Square position;
  Color color;
public:
  Piece(Square position_, Color color_) : 
  position(position_),
  color(color_) 
  { 
  }
  virtual ~Piece() = default;
  Color GetColor() const { return color; }
  virtual U64 GetAttacks(U64 occupancy_) const = 0; // pure virtual
  U64 GetPosition() const { return position; }
  void SetPosition(Square new_position) { position = new_position; }
};