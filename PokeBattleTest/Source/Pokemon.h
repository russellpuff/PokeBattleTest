#pragma once
#include <string>
#include "JsonStructs.h"
#include "TypeCategory.h"
namespace mon {
	class Move {
	private:
		int moveID;
		std::string name;
		tc::Type type;
		tc::MoveCategory category;
		int powerPoints;
		int power;
		int accuracy;
		int priority;
		bool procMutable; // If this is false, the move will never miss, regardless of accuracy and evasion modifiers. 
	public:
		int GetID() { return moveID; }
		std::string GetName() { return name; }
		tc::Type GetType() { return type; }
		tc::MoveCategory GetCategory() { return category; }
		int GetPowerPoints() { return powerPoints; }
		int GetPower() { return power; }
		int GetAccuracy() { return accuracy; }
		int GetPriority() { return priority; }
		bool GetIsProcMutable() { return procMutable; }
		Move(int _moveID);
	};

	struct Stats {
		int hpBase;
		int atkBase;
		int defBase;
		int spAtkBase;
		int spDefBase;
		int spdBase;
		// Current stats typically use a formula to determine what they are based on level and IVs.
		// The constructor for Pokemon will do this automatically when reading files. 
		int hpCurrent;
		int hpFinal;
		int atkFinal;
		int defFinal;
		int spAtkFinal;
		int spDefFinal;
		int spdFinal;
		Stats();
	};

	class Pokemon {
		// This class is entirely read-only.During a Battle, attributes will only be referenced.
		// In a more elaborate future project, it will need to permanently store certain data like current/max HP.
		// This application has no need for that, so Battle tracks it. 
	private:
		std::string name;
		std::string form; // Form determines whether or not a template overwrite occurs, comes up in constructing.
		Stats stats;
		std::array<Move, 4> moves; // 4 moves always, there will be a special "empty move" to represent having fewer than four. 
		tc::Type firstType;
		tc::Type secondType;
		int level;
		// Abilities and Items not included because too complex and not enough time.
		// Everything defined in species.db but not here is irrelevant. 
	public:
		std::string GetName() { return name; }
		std::string GetForm() { return form; }
		tc::Type GetType1() { return firstType; }
		tc::Type GetType2() { return secondType; }
		Move GetMove1() { return moves[0]; }
		Move GetMove2() { return moves[1]; }
		Move GetMove3() { return moves[2]; }
		Move GetMove4() { return moves[3]; }
		int GetLevel() { return level; }
		int GetBaseHP() { return stats.hpBase; }
		int GetBaseAtk() { return stats.atkBase; }
		int GetBaseDef() { return stats.defBase; }
		int GetBaseSpAtk() { return stats.spAtkBase; }
		int GetBaseSpDef() { return stats.spDefBase; }
		int GetBaseSpd() { return stats.spdBase; }
		int GetCurrentHP() { return stats.hpCurrent; }
		int GetFinalHP() { return stats.hpFinal; }
		int GetFinalAtk() { return stats.atkFinal; }
		int GetFinalDef() { return stats.defFinal; }
		int GetFinalSpAtk() { return stats.spAtkFinal; }
		int GetFinalSpDef() { return stats.spDefFinal; }
		int GetFinalSpd() { return stats.spdFinal; }
		void ModCurrentHP(int val) { stats.hpCurrent += val; }
		Pokemon(jt::JsonPkmn p, jt::JsonTemplate t, std::array<mon::Move, 4>& m);
	};
}