#pragma once
#include "SuperChessGame.cpp"
#include "Definitions.hpp"

class SuperPiece;
class Ability
{
protected:
    std::string name;
    SuperChessGame *game;
public:
    Ability() = default;
    Ability(const std::string &name_, SuperChessGame *game_);
    virtual ~Ability() = default;
    
    //methods
    virtual void Effect(const SuperPiece& piece);
    virtual std::unique_ptr<Ability> Clone() const = 0;
    void UpdateLocation(const Square &square);

    //accessors
    const std::string &GetName() const;
};

Ability::Ability(const std::string &name_, SuperChessGame *game_) : name(name_), game(game_) {}

const std::string &Ability::GetName() const
{
    return name;
}

void Ability::Effect(const SuperPiece& piece)
{
    std::cout << "Normal effect" << std::endl;
}

std::unique_ptr<Ability> Ability::Clone() const
{
    return nullptr;
}