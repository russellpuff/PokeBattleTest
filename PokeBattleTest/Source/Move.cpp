#include <random>
#include "Move.h"
#include "BattleEffect.h"
#include "EventListener.h"
#include "Battle.h"

void mv::IncreasedCriticalOneStage(bat::Battle& battle) // pre
{
	// Attacker is target, 100% chance to increase by 1 stage for 1 turn. 
	battle.battleEffects.push_back(new bfx::ModCriticalRatio(battle.attackerIsPlayer, 1, 100, 1));
	std::string msg = "Increased attacker critical hit ratio by 1 stage for this move.";
	Events::Log(msg);
}

void mv::Hit2to5Times(bat::Battle& battle) // post
{
	std::random_device rd;
	std::mt19937_64 eng(rd());
	std::uniform_int_distribution<int> uniform_dist(1, 100);
	int res = uniform_dist(eng);
	int hits;
	if (res >= 1 && res <= 35) { hits = 2; } // 35% chance
	else if (res >= 36 && res <= 70) { hits = 3; } // 35% chance
	else if (res >= 71 && res <= 85) { hits = 4; } // 15% chance
	else { hits = 5; } // 15% chance
	battle.battleEffects.push_back(new bfx::MultiHit(battle.attackerIsPlayer, hits));
}

void mv::HitTwice(bat::Battle& battle)
{
	battle.battleEffects.push_back(new bfx::MultiHit(battle.attackerIsPlayer, 2));
}

void mv::InstantHealSelf(bat::Battle& battle, int percent)
{
	std::string attacker = battle.attackerIsPlayer ? battle.player.GetName() : battle.rival.GetName();
	int maxVal = battle.attackerIsPlayer ? battle.player.GetCurrentHP() : battle.rival.GetCurrentHP();
	int* current = battle.attackerIsPlayer ? &battle.playerCurrentHP : &battle.rivalCurrentHP;
	int healVal = std::floor(maxVal * (percent / 100.0F));
	if ((healVal + *current) > maxVal) { healVal = maxVal - *current; }
	*current += healVal;
	std::string msg = "Attacker regained " + std::to_string(healVal) + " hit points.";
	Events::Log(msg);
	msg = attacker + " regained health!";
	battle.WriteToScreen(msg);
}

void mv::HealOnDamage(bat::Battle& battle, int percent)
{
	std::string defender = battle.attackerIsPlayer ? battle.rival.GetName() : battle.player.GetName();
	int maxVal = battle.attackerIsPlayer ? battle.player.GetCurrentHP() : battle.rival.GetCurrentHP();
	int* current = battle.attackerIsPlayer ? &battle.playerCurrentHP : &battle.rivalCurrentHP;
	int damageDealt = battle.attackerIsPlayer ? battle.p_dmgThisTurn : battle.r_dmgThisTurn;
	int healVal = std::floor(damageDealt * (percent / 100.0F));
	if ((healVal + *current) > maxVal) { healVal = maxVal - *current; }
	*current += healVal;
	std::string msg = "Attacker regained " + std::to_string(healVal) + " hit points.";
	Events::Log(msg);
	msg = "The opposing " + defender + " had its energy drained.";
	battle.WriteToScreen(msg);
}
