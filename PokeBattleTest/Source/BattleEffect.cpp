#include <string>
#include <random>
#include "Pokemon.h"
#include "Battle.h"
#include "EventListener.h"

void bfx::ModCriticalRatio::Execute(bat::Turn& turn)
{
	if (targetIsPlayer == turn.attackerIsPlayer) {
		switch (stages) {
		case 1: turn.critChance = 8; break;
		case 2: turn.critChance = 2; break;
		default: turn.critChance = 1; break;
		}
	}
}

void bfx::MultiHit::Execute(bat::Turn& turn)
{
	if (this->targetIsPlayer == turn.attackerIsPlayer) { turn.moveHits = hits; }
}

void bfx::Flinch::Execute(bat::Turn& turn)
{
	std::string msg =  turn.attacker.GetName() + " flinched and couldn't move!";
	Events::WriteToScreen(msg);
	turn.interruptTurn = true;
}

void bfx::BurnAttackReduction::Execute(bat::Turn& turn)
{
	turn.a_atkMod *= 0.5F;
}

void bfx::BurnDamage::Execute(bat::Turn& turn)
{
	int dmgVal = std::floor(turn.attacker.GetFinalHP() / 8.0F) * -1;
	turn.attacker.ModCurrentHP(dmgVal);
	std::string msg = turn.attacker.GetName() + " took damage from their burn.";
	Events::WriteToScreen(msg);
	msg = "Attacker took " + std::to_string(dmgVal) + " damage.";
	Events::Log(msg);
}

void bfx::Paralysis::Execute(bat::Turn& turn)
{
	turn.a_spdMod *= 0.5F;

	std::random_device rd;
	std::mt19937_64 eng(rd());
	std::uniform_int_distribution<int> uniform_dist(1, 100);
	if (uniform_dist(eng) <= 25) {
		std::string msg = turn.attacker.GetName() + " is paralyzed! It can't move!";
		Events::WriteToScreen(msg);
		turn.interruptTurn = true;
	}
}

void bfx::Drowsy::Execute(bat::Turn& turn)
{
	std::random_device rd;
	std::mt19937_64 eng(rd());
	std::uniform_int_distribution<int> uniform_dist(1, 100);
	if (uniform_dist(eng) <= 25) {
		std::string msg = turn.attacker.GetName() + " was too drowsy to move!";
		Events::WriteToScreen(msg);
		turn.interruptTurn = true;
	}
}

void bfx::DrowsyDamageBoost::Execute(bat::Turn& turn)
{
	turn.a_atkMod = turn.a_spAtkMod = 1.25F;
}

void bfx::FrostbiteDamage::Execute(bat::Turn& turn)
{
	int dmgVal = std::floor(turn.attacker.GetFinalHP() / 8.0F) * -1;
	turn.attacker.ModCurrentHP(dmgVal);
	std::string msg = turn.attacker.GetName() + " took damage from their frostbite.";
	Events::WriteToScreen(msg);
	msg = "Attacker took " + std::to_string(dmgVal) + " damage.";
	Events::Log(msg);
}

void bfx::FrostbiteSpecialAttackReduction::Execute(bat::Turn& turn)
{
	turn.a_spAtkMod *= 0.5F;
}

void bfx::Poisoned::Execute(bat::Turn& turn)
{
	int dmgVal = std::floor(turn.attacker.GetFinalHP() / 8.0F) * -1;
	turn.attacker.ModCurrentHP(dmgVal);
	std::string msg = turn.attacker.GetName() + " took damage from their poison.";
	Events::WriteToScreen(msg);
	msg = "Attacker took " + std::to_string(dmgVal) + " damage.";
	Events::Log(msg);
}

void bfx::BadlyPoisoned::Execute(bat::Turn& turn)
{
	float d = 0.0625 * cumulativeTurns;
	int dmgVal = std::floor(turn.attacker.GetFinalHP() / d) * -1;
	turn.attacker.ModCurrentHP(dmgVal);
	std::string msg = turn.attacker.GetName() + " took damage from their poison.";
	Events::WriteToScreen(msg);
	msg = "Attacker took " + std::to_string(dmgVal) + " damage.";
	Events::Log(msg);
	++cumulativeTurns;
}

void bfx::Confusion::Execute(bat::Turn& turn)
{
	std::string msg = turn.attacker.GetName() + " is confused!";
	Events::WriteToScreen(msg);
	if (duration == 1) { // On the last active turn, the victim snaps out. Not possible to check this when the duration hits zero. 
		msg = "It snapped out of its confusion!";
		Events::WriteToScreen(msg);
	}
	else {
		std::random_device rd;
		std::mt19937_64 eng(rd());
		std::uniform_int_distribution<int> uniform_dist(1, 100);
		if (uniform_dist(eng) <= 33) {
			msg = "It hurt itself in its confusion!";
			Events::WriteToScreen(msg);
			turn.interruptTurn = true;
			turn.interruptBecauseConfused = true;
		}
	}
}

void bfx::ConfusionAttackSelf::Execute(bat::Turn& turn)
{
	if (turn.interruptBecauseConfused) {
		std::random_device rd;
		std::mt19937_64 eng(rd());
		std::uniform_int_distribution<int> rand_factor(85, 100);

		float r_factor = rand_factor(eng) / -100.0F;
		int dmg = std::floor( // Condensed attack formula. Can't crit, or benefit from STAB. 
			(((8 * turn.attacker.GetLevel() * turn.attacker.GetFinalAtk() * turn.a_atkMod) / 
			(25 * turn.attacker.GetFinalDef() * turn.a_defMod)) + 2) * r_factor);
		turn.attacker.ModCurrentHP(dmg);
		std::string msg = "Attacker dealt " + std::to_string(dmg) + " damage to itself.";
		Events::Log(msg);
	}
}
