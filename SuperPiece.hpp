#pragma once
#include "Ability.hpp"
#include "Definitions.hpp"
class SuperPiece
{
private:
    std::unordered_map<Tier, std::unique_ptr<Ability>> abilities;
    SuperPieceInfo info;
    Square sq;
    Color color;
public:
    SuperPiece(std::vector<std::unique_ptr<Ability>>& abilities_, SuperPieceInfo info_, Square sq_, Color color_);
    ~SuperPiece() = default;
    bool UseAbility(Tier key);
};
