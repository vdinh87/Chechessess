#include "AbilityLibrary.hpp"
#include "SuperChessGame.hpp"

// Include all the ability headers
#include "PawnTwoTiles.hpp"
#include "PawnConvert.hpp"
#include "PawnConvertSuper.hpp"
#include "PawnTwoTilesBackwards.hpp"
#include "KingInspire.hpp"
#include "KingTurnIntoDead.hpp"
#include "KingSniperShot.hpp"
#include "KingTeleport.hpp"
#include "KingConvert.hpp"
#include "BishopSwap.hpp"
#include "BishopTakeCover.hpp"
#include "BishopResurrect.hpp"
#include "KnightBigL.hpp"
#include "KnightProtection.hpp"
#include "RookSwap.hpp"
#include "RookRamBuff.hpp"
#include "QueenInspire.hpp"
#include "QueenKamikaze.hpp"
#include "KnightTier2.hpp"
#include "RookTier2.hpp"

AbilityLibrary::AbilityLibrary(SuperChessGame &game_, Logger &log_)
    : game(game_), log(log_)
{
    lib[std::make_pair(Piece::Pawn, Tier::T0)] = std::make_unique<PawnTwoTiles>(game_, log_);
    lib[std::make_pair(Piece::Pawn, Tier::T1)] = std::make_unique<PawnConvert>(game_, log_);
    lib[std::make_pair(Piece::Pawn, Tier::T2)] = std::make_unique<PawnTwoTilesBackwards>(game_, log_);
    lib[std::make_pair(Piece::Pawn, Tier::T3)] = std::make_unique<PawnConvertSuper>(game_, log_);
    lib[std::make_pair(Piece::Knight, Tier::T0)] = std::make_unique<KnightBigL>(game_, log_);
    lib[std::make_pair(Piece::Knight, Tier::T1)] = std::make_unique<KnightProtection>(game_, log_);
    lib[std::make_pair(Piece::Bishop, Tier::T0)] = std::make_unique<BishopSwap>(game_, log_);
    lib[std::make_pair(Piece::Bishop, Tier::T1)] = std::make_unique<BishopTakeCover>(game_, log_);
    lib[std::make_pair(Piece::Bishop, Tier::T2)] = std::make_unique<BishopResurrect>(game_, log_);
    lib[std::make_pair(Piece::Rook, Tier::T0)] = std::make_unique<RookSwap>(game_, log_);
    lib[std::make_pair(Piece::Rook, Tier::T1)] = std::make_unique<RookRamBuff>(game_, log_);
    lib[std::make_pair(Piece::Queen, Tier::T0)] = std::make_unique<QueenInspire>(game_, log_);
    lib[std::make_pair(Piece::Queen, Tier::T1)] = std::make_unique<QueenKamikaze>(game_, log_);
    lib[std::make_pair(Piece::King, Tier::T0)] = std::make_unique<KingInspire>(game_, log_);
    lib[std::make_pair(Piece::King, Tier::T1)] = std::make_unique<KingTurnIntoDead>(game_, log_);
    lib[std::make_pair(Piece::King, Tier::T2)] = std::make_unique<KingTeleport>(game_, log_);
    lib[std::make_pair(Piece::King, Tier::T3)] = std::make_unique<KingSniperShot>(game_, log_);
    lib[std::make_pair(Piece::King, Tier::T4)] = std::make_unique<KingConvert>(game_, log_);

    // TO BE REPLACED
    lib[std::make_pair(Piece::Knight, Tier::T2)] = std::make_unique<KnightTier2>(game_, log_);
    lib[std::make_pair(Piece::Rook, Tier::T2)] = std::make_unique<RookTier2>(game_, log_);
}

std::unique_ptr<Ability> AbilityLibrary::GetAbility(const SuperPieceInfo &key)
{
    auto it = lib.find(key);
    if (it != lib.end())
        return it->second->Clone();

    return nullptr;
}

Tier AbilityLibrary::GetMaxTier(const Piece p_type) const
{
    return MAX_TIERS[p_type];
}