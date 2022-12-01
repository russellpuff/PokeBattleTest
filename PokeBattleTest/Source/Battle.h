#pragma once
#include <string>
#include <vector>
#include "Pokemon.h"
#include "BattleEffect.h"

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
		void WriteToScreen(std::string message);
		int critChance = 24; // Algorithm will generate a number between 1 and this to determine crit. Default 1/24, mod can be 1/8, 1/2, or 1/1 (always crit) 
	public:
		void Act(Battle& battle);
		friend class bfx::BattleEffect;
		Turn(mon::Move& _move, bool _attacker) : move(_move), attackerIsPlayer(_attacker) {}
	};

	class Battle {
	private:
		std::vector<bfx::BattleEffect*> battleEffects;
		mon::Pokemon player;
		mon::Pokemon rival;
		mon::Move p_move;
		mon::Move r_move;
		int playerCurrentHP;
		int rivalCurrentHP;
		float p_speedMult = 1.0F;
		float r_speedMult = 1.0F;
		bool p_grounded = true; // If a pokemon is not grounded, it's immune to flying moves and terrain. 
		bool r_grounded = true;
	public:
		void Round(mon::Move& playerMove, mon::Move& rivalMove);
		friend void Turn::Act(Battle& battle);
		Battle(mon::Pokemon& _player, mon::Pokemon& _rival) :
			player(_player), rival(_rival), p_move(0), r_move(0), 
			playerCurrentHP(_player.GetCurrentHP()), rivalCurrentHP(_rival.GetCurrentHP()) {}
	};
}