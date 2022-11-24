#pragma once
#include <string>
#include <array>
namespace mon {
	enum Type : short; // forward declarations
	enum MoveCategory : short;

	class Move {
	private:
		int moveID; // Critical component for committing moves and utilizing battlefield effects. 
		std::string name;
		Type type;
		MoveCategory category;
		int powerPoints;
		int power;
		int accuracy;
		int priority;
	public:
		Move();
		Move(int _moveID); // real constructor
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
		Stats(int _dexid); // real constructor
	};

	class Pokemon {
		/*
		
		
		
		
		
		
		
		
		
		
		
		
		
		You are writing the definition for this class's constructor, but you have to finish MonDesigner before this will work. 
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		*/
		// This class is entirely read-only.During a Battle, attributes will only be referenced.
		// In a more elaborate future project, it will need to permanently store certain data like current/max HP.
		// This application has no need for that, so Battle tracks it. 
	private:
		std::string name;
		std::string form; // Form determines whether or not a template overwrite occurs, comes up in constructing.
		Stats stats;
		std::array<Move, 4> moves; // 4 moves always, there will be a special "empty move" to represent having fewer than four. 
		Type firstType;
		Type secondType;
		// Abilities and Items not included because too complex and not enough time.
		// Everything defined in species.db but not here is irrelevant. 
	public:
		std::string GetName() { return name; }
		Pokemon(bool player);
	};

	enum Type : short {
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

	enum MoveCategory : short {
		Physical,
		Special,
		Status
	};
}