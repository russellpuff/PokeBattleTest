#pragma once
#include <string>
#include "Pokemon.h"
#include "TypeCategory.h"
namespace bat { class Turn; } // Forward declaration to avoid an include loop. 

namespace bfx {
	// This is a special exception to represent either a battle effect going wrong OR it simply not being constructed (on purpose) due to proc.
	// It's designed to be caught during Turn(), the battle won't be interrupted if it's thrown, the effect will simply not take effect. 
	class BattleEffectFailed : public std::exception {
	private:
		const char* message;
	public:
		const char* what() { return message; }
		BattleEffectFailed(const char* _msg) : message(_msg) {}
	};

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
		BattleEffect& operator--(); // Decrements duration. 
	protected:
		TurnPhase turnPhase;
		bool targetIsPlayer; // When true, the target is the PLAYER, when false the target is the RIVAL
		int duration; // In turns, if this value is -1, it lasts until something causes it to be cancelled.
		BattleEffect(bool _target, int _duration, TurnPhase _turnphase, int _proc); // Proc is a chance out of 100 for the effect to take place, if it fails, the effect deletes itself.
	};
	//
	// Stat mods
	//
	class ModStat : public BattleEffect {
	protected:
		int stages; // Range between -6 and 6.
		ModStat(bool _target, int _duration, int _proc, int _stages);
	public:
		int GetStages() { return stages; }
	};

	struct ModAttack : ModStat {
		void Execute(bat::Turn& turn) override;
		ModAttack(bool _target, int _duration, int _proc, int _stages) :
			ModStat(_target, _duration, _proc, _stages) {}
	};

	struct ModDefense : ModStat {
		void Execute(bat::Turn& turn) override;
		ModDefense(bool _target, int _duration, int _proc, int _stages) :
			ModStat(_target, _duration, _proc, _stages) {}
	};

	struct ModSpecialAttack : ModStat {
		void Execute(bat::Turn& turn) override;
		ModSpecialAttack(bool _target, int _duration, int _proc, int _stages) :
			ModStat(_target, _duration, _proc, _stages) {}
	};

	struct ModSpecialDefense : ModStat {
		void Execute(bat::Turn& turn) override;
		ModSpecialDefense(bool _target, int _duration, int _proc, int _stages) :
			ModStat(_target, _duration, _proc, _stages) {}
	};

	struct ModSpeed : ModStat {
		void Execute(bat::Turn& turn) override;
		ModSpeed(bool _target, int _duration, int _proc, int _stages) :
			ModStat(_target, _duration, _proc, _stages) {}
	};

	struct ModCriticalRatio : ModStat {
		void Execute(bat::Turn& turn) override;
		ModCriticalRatio(bool _target, int _duration, int _proc, int _stages) :
			ModStat(_target, _duration, _proc, _stages) {}
	};

	struct ModAccuracy : ModStat {
		void Execute(bat::Turn& turn) override;
		ModAccuracy(bool _target, int _duration, int _proc, int _stages) :
			ModStat(_target, _duration, _proc, _stages) {}
	};

	struct ModEvasion : ModStat {
		void Execute(bat::Turn& turn) override;
		ModEvasion(bool _target, int _duration, int _proc, int _stages) :
			ModStat(_target, _duration, _proc, _stages) {}
	};
	//
	// Status effects
	//
	struct BurnAttackReduction : BattleEffect {
		void Execute(bat::Turn& turn) override;
		BurnAttackReduction(bool _target, int _duration, int _proc) :
			BattleEffect(_target, duration, TurnPhase::BeforeMoveExecuted, _proc) {}
	};

	// This is some dumb shit. A tomb of my own making. If BurnAttackReduction fails to construct, the catch will skip over constructing this.
	struct BurnDamage : BattleEffect {
		void Execute(bat::Turn& turn) override;
		BurnDamage(bool _target, int _duration) :
			BattleEffect(_target, _duration, TurnPhase::AfterMoveExecuted, 100) {}
	};

	struct DrowsyInterrupt : BattleEffect {
		void Execute(bat::Turn& turn) override;
		DrowsyInterrupt(bool _target, int _duration, int _proc) :
			BattleEffect(_target, _duration, TurnPhase::BeforeMoveExecuted, _proc) {}
	};

	struct DrowsyDamageBoost : BattleEffect { // This is a positive BattleEffect that buffs the other pokemon while Drowsy is active.
		void Execute(bat::Turn& turn) override;
		DrowsyDamageBoost(bool _target, int _duration) :
			BattleEffect(_target, _duration, TurnPhase::BeforeMoveExecuted, 100) {}
	};

	struct Paralysis : BattleEffect {
		void Execute(bat::Turn& turn) override;
		Paralysis(bool _target, int _duration, int _proc) :
			BattleEffect(_target, _duration, TurnPhase::DuringOrderCheck, _proc) {}
	};

	struct ParalysisInterrupt : BattleEffect {
		void Execute(bat::Turn& turn) override;
		ParalysisInterrupt(bool _target, int _duration) :
			BattleEffect(_target, _duration, TurnPhase::BeforeMoveExecuted, 100) {}
	};

	struct FrostbiteDamage : BattleEffect {
		void Execute(bat::Turn& turn) override;
		FrostbiteDamage(bool _target, int _duration, int _proc) :
			BattleEffect(_target, _duration, TurnPhase::AfterMoveExecuted, _proc) {}
	};

	struct FrostbiteSpecialAttackReduction : BattleEffect {
		void Execute(bat::Turn& turn) override;
		FrostbiteSpecialAttackReduction(bool _target, int _duration) :
			BattleEffect(_target, _duration, TurnPhase::BeforeMoveExecuted, 100) {}
	};

	struct Poisoned : BattleEffect {
		void Execute(bat::Turn& turn) override;
		Poisoned(bool _target, int _duration, int _proc) :
			BattleEffect(_target, _duration, TurnPhase::AfterMoveExecuted, _proc) {}
	};

	struct BadlyPoisoned : BattleEffect {
		void Execute(bat::Turn& turn) override;
		BadlyPoisoned(bool _target, int _duration, int _proc) :
			BattleEffect(_target, _duration, TurnPhase::AfterMoveExecuted, _proc) {}
	private:
		int cumulativeTurns = 1;
	};

	struct Confusion : BattleEffect {
		void Execute(bat::Turn& turn) override;
		Confusion(bool _target, int _duration, int _proc) :
			BattleEffect(_target, _duration, TurnPhase::BeforeUpkeep, _proc) {} // Not sure where to place this. Creates ConfusionInterrupt and ConfusionAttackSelf
	};

	struct ConfusionInterrupt : BattleEffect {
		void Execute(bat::Turn& turn) override;
		ConfusionInterrupt(bool _target, int _duration) :
			BattleEffect(_target, _duration, TurnPhase::BeforeMoveExecuted, 100) {}
	};

	struct ConfusionAttackSelf : BattleEffect {
		void Execute(bat::Turn& turn) override;
		ConfusionAttackSelf(bool _target, int _duration) :
			BattleEffect(_target, _duration, TurnPhase::AfterMoveExecuted, 100) {}
	};
	//
	// Weather & Terrain
	//

	// Other
	struct MultiHit : BattleEffect {
		void Execute(bat::Turn& turn) override;
		MultiHit(bool _target, int _hits) : BattleEffect(_target, 1, TurnPhase::BeforeMoveExecuted, 100), hits(_hits) {}
	private:
		int hits;
	};
}