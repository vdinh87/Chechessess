#pragma once
#include <list>
#include <string>

class Piece {

private:
 bool color_; //0 black   |    1 white
 bool is_superpiece;
 std::list<std::string> movesList;
 std::string location;
 int tier;
 std::string type_;

public:
 Piece(std::string type, bool color): type_(type), color_(color) {}
 virtual ~Piece() = default;

 //getters
 bool getColor() const  {
  return color_;
 }
 std::string getType() const {
  return type_;
 }
 bool isSuperPiece() const {
  return is_superpiece;
 }
 std::list<std::string> getmovesList() const {
  return movesList;
 }
 std::string getLocation() const {
  return location;
 }
 int getTier() const {
  return tier;
 }


};

