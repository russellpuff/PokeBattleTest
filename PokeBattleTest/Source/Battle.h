#pragma once
#include <string>
#include <vector>
#include <map>
#include "Pokemon.h"
#include "BattleEffect.h"
#include "MoveLogic.h"
#include <queue>

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
		float a_absoluteDmgMod = 1.0F;
		// Accuracy and evasion stages, resolved as a part of the Action phase.
		int a_accuracyStages = 0;
		int d_evasionStages = 0;
		// Other mods
		bool interruptTurn = false; // If this is true, such as from a paralysis battle effect, the pokemon skips their action phase.
		bool interruptBecauseConfused = false; // A temporary? Workaround to a problem with ConfusionAttackSelf not being able to selectively execute. 
		bool a_bypassInvulnerable = false;
		bool d_SemiInvulnerable = false;
		tc::Type a_type1Override = tc::Type::NoType;
		tc::Type a_type2Override = tc::Type::NoType;
		tc::Type d_type1Override = tc::Type::NoType;
		tc::Type d_type2Override = tc::Type::NoType;
		tc::Type a_moveTypeOverride = tc::Type::NoType;
		int critChance = 24; // Algorithm will generate a number between 1 and this to determine crit. Default 1/24, mod can be 1/8, 1/2, or 1/1 (always crit) 
		int numAttacks = 1; // Some BattleEffects hit twice, others 2-5 times.
	public:
		bool Act(Battle& battle); // If this returns false, a pokemon was knocked out and the Round() method instantly ends.
		Turn(mon::Pokemon& _attacker, mon::Pokemon& _defender, mon::Move& _move, bool _atkr) 
			: attacker(_attacker), defender(_defender), move(_move), attackerIsPlayer(_atkr) {}
		// BattleEffect friends
		friend void bfx::ModAttack::Execute(Turn& turn);
		friend void bfx::ModDefense::Execute(Turn& turn);
		friend void bfx::ModSpecialAttack::Execute(Turn& turn);
		friend void bfx::ModSpecialDefense::Execute(Turn& turn);
		friend void bfx::ModSpeed::Execute(Turn& turn);
		friend void bfx::ModCriticalRatio::Execute(Turn& turn);
		friend void bfx::TemporaryCritIncrease::Execute(bat::Turn& turn);
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

		friend void bfx::ElectricTerrain::Execute(bat::Turn& turn);
		friend void bfx::MistyTerrain::Execute(bat::Turn& turn);
		friend void bfx::PsychicTerrain::Execute(bat::Turn& turn);
		friend void bfx::GrassyTerrain::Execute(bat::Turn& turn);

		friend void bfx::MultiHit::Execute(bat::Turn& turn);
		friend void bfx::MoveAutoHit::Execute(bat::Turn& turn);
		friend void bfx::FlatDamageMod::Execute(bat::Turn& turn);
		friend void bfx::BypassSemiInvulnerable::Execute(bat::Turn& turn);
		friend void bfx::InterruptMove::Execute(bat::Turn& turn);
		friend void bfx::DamageTrap::Execute(bat::Turn& turn);
		friend void bfx::ReflectDefenseBoost::Execute(bat::Turn& turn);
		friend void bfx::LightScreenSpecialDefenseBoost::Execute(bat::Turn& turn);
		friend void bfx::AuroraVeilDefSpdefBoost::Execute(bat::Turn& turn);
	};

	class Battle {
		typedef std::function<void(Battle& battle)> m_func;
		enum MoveRestriction;
	private:
		std::vector<std::unique_ptr<bfx::BattleEffect>> battleEffects;
		std::deque<int> p_movesUsed;
		std::deque<int> r_movesUsed;
		std::map<int, m_func> pre_moveFuncs;
		std::map<int, m_func> post_moveFuncs;
		mon::Pokemon player;
		mon::Pokemon rival;
		mon::Move p_move;
		mon::Move r_move;
		float p_speedMult = 1.0F;
		float r_speedMult = 1.0F;
		int p_dmgThisTurn = 0;
		int r_dmgThisTurn = 0;
		bool attackerIsPlayer = false;
		bool playerGoesFirst = false;
		bool victorDeclared = false;
	public:
		bool GetVictorDeclared() { return victorDeclared; }
		void Round(mon::Move& playerMove, mon::Move& rivalMove);
		friend bool Turn::Act(Battle& battle);
		Battle(mon::Pokemon& _player, mon::Pokemon& _rival);
		// MoveLogic friends
		friend void mv::IncreasedCriticalOneStage(bat::Battle& battle);
		friend void mv::Hit2to5Times(bat::Battle& battle);
		friend void mv::HitTwice(bat::Battle& battle);
		friend void mv::InstantHealSelf(bat::Battle& battle, int percent);
		friend void mv::HealOnDamage(bat::Battle& battle, int percent);
		friend void mv::ChanceFlinch(bat::Battle& battle, int proc);
		friend void mv::ChanceStatusEffect(bat::Battle& battle, StatusEffect status, tc::Type typeImmune, int proc);
		friend void mv::ModMultipleStats(bat::Battle& battle, int proc, int atk, int def, int spatk, int spdef, int spd, int acc, int eva, int crit, bool targetAttacker);
		friend void mv::CheckTargetMinimized(bat::Battle& battle);
		friend void mv::CanHitDuringFlyFreefall(bat::Battle& battle, bool moveIsSwift);
		friend void mv::CanHitDuringSemiInvulnerable(bat::Battle& battle, int moveID, bool moveIsSwift);
		friend void mv::DoublePowerIfStatusEffect(bat::Battle& battle);
		friend void mv::PassiveDamageTrap(bat::Battle& battle, std::string cause);
		friend void mv::Reflect(bat::Battle& battle);
		friend void mv::LightScreen(bat::Battle& battle);
		friend void mv::AuroraVeil(bat::Battle& battle);
		friend void mv::ElectricField(bat::Battle& battle);
		friend void mv::MistField(bat::Battle& battle);
		friend void mv::PsychoField(bat::Battle& battle);
		friend void mv::GrassField(bat::Battle& battle);
	};
}