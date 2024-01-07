#pragma once
#include "Definitions.hpp"
class Logger{
private:
    std::vector<ChessMove> log;
public:
    Logger() = default;
    ~Logger() = default;

    void AddToLog(ChessMove move);
    const std::vector<ChessMove>& ReadLog() const;
    int GetCurrentTurn() const;
    int GetSize() const;
    int CalculateTurnDiff(int other_turn) const;
    bool IsEmpty() const;
    ChessMove GetLastMove() const;
    void PrintLog();

};