#include <string>
#include <random>
#include "Pokemon.h"
#include "Battle.h"

bfx::BattleEffect::BattleEffect(bool _target, int _duration, TurnPhase _turnphase, int _proc)
{
	target = _target;
	duration = _duration;
	turnPhase = _turnphase;
	// To be SOUPER random.
	std::random_device rd;
	std::mt19937_64 eng(rd());
	std::uniform_int_distribution<int> uniform_dist(1, 100);
	// This might not be the best way to handle it, but I'm trying to separate the battle effects logic from the moves that create them.
	if (uniform_dist(eng) > _proc) { throw BattleEffectFailed("Proc failed, effect does not occur."); }
}

bfx::BattleEffect& bfx::BattleEffect::operator--()
{
	int next = this->duration - 1;
	return *this;
}

bfx::ModStat::ModStat(bool _target, int _duration, int _proc, int _stages) :
	BattleEffect(_target, _duration, TurnPhase::BeforeMoveExecuted, _proc)
{
	if (_stages > 6 || stages < -6) { throw BattleEffectFailed("ModStat cannot go above 6 stages or below -6."); }
	stages = _stages;
}
