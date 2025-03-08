#include "Ability.hpp"
#include "SuperChessGame.hpp"
#include "SuperPiece.hpp"
#include <iostream>

Ability::Ability(const std::string &name_, const AbilityType &type_, SuperChessGame &game_, Logger &log_)
    : name(name_), type(type_), game(game_), log(log_)
{
}

Ability::Ability(
    const std::string &name_,
    const AbilityType &type_,
    SuperChessGame &game_,
    Logger &log_,
    int cooldown_,
    int activation_turn_)
    : name(name_), type(type_), game(game_), log(log_), cooldown(cooldown_), activation_turn(activation_turn_)
{
}

std::string Ability::GetName() const
{
    return name;
}

AbilityType Ability::GetType() const
{
    return type;
}

bool Ability::Effect(const SuperPiece &piece)
{
    std::cout << "Normal effect" << std::endl;
    return true;
}

void Ability::Modify(U64 &move, Square piece)
{
    std::cout << "Normal modify" << std::endl;
}

bool Ability::OnCapture(SuperPiece &piece, Square to_capture, Square from_sq, std::vector<Square> &keys_to_remove)
{
    std::cout << "Normal on capture" << std::endl;
    return false;
}

void Ability::OnGameStart(SuperPiece &piece)
{
    std::cout << "Normal on game start" << std::endl;
}

Square Ability::GetInputSquare(const std::string &prompt) const
{
    std::string input_str;
    Square sq;
    std::cout << prompt;

    std::cin >> input_str;
    std::cout << std::endl;
    auto it = SqStrMap.find(input_str);
    if (it != SqStrMap.end())
        sq = it->second;
    else
    {
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