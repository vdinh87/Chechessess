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
    virtual void Modify(U64& move, Square piece);
    virtual std::unique_ptr<Ability> Clone() const = 0;
    Square GetInputSquare() const;
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

void Ability::Modify(U64& move, Square piece)
{
}

Square Ability::GetInputSquare() const
{
    std::string input_str;
    Square sq;
    std::cout << "Choose square to move to: ";
    
    std::cin >> input_str;
    std::cout << std::endl;
    auto it = SqStrMap.find(input_str);
    if (it != SqStrMap.end())
        sq = it->second;
    else {
        std::cout << "Invalid square" << std::endl;
        return Square::invalid;
    }
    return sq;
}

std::unique_ptr<Ability> Ability::Clone() const
{
    return nullptr;
}