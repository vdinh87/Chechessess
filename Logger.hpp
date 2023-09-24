#pragma once
#include "Definitions.hpp"
#include "Listener.hpp"
class Logger{
private:
    std::vector<ChessMove> log;
    std::vector<Listener*> listeners;
public:
    Logger() = default;
    ~Logger();
    void AddListener(Listener* l);
    void RemoveListener(Listener* l);
    void Log(ChessMove move);
    void PrintLog();

};