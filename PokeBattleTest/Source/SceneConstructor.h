#pragma once
#include <string>
#include <vector>
#include "Pokemon.h"

namespace sc {
	enum MoveRestriction;
	extern std::array<MoveRestriction, 4> p_restrictMoves;
	extern std::array<MoveRestriction, 4> r_restrictMoves;
	std::tuple<pkmn::Pokemon, pkmn::Pokemon> PrepareAndConstructPokemon();
}