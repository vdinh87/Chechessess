#pragma once
#include "Ability.hpp"
#include "Listener.hpp"
#include "SuperChessGame.cpp"

class PawnConvert : public Ability
{
public:
    PawnConvert(SuperChessGame& game_, Logger& log_);
    ~PawnConvert() = default;

    void OnCapture(SuperPiece& piece, SuperPiece& other_piece) override;
    std::unique_ptr<Ability> Clone() const override;
    void Notify(const std::vector<ChessMove>& log) override;
    
    bool HasBuff(SuperPiece& piece) const;
};

