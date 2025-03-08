#pragma once

#include "SuperChessGame.fwd.hpp"
#include "Ability.hpp"
#include "Logger.hpp"

// Forward declarations for all ability classes
class PawnTwoTiles;
class PawnConvert;
class PawnConvertSuper;
class PawnTwoTilesBackwards;
class KingInspire;
class KingTurnIntoDead;
class KingSniperShot;
class KingTeleport;
class KingConvert;
class BishopSwap;
class BishopTakeCover;
class BishopResurrect;
class KnightBigL;
class KnightProtection;
class RookSwap;
class RookRamBuff;
class QueenInspire;
class QueenKamikaze;
class KnightTier2;
class RookTier2;

#include "Definitions.hpp"

class AbilityLibrary
{
private:
    SuperChessGame &game;
    Logger &log;
    std::unordered_map<SuperPieceInfo, std::unique_ptr<Ability>, PairEnumHash> lib;
    const std::vector<Tier> MAX_TIERS = {Tier::T3, Tier::T2, Tier::T2, Tier::T2, Tier::T1, Tier::T4};

public:
    AbilityLibrary(SuperChessGame &game_, Logger &log_);
    ~AbilityLibrary() = default;
    std::unique_ptr<Ability> GetAbility(const SuperPieceInfo &key);
    Tier GetMaxTier(const Piece p_type) const;
};