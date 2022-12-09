#include <random>
#include "MoveLogic.h"
#include "BattleEffect.h"
#include "EventListener.h"
#include "Battle.h"
#include "Pokemon.h"
#include "TypeCategory.h"
bool AttemptProc(int proc);
bool CheckForExistingStatus(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, bool attackerIsPlayer);
void DisplayStatChangeResults(int code, std::string who, std::string stat);
//
// Using battle.attackerIsPlayer as the target will resolve to the target being the attacker.
// If !battle.attackerIsPlayer is passed, the target is the defender.
//

void mv::IncreasedCriticalOneStage(bat::Battle& battle) // pre
{ 
	battle.battleEffects.push_back(std::make_unique<bfx::TemporaryCritIncrease>(battle.attackerIsPlayer));
	std::string msg = "Increased attacker critical hit ratio by 1 stage for this move.";
	Events::Log(msg);
}

void mv::Hit2to5Times(bat::Battle& battle) // pre
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
	battle.battleEffects.push_back(std::make_unique<bfx::MultiHit>(battle.attackerIsPlayer, hits));
}

void mv::HitTwice(bat::Battle& battle) // post
{
	battle.battleEffects.push_back(std::make_unique<bfx::MultiHit>(battle.attackerIsPlayer, 2));
}

void mv::InstantHealSelf(bat::Battle& battle, int percent) // post
{
	std::string attacker = battle.attackerIsPlayer ? battle.player.GetName() : battle.rival.GetName();

	int maxVal = battle.attackerIsPlayer ? battle.player.GetFinalHP() : battle.rival.GetFinalHP();
	int current = battle.attackerIsPlayer ? battle.player.GetCurrentHP() : battle.rival.GetCurrentHP();
	int healVal = std::floor(maxVal * (percent / 100.0F));
	
	if ((healVal + current) > maxVal) { healVal = maxVal - current; }
	if (battle.attackerIsPlayer) { battle.player.ModCurrentHP(healVal); }
	else { battle.rival.ModCurrentHP(healVal); }

	std::string msg = "Attacker regained " + std::to_string(healVal) + " hit points.";
	Events::Log(msg);
	msg = attacker + " regained health!";
	Events::WriteToScreen(msg);
}

void mv::HealOnDamage(bat::Battle& battle, int percent) // post
{
	std::string defender = battle.attackerIsPlayer ? battle.rival.GetName() : battle.player.GetName();

	int maxVal = battle.attackerIsPlayer ? battle.player.GetFinalHP() : battle.rival.GetFinalHP();
	int current = battle.attackerIsPlayer ? battle.player.GetCurrentHP() : battle.rival.GetCurrentHP();
	int damageDealt = battle.attackerIsPlayer ? battle.p_dmgThisTurn : battle.r_dmgThisTurn;
	int healVal = std::floor(damageDealt * (percent / 100.0F));

	if ((healVal + current) > maxVal) { healVal = maxVal - current; }
	if (battle.attackerIsPlayer) { battle.player.ModCurrentHP(healVal); }
	else { battle.rival.ModCurrentHP(healVal); }

	std::string msg = "Attacker regained " + std::to_string(healVal) + " hit points.";
	Events::Log(msg);
	msg = "The opposing " + defender + " had its energy drained.";
	Events::WriteToScreen(msg);
}

void mv::ChanceFlinch(bat::Battle& battle, int proc) // post
{
	std::string msg;
	if (AttemptProc(proc)) {
		battle.battleEffects.push_back(std::make_unique<bfx::Flinch>(not battle.attackerIsPlayer));
		msg = "Procced Flinch on defender.";
	} else 
	{ msg = "Attempted to proc Flinch on defender, but it failed. (STATUS OK)"; }	
	Events::Log(msg);
}

void mv::ChanceBurn(bat::Battle& battle, int proc)
{
	mv::ChanceStatusEffect(battle, Burn, tc::Fire, proc);
}

void mv::ChanceParalyze(bat::Battle& battle, int proc)
{
	mv::ChanceStatusEffect(battle, Paralysis, tc::Electric, proc);
}

void mv::ChanceDrowsy(bat::Battle& battle, int proc)
{
	mv::ChanceStatusEffect(battle, Drowsy, tc::NoType, proc);
}

void mv::ChanceFrostbite(bat::Battle& battle, int proc)
{
	mv::ChanceStatusEffect(battle, Frostbite, tc::Ice, proc);
}

void mv::ChancePoison(bat::Battle& battle, int proc)
{
	mv::ChanceStatusEffect(battle, Poison, tc::Poison, proc);
}

void mv::ChanceBadlyPoison(bat::Battle& battle, int proc)
{
	mv::ChanceStatusEffect(battle, BadlyPoison, tc::Poison, proc);
}

void mv::ChanceConfusion(bat::Battle& battle, int proc)
{
	mv::ChanceStatusEffect(battle, Confusion, tc::NoType, proc);
}

void mv::ModAttack(bat::Battle& battle, int proc, int stages, bool targetAttacker)
{ // Why have all these resolve to ModMultipleStats? It just feels better to me. Like you have a better idea of what's going on. 
	ModMultipleStats(battle, proc, stages, 0, 0, 0, 0, 0, 0, 0, targetAttacker);
}

void mv::ModDefense(bat::Battle& battle, int proc, int stages, bool targetAttacker)
{
	ModMultipleStats(battle, proc, 0, stages, 0, 0, 0, 0, 0, 0, targetAttacker);
}

void mv::ModSpecialAttack(bat::Battle& battle, int proc, int stages, bool targetAttacker)
{
	ModMultipleStats(battle, proc, 0, 0, stages, 0, 0, 0, 0, 0, targetAttacker);
}

void mv::ModSpecialDefense(bat::Battle& battle, int proc, int stages, bool targetAttacker)
{
	ModMultipleStats(battle, proc, 0, 0, 0, stages, 0, 0, 0, 0, targetAttacker);
}

void mv::ModSpeed(bat::Battle& battle, int proc, int stages, bool targetAttacker)
{
	ModMultipleStats(battle, proc, 0, 0, 0, 0, stages, 0, 0, 0, targetAttacker);
}

void mv::ModAccuracy(bat::Battle& battle, int proc, int stages, bool targetAttacker)
{
	ModMultipleStats(battle, proc, 0, 0, 0, 0, 0, stages, 0, 0, targetAttacker);
}

void mv::ModEvasion(bat::Battle& battle, int proc, int stages, bool targetAttacker)
{
	ModMultipleStats(battle, proc, 0, 0, 0, 0, 0, 0, stages, 0, targetAttacker);
}

void mv::ModCritical(bat::Battle& battle, int proc, int stages, bool targetAttacker)
{
	ModMultipleStats(battle, proc, 0, 0, 0, 0, 0, 0, 0, stages, targetAttacker);
}

void mv::ModMultipleStats(bat::Battle& battle, int proc, int atk, int def, int spatk, int spdef, int spd, int acc, int eva, int crit, bool targetAttacker) // post
{
	bool be_Affects = targetAttacker == battle.attackerIsPlayer; // XNOR resolves the BattleEffect's target from who the stat change affects and who's attacking.
	std::string who = be_Affects ? battle.player.GetName() : battle.rival.GetName();
	int compare[8] = { atk, def, spatk, spdef, spd, acc, eva, crit };
	int results[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	std::string stats[8] = { "Attack", "Defense", "Special Attack", "Special Defense", "Speed", "Accuracy", "Evasion", "Critical Ratio" };
	if (AttemptProc(proc)) {
		for (const std::unique_ptr<bfx::BattleEffect>& b : battle.battleEffects) { // Check for active battle effects. I don't know if this is the most efficient, but it heavily cuts down on reused code. 
			if (atk != 0 && (dynamic_cast<bfx::ModAttack*>(b.get()) != nullptr && b.get()->GetTarget() == be_Affects)) {
				results[0] = dynamic_cast<bfx::ModAttack*>(b.get())->ModStages(atk); atk = 0; continue; // After successfully modding, set to zero to short-circuit skip future casts.
			}
			if (def != 0 && (dynamic_cast<bfx::ModDefense*>(b.get()) != nullptr && b.get()->GetTarget() == be_Affects)) {
				results[1] = dynamic_cast<bfx::ModDefense*>(b.get())->ModStages(def); def = 0; continue;
			}
			if (spatk != 0 && (dynamic_cast<bfx::ModSpecialAttack*>(b.get()) != nullptr && b.get()->GetTarget() == be_Affects)) { 
				results[2] = dynamic_cast<bfx::ModSpecialAttack*>(b.get())->ModStages(spatk); spatk = 0; continue;
			}
			if (spdef != 0 && (dynamic_cast<bfx::ModSpecialDefense*>(b.get()) != nullptr && b.get()->GetTarget() == be_Affects)) { 
				results[3] = dynamic_cast<bfx::ModSpecialDefense*>(b.get())->ModStages(spdef); spdef = 0; continue;
			}
			if (spd != 0 && (dynamic_cast<bfx::ModSpeed*>(b.get()) != nullptr && b.get()->GetTarget() == be_Affects)) { 
				results[4] = dynamic_cast<bfx::ModSpeed*>(b.get())->ModStages(spd); spd = 0; continue;
			}
			if (acc != 0 && (dynamic_cast<bfx::ModAccuracy*>(b.get()) != nullptr && b.get()->GetTarget() == be_Affects)) { 
				results[5] = dynamic_cast<bfx::ModAccuracy*>(b.get())->ModStages(acc); acc = 0; continue;
			}
			if (eva != 0 && (dynamic_cast<bfx::ModEvasion*>(b.get()) != nullptr && b.get()->GetTarget() == be_Affects)) {
				results[6] = dynamic_cast<bfx::ModEvasion*>(b.get())->ModStages(eva); eva = 0; continue;
			}
			if (crit != 0 && (dynamic_cast<bfx::ModCriticalRatio*>(b.get()) != nullptr && b.get()->GetTarget() == be_Affects)) {
				results[7] = dynamic_cast<bfx::ModCriticalRatio*>(b.get())->ModStages(crit); crit = 0; continue;
			}
		}
		// Where results[x] wasn't changed (stat wasn't modded) but a pending stat change exists, create new BattleEffect.
		for (int i = 0; i < 8; ++i) { if (results[i] == 0 && compare[i] != 0) { DisplayStatChangeResults(results[i], who, stats[i]); } }
		if (results[0] == 0 && compare[0] != 0) { battle.battleEffects.push_back(std::make_unique<bfx::ModAttack>(be_Affects, atk)); }
		if (results[1] == 0 && compare[1] != 0) { battle.battleEffects.push_back(std::make_unique<bfx::ModDefense>(be_Affects, def)); }
		if (results[2] == 0 && compare[2] != 0) { battle.battleEffects.push_back(std::make_unique<bfx::ModSpecialAttack>(be_Affects, spatk)); }
		if (results[3] == 0 && compare[3] != 0) { battle.battleEffects.push_back(std::make_unique<bfx::ModSpecialDefense>(be_Affects, spdef)); }
		if (results[4] == 0 && compare[4] != 0) { battle.battleEffects.push_back(std::make_unique<bfx::ModSpeed>(be_Affects, spd)); }
		if (results[5] == 0 && compare[5] != 0) { battle.battleEffects.push_back(std::make_unique<bfx::ModAccuracy>(be_Affects, acc)); }
		if (results[6] == 0 && compare[6] != 0) { battle.battleEffects.push_back(std::make_unique<bfx::ModEvasion>(be_Affects, eva)); }
		if (results[7] == 0 && compare[7] != 0) { battle.battleEffects.push_back(std::make_unique<bfx::ModCriticalRatio>(be_Affects, crit)); }
		for (int i = 0; i < 8; ++i) { if (results[i] != 0) { DisplayStatChangeResults(results[i], who, stats[i]); } }
	}
	else {
		std::string msg = "Attempted to stat change " +
			std::to_string(atk) + "\\" +
			std::to_string(def) + "\\" +
			std::to_string(spatk) + "\\" +
			std::to_string(spdef) + "\\" +
			std::to_string(spd) + "\\" +
			std::to_string(acc) + "\\" +
			std::to_string(eva) + "\\" +
			std::to_string(crit) + " on ";
		msg += targetAttacker ? "attacker, " : "defender, ";
		msg += "but it failed. (STATUS OK)";
		Events::Log(msg);

	}
}

void mv::CheckTargetMinimized(bat::Battle& battle) // pre
{
	bool minimized = false;
	if (battle.attackerIsPlayer) { for (int m : battle.r_movesUsed) { if (m == 458) { minimized = true; break; } } }
	else { for (int m : battle.p_movesUsed) { if (m == 458) { minimized = true; } } }
	if (minimized) {
		battle.battleEffects.push_back(std::make_unique<bfx::MoveAutoHit>(battle.attackerIsPlayer));
		battle.battleEffects.push_back(std::make_unique<bfx::FlatDamageMod>(battle.attackerIsPlayer, 1, 2.0F));
		std::string msg = "Target was minimized, this move dictates it bypasses accuracy checks and does double damage.";
		Events::Log(msg);
	}
}

void mv::CanHitDuringFlyFreefall(bat::Battle& battle, bool moveIsSwift)
{
	bool flying_or_falling = false;
	if ((battle.attackerIsPlayer && (battle.r_movesUsed.front() == 219 || battle.r_movesUsed.front() == 220)) ||
		!battle.attackerIsPlayer && (battle.p_movesUsed.front() == 219 || battle.p_movesUsed.front() == 220)) {
		flying_or_falling = true;
	}
	if (flying_or_falling) {
		int dur = battle.attackerIsPlayer == battle.playerGoesFirst ? 1 : 2; // XNOR resolves whether the interrupt occurs this round or next. 
		battle.battleEffects.push_back(std::make_unique<bfx::BypassSemiInvulnerable>(battle.attackerIsPlayer));
		std::string msg = "Target was flying or falling, this move dictates it can hit during this semi-invulnerable period.";
		if (!moveIsSwift) {
			battle.battleEffects.push_back(std::make_unique<bfx::InterruptMove>(not battle.attackerIsPlayer, dur));
			battle.battleEffects.push_back(std::make_unique<bfx::FlatDamageMod>(battle.attackerIsPlayer, 1, 1.5F));
			msg += " The move also indicates it crashes the target and deals 1.5x damage.";
		}
		Events::Log(msg);
	}
}

void mv::CanHitDuringDig(bat::Battle& battle, bool moveIsSwift)
{
	bool digging = false;
	if ((battle.attackerIsPlayer && battle.r_movesUsed.front() == 258) || 
		(!battle.attackerIsPlayer && battle.p_movesUsed.front() == 258)) { 
		digging = true; 
	}
	if (digging) {
		std::string msg = "Target was digging, this move dictates it can hit during this semi-invulnerable period.";
		battle.battleEffects.push_back(std::make_unique<bfx::BypassSemiInvulnerable>(battle.attackerIsPlayer));
		if (!moveIsSwift) { 
			battle.battleEffects.push_back(std::make_unique<bfx::FlatDamageMod>(battle.attackerIsPlayer, 1, 2.0F)); 
			msg += " The move also indicates it deals 2x damage.";
		}
		Events::Log(msg);
	}
}

void mv::CanHitDuringDive(bat::Battle& battle, bool moveIsSwift)
{
	bool diving = false;
	if ((battle.attackerIsPlayer && battle.r_movesUsed.front() == 328) ||
		(!battle.attackerIsPlayer && battle.p_movesUsed.front() == 328)) {
		diving = true;
	}
	if (diving) {
		std::string msg = "Target was diving, this move dictates it can hit during this semi-invulnerable period.";
		battle.battleEffects.push_back(std::make_unique<bfx::BypassSemiInvulnerable>(battle.attackerIsPlayer));
		if (!moveIsSwift) { 
			battle.battleEffects.push_back(std::make_unique<bfx::FlatDamageMod>(battle.attackerIsPlayer, 1, 2.0F)); 
			msg += " The move also indicates it deals 2x damage.";
		}
		Events::Log(msg);
	}
}

void mv::CanHitDuringPhantomForce(bat::Battle& battle)
{ // Unlike the others, only Swift calls this function, so there's no reason to check. 
	bool morbing = false;
	if ((battle.attackerIsPlayer && battle.r_movesUsed.front() == 292) ||
		(!battle.attackerIsPlayer && battle.p_movesUsed.front() == 292)) {
		morbing = true;
	}
	if (morbing) {
		battle.battleEffects.push_back(std::make_unique<bfx::BypassSemiInvulnerable>(battle.attackerIsPlayer)); 
		std::string msg = "Target was using Phantom Force, this move dictates it can hit during this semi-invulnerable period.";
		Events::Log(msg);
	}
}

void mv::ChanceStatusEffect(bat::Battle& battle, StatusEffect status, tc::Type typeImmune, int proc) // post
{
	std::string st;
	switch (status) {
	case Burn: st = "Burn"; break;
	case Paralysis: st = "Paralysis"; break;
	case Frostbite: st = "Frostbite"; break;
	case Drowsy: st = "Drowsy"; break;
	case Poison: st = "Poison"; break;
	case BadlyPoison: st = "Toxic"; break;
	case Confusion: st = "Confusion"; break;
	}
	std::string msg;
	bool okTryProc = true;
	if (typeImmune != tc::NoType) {
		if ((battle.attackerIsPlayer && (battle.rival.GetType1() == typeImmune || battle.rival.GetType2() == typeImmune)) ||
			(!battle.attackerIsPlayer && (battle.player.GetType1() == typeImmune || battle.player.GetType2() == typeImmune))) {
			msg = "Wanted to try to proc " + st + " but the defender was " + tc::typeToString.at(typeImmune) + " type.";
			okTryProc = false;
		}
		if (status == Poison || status == BadlyPoison) { // Poison and Toxic also check if the defender is Steel-type.
			if ((battle.attackerIsPlayer && (battle.rival.GetType1() == tc::Steel || battle.rival.GetType2() == tc::Steel)) ||
				(!battle.attackerIsPlayer && (battle.player.GetType1() == tc::Steel || battle.player.GetType2() == tc::Steel))) {
				msg = "Wanted to try to proc Toxic but the defender was Steel type.";
				okTryProc = false;
			}
		}
	}
	if (okTryProc) {
		if (CheckForExistingStatus(battle.battleEffects, battle.attackerIsPlayer) || status == Confusion) { // Check for existing status effect. Confusion can coexist.
			if (AttemptProc(proc)) {
				std::string who = battle.attackerIsPlayer ? battle.rival.GetName() : battle.player.GetName();
				switch (status) {
				case Burn:
					battle.battleEffects.push_back(std::make_unique<bfx::BurnAttackReduction>(not battle.attackerIsPlayer));
					battle.battleEffects.push_back(std::make_unique<bfx::BurnDamage>(not battle.attackerIsPlayer));
					msg = who + " was burned!";
					break;
				case Paralysis:
					battle.battleEffects.push_back(std::make_unique<bfx::Paralysis>(not battle.attackerIsPlayer));
					msg = who + " was paralyzed!";
					break;
				case Frostbite:
					battle.battleEffects.push_back(std::make_unique<bfx::FrostbiteDamage>(not battle.attackerIsPlayer));
					battle.battleEffects.push_back(std::make_unique<bfx::FrostbiteSpecialAttackReduction>(not battle.attackerIsPlayer));
					msg = who + " was inflicted with frostbite!";
					break;
				case Drowsy:
					battle.battleEffects.push_back(std::make_unique<bfx::Drowsy>(not battle.attackerIsPlayer));
					battle.battleEffects.push_back(std::make_unique<bfx::DrowsyDamageBoost>(battle.attackerIsPlayer)); // Attacker gets damage boost.
					msg = who + " became drowsy!";
					break;
				case Poison:
					battle.battleEffects.push_back(std::make_unique<bfx::Poisoned>(not battle.attackerIsPlayer));
					msg = who + " was poisoned!";
					break;
				case BadlyPoison:
					battle.battleEffects.push_back(std::make_unique<bfx::BadlyPoisoned>(not battle.attackerIsPlayer));
					msg = who + " was badly poisoned!";
					break;
				case Confusion:
					std::random_device rd;
					std::mt19937_64 eng(rd());
					std::uniform_int_distribution<int> uniform_dist(2, 5);
					int dur = uniform_dist(eng);
					msg = "Generated Confusion for " + std::to_string(dur) + " turns.";
					Events::Log(msg);
					battle.battleEffects.push_back(std::make_unique<bfx::Confusion>(not battle.attackerIsPlayer, dur));
					battle.battleEffects.push_back(std::make_unique<bfx::ConfusionAttackSelf>(not battle.attackerIsPlayer, dur));
					msg = who + " became confused!";
					break;
				}
				Events::WriteToScreen(msg);
				msg = "Procced " + st + " on defender.";
			}
			else { msg = "Attempted to proc " + st + " on defender, but it failed. (STATUS OK)"; }
		}
		else { msg = "Wanted to try to proc " + st + " on defender, but they already had a status effect."; }
	}
	Events::Log(msg);
}

bool AttemptProc(int proc) {
	std::random_device rd;
	std::mt19937_64 eng(rd());
	std::uniform_int_distribution<int> uniform_dist(1, 100);
	return uniform_dist(eng) <= proc;
}

bool CheckForExistingStatus(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, bool attackerIsPlayer)
{
	for (const std::unique_ptr<bfx::BattleEffect>& b : battleEffects) {
		// Check for burn.
		if (dynamic_cast<bfx::BurnDamage*>(b.get()) != nullptr && b.get()->GetTarget() == !attackerIsPlayer) { return false; }
		// Check for frostbite.
		if (dynamic_cast<bfx::FrostbiteDamage*>(b.get()) != nullptr && b.get()->GetTarget() == !attackerIsPlayer) { return false; }
		// Check for paralysis.
		if (dynamic_cast<bfx::Paralysis*>(b.get()) != nullptr && b.get()->GetTarget() == !attackerIsPlayer) { return false; }
		// Check for poison.
		if (dynamic_cast<bfx::Poisoned*>(b.get()) != nullptr && b.get()->GetTarget() == !attackerIsPlayer) { return false; }
		// Check for badly poisoned.
		if (dynamic_cast<bfx::BadlyPoisoned*>(b.get()) != nullptr && b.get()->GetTarget() == !attackerIsPlayer) { return false; }
		// Check for drowsy.
		if (dynamic_cast<bfx::Drowsy*>(b.get()) != nullptr && b.get()->GetTarget() == !attackerIsPlayer) { return false; }
	}
	return true;
}

void DisplayStatChangeResults(int code, std::string who, std::string stat) {
	std::string msg = who + "'s " + stat;
	switch (code) {
	case 1: msg += " won't go any higher!"; break;
	case 2: msg += " won't go any lower!"; break;
	case 3: msg += " maxed out!"; break;
	case 4: msg += " bottomed out!"; break;
	case 5: msg += " fell!"; break;
	case 6: msg += " harshly fell!"; break;
	case 7: msg += " severely fell!"; break;
	case 8: msg += " rose!"; break;
	case 9: msg += " rose sharply!"; break;
	case 10: msg += " rose drastically!"; break;
	default: msg += " was modified badly. (STATUS NOT OK)"; break;
	}
	Events::WriteToScreen(msg);
}