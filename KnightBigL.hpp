#pragma once
#include "Ability.hpp"
#include "SuperChessGame.cpp"

class KnightBigL : public Ability
{
private:
    // std::string name = "Knight Big L";
    
public:
    KnightBigL(SuperChessGame& game_, Logger& log_);
    ~KnightBigL() = default;

    void Modify(U64& move, Square piece) override;
    std::unique_ptr<Ability> Clone() const override;
};

