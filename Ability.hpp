#pragma once
#include "SuperChessGame.cpp"
#include "Definitions.hpp"
class Ability
{
protected:
    std::string name;
    SuperChessGame* game;

public:
    Ability();
    Ability(const std::string& name_, SuperChessGame* game_);
    virtual ~Ability();
    const std::string& GetName() const;
    virtual void Effect();
};

Ability::Ability()
{

}

Ability::Ability(const std::string& name_, SuperChessGame* game_)
{
    name = name_;
    game = game_;
}

Ability::~Ability()
{
}

const std::string& Ability::GetName() const
{
    return name;
}

void Ability::Effect()
{
    std::cout << "Normal effect" << std::endl;
}