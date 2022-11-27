#pragma once
#include <string>
#include <array>
#include <JsonStructs.h>
#include <TypeFixer.h>
namespace mon {
	class Move {
	private:
		int moveID;
		std::string name;
		tf::Type type;
		tf::MoveCategory category;
		int powerPoints;
		int power;
		int accuracy;
		int priority;
		bool procMutable; // If this is false, the move will never miss, regardless of accuracy and evasion modifiers. 
	public:
		std::string GetName() { return name; }
		std::string GetType() { return tf::typeToString.at(type); }
		std::string GetCategory() { return tf::categoryToString.at(category); }
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
		int atkCurrent;
		int defCurrent;
		int spAtkCurrent;
		int spDefCurrent;
		int spdCurrent;
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
		tf::Type firstType;
		tf::Type secondType;
		int level;
		// Abilities and Items not included because too complex and not enough time.
		// Everything defined in species.db but not here is irrelevant. 
	public:
		std::string GetName() { return name; }
		std::string GetForm() { return form; }
		std::string GetType1() { return tf::typeToString.at(firstType); }
		std::string GetType2() { return tf::typeToString.at(secondType); }
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
		int GetCurrentAtk() { return stats.atkCurrent; }
		int GetCurrentDef() { return stats.defCurrent; }
		int GetCurrentSpAtk() { return stats.spAtkCurrent; }
		int GetCurrentSpDef() { return stats.spDefCurrent; }
		int GetCurrentSpd() { return stats.spdCurrent; }
		Pokemon(jt::JsonPkmn p, jt::JsonTemplate t, std::array<mon::Move, 4>& m);
	};
}