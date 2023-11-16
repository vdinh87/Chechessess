#pragma once
#include "BishopResurrect.hpp"

BishopResurrect::BishopResurrect(SuperChessGame& game_, Logger& log_) : 
Ability("Bishop Resurrect", AbilityType::active, game_, log_, 6, 8)
{
}

void BishopResurrect::Effect(const SuperPiece &piece)
{
    int current_turn = log.GetCurrentTurn();

    const Color bishop_color = piece.GetColor();

    std::vector<SuperPieceInfo> graveyard = game.GetPiecesInGraveyard(bishop_color);

    if (GetCooldownTracker() < cooldown)
    {
        std::cout << name << " is Still on CoolDown... Turns till Cooldown: " << cooldown - GetCooldownTracker() << "\n";
        return;
    }
    if (current_turn < activation_turn)
    {
        std::cout << name << " is only Available at turn 8. It's currently Turn [" << current_turn << "]\n";
        return;
    }
    if (graveyard.empty())
    {
        std::cout << "Graveyard is empty.\n";
        return;
    }

    int counter = 0;
    std::string output;
    U64 board = game.GetBoard();

    std::vector<U64> spawnlocations = (bishop_color == white) ? WhiteSpawnLocations : BlackSpawnLocations;
    for (const auto &p : graveyard) // showing options
    {
        U64 available_spawnlocations = spawnlocations[p.first] & ~board;
        if (available_spawnlocations != 0ULL)
        {
            std::string sp_str;
            if (p.second != Tier::not_superpiece) // checks if it's a superpiece.
                sp_str = TierStrings[p.second] + " Super";

            output += "(" + std::to_string(counter) + ") " + sp_str + PieceStrings[piece.GetInfo().first] + " ";
        }

        ++counter;
    }

    std::cout << "Choose piece to Resurrect: ";
    int selection;
    std::cin >> selection;
    std::cout << std::endl;
    // selection complete
    U64 spawn = 0ULL;
    SuperPieceInfo key;
    if (selection < graveyard.size() && selection >= 0)
    {
        key = graveyard[selection]; // gets superpieceinfo from graveyard.
        spawn = spawnlocations[key.first];

        if ((spawn & ~board) == 0ULL)
        {
            std::cout << "Invalid Selection. Please choose from the provided options." << std::endl;
            return;
        }
    }

    //Choosing a spawnlocation.
    std::cout << "Available squares to choose from:" << std::endl;
    while (spawn != 0) {
        Square squareIndex = static_cast<Square>get_LSB(spawn);
        std::string squareString;

        for (const auto& pair : SqStrMap) {
            if (pair.second == (squareIndex)) {
                squareString = pair.first;
                break;
            }
        }

        std::cout << squareString << " ";
        spawn &= (spawn - 1);
    }

    std::cout << "\nEnter your selection: ";
    std::string userSelection;
    std::cin >> userSelection;

    Square sq = SqStrMap.at(userSelection);
    if (spawnlocations[key.first] & static_cast<U64>(1ULL<<sq) == 0ULL) { //couldn't use spawn since I modified it iterating.
        std::cout << "Invalid selection. Please choose a valid square." << std::endl;
        return;
    }


    if (key.second != Tier::not_superpiece)
        game.AddSuperPiece(key, sq, bishop_color, false);
    else
        game.AddPiece(sq, bishop_color, key.first);

    game.RemoveFromGraveYard( std::make_pair(bishop_color, key) );

    std::cout << "☆⌒(*^-゜)v huzzah Resurrection!!\n";
    turn_last_used_ability = log.GetCurrentTurn();
}

std::unique_ptr<Ability> BishopResurrect::Clone() const
{
    return std::make_unique<BishopResurrect>(*this);
}