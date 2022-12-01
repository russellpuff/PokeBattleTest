#pragma once
#include <string>
#include <queue>
#include "Pokemon.h"

namespace sc {
	std::tuple<mon::Pokemon, mon::Pokemon> PrepareAndConstructPokemon(std::queue<std::string>& log);
}