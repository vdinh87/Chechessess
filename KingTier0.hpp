#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class KingTier0 : public Ability
{
private:
    
public:
    KingTier0(SuperChessGame& game_, Logger& log_);
    ~KingTier0() = default;

    std::unique_ptr<Ability> Clone() const override;
};

KingTier0::KingTier0(SuperChessGame& game_, Logger& log_):
Ability("KingTier0", AbilityType::active, game_, log_)
{
}

std::unique_ptr<Ability> KingTier0::Clone() const
{
    return std::make_unique<KingTier0>(*this);
}

