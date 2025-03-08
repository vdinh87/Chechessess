#include "SuperPiece.hpp"

SuperPiece::SuperPiece(std::vector<std::unique_ptr<Ability>> &abilities_, SuperPieceInfo info_, Square sq_, Color color_) : info(info_), sq(sq_), color(color_)
{
    AddAbilities(abilities_);
}

bool SuperPiece::UseAbility(Tier key)
{
    auto it = abilities.find(key);
    if (it != abilities.end())
    {
        return it->second->Effect(*this);
    }

    return false;
}

void SuperPiece::ModifyMove(U64 &move)
{
    for (const auto &pair : abilities)
    {
        if (pair.second->GetType() == AbilityType::move_modifer)
            pair.second->Modify(move, sq);
    }
}

bool SuperPiece::OnCaptureEffects(Square to_capture, Square from_sq, std::vector<Square> &keys_to_remove)
{
    bool success = false;

    for (const auto &pair : abilities)
    {
        if ((pair.second->GetType() == AbilityType::on_capture) && pair.second->OnCapture(*this, to_capture, from_sq, keys_to_remove))
        {
            success = true;
        }
    }
    return success;
}

void SuperPiece::OnGameStartEffects()
{
    for (const auto &pair : abilities)
    {
        if (pair.second->GetType() == AbilityType::on_game_start)
        {
            pair.second->OnGameStart(*this);
        }
    }
}

// updaters
void SuperPiece::UpdateSquare(Square to_sq)
{
    sq = to_sq;
}

void SuperPiece::UpdateColor(Color new_color)
{
    color = new_color;
}

void SuperPiece::UpdateTier(Tier t)
{
    info.second = t;
}

void SuperPiece::AddAbilities(std::vector<std::unique_ptr<Ability>> &abilities_)
{
    int i = 0;
    for (auto &a : abilities_)
    {
        abilities[static_cast<Tier>(i)] = std::move(a);
        ++i;
    }

    abilities_.clear();
}

// getters
SuperPieceInfo SuperPiece::GetInfo() const
{
    return info;
}
Square SuperPiece::GetSquare() const
{
    return sq;
}
Color SuperPiece::GetColor() const
{
    return color;
}
Piece SuperPiece::GetPieceType() const
{
    return info.first;
}
Tier SuperPiece::GetTier() const
{
    return info.second;
}

std::vector<std::string> SuperPiece::GetAbilityNames() const
{
    std::vector<std::string> v;
    for (auto &a : abilities)
        v.push_back(a.second->GetName());

    return v;
}

std::vector<std::string> SuperPiece::GetAbilityNames(AbilityType type) const
{
    std::vector<std::string> v;
    for (auto &a : abilities)
    {
        if (a.second->GetType() == type)
            v.push_back(a.second->GetName());
    }

    return v;
}

int SuperPiece::GetNumberOfAbilities() const
{
    return abilities.size();
}