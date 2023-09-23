#pragma once
#include "Logger.hpp"

void Logger::Log(ChessMove move)
{
    log.push_back(move);
}

void Logger::PrintLog()
{
    std::string output;
    int width = 10;
    std::cout << "| White   | Black " << std::endl; 
    
    for(int i = 0; i < log.size(); i++)
    {
        std::string line;
        ChessMove move = log[i];
    
        //add piece type
        line += PieceStrings[move.type];

        //capture
        auto it = std::find(move.actions.begin(), move.actions.end(), Action::Capture);
        if(it != move.actions.end()) //
            line += "x";

        //destination square
        if( move.to != Square::invalid )
            line += SquareStrings[move.to];
        
        //promotion
        it = std::find(move.actions.begin(), move.actions.end(), Action::Promotion);
        if(it != move.actions.end()) //
            line += "=" + PieceStrings[move.type];
        
        //castling
        it = std::find(move.actions.begin(), move.actions.end(), Action::Castle);
        if(it != move.actions.end()) //
            line += "O-O";

        //checkmate
        it = std::find(move.actions.begin(), move.actions.end(), Action::Checkmate);
        if(it != move.actions.end()) //
            line += "#";

        //check
        if( it == move.actions.end() ) //if not checkmate proceed
        {
            it = std::find(move.actions.begin(), move.actions.end(), Action::Check);
            if(it != move.actions.end()) //
                line += "+";
        }
        
        int width = 8;
        std::cout << "| " << std::setw(width) << std::left << line ;
        if( i % 2 == 1) //black move
            std::cout << std::endl;
    }
}