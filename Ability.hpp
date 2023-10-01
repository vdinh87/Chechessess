#pragma once
#include "SuperChessGame.cpp"
#include "Definitions.hpp"
class Ability
{
protected:
<<<<<<< HEAD
    std::string name;
    SuperChessGame* game;

public:
    Ability();
    Ability(const std::string& name_, SuperChessGame* game_);
    virtual ~Ability();
=======
    Square piece;
    std::string name;
    SuperChessGame *game;
public:
    Ability() = default;
    Ability(const std::string& name_, SuperChessGame* game_);
    virtual ~Ability() = default;
    void UpdatePiece(const Square& square);
>>>>>>> 74671688b6bc55b6f48211bc66829fc6b81d06c8
    const std::string& GetName() const;
    virtual void Effect();
};

Ability::Ability(const std::string &name_, SuperChessGame *game_) : name(name_), game(game_) {}

<<<<<<< HEAD
}

Ability::Ability(const std::string& name_, SuperChessGame* game_)
{
    name = name_;
    game = game_;
}

Ability::~Ability()
=======
void Ability::UpdatePiece(const Square &square)
>>>>>>> 74671688b6bc55b6f48211bc66829fc6b81d06c8
{
    piece = square;
}

const std::string& Ability::GetName() const
{
    return name;
}

void Ability::Effect()
{
    std::cout << "Normal effect" << std::endl;
}