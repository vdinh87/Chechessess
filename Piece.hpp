#pragma once
#include <list>
#include <string>
#include <unordered_map>

class Piece
{
private:
 bool color_; // 0 black | 1 white
 bool is_superpiece;
 std::string location;
 int tier;
 std::string type_;
 static std::unordered_map<std::string, std::list<std::string>> movelist_;

public:
 Piece(std::string type, bool color) : type_(type), color_(color), is_superpiece(false)  {
  if (movelist_.empty())
   movelist_ = std::unordered_map<std::string, std::list<std::string>>();
 }

 virtual ~Piece() = default;

 // Getters
 bool getColor() const { return color_; }
 std::string getType() const { return type_; }
 bool isSuperPiece() const { return is_superpiece; }
 std::string getLocation() const { return location; }
 int getTier() const { return tier; }
 static std::unordered_map<std::string, std::list<std::string>> &getMoveList() { return movelist_; }
};

// Initialize the static member variable outside the class definition
std::unordered_map<std::string, std::list<std::string>> Piece::movelist_;