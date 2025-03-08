#pragma once
#include "Ability.hpp"
#include "Definitions.hpp"

class SuperPiece
{
private:
    std::map<Tier, std::unique_ptr<Ability>, EnumClassComparer> abilities;
    // std::unordered_map<Tier, std::unique_ptr<Ability>> abilities;
    SuperPieceInfo info;
    Square sq;
    Color color;

public:
    SuperPiece(std::vector<std::unique_ptr<Ability>> &abilities_, SuperPieceInfo info_, Square sq_, Color color_);
    ~SuperPiece() = default;

    // Ability use
    bool UseAbility(Tier key);
    void ModifyMove(U64 &move);
    bool OnCaptureEffects(Square to_capture, Square from_sq, std::vector<Square> &keys_to_remove);
    void OnGameStartEffects();
    // updaters
    void UpdateSquare(Square to_sq);
    void UpdateColor(Color new_color);
    void UpdateTier(Tier t);
    void AddAbilities(std::vector<std::unique_ptr<Ability>> &abilities_);

    // Getters
    SuperPieceInfo GetInfo() const;
    Square GetSquare() const;
    Color GetColor() const;
    Piece GetPieceType() const;
    Tier GetTier() const;

    // to be removed? just for testing tbh
    std::vector<std::string> GetAbilityNames() const;
    std::vector<std::string> GetAbilityNames(AbilityType type) const;

    int GetNumberOfAbilities() const;
};
