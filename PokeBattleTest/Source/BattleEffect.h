#pragma once
#include <string>
namespace bat { class Turn; } // Forward declaration to avoid an include loop. 

namespace bfx {
#pragma region TheBeegBoye
	class BattleEffect {
	public:
		enum ChildType : short { // Enum to discern child type cuts down on resources being devoured by multiple dynamic_cast calls.
			AttackMod,
			DefenseMod,
			SpecialAttackMod,
			SpecialDefenseMod,
			SpeedMod,
			AccuracyMod,
			EvasionMod,
			CriticalRatioMod,
			BurnStatus,
			DrowsyStatus,
			ParalysisStatus,
			FrostbiteStatus,
			PoisonedStatus,
			ConfusionStatus,
			FlinchStatus,
			MultiHit,
			AutoHit,
			FlatDamageMod,
			BypassSemiInvulnerable,
			InterruptMove,
			DamageTrap,
			FieldGuard,
			Terrain,
			Weather,
			TrickRoom,
			WonderRoom,
			MudSport,
			SplashSport,
			IonDeluge,
			ShedType,
			MoveOverride,
			DefenseOverride,
			SpecialDefenseOverride,
			ExactDamageOverride,
		};
		enum TurnPhase : short { // What phase of Turn() does this effect take place. 
			BeforeUpkeep, // This is before the upkeep phase that checks the remaining duration of Battle effect and cleanses expired ones.
			DuringOrderCheck, // While determining which pokemon moves first. 
			BeforeMoveExecuted, // After the upkeep phase ends but before the affected pokemon executes their move. 
			AfterMoveExecuted, // This occurs after they've executed the move and its taken effect or failed.
			AfterBothMoves // After both pokemon have moved. 
		};
		virtual void Execute(bat::Turn& turn) = 0;
		TurnPhase GetTurnPhase() { return turnPhase; }
		ChildType GetTypeOfEffect() { return childType; }
		bool GetTarget() { return targetIsPlayer; }
		int GetDuration() { return duration; }
		void DecrementDuration() { if (duration > 0) { --duration; } }
		virtual ~BattleEffect() {}
	protected:
		ChildType childType;
		TurnPhase turnPhase;
		bool targetIsPlayer; // When true, the target is the PLAYER, when false the target is the RIVAL
		int duration; // In turns, if this value is -1, it lasts until something causes it to be cancelled.
		BattleEffect(bool _target, int _duration, TurnPhase _turnphase, ChildType _childtype) :
			targetIsPlayer(_target), duration(_duration), turnPhase(_turnphase), childType(_childtype) {}
	};
#pragma endregion

#pragma region StatMods
	class ModStat : public BattleEffect {
	protected:
		int stages; // Range between -6 and 6.
		ModStat(bool _target, int _stages, ChildType _childtype);
	public:
		int GetStages() { return stages; }
		int ModStages(int mod);
	};

	struct ModAttack : ModStat {
		void Execute(bat::Turn& turn) override;
		ModAttack(bool _target, int _stages) :
			ModStat(_target, _stages, ChildType::AttackMod) {}
	};

	struct ModDefense : ModStat {
		void Execute(bat::Turn& turn) override;
		ModDefense(bool _target, int _stages) :
			ModStat(_target, _stages, ChildType::DefenseMod) {}
	};

	struct ModSpecialAttack : ModStat {
		void Execute(bat::Turn& turn) override;
		ModSpecialAttack(bool _target, int _stages) :
			ModStat(_target, _stages, ChildType::SpecialAttackMod) {}
	};

	struct ModSpecialDefense : ModStat {
		void Execute(bat::Turn& turn) override;
		ModSpecialDefense(bool _target, int _stages) :
			ModStat(_target, _stages, ChildType::SpecialDefenseMod) {}
	};

	struct ModSpeed : ModStat {
		void Execute(bat::Turn& turn) override;
		ModSpeed(bool _target, int _stages) :
			ModStat(_target, _stages, ChildType::SpeedMod) {}
	};

	struct ModAccuracy : ModStat {
		void Execute(bat::Turn& turn) override;
		ModAccuracy(bool _target, int _stages) :
			ModStat(_target, _stages, ChildType::AccuracyMod) {}
	};

	struct ModEvasion : ModStat {
		void Execute(bat::Turn& turn) override;
		ModEvasion(bool _target, int _stages) :
			ModStat(_target, _stages, ChildType::EvasionMod) {}
	};

	struct ModCriticalRatio : ModStat {
		void Execute(bat::Turn& turn) override;
		ModCriticalRatio(bool _target, int _stages) :
			ModStat(_target, _stages, ChildType::CriticalRatioMod) {}
	};

	struct TemporaryCritIncrease : BattleEffect {
		void Execute(bat::Turn& turn) override;
		TemporaryCritIncrease(bool _target) :
			BattleEffect(_target, 1, TurnPhase::BeforeMoveExecuted, ChildType::CriticalRatioMod) {}
	};
#pragma endregion

#pragma region StatusEffects
	struct BurnAttackReduction : BattleEffect {
		void Execute(bat::Turn& turn) override;
		BurnAttackReduction(bool _target) :
			BattleEffect(_target, -1, TurnPhase::BeforeMoveExecuted, ChildType::BurnStatus) {}
	};

	struct BurnDamage : BattleEffect {
		void Execute(bat::Turn& turn) override;
		BurnDamage(bool _target) :
			BattleEffect(_target, -1, TurnPhase::AfterMoveExecuted, ChildType::BurnStatus) {}
	};

	struct Drowsy : BattleEffect {
		void Execute(bat::Turn& turn) override;
		Drowsy(bool _target) :
			BattleEffect(_target, -1, TurnPhase::BeforeMoveExecuted, ChildType::DrowsyStatus) {}
	};

	struct DrowsyDamageBoost : BattleEffect { // This is a positive BattleEffect that buffs the other pokemon while Drowsy is active.
		void Execute(bat::Turn& turn) override;
		DrowsyDamageBoost(bool _target) :
			BattleEffect(_target, -1, TurnPhase::BeforeMoveExecuted, ChildType::DrowsyStatus) {}
	};

	struct Paralysis : BattleEffect {
		void Execute(bat::Turn& turn) override;
		Paralysis(bool _target) :
			BattleEffect(_target, -1, TurnPhase::BeforeMoveExecuted, ChildType::ParalysisStatus) {}
	};

	struct FrostbiteDamage : BattleEffect {
		void Execute(bat::Turn& turn) override;
		FrostbiteDamage(bool _target) :
			BattleEffect(_target, -1, TurnPhase::AfterMoveExecuted, ChildType::FrostbiteStatus) {}
	};

	struct FrostbiteSpecialAttackReduction : BattleEffect {
		void Execute(bat::Turn& turn) override;
		FrostbiteSpecialAttackReduction(bool _target) :
			BattleEffect(_target, -1, TurnPhase::BeforeMoveExecuted, ChildType::FrostbiteStatus) {}
	};

	struct Poisoned : BattleEffect {
		void Execute(bat::Turn& turn) override;
		Poisoned(bool _target) :
			BattleEffect(_target, -1, TurnPhase::AfterMoveExecuted, ChildType::PoisonedStatus) {}
	};

	struct BadlyPoisoned : BattleEffect {
		void Execute(bat::Turn& turn) override;
		BadlyPoisoned(bool _target) :
			BattleEffect(_target, -1, TurnPhase::AfterMoveExecuted, ChildType::PoisonedStatus) {}
	private:
		int cumulativeTurns = 1;
	};

	struct Confusion : BattleEffect {
		void Execute(bat::Turn& turn) override;
		Confusion(bool _target, int _duration) :
			BattleEffect(_target, _duration, TurnPhase::BeforeMoveExecuted, ChildType::ConfusionStatus) {}
	};

	struct ConfusionAttackSelf : BattleEffect {
		void Execute(bat::Turn& turn) override;
		ConfusionAttackSelf(bool _target, int _duration) :
			BattleEffect(_target, _duration, TurnPhase::AfterMoveExecuted, ChildType::ConfusionStatus) {}
	};

	// One-turn status effect that, if Executed, interrupts the victim's turn. Only Executes if the victim moves after its procced in the same turn.
	struct Flinch : BattleEffect {
		void Execute(bat::Turn& turn) override;
		Flinch(bool _target) :
			BattleEffect(_target, 1, TurnPhase::BeforeMoveExecuted, ChildType::FlinchStatus) {}
	};
#pragma endregion

#pragma region Terrain
	class Terrain : public BattleEffect {
	public:
		enum TerrainType {
			ElectricTerrain,
			MistyTerrain,
			PsychicTerrain,
			GrassyTerrain
		};
		TerrainType GetTerrainType() { return terrainType; }
		void RefreshDuration() { duration = 5; }
	protected:
		TerrainType terrainType;
		Terrain(TerrainType _terraintype, TurnPhase _turnphase) :	// Terrains are programmed to affect the player, but their Execute() functions let them affect both without needing dupes.
			BattleEffect(true, 5, _turnphase, ChildType::Terrain), terrainType(_terraintype) {}
	};
	struct ElectricTerrain : Terrain {
		void Execute(bat::Turn& turn) override;
		ElectricTerrain() :
			Terrain(TerrainType::ElectricTerrain, TurnPhase::BeforeMoveExecuted) {}
	};

	struct MistyTerrain : Terrain {
		void Execute(bat::Turn& turn) override;
		MistyTerrain() :
			Terrain(TerrainType::MistyTerrain, TurnPhase::BeforeMoveExecuted) {}
	};

	struct PsychicTerrain : Terrain {
		void Execute(bat::Turn& turn) override;
		PsychicTerrain() :
			Terrain(TerrainType::PsychicTerrain, TurnPhase::BeforeMoveExecuted) {}
	};

	struct GrassyTerrain : Terrain {
		void Execute(bat::Turn& turn) override;
		GrassyTerrain() :
			Terrain(TerrainType::GrassyTerrain, TurnPhase::BeforeMoveExecuted) {}
	};

	struct GrassyTerrainHeal : Terrain {
		void Execute(bat::Turn& turn) override;
		GrassyTerrainHeal() :
			Terrain(TerrainType::GrassyTerrain, TurnPhase::AfterMoveExecuted) {}
	};
#pragma endregion

#pragma region Weather
	class Weather : public BattleEffect {
	public:
		enum WeatherType {
			HarshSunlight,
			Rain,
			Sandstorm,
			Hail,
			StrongWinds,
			IntenseGravity
		};
		WeatherType GetWeatherType() { return weatherType; }
		void RefreshDuration() { duration = 5; }
	protected:
		WeatherType weatherType;
		Weather(WeatherType _weathertype, BattleEffect::TurnPhase _turnphase) :
			BattleEffect(true, 5, _turnphase, BattleEffect::Weather), weatherType(_weathertype) {}
	};

	struct HarshSunlight : Weather {
		void Execute(bat::Turn& turn) override;
		HarshSunlight() :
			Weather(Weather::HarshSunlight, TurnPhase::BeforeMoveExecuted) {}
	};

	struct Rain : Weather {
		void Execute(bat::Turn& turn) override;
		Rain() :
			Weather(Weather::Rain, TurnPhase::BeforeMoveExecuted) {}
	};

	struct Sandstorm : Weather {
		void Execute(bat::Turn& turn) override;
		Sandstorm() :
			Weather(Weather::Sandstorm, TurnPhase::BeforeMoveExecuted) {}
	};

	struct SandstormDamage : Weather {
		void Execute(bat::Turn& turn) override;
		SandstormDamage() :
			Weather(Weather::Sandstorm, TurnPhase::AfterMoveExecuted) {}
	};

	struct Hail : Weather {
		void Execute(bat::Turn& turn) override;
		Hail() :
			Weather(Weather::Hail, TurnPhase::BeforeMoveExecuted) {}
	};

	struct HailDamage : Weather {
		void Execute(bat::Turn& turn) override;
		HailDamage() :
			Weather(Weather::Hail, TurnPhase::AfterMoveExecuted) {}
	};

	struct StrongWinds : Weather {
		void Execute(bat::Turn& turn) override;
		StrongWinds() :
			Weather(Weather::StrongWinds, TurnPhase::BeforeMoveExecuted) {}
	};

	struct IntenseGravity : Weather {
		void Execute(bat::Turn& turn) override;
		IntenseGravity() :
			Weather(Weather::IntenseGravity, TurnPhase::BeforeMoveExecuted) {}
	};
#pragma endregion

#pragma region FieldGuards
	class FieldGuard : public BattleEffect {
	public:
		enum GuardType {
			Reflect,
			LightScreen,
			AuroraVeil
		};
		void RefreshDuration() { duration = 5; }
		GuardType GetGuardType() { return guardType; }
	protected:
		GuardType guardType;
		FieldGuard(bool _target, GuardType _guardtype) :
			BattleEffect(_target, 5, TurnPhase::BeforeMoveExecuted, ChildType::FieldGuard), guardType(_guardtype) {}
	};

	struct ReflectDefenseBoost : FieldGuard {
		void Execute(bat::Turn& turn) override;
		void RefreshDuration() { duration = 5; }
		ReflectDefenseBoost(bool _target) :
			FieldGuard(_target, FieldGuard::GuardType::Reflect) {}
		virtual ~ReflectDefenseBoost();
	};

	struct LightScreenSpecialDefenseBoost : FieldGuard {
		void Execute(bat::Turn& turn) override;
		void RefreshDuration() { duration = 5; }
		LightScreenSpecialDefenseBoost(bool _target) :
			FieldGuard(_target, FieldGuard::GuardType::LightScreen) {}
		virtual ~LightScreenSpecialDefenseBoost();
	};

	struct AuroraVeilDefSpdefBoost : FieldGuard {
		void Execute(bat::Turn& turn) override;
		void RefreshDuration() { duration = 5; }
		AuroraVeilDefSpdefBoost(bool _target) :
			FieldGuard(_target, FieldGuard::GuardType::AuroraVeil) {}
		virtual ~AuroraVeilDefSpdefBoost();
	};
#pragma endregion

#pragma region Rooms
	struct TrickRoom : BattleEffect { // This BattleEffect doesn't have an execute function as it is only considered during the turn order calculator.
		void Execute(bat::Turn& turn) override {}
		TrickRoom() :
			BattleEffect(true, -1, TurnPhase::BeforeMoveExecuted, ChildType::TrickRoom) {}
	};

	struct WonderRoom : BattleEffect {
		void Execute(bat::Turn& turn) override;
		WonderRoom() :
			BattleEffect(true, -1, TurnPhase::BeforeMoveExecuted, ChildType::WonderRoom) {}
	};
#pragma endregion

#pragma region FieldEffects
	struct MudSport : BattleEffect {
		void Execute(bat::Turn& turn) override;
		void RefreshDuration() { duration = 5; }
		MudSport() :
			BattleEffect(true, 5, TurnPhase::BeforeMoveExecuted, ChildType::MudSport) {}
	};

	struct SplashSport : BattleEffect {
		void Execute(bat::Turn& turn) override;
		void RefreshDuration() { duration = 5; }
		SplashSport() :
			BattleEffect(true, 5, TurnPhase::BeforeMoveExecuted, ChildType::SplashSport) {}
	};

	struct IonDeluge : BattleEffect {
		void Execute(bat::Turn& turn) override;
		void RefreshDuration() { duration = 5; }
		IonDeluge() :
			BattleEffect(true, 5, TurnPhase::BeforeMoveExecuted, ChildType::IonDeluge) {}
	};
#pragma endregion

#pragma region Other
	struct MultiHit : BattleEffect {
		void Execute(bat::Turn& turn) override;
		MultiHit(bool _target, int _hits) :
			BattleEffect(_target, 1, TurnPhase::BeforeMoveExecuted, ChildType::MultiHit), hits(_hits) {}
	private:
		int hits;
	};

	struct MoveAutoHit : BattleEffect {
		void Execute(bat::Turn& turn) override;
		MoveAutoHit(bool _target) :
			BattleEffect(_target, 1, TurnPhase::BeforeMoveExecuted, ChildType::AutoHit) {}
	};

	struct FlatDamageMod : BattleEffect {
		void Execute(bat::Turn& turn) override;
		FlatDamageMod(bool _target, int _duration, float _mod) :
			BattleEffect(_target, _duration, TurnPhase::BeforeMoveExecuted, ChildType::FlatDamageMod), mod(_mod) {}
	private:
		float mod;
	};

	struct BypassSemiInvulnerable : BattleEffect {
		void Execute(bat::Turn& turn) override;
		BypassSemiInvulnerable(bool _target) :
			BattleEffect(_target, 1, TurnPhase::BeforeMoveExecuted, ChildType::BypassSemiInvulnerable) {}
	};

	struct InterruptMove : BattleEffect {
		void Execute(bat::Turn& turn) override;
		InterruptMove(bool _target, int _duration) :
			BattleEffect(_target, _duration, TurnPhase::BeforeMoveExecuted, ChildType::InterruptMove) {}
	};

	struct DamageTrap : BattleEffect {
		void Execute(bat::Turn& turn) override;
		DamageTrap(bool _target, std::string _cause) :
			BattleEffect(_target, 5, TurnPhase::AfterMoveExecuted, ChildType::DamageTrap), cause(_cause) {}
		~DamageTrap();
	private:
		std::string cause;
	};

	struct MoveOverride : BattleEffect {
		void Execute(bat::Turn& turn) override;
		MoveOverride(bool _target, int _newMove) :
			BattleEffect(_target, 1, TurnPhase::BeforeMoveExecuted, ChildType::MoveOverride), newMove(_newMove) {}
	private:
		int newMove;
	};

	struct Recoil : BattleEffect {
		void Execute(bat::Turn& turn) override;
		Recoil(bool _target, int _percent) :
			BattleEffect(_target, 1, TurnPhase::AfterMoveExecuted, ChildType::Recoil), percent(_percent) {}
	private:
		int percent;
	};

	struct ExactDamageOverride : BattleEffect {
		void Execute(bat::Turn& turn) override;
		ExactDamageOverride(bool _target, int _exactdmg) :
			BattleEffect(_target, 1, TurnPhase::BeforeMoveExecuted, ChildType::ExactDamageOverride), exactDamage(_exactdmg) {}
	private:
		int exactDamage;

	};

	struct ShedType : BattleEffect {
		void Execute(bat::Turn& turn) override;
		ShedType(bool _target, tc::Type _type, int _duration) :
			BattleEffect(_target, _duration, TurnPhase::BeforeMoveExecuted, ChildType::ShedType), typeToShed(_type) {}
	private:
		tc::Type typeToShed;
	};

	struct SpecialDefenseOverride : BattleEffect {
		void Execute(bat::Turn& turn) override;
		SpecialDefenseOverride(bool _target, int _duration, int _override) :
			BattleEffect(_target, _duration, TurnPhase::BeforeMoveExecuted, ChildType::SpecialDefenseOverride), spDefOverride(_override) {}
	private:
		int spDefOverride;
	};
#pragma endregion
}