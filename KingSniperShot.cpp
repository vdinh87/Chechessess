#pragma once
#include "KingSniperShot.hpp"

KingSniperShot::KingSniperShot(SuperChessGame* game_) :
Ability("King Sniper Shot")
{
    game = game_;
}

void KingSniperShot::Effect()
{
    std::unordered_map<std::string, Square> const table = { 
        {"a1", Square::a1}, {"a2", Square::a2}, {"a3", Square::a3}, {"a4", Square::a4},
        {"a5", Square::a5}, {"a6", Square::a6}, {"a7", Square::a7}, {"a8", Square::a8},
        {"b1", Square::b1}, {"b2", Square::b2}, {"b3", Square::b3}, {"b4", Square::b4},
        {"b5", Square::b5}, {"b6", Square::b6}, {"b7", Square::b7}, {"b8", Square::b8},
        {"c1", Square::c1}, {"c2", Square::c2}, {"c3", Square::c3}, {"c4", Square::c4},
        {"c5", Square::c5}, {"c6", Square::c6}, {"c7", Square::c7}, {"c8", Square::c8},
        {"d1", Square::d1}, {"d2", Square::d2}, {"d3", Square::d3}, {"d4", Square::d4},
        {"d5", Square::d5}, {"d6", Square::d6}, {"d7", Square::d7}, {"d8", Square::d8},
        {"e1", Square::e1}, {"e2", Square::e2}, {"e3", Square::e3}, {"e4", Square::e4},
        {"e5", Square::e5}, {"e6", Square::e6}, {"e7", Square::e7}, {"e8", Square::e8},
        {"f1", Square::f1}, {"f2", Square::f2}, {"f3", Square::f3}, {"f4", Square::f4},
        {"f5", Square::f5}, {"f6", Square::f6}, {"f7", Square::f7}, {"f8", Square::f8},
        {"g1", Square::g1}, {"g2", Square::g2}, {"g3", Square::g3}, {"g4", Square::g4},
        {"g5", Square::g5}, {"g6", Square::g6}, {"g7", Square::g7}, {"g8", Square::g8},
        {"h1", Square::h1}, {"h2", Square::h2}, {"h3", Square::h3}, {"h4", Square::h4},
        {"h5", Square::h5}, {"h6", Square::h6}, {"h7", Square::h7}, {"h8", Square::h8}
    };

    game->PrintBoard();

    std::string input_str;
    Square sq;
    std::cout << "Choose piece to delete: ";

    std::cin >> input_str;
    std::cout << std::endl;
    auto it = table.find(input_str);
    if (it != table.end())
        sq = it->second;
    else 
        std::cout << "Invalid square" << std::endl;
    
    game->RemovePiece(sq);
    std::cout << "KingSniperShot succeeded" << std::endl;
}

void KingSniperShot::Notify(const std::vector<ChessMove>& log)
{
    log_ = log;
}