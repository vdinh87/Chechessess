#pragma once
#include "SuperChessGame.fwd.hpp"
#include "Logger.hpp"
#include "Definitions.hpp"
#include <iostream>

class SuperPiece;
class Ability
{
protected:
    std::string name = "";
    AbilityType type;
    SuperChessGame &game;

    Logger &log;

    const int cooldown = 0;
    const int activation_turn = 0;
    int turn_last_used_ability = -999;

public:
    Ability(const std::string &name_, const AbilityType &type_, SuperChessGame &game_, Logger &log_);
    Ability(
        const std::string &name_,
        const AbilityType &type_,
        SuperChessGame &game_,
        Logger &log_,
        int cooldown_,
        int activation_turn_);
    virtual ~Ability() = default;

    // methods
    virtual bool Effect(const SuperPiece &piece);
    virtual void Modify(U64 &move, Square piece);
    virtual bool OnCapture(SuperPiece &piece, Square to_capture, Square from_sq, std::vector<Square> &keys_to_remove);
    virtual void OnGameStart(SuperPiece &piece);

    // temp?
    Square GetInputSquare(const std::string &prompt) const;

    //
    int GetCooldownTracker() const;

    // accessors
    std::string GetName() const;
    AbilityType GetType() const;

    // cloning
    virtual std::unique_ptr<Ability> Clone() const = 0;
};