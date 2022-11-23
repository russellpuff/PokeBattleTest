#pragma once
#include <string>

namespace Battle {
	class Battle {
		// Map moveID int key to function, include random chance with pair?
	};

	class Turn {

	};

	class Move {
		int moveID; // Critical component for committing moves and utilizing battlefield effects. 
	};

	enum Type {
		NoType,
		Normal,
		Fighting,
		Flying,
		Poison,
		Ground,
		Rock,
		Bug,
		Ghost,
		Steel,
		Fire,
		Water,
		Grass,
		Electric,
		Psychic,
		Ice,
		Dragon,
		Dark,
		Fairy
	};

	struct Stats {
		int hpBase;
		int atkBase;
		int defBase;
		int spAtkBase;
		int spDefBase;
		int spdBase;
		// Current stats typically use a formula to determine what they are based on level and IVs.
		// The partner application will do the math automatically.
		int hpCurrent;
		int atkCurrent;
		int defCurrent;
		int spAtkCurrent;
		int spDefCurrent;
		int spdCurrent;
	};

	class Pokemon {
		// This class is entirely read-only.During a Battle, attributes will only be referenced.
		// In a more elaborate future project, it will need to permanently store certain data like current HP.
		// This application has no need for that, so Battle tracks it. 
	private:
		std::string name;
		std::string form; // Form determines whether or not a template overwrite occurs, comes up in constructing.
		Stats stats;
		Move moves[4]; // 4 moves always, there will be a special "empty move" to represent having fewer than four. 
		Type firstType;
		Type secondType;
		// Abilities and Items not included because too complex and not enough time.
		// Everything defined in species.db but not here is irrelevant. 
	public:
		std::string GetName() { return name; }

	};
}