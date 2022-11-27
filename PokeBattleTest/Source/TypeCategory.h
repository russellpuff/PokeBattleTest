#pragma once
#include <map>
#include <string>

namespace tc {
	// This file was originally created to fix a problem with the Type enum that generated 250 compiler errors. 
	// Now it fixes a bunch of other stuff that would've caused untold numbers of additional errors. 
	const enum Type : short {
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

	const enum MoveCategory : short {
		Physical,
		Special,
		Status
	};

	const std::map<std::string, Type> strToType = {
		{"Normal", Type::Normal},
		{"normal", Type::Normal},
		{"NORMAL", Type::Normal},
		{"Fighting", Type::Fighting},
		{"fighting", Type::Fighting},
		{"FIGHTING", Type::Fighting},
		{"Flying", Type::Flying},
		{"flying", Type::Flying},
		{"FLYING", Type::Flying},
		{"Poison", Type::Poison},
		{"poison", Type::Poison},
		{"POISON", Type::Poison},
		{"Ground", Type::Ground},
		{"ground", Type::Ground},
		{"GROUND", Type::Ground},
		{"Rock", Type::Rock},
		{"rock", Type::Rock},
		{"ROCK", Type::Rock},
		{"Bug", Type::Bug},
		{"bug", Type::Bug},
		{"BUG", Type::Bug},
		{"Ghost", Type::Ghost},
		{"ghost", Type::Ghost},
		{"GHOST", Type::Ghost},
		{"Steel", Type::Steel},
		{"steel", Type::Steel},
		{"STEEL", Type::Steel},
		{"Fire", Type::Fire},
		{"fire", Type::Fire},
		{"FIRE", Type::Fire},
		{"Water", Type::Water},
		{"water", Type::Water},
		{"WATER", Type::Water},
		{"Grass", Type::Grass},
		{"grass", Type::Grass},
		{"GRASS", Type::Grass},
		{"Electric", Type::Electric},
		{"electric", Type::Electric},
		{"ELECTRIC", Type::Electric},
		{"Psychic", Type::Psychic},
		{"psychic", Type::Psychic},
		{"PSYCHIC", Type::Psychic},
		{"Ice", Type::Ice},
		{"ice", Type::Ice},
		{"ICE", Type::Ice},
		{"Dragon", Type::Dragon},
		{"dragon", Type::Dragon},
		{"DRAGON", Type::Dragon},
		{"Dark", Type::Dark},
		{"dark", Type::Dark},
		{"DARK", Type::Dark},
		{"Fairy", Type::Fairy},
		{"fairy", Type::Fairy},
		{"FAIRY", Type::Fairy},
	};

	const std::map<Type, std::string> typeToString = {
		{Type::Normal, "Normal"},
		{Type::Fighting, "Fighting"},
		{Type::Flying, "Flying"},
		{Type::Poison, "Poison"},
		{Type::Ground, "Ground"},
		{Type::Rock, "Rock"},
		{Type::Bug, "Bug"},
		{Type::Ghost, "Ghost"},
		{Type::Steel, "Steel"},
		{Type::Fire, "Fire"},
		{Type::Water, "Water"},
		{Type::Grass, "Grass"},
		{Type::Electric, "Electric"},
		{Type::Psychic, "Psychic"},
		{Type::Ice, "Ice"},
		{Type::Dragon, "Dragon"},
		{Type::Dark, "Dark"},
		{Type::Fairy, "Fairy"},
	};

	const std::map<std::string, MoveCategory> strToCategory = {
		{"PHYSICAL", MoveCategory::Physical},
		{"SPECIAL", MoveCategory::Special},
		{"STATUS", MoveCategory::Status}
	};

	const std::map<MoveCategory, std::string> categoryToString = {
		{MoveCategory::Physical, "Physical"},
		{MoveCategory::Special, "Special"},
		{MoveCategory::Status, "Status"}
	};
}