#pragma once
#include "SuperChessGame.cpp"
#include "Definitions.hpp"
class Ability
{
protected:
    Square piece;
    std::string name;
    SuperChessGame *game;
public:
    Ability() = default;
    Ability(const std::string &name_, SuperChessGame *game_);
    virtual ~Ability() = default;
    
    //methods
    virtual void Effect();
    virtual std::unique_ptr<Ability> Clone() const = 0;
    void UpdatePiece(const Square &square);

    //accessors
    const std::string &GetName() const;
};

Ability::Ability(const std::string &name_, SuperChessGame *game_) : name(name_), game(game_) {}

void Ability::UpdatePiece(const Square &square)
{
    piece = square;
}

const std::string &Ability::GetName() const
{
    return name;
}

void Ability::Effect()
{
    std::cout << "Normal effect" << std::endl;
}

std::unique_ptr<Ability> Ability::Clone() const
{
    return nullptr;
}