#pragma once
#include "Pokemon.h"
#include <map>
#include <string>

namespace tf {
	/*
	I may have made a fucky wucky with inconsistent casing for these, so this is an allpurpose redefiner.
	It's all alone in its own file because grouping it with the others in the mon namespace generated exactly 250 compiler errors.
	I'd call this quarantine.
	*/
	std::map<std::string, mon::Type> strToType{ 
		{"Normal", mon::Type::Normal},
		{"normal", mon::Type::Normal},
		{"NORMAL", mon::Type::Normal},
		{"Fighting", mon::Type::Fighting},
		{"fighting", mon::Type::Fighting},
		{"FIGHTING", mon::Type::Fighting},
		{"Flying", mon::Type::Flying},
		{"flying", mon::Type::Flying},
		{"FLYING", mon::Type::Flying},
		{"Poison", mon::Type::Poison},
		{"poison", mon::Type::Poison},
		{"POISON", mon::Type::Poison},
		{"Ground", mon::Type::Ground},
		{"ground", mon::Type::Ground},
		{"GROUND", mon::Type::Ground},
		{"Rock", mon::Type::Rock},
		{"rock", mon::Type::Rock},
		{"ROCK", mon::Type::Rock},
		{"Bug", mon::Type::Bug},
		{"bug", mon::Type::Bug},
		{"BUG", mon::Type::Bug},
		{"Ghost", mon::Type::Ghost},
		{"ghost", mon::Type::Ghost},
		{"GHOST", mon::Type::Ghost},
		{"Steel", mon::Type::Steel},
		{"steel", mon::Type::Steel},
		{"STEEL", mon::Type::Steel},
		{"Fire", mon::Type::Fire},
		{"fire", mon::Type::Fire},
		{"FIRE", mon::Type::Fire},
		{"Water", mon::Type::Water},
		{"water", mon::Type::Water},
		{"WATER", mon::Type::Water},
		{"Grass", mon::Type::Grass},
		{"grass", mon::Type::Grass},
		{"GRASS", mon::Type::Grass},
		{"Electric", mon::Type::Electric},
		{"electric", mon::Type::Electric},
		{"ELECTRIC", mon::Type::Electric},
		{"Psychic", mon::Type::Psychic},
		{"psychic", mon::Type::Psychic},
		{"PSYCHIC", mon::Type::Psychic},
		{"Ice", mon::Type::Ice},
		{"ice", mon::Type::Ice},
		{"ICE", mon::Type::Ice},
		{"Dragon", mon::Type::Dragon},
		{"dragon", mon::Type::Dragon},
		{"DRAGON", mon::Type::Dragon},
		{"Dark", mon::Type::Dark},
		{"dark", mon::Type::Dark},
		{"DARK", mon::Type::Dark},
		{"Fairy", mon::Type::Fairy},
		{"fairy", mon::Type::Fairy},
		{"FAIRY", mon::Type::Fairy},
	};

	std::map<std::string, mon::MoveCategory> stfToCategory{
		{"PHYSICAL", mon::MoveCategory::Physical},
		{"SPECIAL", mon::MoveCategory::Special},
		{"STATUS", mon::MoveCategory::Status}
	};
}