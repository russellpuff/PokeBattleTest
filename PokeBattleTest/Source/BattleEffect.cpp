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


bfx::LightScreenSpecialDefenseBoost::~LightScreenSpecialDefenseBoost() 
{
	std::string msg = "Light Screen wore off.";
	Events::WriteToScreen(msg);
}

void bfx::AuroraVeilDefSpdefBoost::Execute(bat::Turn& turn)
{
	if (targetIsPlayer != turn.attackerIsPlayer) { 
		turn.d_defMod *= 2.0F;
		turn.d_spDefMod *= 2.0F;
	}
}

bfx::AuroraVeilDefSpdefBoost::~AuroraVeilDefSpdefBoost() 
{
	std::string msg = "Aurora Veil wore off.";
	Events::WriteToScreen(msg);
}

void bfx::ElectricTerrain::Execute(bat::Turn& turn)
{ // Electric Terrain's drowsy-blocking property is handled in the status effects MoveLogic function.
	if (turn.a_grounded) {
		if (turn.move.GetID() == 459) {
			std::string msg = "Nature Power became Thunderbolt!";
			Events::WriteToScreen(msg);
			turn.move = pkmn::Move(84);
		}
		if (turn.move.GetType() == tc::Electric || turn.a_moveTypeOverride == tc::Electric) { turn.a_absoluteDmgMod *= 1.3F; }
	}
}

void bfx::MistyTerrain::Execute(bat::Turn& turn)
{ // Misty Terrain's status-blocking property is also handled in status effects.
	if (turn.d_grounded && turn.move.GetType() == tc::Dragon) { turn.a_absoluteDmgMod *= 0.5F; }
	if (turn.a_grounded && turn.move.GetID() == 459) {
		std::string msg = "Nature Power became Moonblast!";
		Events::WriteToScreen(msg);
		turn.move = pkmn::Move(99);
	}
}

void bfx::PsychicTerrain::Execute(bat::Turn& turn)
{
	if (turn.d_grounded && turn.move.GetPriority() > 0) {
		std::string msg = "The Psychic Terrain makes the opponent immune to priority moves!";
		Events::WriteToScreen(msg);
		turn.interruptTurn = true;
		return;
	}
	if (turn.a_grounded) {
		if (turn.move.GetID() == 459) {
			std::string msg = "Nature Power became Psychic!";
			Events::WriteToScreen(msg);
			turn.move = pkmn::Move(150);
		}
		if (turn.move.GetType() == tc::Psychic || turn.a_moveTypeOverride == tc::Psychic) { turn.a_absoluteDmgMod *= 1.3F; }
	}
}

void bfx::GrassyTerrain::Execute(bat::Turn& turn)
{
	if (turn.a_grounded) {
		// A silly solution. Floral Healing gets boosted from 50% to 66%, so instead I restore 16% first, then let Floral Healing do its thing.
		if (turn.move.GetID() == 108) {
			int maxVal = turn.attacker.GetFinalHP();
			int current = turn.attacker.GetCurrentHP();
			int healVal = std::floor(maxVal * (16.0F / 100.0F));
			if ((healVal + current) > maxVal) { healVal = maxVal - current; }
			turn.attacker.ModCurrentHP(healVal);
		}
		if (turn.move.GetID() == 459) {
			std::string msg = "Nature Power became Energy Ball!";
			Events::WriteToScreen(msg);
			turn.move = pkmn::Move(363);
		}
		if (turn.move.GetType() == tc::Grass || turn.a_moveTypeOverride == tc::Grass) { turn.a_absoluteDmgMod *= 1.3F; }
	}
	if (turn.d_grounded) {
		if (turn.move.GetID() == 257 || turn.move.GetID() == 260) { turn.a_absoluteDmgMod *= 0.5F; }
	}
}

void bfx::GrassyTerrainHeal::Execute(bat::Turn& turn)
{
	if (turn.a_grounded) {
		int maxVal = turn.attacker.GetFinalHP();
		int current = turn.attacker.GetCurrentHP();
		int healVal = std::floor(maxVal / 16.0F);
		if ((healVal + current) > maxVal) { healVal = maxVal - current; }
		turn.attacker.ModCurrentHP(healVal);
		std::string msg = turn.attacker.GetName() + " regained a little HP from the Grassy Terrain!";
		Events::WriteToScreen(msg);
	}
}

void bfx::HarshSunlight::Execute(bat::Turn& turn)
{
	if (turn.move.GetID() == 428) {
		turn.a_absoluteDmgMod *= 2.0F;
		turn.a_moveTypeOverride = tc::Fire;
		std::string msg = "Weather ball became Fire-type!";
		Events::WriteToScreen(msg);
	}
	if (turn.move.GetType() == tc::Fire) { turn.a_absoluteDmgMod *= 1.5F; }
	if (turn.move.GetType() == tc::Water) { turn.a_absoluteDmgMod *= 0.5F; }
	if (turn.move.GetID() == 79 || turn.move.GetID() == 229) { turn.a_accMod *= 0.5F; } // Lowers accuracy of Hurricaine and Lightning Strike by half.
}

void bfx::Rain::Execute(bat::Turn& turn)
{
	if (turn.move.GetID() == 428) {
		turn.a_absoluteDmgMod *= 2.0F;
		turn.a_moveTypeOverride = tc::Water;
		std::string msg = "Weather ball became Water-type!";
		Events::WriteToScreen(msg);
	}
	if (turn.move.GetType() == tc::Water) { turn.a_absoluteDmgMod *= 1.5F; }
	if (turn.move.GetType() == tc::Fire) { turn.a_absoluteDmgMod *= 0.5F; }
	if (turn.move.GetID() == 79 || turn.move.GetID() == 229) { turn.moveHits = true; } // Hurricaine and Lightning Strike bypass accuracy checks. 
	if (turn.move.GetID() == 360 || turn.move.GetID() == 368) { turn.a_absoluteDmgMod *= 0.5F; } // Halves power of Solar Beam and Solar Blade.
}

void bfx::Sandstorm::Execute(bat::Turn& turn)
{
	if (turn.move.GetID() == 428) {
		turn.a_absoluteDmgMod *= 2.0F;
		turn.a_moveTypeOverride = tc::Rock;
		std::string msg = "Weather ball became Rock-type!";
		Events::WriteToScreen(msg);
	}
	if (turn.defender.GetType1() == tc::Rock || turn.defender.GetType2() == tc::Rock) { turn.d_spDefMod *= 1.5F; }
	if (turn.move.GetID() == 360 || turn.move.GetID() == 368) { turn.a_absoluteDmgMod *= 0.5F; } // Halves power of Solar Beam and Solar Blade.
}

void bfx::SandstormDamage::Execute(bat::Turn& turn)
{
	if (turn.attacker.GetType1() == tc::Rock || turn.attacker.GetType2() == tc::Rock ||
		turn.attacker.GetType1() == tc::Steel || turn.attacker.GetType2() == tc::Steel ||
		turn.attacker.GetType1() == tc::Ground || turn.attacker.GetType2() == tc::Ground) 
	{ return; }
	int dmgVal = std::floor(turn.attacker.GetFinalHP() / 16.0F) * -1;
	turn.attacker.ModCurrentHP(dmgVal);
	std::string msg = turn.attacker.GetName() + " was buffeted by the Sandstorm!";
	Events::WriteToScreen(msg);
}

void bfx::Hail::Execute(bat::Turn& turn)
{
	if (turn.move.GetID() == 428) {
		turn.a_absoluteDmgMod *= 2.0F;
		turn.a_moveTypeOverride = tc::Ice;
		std::string msg = "Weather ball became Ice-type!";
		Events::WriteToScreen(msg);
	}
	if (turn.defender.GetType1() == tc::Ice || turn.defender.GetType2() == tc::Ice) { turn.d_defMod *= 1.5F; }
	if (turn.move.GetID() == 386) { turn.moveHits = true; } // Blizzard bypasses accuracy checks.
	if (turn.move.GetID() == 360 || turn.move.GetID() == 368) { turn.a_absoluteDmgMod *= 0.5F; } // Halves power of Solar Beam and Solar Blade.
}

void bfx::HailDamage::Execute(bat::Turn& turn)
{
	if (turn.attacker.GetType1() == tc::Ice || turn.attacker.GetType2() == tc::Ice) { return;  }
	int dmgVal = std::floor(turn.attacker.GetFinalHP() / 16.0F) * -1;
	turn.attacker.ModCurrentHP(dmgVal);
	std::string msg = turn.attacker.GetName() + " was buffeted by the Hail!";
	Events::WriteToScreen(msg);
}

void bfx::StrongWinds::Execute(bat::Turn& turn)
{
	if (turn.move.GetID() == 428) {
		turn.a_absoluteDmgMod *= 2.0F;
		turn.a_moveTypeOverride = tc::Flying;
		std::string msg = "Weather ball became Flying-type!";
		Events::WriteToScreen(msg);
	}
	if (turn.move.GetType() == tc::Flying) { turn.a_absoluteDmgMod *= 1.5F; }
	if (turn.move.GetID() == 229) { turn.moveHits = true; } // Hurricane bypasses accuracy checks.
}

void bfx::IntenseGravity::Execute(bat::Turn& turn)
{ // Also causes all pokemon to become grounded. This is acknowledged in the bat::Turn() constructor.
	if (turn.move.GetID() == 428) {
		turn.a_absoluteDmgMod *= 2.0F;
		turn.a_moveTypeOverride = tc::Steel;
		std::string msg = "Weather ball became Steel-type!";
		Events::WriteToScreen(msg);
	}
	turn.a_accMod *= 6840.0F / 4096.0F;
	if (turn.move.GetID() == 219 || turn.move.GetID() == 197 || turn.move.GetID() == 220 ||
		turn.move.GetID() == 91 || turn.move.GetID() == 179 || turn.move.GetID() == 473) {
		std::string msg = "But it failed!";
		Events::WriteToScreen(msg);
		turn.interruptTurn = true;
	}
}

void bfx::WonderRoom::Execute(bat::Turn& turn)
{
	turn.a_defOverride = turn.attacker.GetBaseSpDef();
	turn.a_spDefOverride = turn.attacker.GetBaseDef();
	turn.d_defOverride = turn.defender.GetBaseSpDef();
	turn.d_spDefOverride = turn.defender.GetBaseDef();
}

void bfx::MudSport::Execute(bat::Turn& turn)
{
	if (turn.move.GetType() == tc::Electric) { turn.a_absoluteDmgMod *= 67.0F / 100.0F; }
}

void bfx::SplashSport::Execute(bat::Turn& turn)
{
	if (turn.move.GetType() == tc::Fire) { turn.a_absoluteDmgMod *= 67.0F / 100.0F; }
}

void bfx::IonDeluge::Execute(bat::Turn& turn)
{
	if (turn.move.GetType() == tc::Normal) { turn.a_moveTypeOverride = tc::Electric; }
}

void bfx::MoveOverride::Execute(bat::Turn& turn)
{
	if (targetIsPlayer == turn.attackerIsPlayer) {
		turn.move = pkmn::Move(newMove);
	}
}

void bfx::Recoil::Execute(bat::Turn& turn)
{
	if (targetIsPlayer == turn.attackerIsPlayer) {
		// Unimplemented for now, my method for tracking damage during the round is flawed and it's a low priority to restructure anything.
		std::string msg = turn.attacker.GetName() + " was supposed to take recoil damage but it wasn't coded in. " +
			"Kindly hit yourself sufficiently hard enough in the head with a mallet to take the appropriate amount of damage.";
		Events::WriteToScreen(msg);
	}
}
