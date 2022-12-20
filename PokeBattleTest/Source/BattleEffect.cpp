#include <string>
#include <random>
#include "Pokemon.h"
#include "Battle.h"
#include "EventListener.h"
#include "BattleEffect.h"
float ModStageMultiplier(int stageVal);

bfx::ModStat::ModStat(bool _target, int _stages, ChildType _childtype) :
	BattleEffect(_target, -1, TurnPhase::BeforeMoveExecuted, _childtype)
{
	stages = 0;
	ModStages(_stages);
}

int bfx::ModStat::ModStages(int mod)
{
	// This returns a status code that is used in MoveLogic.cpp in the mv::ModMultipleStats function.
	std::string msg;
	if ((stages == 6 && mod > 0) || (stages == -6 && mod < 0)) { return mod > 0 ? 1 : 2; } // Code 1/2 = Stat can't be raised/lowered any more.
	if ((mod + stages) >= 6) { stages = 6; return 3; } // Code 3, stat maxed out.
	if ((mod + stages) <= -6) { stages = -6; return 4; } // Code 4, stat bottomed out.
	stages += mod;
	switch (mod) {
	case -1: return 5;
	case -2: return 6;
	case -3: return 7;
	case 1: return 8;
	case 2: return 9;
	case 3: return 10;
	}
	return -1; // Code -1, stat was modified in an unexpected way. The only known outlier is Belly Drum, but that would return code 3. 
}

void bfx::MultiHit::Execute(bat::Turn& turn) // before
{
	if (targetIsPlayer == turn.attackerIsPlayer) { turn.moveHits = hits; }
}

void bfx::Flinch::Execute(bat::Turn& turn)
{
	if (targetIsPlayer == turn.attackerIsPlayer) {
		std::string msg = turn.attacker.GetName() + " flinched and couldn't move!";
		Events::WriteToScreen(msg);
		turn.interruptTurn = true;
	}
}

void bfx::BurnAttackReduction::Execute(bat::Turn& turn) // before
{
	if (targetIsPlayer == turn.attackerIsPlayer) { turn.a_atkMod *= 0.5F; }
}

void bfx::BurnDamage::Execute(bat::Turn& turn) // after
{
	if (targetIsPlayer == turn.attackerIsPlayer) {
		int dmgVal = std::floor(turn.attacker.GetFinalHP() / 8.0F) * -1;
		turn.attacker.ModCurrentHP(dmgVal);
		std::string msg = turn.attacker.GetName() + " took damage from their burn.";
		Events::WriteToScreen(msg);
		msg = "Attacker took " + std::to_string(dmgVal) + " damage.";
		Events::Log(msg);
	}
}

void bfx::Paralysis::Execute(bat::Turn& turn) // before
{
	if (targetIsPlayer == turn.attackerIsPlayer) {
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
}

void bfx::Drowsy::Execute(bat::Turn& turn) // before
{
	if (targetIsPlayer == turn.attackerIsPlayer) {
		std::random_device rd;
		std::mt19937_64 eng(rd());
		std::uniform_int_distribution<int> uniform_dist(1, 100);
		if (uniform_dist(eng) <= 25) {
			std::string msg = turn.attacker.GetName() + " was too drowsy to move!";
			Events::WriteToScreen(msg);
			turn.interruptTurn = true;
		}
	}
}

void bfx::DrowsyDamageBoost::Execute(bat::Turn& turn) // before
{
	if (targetIsPlayer == turn.attackerIsPlayer) { turn.a_atkMod = turn.a_spAtkMod = 1.25F; }
}

void bfx::FrostbiteDamage::Execute(bat::Turn& turn) // after
{
	if (targetIsPlayer == turn.attackerIsPlayer) {
		int dmgVal = std::floor(turn.attacker.GetFinalHP() / 8.0F) * -1;
		turn.attacker.ModCurrentHP(dmgVal);
		std::string msg = turn.attacker.GetName() + " took damage from their frostbite.";
		Events::WriteToScreen(msg);
		msg = "Attacker took " + std::to_string(dmgVal) + " damage.";
		Events::Log(msg);
	}
}

void bfx::FrostbiteSpecialAttackReduction::Execute(bat::Turn& turn) // before
{
	if (targetIsPlayer == turn.attackerIsPlayer) { turn.a_spAtkMod *= 0.5F; }
}

void bfx::Poisoned::Execute(bat::Turn& turn) // after
{
	if (targetIsPlayer == turn.attackerIsPlayer) {
		int dmgVal = std::floor(turn.attacker.GetFinalHP() / 8.0F) * -1;
		turn.attacker.ModCurrentHP(dmgVal);
		std::string msg = turn.attacker.GetName() + " took damage from their poison.";
		Events::WriteToScreen(msg);
		msg = "Attacker took " + std::to_string(dmgVal) + " damage.";
		Events::Log(msg);
	}
}

void bfx::BadlyPoisoned::Execute(bat::Turn& turn) // after
{
	if (targetIsPlayer == turn.attackerIsPlayer) {
		float d = 0.0625F * cumulativeTurns;
		int dmgVal = std::floor(turn.attacker.GetFinalHP() / d) * -1;
		turn.attacker.ModCurrentHP(dmgVal);
		std::string msg = turn.attacker.GetName() + " took damage from their poison.";
		Events::WriteToScreen(msg);
		msg = "Attacker took " + std::to_string(dmgVal) + " damage.";
		Events::Log(msg);
		++cumulativeTurns;
	}
}

void bfx::Confusion::Execute(bat::Turn& turn)
{
	if (targetIsPlayer == turn.attackerIsPlayer) {
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

void bfx::ModAttack::Execute(bat::Turn& turn)
{
	float mult = ModStageMultiplier(stages);
	if (targetIsPlayer == turn.attackerIsPlayer) { turn.a_atkMod = mult; }
	else { turn.d_atkMod = mult; }
}

void bfx::ModDefense::Execute(bat::Turn& turn)
{
	float mult = ModStageMultiplier(stages);
	if (targetIsPlayer == turn.attackerIsPlayer) { turn.a_defMod = mult; }
	else { turn.d_defMod = mult; }
}

void bfx::ModSpecialAttack::Execute(bat::Turn& turn)
{
	float mult = ModStageMultiplier(stages);
	if (targetIsPlayer == turn.attackerIsPlayer) { turn.a_spAtkMod = mult; }
	else { turn.d_spAtkMod = mult; }
}

void bfx::ModSpecialDefense::Execute(bat::Turn& turn)
{
	float mult = ModStageMultiplier(stages);
	if (targetIsPlayer == turn.attackerIsPlayer) { turn.a_spDefMod = mult; }
	else { turn.d_spDefMod = mult; }
}

void bfx::ModSpeed::Execute(bat::Turn& turn)
{
	float mult = ModStageMultiplier(stages);
	if (targetIsPlayer == turn.attackerIsPlayer) { turn.a_spdMod = mult; }
	else { turn.d_spdMod = mult; }
}

void bfx::ModAccuracy::Execute(bat::Turn& turn)
{
	if (targetIsPlayer == turn.attackerIsPlayer) { turn.a_accuracyStages = stages; }
}

void bfx::ModEvasion::Execute(bat::Turn& turn)
{
	if (targetIsPlayer != turn.attackerIsPlayer) { turn.d_evasionStages = stages; }
}

void bfx::ModCriticalRatio::Execute(bat::Turn& turn)
{
	if (targetIsPlayer == turn.attackerIsPlayer) {
		if (turn.critChance != 24) { turn.critChance = turn.critChance == 8 ? 2 : 1; }
		else {
			switch (stages) {
			case 1: turn.critChance = 8; break;
			case 2: turn.critChance = 2; break;
			default: turn.critChance = 1; break;
			}
		}
	}
}

void bfx::TemporaryCritIncrease::Execute(bat::Turn& turn)
{ // This a workaround to a problem where increased critical moves don't know about existing crit buffs. 
	if (targetIsPlayer == turn.attackerIsPlayer) {
		if (turn.critChance != 24) { turn.critChance = turn.critChance == 8 ? 2 : 1; }
		else { turn.critChance = 8; }
	}
}

void bfx::MoveAutoHit::Execute(bat::Turn& turn)
{
	if (targetIsPlayer == turn.attackerIsPlayer) { turn.moveHits = true; }
}

void bfx::FlatDamageMod::Execute(bat::Turn& turn)
{
	if (targetIsPlayer == turn.attackerIsPlayer) { turn.a_absoluteDmgMod *= mod; }
}

void bfx::BypassSemiInvulnerable::Execute(bat::Turn& turn) 
{
	if (targetIsPlayer == turn.attackerIsPlayer) { turn.a_bypassInvulnerable = true; }
}

void bfx::InterruptMove::Execute(bat::Turn& turn) 
{
	if (targetIsPlayer == turn.attackerIsPlayer) { 
		std::string msg = turn.attacker.GetName() + " had their move interrupted!";
		turn.interruptTurn = true; 
	}
}

float ModStageMultiplier(int stageVal) { // Rounds to 3 decimal places because I need to hold this value and can't have the computer ruining it. 
	if (stageVal >= 0) { return roundf(((2.0F + stageVal) / 2.0F) * 1000.0F) / 1000.0F; }
	else { return roundf((2.0F / (2.0F - stageVal)) * 1000.0F) / 1000.0F; }
}

void bfx::DamageTrap::Execute(bat::Turn& turn)
{
	if (targetIsPlayer == turn.attackerIsPlayer) {
		int dmgVal = std::floor(turn.attacker.GetFinalHP() / 8.0F) * -1;
		turn.attacker.ModCurrentHP(dmgVal);
		std::string msg = turn.attacker.GetName() + " took damage from the " + cause;
		Events::WriteToScreen(msg);
		msg = "Attacker took " + std::to_string(dmgVal) + " damage.";
		Events::Log(msg);
	}
}

bfx::DamageTrap::~DamageTrap()
{
	std::string msg = cause + " wore off.";
	Events::WriteToScreen(msg);
}

void bfx::ReflectDefenseBoost::Execute(bat::Turn& turn)
{
	if (targetIsPlayer != turn.attackerIsPlayer) { turn.d_defMod *= 2.0F; }
}

bfx::ReflectDefenseBoost::~ReflectDefenseBoost() {
	std::string msg = "Reflect wore off.";
	Events::WriteToScreen(msg);
}

void bfx::LightScreenSpecialDefenseBoost::Execute(bat::Turn& turn)
{
	if (targetIsPlayer != turn.attackerIsPlayer) { turn.d_spDefMod *= 2.0F; }
}


bfx::LightScreenSpecialDefenseBoost::~LightScreenSpecialDefenseBoost() {
	std::string msg = "Light Screen wore off.";
	Events::WriteToScreen(msg);
}

void bfx::AuroraVeilDefSpdefBoost::Execute(bat::Turn& turn)
{
	if (targetIsPlayer != turn.attackerIsPlayer) { 
		turn.d_defMod *= 2732.0F / 4096.0F;
		turn.d_spDefMod *= 2732.0F / 4096.0F;
	}
}

bfx::AuroraVeilDefSpdefBoost::~AuroraVeilDefSpdefBoost() {
	std::string msg = "Aurora Veil wore off.";
	Events::WriteToScreen(msg);
}

void bfx::ElectricTerrain::Execute(bat::Turn& turn)
{
	if (turn.attackerIsPlayer && turn.a_groun)
}

void bfx::MistyTerrain::Execute(bat::Turn& turn)
{

}

void bfx::PsychicTerrain::Execute(bat::Turn& turn)
{

}

void bfx::GrassyTerrain::Execute(bat::Turn& turn)
{

}
