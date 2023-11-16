#pragma once
#include "SuperChessGame.hpp"
#include "Logger.hpp"
#include "Definitions.hpp"

class SuperPiece;
class Ability
{
protected:
    std::string name;
    AbilityType type;
    SuperChessGame& game;

    Logger& log;

    const int cooldown = 0;
    const int activation_turn = 0;
    int turn_last_used_ability = 0;
public:
    Ability() = default;
    Ability(const std::string &name_, const AbilityType &type_, SuperChessGame& game_, Logger& log_);
    Ability(
            const std::string &name_, 
            const AbilityType &type_, 
            SuperChessGame& game_,
            Logger& log_,
            int cooldown_,
            int activation_turn_
    );
    virtual ~Ability() = default;
    
    //methods
    virtual void Effect(const SuperPiece& piece);
    virtual void Modify(U64& move, Square piece);
    virtual void OnCapture(SuperPiece& piece, SuperPiece& other_piece);

    //temp?
    Square GetInputSquare(const std::string& prompt) const;

    //
    int GetCooldownTracker() const;
    //accessors
    const std::string& GetName() const;
    const AbilityType& GetType() const;

    //cloning
    virtual std::unique_ptr<Ability> Clone() const = 0;

    
};

Ability::Ability(const std::string &name_, const AbilityType &type_, SuperChessGame& game_, Logger& log_) : 
name(name_), type(type_), game(game_), log(log_)
{}

Ability::Ability(
    const std::string &name_, 
    const AbilityType &type_, 
    SuperChessGame& game_, 
    Logger& log_, 
    int cooldown_, 
    int activation_turn_
) :
name(name_), type(type_), game(game_), log(log_), cooldown(cooldown_), activation_turn(activation_turn_)
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
    std::cout << "Normal modify" << std::endl;
}

void Ability::OnCapture(SuperPiece& piece, SuperPiece& other_piece)
{
    std::cout << "Normal on capture" << std::endl;
}

Square Ability::GetInputSquare(const std::string& prompt) const
{
    std::string input_str;
    Square sq;
    std::cout << prompt;
    
    std::cin >> input_str;
    std::cout << std::endl;
    auto it = SqStrMap.find(input_str);
    if (it != SqStrMap.end())
        sq = it->second;
    else {
        return Square::invalid;
    }
    return sq;
}

int Ability::GetCooldownTracker() const
{
    return log.CalculateTurnDiff(turn_last_used_ability);
}

std::unique_ptr<Ability> Ability::Clone() const
{
    return nullptr;
}