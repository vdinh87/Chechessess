#pragma once
#include "Definitions.hpp"
class Logger{
private:
    std::vector<ChessMove> log;
public:
    Logger() = default;
    ~Logger() = default;
    void Log(ChessMove move);
    void PrintLog();

};