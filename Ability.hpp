#pragma once
#include "SuperChessGame.cpp"
#include "Definitions.hpp"
class Ability
{
private:
    std::string name;
public:
    Ability();
    Ability(const std::string& name_);
    virtual ~Ability();
    const std::string& GetName() const;
    virtual void Effect();
};

Ability::Ability()
{

}

Ability::Ability(const std::string& name_)
{
    name = name_;
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