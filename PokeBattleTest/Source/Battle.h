#pragma once
#include <string>
#include <vector>
#include <map>
#include "Pokemon.h"
#include "BattleEffect.h"
#include "Move.h"

namespace bat {
	class Battle;

	class Turn {
	private:
		mon::Move move;
		bool attackerIsPlayer; // BattleEffects check for who they affect. I need a better way to handle this...
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
		tc::Type type1Override = tc::Type::NoType;
		tc::Type type2Override = tc::Type::NoType;
		int critChance = 24; // Algorithm will generate a number between 1 and this to determine crit. Default 1/24, mod can be 1/8, 1/2, or 1/1 (always crit) 
		int numAttacks = 1; // Some BattleEffects hit twice, others 2-5 times.
	public:
		void Act(Battle& battle);
		Turn(mon::Move& _move, bool _attacker) : move(_move), attackerIsPlayer(_attacker) {}
		// friends
		friend class bfx::BattleEffect;
		friend class bfx::ModStat;
		friend struct bfx::ModAttack;
		friend struct bfx::ModDefense;
		friend struct bfx::ModSpecialAttack;
		friend struct bfx::ModSpecialDefense;
		friend struct bfx::ModSpeed;
		friend struct bfx::ModCriticalRatio;
		friend struct bfx::ModAccuracy;
		friend struct bfx::ModEvasion;

		friend struct bfx::MultiHit;
	};

	class Battle {
		typedef std::function<void(Battle& battle)> m_func;
	private:
		std::vector<bfx::BattleEffect*> battleEffects;
		std::map<int, m_func> pre_moveFuncs;
		std::map<int, m_func> post_moveFuncs;
		mon::Pokemon player;
		mon::Pokemon rival;
		mon::Move p_move;
		mon::Move r_move;
		int playerCurrentHP;
		int rivalCurrentHP;
		float p_speedMult = 1.0F;
		float r_speedMult = 1.0F;
		bool p_grounded = true; // If a pokemon is not grounded, it's immune to ground moves and terrain. 
		bool r_grounded = true;
		int p_dmgThisTurn = 0;
		int r_dmgThisTurn = 0;
		bool attackerIsPlayer = false;
	public:
		void Round(mon::Move& playerMove, mon::Move& rivalMove);
		void WriteToScreen(std::string& message);
		friend void Turn::Act(Battle& battle);
		Battle(mon::Pokemon& _player, mon::Pokemon& _rival);
		// MoveCode friends
		friend void mv::IncreasedCriticalOneStage(bat::Battle& battle);
		friend void mv::Hit2to5Times(bat::Battle& battle);
		friend void mv::HitTwice(bat::Battle& battle);
		friend void mv::InstantHealSelf(bat::Battle& battle, int percent);
		friend void mv::HealOnDamage(bat::Battle& battle, int percent);
	};
}