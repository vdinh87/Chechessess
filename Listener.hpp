#pragma once
#include "Definitions.hpp"
class Listener {
public:
    virtual ~Listener() = default;
    virtual void Notify(const std::vector<ChessMove>& log) = 0;
};