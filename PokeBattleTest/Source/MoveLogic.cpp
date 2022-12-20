#include <random>
#include <map>
#include "MoveLogic.h"
#include "BattleEffect.h"
#include "EventListener.h"
#include "Battle.h"
#include "Pokemon.h"
#include "TypeCategory.h"
bool AttemptProc(int proc);
bool CheckForExistingStatus(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, bool attackerIsPlayer);
void DisplayStatChangeResults(int code, std::string who, std::string stat);
void RaiseFieldGuard(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, bool attackerIsPlayer, std::string msg);
void CheckForAndEraseExistingTerrain(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, std::string terrain);
void AnalyzeExistingTerrain(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, std::string terrain, bfx::BattleEffect::ChildType ct);
//
// Using battle.attackerIsPlayer as the target will resolve to the target being the attacker.
// If not battle.attackerIsPlayer is passed, the target is the defender.
//
#pragma region StatusEffects
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
			(not battle.attackerIsPlayer && (battle.player.GetType1() == typeImmune || battle.player.GetType2() == typeImmune))) {
			msg = "Wanted to try to proc " + st + " but the defender was " + tc::typeToString.at(typeImmune) + " type.";
			okTryProc = false;
		}
		if (status == Poison || status == BadlyPoison) { // Poison and Toxic also check if the defender is Steel-type.
			if ((battle.attackerIsPlayer && (battle.rival.GetType1() == tc::Steel || battle.rival.GetType2() == tc::Steel)) ||
				(not battle.attackerIsPlayer && (battle.player.GetType1() == tc::Steel || battle.player.GetType2() == tc::Steel))) {
				msg = "Wanted to try to proc Toxic but the defender was Steel type.";
				okTryProc = false;
			}
		}
	}
	if (okTryProc) {
		if (CheckForExistingStatus(battle.battleEffects, not battle.attackerIsPlayer) || status == Confusion) { // Check for existing status effect. Confusion can coexist.
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

bool CheckForExistingStatus(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, bool attackerIsPlayer)
{
	for (const std::unique_ptr<bfx::BattleEffect>& b : battleEffects) {
		// Check for burn.
		if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::ChildType::BurnStatus && 
			b.get()->GetTarget() == !attackerIsPlayer) { return false; }
		// Check for frostbite.
		if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::ChildType::FrostbiteStatus && 
			b.get()->GetTarget() == !attackerIsPlayer) { return false; }
		// Check for paralysis.
		if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::ChildType::ParalysisStatus &&
			b.get()->GetTarget() == !attackerIsPlayer) { return false; }
		// Check for poison.
		if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::ChildType::PoisonedStatus &&
			b.get()->GetTarget() == !attackerIsPlayer) { return false; }
		// Check for drowsy.
		if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::ChildType::DrowsyStatus &&
			b.get()->GetTarget() == !attackerIsPlayer) { return false; }
	}
	return true;
}
#pragma endregion

#pragma region StatMods
void mv::ModAttack(bat::Battle& battle, int proc, int stages, bool targetAttacker)
{ // Why have all these resolve to ModMultipleStats? It just feels better to me. Feels like you have a better idea of what's going on. 
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
			if (atk != 0 && b.get()->GetTypeOfEffect() == bfx::BattleEffect::ChildType::AttackMod && b.get()->GetTarget() == be_Affects) {
				results[0] = dynamic_cast<bfx::ModAttack*>(b.get())->ModStages(atk); atk = 0; continue;
			}
			if (def != 0 && b.get()->GetTypeOfEffect() == bfx::BattleEffect::ChildType::DefenseMod && b.get()->GetTarget() == be_Affects) {
				results[1] = dynamic_cast<bfx::ModDefense*>(b.get())->ModStages(def); def = 0; continue;
			}
			if (spatk != 0 && b.get()->GetTypeOfEffect() == bfx::BattleEffect::ChildType::SpecialAttackMod && b.get()->GetTarget() == be_Affects) {
				results[2] = dynamic_cast<bfx::ModSpecialAttack*>(b.get())->ModStages(spatk); spatk = 0; continue;
			}
			if (spdef != 0 && b.get()->GetTypeOfEffect() == bfx::BattleEffect::ChildType::SpecialDefenseMod && b.get()->GetTarget() == be_Affects) {
				results[3] = dynamic_cast<bfx::ModSpecialDefense*>(b.get())->ModStages(spdef); spdef = 0; continue;
			}
			if (spd != 0 && b.get()->GetTypeOfEffect() == bfx::BattleEffect::ChildType::SpeedMod && b.get()->GetTarget() == be_Affects) {
				results[4] = dynamic_cast<bfx::ModSpeed*>(b.get())->ModStages(spd); spd = 0; continue;
			}
			if (acc != 0 && b.get()->GetTypeOfEffect() == bfx::BattleEffect::ChildType::AccuracyMod && b.get()->GetTarget() == be_Affects) {
				results[5] = dynamic_cast<bfx::ModAccuracy*>(b.get())->ModStages(acc); acc = 0; continue;
			}
			if (eva != 0 && b.get()->GetTypeOfEffect() == bfx::BattleEffect::ChildType::EvasionMod && b.get()->GetTarget() == be_Affects) {
				results[6] = dynamic_cast<bfx::ModEvasion*>(b.get())->ModStages(eva); eva = 0; continue;
			}
			if (crit != 0 && b.get()->GetTypeOfEffect() == bfx::BattleEffect::ChildType::CriticalRatioMod && b.get()->GetTarget() == be_Affects) {
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
#pragma endregion

#pragma region SpecialHitChecks
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

void mv::CanHitDuringFlyFreefall(bat::Battle& battle, bool moveIsSwift) {
	CanHitDuringSemiInvulnerable(battle, 219, moveIsSwift);
	CanHitDuringSemiInvulnerable(battle, 220, moveIsSwift);
}

void mv::CanHitDuringSemiInvulnerable(bat::Battle& battle, int moveID, bool moveIsSwift) {
	int dur; // die
	bool semi_invulnerable = (battle.attackerIsPlayer && battle.r_movesUsed.front() == moveID) ||
		(not battle.attackerIsPlayer && battle.p_movesUsed.front() == moveID);
	if (semi_invulnerable) {
		std::string msg = "This log message indicates a move that can hit during a semi-invulnerable turn was used in the appropriate scenario, but the move ID was invalid.";
		switch (moveID) {
		case 219: // Fly
		case 220: // Freefall
			dur = battle.attackerIsPlayer == battle.playerGoesFirst ? 1 : 2; // XNOR resolves whether the interrupt occurs this round or next. 
			battle.battleEffects.push_back(std::make_unique<bfx::BypassSemiInvulnerable>(battle.attackerIsPlayer));
			msg = "Target was flying or falling, this move dictates it can hit during this semi-invulnerable period.";
			if (!moveIsSwift) {
				battle.battleEffects.push_back(std::make_unique<bfx::InterruptMove>(not battle.attackerIsPlayer, dur));
				battle.battleEffects.push_back(std::make_unique<bfx::FlatDamageMod>(battle.attackerIsPlayer, 1, 1.5F));
				msg += " The move also indicates it crashes the target and deals 1.5x damage.";
			}
			break;
		case 258: // Dig
			msg = "Target was digging, this move dictates it can hit during this semi-invulnerable period.";
			battle.battleEffects.push_back(std::make_unique<bfx::BypassSemiInvulnerable>(battle.attackerIsPlayer));
			if (!moveIsSwift) {
				battle.battleEffects.push_back(std::make_unique<bfx::FlatDamageMod>(battle.attackerIsPlayer, 1, 2.0F));
				msg += " The move also indicates it deals 2x damage.";
			}
			Events::Log(msg);
			break;
		case 328: // Dive
			msg = "Target was diving, this move dictates it can hit during this semi-invulnerable period.";
			battle.battleEffects.push_back(std::make_unique<bfx::BypassSemiInvulnerable>(battle.attackerIsPlayer));
			if (!moveIsSwift) {
				battle.battleEffects.push_back(std::make_unique<bfx::FlatDamageMod>(battle.attackerIsPlayer, 1, 2.0F));
				msg += " The move also indicates it deals 2x damage.";
			}
			Events::Log(msg);
			break;
		case 292: // Phantom Force
			// Only swift can hit during phantom force, hence the lack of check.
			battle.battleEffects.push_back(std::make_unique<bfx::BypassSemiInvulnerable>(battle.attackerIsPlayer));
			std::string msg = "Target was using Phantom Force, this move dictates it can hit during this semi-invulnerable period.";
			Events::Log(msg);
			break;
		}
		Events::Log(msg);
	}
}
#pragma endregion

#pragma region FieldGuards
void mv::Reflect(bat::Battle& battle)
{
	std::string msg = "Reflect raised Defense on their side!";
	RaiseFieldGuard(battle.battleEffects, battle.attackerIsPlayer, msg);
}

void mv::LightScreen(bat::Battle& battle)
{
	std::string msg = "Light Screen raised Special Defense on their side!";
	RaiseFieldGuard(battle.battleEffects, battle.attackerIsPlayer, msg);
}

void mv::AuroraVeil(bat::Battle& battle)
{
	std::string msg = "Aurora Veil raised Defense and Special Defense on their side!";
	RaiseFieldGuard(battle.battleEffects, battle.attackerIsPlayer, msg);
}

void RaiseFieldGuard(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, bool attackerIsPlayer, std::string msg) {
	bool noGuard = true;
	for (const std::unique_ptr<bfx::BattleEffect>& b : battleEffects) { // Check for existing field guards. If so, refresh duration.
		if (msg[0] == 'R' && b.get()->GetTypeOfEffect() == bfx::BattleEffect::ChildType::ReflectGuard && b.get()->GetTarget() == attackerIsPlayer) {
			noGuard = false;
			dynamic_cast<bfx::ReflectDefenseBoost*>(b.get())->RefreshDuration();
			break;
		}
		if (msg[0] == 'L' && b.get()->GetTypeOfEffect() == bfx::BattleEffect::ChildType::LightScreenGuard && b.get()->GetTarget() == attackerIsPlayer) {
			noGuard = false;
			dynamic_cast<bfx::LightScreenSpecialDefenseBoost*>(b.get())->RefreshDuration();
			break;
		}
		if (msg[0] == 'A' && b.get()->GetTypeOfEffect() == bfx::BattleEffect::ChildType::AuroraVeilGuard && b.get()->GetTarget() == attackerIsPlayer) {
			noGuard = false;
			dynamic_cast<bfx::AuroraVeilDefSpdefBoost*>(b.get())->RefreshDuration();
			break;
		}
	}
	if (noGuard) {
		switch (msg[0]) {
		case 'R': battleEffects.push_back(std::make_unique<bfx::ReflectDefenseBoost>(attackerIsPlayer)); break;
		case 'L': battleEffects.push_back(std::make_unique<bfx::LightScreenSpecialDefenseBoost>(attackerIsPlayer)); break;
		case 'A': battleEffects.push_back(std::make_unique<bfx::AuroraVeilDefSpdefBoost>(attackerIsPlayer)); break;
		}
		Events::WriteToScreen(msg);
	}
}
#pragma endregion

#pragma region Terrain
void mv::ElectricField(bat::Battle& battle)
{
	CheckForAndEraseExistingTerrain(battle.battleEffects, "Electric");
	battle.battleEffects.push_back(std::make_unique<bfx::ElectricTerrain>());
}

void mv::MistField(bat::Battle& battle)
{
	CheckForAndEraseExistingTerrain(battle.battleEffects, "Misty");
	battle.battleEffects.push_back(std::make_unique<bfx::MistyTerrain>());
}

void mv::PsychoField(bat::Battle& battle)
{
	CheckForAndEraseExistingTerrain(battle.battleEffects, "Psychic");
	battle.battleEffects.push_back(std::make_unique<bfx::PsychicTerrain>());
}

void mv::GrassField(bat::Battle& battle)
{
	CheckForAndEraseExistingTerrain(battle.battleEffects, "Grassy");
	battle.battleEffects.push_back(std::make_unique<bfx::GrassyTerrain>());
}

void CheckForAndEraseExistingTerrain(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, std::string terrain) {
	AnalyzeExistingTerrain(battleEffects, terrain, bfx::BattleEffect::ChildType::ElectricTerrain);
	AnalyzeExistingTerrain(battleEffects, terrain, bfx::BattleEffect::ChildType::MistyTerrain);
	AnalyzeExistingTerrain(battleEffects, terrain, bfx::BattleEffect::ChildType::PsychicTerrain);
	AnalyzeExistingTerrain(battleEffects, terrain, bfx::BattleEffect::ChildType::GrassyTerrain);
}

void AnalyzeExistingTerrain(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, std::string terrain, bfx::BattleEffect::ChildType ct) {
	static const std::map<std::string, bfx::BattleEffect::ChildType> terrains = {
		{ "Electric", bfx::BattleEffect::ChildType::ElectricTerrain },
		{ "Misty", bfx::BattleEffect::ChildType::MistyTerrain },
		{ "Psychic", bfx::BattleEffect::ChildType::PsychicTerrain },
		{ "Grassy", bfx::BattleEffect::ChildType::GrassyTerrain },
	};

	std::string msg = "";
	auto count = std::erase_if(battleEffects, [&](const std::unique_ptr<bfx::BattleEffect>& x)
		{ return x.get()->GetTypeOfEffect() == ct; });
	if (count != 0 && ct == terrains.at(terrain)) { msg = terrain + " Terrain's duration was extended!"; }
	if (msg != "") { Events::WriteToScreen(msg); }
}
#pragma endregion

#pragma region MiscEffects
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
	}
	else
	{
		msg = "Attempted to proc Flinch on defender, but it failed. (STATUS OK)";
	}
	Events::Log(msg);
}

void mv::DoublePowerIfStatusEffect(bat::Battle& battle)
{
	if (CheckForExistingStatus(battle.battleEffects, not battle.attackerIsPlayer)) {
		battle.battleEffects.push_back(std::make_unique<bfx::FlatDamageMod>(battle.attackerIsPlayer, 1, 2.0F));
	}
}

void mv::PassiveDamageTrap(bat::Battle& battle, std::string cause)
{
	bool noTrap = true;
	for (const std::unique_ptr<bfx::BattleEffect>& b : battle.battleEffects) { // Check for existing damage trap.
		if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::ChildType::DamageTrapOther && 
			b.get()->GetTarget() == not battle.attackerIsPlayer) { noTrap = false; break; }
	}
	if (noTrap) {
		battle.battleEffects.push_back(std::make_unique<bfx::DamageTrap>(not battle.attackerIsPlayer, cause));
		std::string who = battle.attackerIsPlayer ? battle.rival.GetName() : battle.player.GetName();
		std::string msg = who + " was trapped!";
		Events::WriteToScreen(msg);
	}
	else {
		std::string msg = "Wanted to damage trap defender but they were already trapped.";
		Events::Log(msg);
	}
}
#pragma endregion

bool AttemptProc(int proc) {
	std::random_device rd;
	std::mt19937_64 eng(rd());
	std::uniform_int_distribution<int> uniform_dist(1, 100);
	return uniform_dist(eng) <= proc;
}