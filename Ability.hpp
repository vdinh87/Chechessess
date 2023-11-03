#pragma once
#include "SuperChessGame.cpp"
#include "Definitions.hpp"

class SuperPiece;
class Ability
{
protected:
    std::string name;
    AbilityType type;
    SuperChessGame *game;
public:
    Ability() = default;
    Ability(const std::string &name_, const AbilityType &type_, SuperChessGame *game_);
    virtual ~Ability() = default;
    
    //methods
    virtual void Effect(const SuperPiece& piece);
    virtual void Modify(U64& move);
    virtual std::unique_ptr<Ability> Clone() const = 0;
    void UpdateLocation(const Square &square);

    //accessors
    const std::string& GetName() const;
    const AbilityType& GetType() const;
};

Ability::Ability(const std::string &name_, const AbilityType &type_, SuperChessGame *game_) : 
name(name_), type(type_), game(game_) 
{}

const std::string &Ability::GetName() const
{
    return name;
}

const AbilityType& Ability::GetType() const
{
    return type;
}

void Ability::Effect(const SuperPiece& piece)
{
    std::cout << "Normal effect" << std::endl;
}

void Ability::Modify(U64& move)
{

}

std::unique_ptr<Ability> Ability::Clone() const
{
    return nullptr;
}