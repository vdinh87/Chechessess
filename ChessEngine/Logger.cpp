#include "Logger.hpp"

void Logger::AddToLog(ChessMove move)
{
    log.push_back(move);
}

const std::vector<ChessMove> &Logger::ReadLog() const
{
    return log;
}

int Logger::CalculateTurnDiff(int other_turn) const
{
    return GetCurrentTurn() - other_turn;
}

int Logger::GetCurrentTurn() const
{
    return (log.size() + 2) / 2;
}

int Logger::GetSize() const
{
    return log.size();
}

bool Logger::IsEmpty() const
{
    return log.empty();
}

ChessMove Logger::GetLastMove() const
{
    return log.back();
}

void Logger::PrintLog()
{
    std::string output;
    int width = 10;
    std::cout << "| White   | Black " << std::endl;

    for (int i = 0; i < log.size(); i++)
    {
        std::string line;
        ChessMove move = log[i];

        // castling
        auto it = std::find(move.actions.begin(), move.actions.end(), Action::Castle);
        if (it != move.actions.end())
            line += "O-O";

        if (it == move.actions.end()) // if not castle proceed
        {
            // add piece type
            line += PieceStrings[move.type];

            // capture
            it = std::find(move.actions.begin(), move.actions.end(), Action::Capture);
            if (it != move.actions.end())
                line += "x";

            // destination square
            if (move.to != Square::invalid)
                line += SquareStrings[move.to];

            // promotion
            it = std::find(move.actions.begin(), move.actions.end(), Action::Promotion);
            if (it != move.actions.end())
                line += "=" + PieceStrings[move.type];

            // checkmate
            it = std::find(move.actions.begin(), move.actions.end(), Action::Checkmate);
            if (it != move.actions.end())
                line += "#";

            // check
            if (it == move.actions.end()) // if not checkmate proceed
            {
                it = std::find(move.actions.begin(), move.actions.end(), Action::Check);
                if (it != move.actions.end())
                    line += "+";
            }
        }
        int width = 8;
        std::cout << "| " << std::setw(width) << std::left << line;
        if (i % 2 == 1) // black move
            std::cout << std::endl;
    }
}
