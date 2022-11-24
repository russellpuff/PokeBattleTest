#pragma once
#include <string>
#include "Pokemon.h"
namespace bat {
	class Battle {
	private:
		mon::Pokemon player;
		mon::Pokemon opponent;
	public:
		Battle(mon::Pokemon& _player, mon::Pokemon& _opponent) : player(_player), opponent(_opponent) {};
	};

	class Turn {

	};
}