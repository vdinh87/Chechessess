#pragma once
#include "Ability.hpp"
#include "Listener.hpp"
#include "SuperChessGame.cpp"

class PawnConvertSuper : public Ability
{
public:
    PawnConvertSuper(SuperChessGame& game_, Logger& log_);
    ~PawnConvertSuper() = default;

    void OnCapture(SuperPiece& piece, SuperPiece& other_piece) override;
    std::unique_ptr<Ability> Clone() const override;
    void Notify(const std::vector<ChessMove>& log) override;
    
    bool HasBuff(SuperPiece& piece) const;
};

