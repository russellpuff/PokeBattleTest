#pragma once
#include <string>
namespace bat { class Turn; } // Forward declaration to avoid an include loop. 

namespace bfx {
	class BattleEffect { // The parent, the beeg boye.
	public:
		enum TurnPhase : short { // What phase of Turn() does this effect take place. 
			BeforeUpkeep, // This is before the upkeep phase that checks the remaining duration of Battle effect and cleanses expired ones.
			DuringOrderCheck, // While determining which pokemon moves first. 
			BeforeMoveExecuted, // After the upkeep phase ends but before the affected pokemon executes their move. 
			AfterMoveExecuted, // This occurs after they've executed the move and its taken effect or failed.
			AfterBothMoves // After both pokemon have moved. 
		};
		virtual void Execute(bat::Turn& turn) = 0;
		TurnPhase GetTurnPhase() { return turnPhase; }
		bool GetTarget() { return targetIsPlayer; }
		int GetDuration() { return duration; }
		void DecrementDuration() { if (duration > 0) { --duration; } }
	protected:
		TurnPhase turnPhase;
		bool targetIsPlayer; // When true, the target is the PLAYER, when false the target is the RIVAL
		int duration; // In turns, if this value is -1, it lasts until something causes it to be cancelled.
		BattleEffect(bool _target, int _duration, TurnPhase _turnphase) :
			targetIsPlayer(_target), duration(_duration), turnPhase(_turnphase) {}
	};
	//
	// Stat mods. All of these increase or decrease a stat based on stages, resolved in their respective Execute functions.
	//
	class ModStat : public BattleEffect {
	protected:
		int stages; // Range between -6 and 6.
		ModStat(bool _target, int _duration, int _stages) :
			BattleEffect(_target, _duration, TurnPhase::BeforeMoveExecuted), stages(_stages) {}
	public:
		int GetStages() { return stages; }
	};

	struct ModAttack : ModStat {
		void Execute(bat::Turn& turn) override;
		ModAttack(bool _target, int _duration, int _stages) :
			ModStat(_target, _duration, _stages) {}
	};

	struct ModDefense : ModStat {
		void Execute(bat::Turn& turn) override;
		ModDefense(bool _target, int _duration, int _stages) :
			ModStat(_target, _duration, _stages) {}
	};

	struct ModSpecialAttack : ModStat {
		void Execute(bat::Turn& turn) override;
		ModSpecialAttack(bool _target, int _duration, int _stages) :
			ModStat(_target, _duration, _stages) {}
	};

	struct ModSpecialDefense : ModStat {
		void Execute(bat::Turn& turn) override;
		ModSpecialDefense(bool _target, int _duration, int _stages) :
			ModStat(_target, _duration, _stages) {}
	};

	struct ModSpeed : ModStat {
		void Execute(bat::Turn& turn) override;
		ModSpeed(bool _target, int _duration, int _stages) :
			ModStat(_target, _duration, _stages) {}
	};

	struct ModCriticalRatio : ModStat {
		void Execute(bat::Turn& turn) override;
		ModCriticalRatio(bool _target, int _duration, int _stages) :
			ModStat(_target, _duration, _stages) {}
	};

	struct ModAccuracy : ModStat {
		void Execute(bat::Turn& turn) override;
		ModAccuracy(bool _target, int _duration, int _stages) :
			ModStat(_target, _duration, _stages) {}
	};

	struct ModEvasion : ModStat {
		void Execute(bat::Turn& turn) override;
		ModEvasion(bool _target, int _duration, int _stages) :
			ModStat(_target, _duration, _stages) {}
	};
	//
	// Status effects
	//
	struct BurnAttackReduction : BattleEffect {
		void Execute(bat::Turn& turn) override;
		BurnAttackReduction(bool _target) :
			BattleEffect(_target, -1, TurnPhase::BeforeMoveExecuted) {}
	};

	struct BurnDamage : BattleEffect {
		void Execute(bat::Turn& turn) override;
		BurnDamage(bool _target) :
			BattleEffect(_target, -1, TurnPhase::AfterMoveExecuted) {}
	};

	struct Drowsy : BattleEffect {
		void Execute(bat::Turn& turn) override;
		Drowsy(bool _target) :
			BattleEffect(_target, -1, TurnPhase::BeforeMoveExecuted) {}
	};

	struct DrowsyDamageBoost : BattleEffect { // This is a positive BattleEffect that buffs the other pokemon while Drowsy is active.
		void Execute(bat::Turn& turn) override;
		DrowsyDamageBoost(bool _target) :
			BattleEffect(_target, -1, TurnPhase::BeforeMoveExecuted) {}
	};

	struct Paralysis : BattleEffect {
		void Execute(bat::Turn& turn) override;
		Paralysis(bool _target) :
			BattleEffect(_target, -1, TurnPhase::BeforeMoveExecuted) {}
	};

	struct FrostbiteDamage : BattleEffect {
		void Execute(bat::Turn& turn) override;
		FrostbiteDamage(bool _target) :
			BattleEffect(_target, -1, TurnPhase::AfterMoveExecuted) {}
	};

	struct FrostbiteSpecialAttackReduction : BattleEffect {
		void Execute(bat::Turn& turn) override;
		FrostbiteSpecialAttackReduction(bool _target) :
			BattleEffect(_target, -1, TurnPhase::BeforeMoveExecuted) {}
	};

	struct Poisoned : BattleEffect {
		void Execute(bat::Turn& turn) override;
		Poisoned(bool _target) : 
			BattleEffect(_target, -1, TurnPhase::AfterMoveExecuted) {}
	};

	struct BadlyPoisoned : BattleEffect {
		void Execute(bat::Turn& turn) override;
		BadlyPoisoned(bool _target) : 
			BattleEffect(_target, -1, TurnPhase::AfterMoveExecuted) {}
	private:
		int cumulativeTurns = 1;
	};

	struct Confusion : BattleEffect {
		void Execute(bat::Turn& turn) override;
		Confusion(bool _target, int _duration) :
			BattleEffect(_target, _duration, TurnPhase::BeforeMoveExecuted) {}
	};

	struct ConfusionAttackSelf : BattleEffect {
		void Execute(bat::Turn& turn) override;
		ConfusionAttackSelf(bool _target, int _duration) : 
			BattleEffect(_target, _duration, TurnPhase::AfterMoveExecuted) {}
	};

	// One-turn status effect that, if Executed, interrupts the victim's turn. Only Executes if the victim moves after its procced in the same turn.
	struct Flinch : BattleEffect {
		void Execute(bat::Turn& turn) override;
		Flinch(bool _target) :
			BattleEffect(_target, 1, TurnPhase::BeforeMoveExecuted) {}
	};
	//
	// Weather & Terrain
	//

	// Other
	struct MultiHit : BattleEffect {
		void Execute(bat::Turn& turn) override;
		MultiHit(bool _target, int _hits) : BattleEffect(_target, 1, TurnPhase::BeforeMoveExecuted), hits(_hits) {}
	private:
		int hits;
	};
}