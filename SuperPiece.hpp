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
    
    //Ability use
    bool UseAbility(Tier key);

    //updaters
    void UpdateSquare(Square to_sq);
    void UpdateColor(Color new_color);
    void UpdateTier(Tier t);
    void AddAbilities(std::vector<std::unique_ptr<Ability>>& abilities_);

    //Getters
    void ModifyMove(U64& move);
    SuperPieceInfo GetInfo() const;
    Square GetSquare() const;
    Color GetColor() const;

    //to be removed? just for testing tbh
    void GetAbilityNames() const;
};
