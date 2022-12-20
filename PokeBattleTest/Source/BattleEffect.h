#pragma once
#include <string>
namespace bat { class Turn; } // Forward declaration to avoid an include loop. 

namespace bfx {
	class BattleEffect { // The parent, the beeg boye.
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
			MultiHitOther,
			AutoHitOther,
			FlatDamageModOther,
			BypassSemiInvulnerableOther,
			InterruptMoveOther,
			DamageTrapOther,
			ReflectGuard,
			LightScreenGuard,
			AuroraVeilGuard,
			PsychicTerrain,
			MistyTerrain,
			GrassyTerrain,
			ElectricTerrain,
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
	protected:
		ChildType childType;
		TurnPhase turnPhase;
		bool targetIsPlayer; // When true, the target is the PLAYER, when false the target is the RIVAL
		int duration; // In turns, if this value is -1, it lasts until something causes it to be cancelled.
		BattleEffect(bool _target, int _duration, TurnPhase _turnphase, ChildType _childtype) :
			targetIsPlayer(_target), duration(_duration), turnPhase(_turnphase), childType(_childtype) {}
	};
	//
	// Stat mods. All of these increase or decrease a stat based on stages, resolved in their respective Execute functions.
	//
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
		ModSpeed(bool _target,int _stages) :
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
	//
	// Status effects
	//
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
	//
	// Weather & Terrain
	//
	struct ElectricTerrain : BattleEffect { // Terrains are programmed to affect the player, but their Execute() functions let them affect both without needing dupes.
		void Execute(bat::Turn& turn) override;
		ElectricTerrain() :
			BattleEffect(true, 5, TurnPhase::BeforeMoveExecuted, ChildType::ElectricTerrain) {}
	};

	struct MistyTerrain : BattleEffect {
		void Execute(bat::Turn& turn) override;
		MistyTerrain() :
			BattleEffect(true, 5, TurnPhase::BeforeMoveExecuted, ChildType::MistyTerrain) {}
	};

	struct PsychicTerrain : BattleEffect {
		void Execute(bat::Turn& turn) override;
		PsychicTerrain() :
			BattleEffect(true, 5, TurnPhase::BeforeMoveExecuted, ChildType::PsychicTerrain) {}
	};

	struct GrassyTerrain : BattleEffect {
		void Execute(bat::Turn& turn) override;
		GrassyTerrain() :
			BattleEffect(true, 5, TurnPhase::BeforeMoveExecuted, ChildType::GrassyTerrain) {}
	};

	// Other
	struct MultiHit : BattleEffect {
		void Execute(bat::Turn& turn) override;
		MultiHit(bool _target, int _hits) : 
			BattleEffect(_target, 1, TurnPhase::BeforeMoveExecuted, ChildType::MultiHitOther), hits(_hits) {}
	private:
		int hits;
	};

	struct MoveAutoHit : BattleEffect {
		void Execute(bat::Turn& turn) override;
		MoveAutoHit(bool _target) :
			BattleEffect(_target, 1, TurnPhase::BeforeMoveExecuted, ChildType::AutoHitOther) {}
	};

	struct FlatDamageMod : BattleEffect {
		void Execute(bat::Turn& turn) override;
		FlatDamageMod(bool _target, int _duration, float _mod) :
			BattleEffect(_target, _duration, TurnPhase::BeforeMoveExecuted, ChildType::FlatDamageModOther), mod(_mod) {}
	private:
		float mod;
	};

	struct BypassSemiInvulnerable : BattleEffect {
		void Execute(bat::Turn& turn) override;
		BypassSemiInvulnerable(bool _target) :
			BattleEffect(_target, 1, TurnPhase::BeforeMoveExecuted, ChildType::BypassSemiInvulnerableOther) {}
	};

	struct InterruptMove : BattleEffect {
		void Execute(bat::Turn& turn) override;
		InterruptMove(bool _target, int _duration) :
			BattleEffect(_target, _duration, TurnPhase::BeforeMoveExecuted, ChildType::InterruptMoveOther) {}
	};

	struct DamageTrap : BattleEffect {
		void Execute(bat::Turn& turn) override;
		DamageTrap(bool _target, std::string _cause) :
			BattleEffect(_target, 5, TurnPhase::AfterMoveExecuted, ChildType::DamageTrapOther), cause(_cause) {}
		~DamageTrap();
	private:
		std::string cause;
	};

	struct ReflectDefenseBoost : BattleEffect {
		void Execute(bat::Turn& turn) override;
		void RefreshDuration() { duration = 5; }
		ReflectDefenseBoost(bool _target) :
			BattleEffect(_target, 5, TurnPhase::BeforeMoveExecuted, ChildType::ReflectGuard) {}
		~ReflectDefenseBoost();
	};

	struct LightScreenSpecialDefenseBoost : BattleEffect {
		void Execute(bat::Turn& turn) override;
		void RefreshDuration() { duration = 5; }
		LightScreenSpecialDefenseBoost(bool _target) :
			BattleEffect(_target, 5, TurnPhase::BeforeMoveExecuted, ChildType::LightScreenGuard) {}
		~LightScreenSpecialDefenseBoost();
	};

	struct AuroraVeilDefSpdefBoost : BattleEffect {
		void Execute(bat::Turn& turn) override;
		void RefreshDuration() { duration = 5; }
		AuroraVeilDefSpdefBoost(bool _target) :
			BattleEffect(_target, 5, TurnPhase::BeforeMoveExecuted, ChildType::AuroraVeilGuard) {}
		~AuroraVeilDefSpdefBoost();
	};
}