#pragma once
#include <string>
#include <vector>
#include <map>
#include "Pokemon.h"
#include "BattleEffect.h"
#include "MoveLogic.h"

namespace bat {
	class Battle;

	class Turn {
	private:
		mon::Pokemon attacker;
		mon::Pokemon defender;
		mon::Move move;
		bool attackerIsPlayer;
		bool moveHits = false;
		// Mods in terms of a flat multiplier.
		float a_atkMod = 1.0F;
		float a_defMod = 1.0F;
		float a_spAtkMod = 1.0F;
		float a_spDefMod = 1.0F;
		float a_spdMod = 1.0F;
		float a_accMod = 1.0F;
		float d_atkMod = 1.0F;
		float d_defMod = 1.0F;
		float d_spAtkMod = 1.0F;
		float d_spDefMod = 1.0F;
		float d_spdMod = 1.0F;
		// Accuracy and evasion stages, resolved as a part of the Action phase.
		int a_accuracyStages = 0;
		int d_evasionStages = 0;
		// Other mods
		bool interruptTurn = false; // If this is true, such as from a paralysis battle effect, the pokemon skips their action phase.
		bool interruptBecauseConfused = false; // A temporary? Workaround to a problem with ConfusionAttackSelf not being able to selectively execute. 
		tc::Type type1Override = tc::Type::NoType;
		tc::Type type2Override = tc::Type::NoType;
		int critChance = 24; // Algorithm will generate a number between 1 and this to determine crit. Default 1/24, mod can be 1/8, 1/2, or 1/1 (always crit) 
		int numAttacks = 1; // Some BattleEffects hit twice, others 2-5 times.
	public:
		bool Act(Battle& battle); // If this returns false, a pokemon was knocked out and the Round() method instantly ends.
		Turn(mon::Pokemon& _attacker, mon::Pokemon& _defender, mon::Move& _move, bool _atkr) 
			: attacker(_attacker), defender(_defender), move(_move), attackerIsPlayer(_atkr) {}
		// friends
		friend void bfx::ModAttack::Execute(Turn& turn);
		friend void bfx::ModDefense::Execute(Turn& turn);
		friend void bfx::ModSpecialAttack::Execute(Turn& turn);
		friend void bfx::ModSpecialDefense::Execute(Turn& turn);
		friend void bfx::ModSpeed::Execute(Turn& turn);
		friend void bfx::ModCriticalRatio::Execute(Turn& turn);
		friend void bfx::ModAccuracy::Execute(Turn& turn);
		friend void bfx::ModEvasion::Execute(Turn& turn);
		friend void bfx::BurnAttackReduction::Execute(Turn& turn);
		friend void bfx::BurnDamage::Execute(Turn& turn);
		friend void bfx::Drowsy::Execute(Turn& turn);
		friend void bfx::DrowsyDamageBoost::Execute(Turn& turn);
		friend void bfx::Paralysis::Execute(Turn& turn);
		friend void bfx::FrostbiteDamage::Execute(Turn& turn);
		friend void bfx::FrostbiteSpecialAttackReduction::Execute(Turn& turn);
		friend void bfx::Poisoned::Execute(Turn& turn);
		friend void bfx::BadlyPoisoned::Execute(Turn& turn);
		friend void bfx::Confusion::Execute(Turn& turn);
		friend void bfx::ConfusionAttackSelf::Execute(Turn& turn);
		friend void bfx::Flinch::Execute(Turn& turn);

		friend struct bfx::MultiHit;
	};

	class Battle {
		typedef std::function<void(Battle& battle)> m_func;
	private:
		std::vector<std::unique_ptr<bfx::BattleEffect>> battleEffects;
		std::map<int, m_func> pre_moveFuncs;
		std::map<int, m_func> post_moveFuncs;
		mon::Pokemon player;
		mon::Pokemon rival;
		mon::Move p_move;
		mon::Move r_move;
		float p_speedMult = 1.0F;
		float r_speedMult = 1.0F;
		bool p_grounded = true; // If a pokemon is not grounded, it's immune to ground moves and terrain. 
		bool r_grounded = true;
		int p_dmgThisTurn = 0;
		int r_dmgThisTurn = 0;
		bool attackerIsPlayer = false;
		bool victorDeclared = false;
	public:
		bool GetVictorDeclared() { return victorDeclared; }
		void Round(mon::Move& playerMove, mon::Move& rivalMove);
		friend bool Turn::Act(Battle& battle);
		Battle(mon::Pokemon& _player, mon::Pokemon& _rival);
		// MoveCode friends
		friend void mv::ChanceStatusEffect(bat::Battle& battle, StatusEffect status, tc::Type typeImmune, int proc);
		friend void mv::IncreasedCriticalOneStage(bat::Battle& battle);
		friend void mv::Hit2to5Times(bat::Battle& battle);
		friend void mv::HitTwice(bat::Battle& battle);
		friend void mv::InstantHealSelf(bat::Battle& battle, int percent);
		friend void mv::HealOnDamage(bat::Battle& battle, int percent);
		friend void mv::ChanceFlinch(bat::Battle& battle, int proc);
		friend void mv::ChanceBurn(bat::Battle& battle, int proc);
		friend void mv::ChanceParalyze(bat::Battle& battle, int proc);
		friend void mv::ChanceDrowsy(bat::Battle& battle, int proc);
		friend void mv::ChanceFrostbite(bat::Battle& battle, int proc);
		friend void mv::ChancePoison(bat::Battle& battle, int proc);
		friend void mv::ChanceBadlyPoison(bat::Battle& battle, int proc);
		friend void mv::ChanceConfusion(bat::Battle& battle, int proc);
		friend void mv::ModAttack(bat::Battle& battle, int proc, int stages, bool targetAttacker);
		friend void mv::ModDefense(bat::Battle& battle, int proc, int stages, bool targetAttacker);
		friend void mv::ModSpecialAttack(bat::Battle& battle, int proc, int stages, bool targetAttacker);
		friend void mv::ModSpecialDefense(bat::Battle& battle, int proc, int stages, bool targetAttacker);
		friend void mv::ModSpeed(bat::Battle& battle, int proc, int stages, bool targetAttacker);
		friend void mv::ModAccuracy(bat::Battle& battle, int proc, int stages, bool targetAttacker);
		friend void mv::ModEvasion(bat::Battle& battle, int proc, int stages, bool targetAttacker);
		friend void mv::ModCritical(bat::Battle& battle, int proc, int stages, bool targetAttacker);
		friend void mv::ModMultipleStats(bat::Battle& battle, int atk, int def, int spatk, int spdef, int spd, int acc, int eva, bool targetAttacker);
	};
}