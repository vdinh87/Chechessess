#pragma once

// Forward declarations to avoid circular dependencies
class Ability;
class AbilityLibrary;
class SuperPiece;
class SuperChessGame;

#include "Definitions.hpp"
#include "ChessGame.hpp"
#include "Logger.hpp"

// Ability class
class Ability
{
protected:
    std::string name = "";
    AbilityType type;
    SuperChessGame &game;
    Logger &log;
    const int cooldown = 0;
    const int activation_turn = 0;
    int turn_last_used_ability = -999;

public:
    Ability(const std::string &name_, const AbilityType &type_, SuperChessGame &game_, Logger &log_);
    Ability(const std::string &name_, const AbilityType &type_, SuperChessGame &game_, Logger &log_, int cooldown_, int activation_turn_);
    virtual ~Ability() = default;

    // Methods
    virtual bool Effect(const SuperPiece &piece);
    virtual void Modify(U64 &move, Square piece);
    virtual bool OnCapture(SuperPiece &piece, Square to_capture, Square from_sq, std::vector<Square> &keys_to_remove);
    virtual void OnGameStart(SuperPiece &piece);
    Square GetInputSquare(const std::string &prompt) const;
    int GetCooldownTracker() const;

    // Accessors
    std::string GetName() const;
    AbilityType GetType() const;

    // Cloning
    virtual std::unique_ptr<Ability> Clone() const = 0;
};

// SuperPiece class
class SuperPiece
{
private:
    std::map<Tier, std::unique_ptr<Ability>, EnumClassComparer> abilities;
    SuperPieceInfo info;
    Square sq;
    Color color;

public:
    SuperPiece(std::vector<std::unique_ptr<Ability>> &abilities_, SuperPieceInfo info_, Square sq_, Color color_);
    ~SuperPiece() = default;

    // Ability use
    bool UseAbility(Tier key);
    void ModifyMove(U64 &move);
    bool OnCaptureEffects(Square to_capture, Square from_sq, std::vector<Square> &keys_to_remove);
    void OnGameStartEffects();

    // Updaters
    void UpdateSquare(Square to_sq);
    void UpdateColor(Color new_color);
    void UpdateTier(Tier t);
    void AddAbilities(std::vector<std::unique_ptr<Ability>> &abilities_);

    // Getters
    SuperPieceInfo GetInfo() const;
    Square GetSquare() const;
    Color GetColor() const;
    Piece GetPieceType() const;
    Tier GetTier() const;

    // Ability names
    std::vector<std::string> GetAbilityNames() const;
    std::vector<std::string> GetAbilityNames(AbilityType type) const;
    int GetNumberOfAbilities() const;
};

// AbilityLibrary class
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

// SuperChessGame class - use inheritance from ChessGame
class SuperChessGame : public ChessGame
{
private:
    // ability related data members
    std::shared_ptr<AbilityLibrary> al;
    std::unordered_map<Square, std::shared_ptr<SuperPiece>> super_pieces;
    std::unordered_map<std::pair<Color, SuperPieceInfo>, int, PairInsidePairEnumHash> graveyard;

    // init methods
    void InitSuperPieces(const SuperPieceInfo &white, const SuperPieceInfo &black);
    void InitGameStartEffects();

public:
    SuperChessGame();
    SuperChessGame(const SuperPieceInfo &white_info, const SuperPieceInfo &black_info);
    ~SuperChessGame() = default;

    bool RemovePiece(Square square) override;
    bool AddSuperPiece(SuperPieceInfo info, Square square, Color color, bool conversion = 0);
    bool UnSuper(Square square);
    bool ConvertPieceToSide(Square square, Color side);
    bool ConvertPiece(Piece from_piece, Color from_color, Square square, Piece to_piece);
    bool ConvertToSuperPiece(SuperPieceInfo info, Square square);
    bool ConvertSuperPiecesofType(SuperPieceInfo info, Color color);
    bool UpgradeSuperPieceTier(Square square, Tier to_tier);

    // utility
    U64 GetBoardOf(Piece piece, Color color);
    U64 GetBoardOf(Color color);
    bool IsSuperPiece(const Square &key) const;
    bool InCheck(Color color) const;
    bool IsWin(Color color) const;
    void CapTier(Tier &t, Piece p_type) const;
    void MakeAbilityVector(std::vector<std::unique_ptr<Ability>> &v, SuperPieceInfo info);
    void Swap(Square from, Square to);
    void SwapSuperPieces(Square from, Square to);

    // graveyard functions
    void AddToGraveyard(Color color, Square sq, Piece piece);
    void RemoveFromGraveYard(const std::pair<Color, SuperPieceInfo> &key);
    bool PieceInGraveyard(Color color, Piece piece);
    std::vector<SuperPieceInfo> GetPiecesInGraveyard(Color color) const;

    // Move functions
    U64 GetAttacks(Square square_) const override;
    U64 GetAttacks(Square square_, U64 team_filter) const;
    std::vector<Action> Move(Square from_sq, Square to_sq) override;
    void ExecuteMove(Color color, Square from_sq, Square to_sq, Piece from_piece, Piece to_piece) override;

    // misc
    bool UseAbility(Square sq, Tier t);
    Action Promote(Square from_sq, Square to_sq, Color from_color, Piece to_piece);

    // debug helpers
    int GetNumAbilities(Square sq);
    void PrintAbilityNames(Square sq);
    void PrintAbilityNames(Square sq, AbilityType type);
};