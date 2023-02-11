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
bool CheckForTerrain(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, bfx::Terrain::TerrainType type);
void DisplayStatChangeResults(int code, std::string who, std::string stat);
bool RaiseOrRefreshFieldGuard(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, bfx::FieldGuard::GuardType type);
bool EraseOrRefreshTerrain(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, bfx::Terrain::TerrainType type);
bool EraseOrRefreshWeather(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, bfx::Weather::WeatherType type);
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
	std::string who = battle.attackerIsPlayer ? battle.rival.GetName() : battle.player.GetName();
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
	if (CheckForTerrain(battle.battleEffects, bfx::Terrain::MistyTerrain)) {
		msg = who + " couldn't be affected by the " + st + " because of the Misty Terrain!";
		okTryProc = false;
	}
	if (okTryProc) {
		if (CheckForExistingStatus(battle.battleEffects, not battle.attackerIsPlayer) || status == Confusion) { // Check for existing status effect. Confusion can coexist.
			if (AttemptProc(proc)) {
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
					if (CheckForTerrain(battle.battleEffects, bfx::Terrain::ElectricTerrain)) {
						msg = who + " couldn't become drowsy due to the Electric Terrain!";
					}
					else {
						battle.battleEffects.push_back(std::make_unique<bfx::Drowsy>(not battle.attackerIsPlayer));
						battle.battleEffects.push_back(std::make_unique<bfx::DrowsyDamageBoost>(battle.attackerIsPlayer)); // Attacker gets damage boost.
						msg = who + " became drowsy!";
					}
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
		if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::BurnStatus && 
			b.get()->GetTarget() == not attackerIsPlayer) { return false; }
		// Check for frostbite.
		if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::FrostbiteStatus && 
			b.get()->GetTarget() == not attackerIsPlayer) { return false; }
		// Check for paralysis.
		if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::ParalysisStatus &&
			b.get()->GetTarget() == not attackerIsPlayer) { return false; }
		// Check for poison.
		if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::PoisonedStatus &&
			b.get()->GetTarget() == not attackerIsPlayer) { return false; }
		// Check for drowsy.
		if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::DrowsyStatus &&
			b.get()->GetTarget() == not attackerIsPlayer) { return false; }
	}
	return true;
}

// For the purpose of blocking the sleep status effect. This is its own function because le transfer of control meme.
bool CheckForTerrain(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, bfx::Terrain::TerrainType type) {
	auto it = std::find_if(battleEffects.begin(), battleEffects.end(), [&type](const std::unique_ptr<bfx::BattleEffect>& x)
		{ return
		x.get()->GetTypeOfEffect() == bfx::BattleEffect::Terrain &&
		dynamic_cast<bfx::Terrain*>(x.get())->GetTerrainType() == type;
		});
	return it != battleEffects.end();
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
			if (atk != 0 && b.get()->GetTypeOfEffect() == bfx::BattleEffect::AttackMod && b.get()->GetTarget() == be_Affects) {
				results[0] = dynamic_cast<bfx::ModAttack*>(b.get())->ModStages(atk); atk = 0; continue;
			}
			if (def != 0 && b.get()->GetTypeOfEffect() == bfx::BattleEffect::DefenseMod && b.get()->GetTarget() == be_Affects) {
				results[1] = dynamic_cast<bfx::ModDefense*>(b.get())->ModStages(def); def = 0; continue;
			}
			if (spatk != 0 && b.get()->GetTypeOfEffect() == bfx::BattleEffect::SpecialAttackMod && b.get()->GetTarget() == be_Affects) {
				results[2] = dynamic_cast<bfx::ModSpecialAttack*>(b.get())->ModStages(spatk); spatk = 0; continue;
			}
			if (spdef != 0 && b.get()->GetTypeOfEffect() == bfx::BattleEffect::SpecialDefenseMod && b.get()->GetTarget() == be_Affects) {
				results[3] = dynamic_cast<bfx::ModSpecialDefense*>(b.get())->ModStages(spdef); spdef = 0; continue;
			}
			if (spd != 0 && b.get()->GetTypeOfEffect() == bfx::BattleEffect::SpeedMod && b.get()->GetTarget() == be_Affects) {
				results[4] = dynamic_cast<bfx::ModSpeed*>(b.get())->ModStages(spd); spd = 0; continue;
			}
			if (acc != 0 && b.get()->GetTypeOfEffect() == bfx::BattleEffect::AccuracyMod && b.get()->GetTarget() == be_Affects) {
				results[5] = dynamic_cast<bfx::ModAccuracy*>(b.get())->ModStages(acc); acc = 0; continue;
			}
			if (eva != 0 && b.get()->GetTypeOfEffect() == bfx::BattleEffect::EvasionMod && b.get()->GetTarget() == be_Affects) {
				results[6] = dynamic_cast<bfx::ModEvasion*>(b.get())->ModStages(eva); eva = 0; continue;
			}
			if (crit != 0 && b.get()->GetTypeOfEffect() == bfx::BattleEffect::CriticalRatioMod && b.get()->GetTarget() == be_Affects) {
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
	if (RaiseOrRefreshFieldGuard(battle.battleEffects, bfx::FieldGuard::GuardType::Reflect)) {
		std::string msg = "Reflect raised Defense on their side!";
		Events::WriteToScreen(msg);
		battle.battleEffects.push_back(std::make_unique<bfx::ReflectDefenseBoost>(battle.attackerIsPlayer));
	}
}

void mv::LightScreen(bat::Battle& battle)
{
	if (RaiseOrRefreshFieldGuard(battle.battleEffects, bfx::FieldGuard::GuardType::LightScreen)) {
		std::string msg = "Light Screen raised Special Defense on their side!";
		Events::WriteToScreen(msg);
		battle.battleEffects.push_back(std::make_unique<bfx::LightScreenSpecialDefenseBoost>(battle.attackerIsPlayer));
	}
}

void mv::AuroraVeil(bat::Battle& battle)
{ // Aurora Veil can only be used in Hail.
	auto it = std::find_if(battle.battleEffects.begin(), battle.battleEffects.end(), [](const std::unique_ptr<bfx::BattleEffect>& x)
		{ return
		x.get()->GetTypeOfEffect() == bfx::BattleEffect::Weather &&
		dynamic_cast<bfx::Weather*>(x.get())->GetWeatherType() == bfx::Weather::Hail;
		});
	if (it != battle.battleEffects.end()) {
		if (RaiseOrRefreshFieldGuard(battle.battleEffects, bfx::FieldGuard::GuardType::AuroraVeil)) {
			std::string msg = "Aurora Veil raised Defense and Special Defense on their side!";
			Events::WriteToScreen(msg);
			battle.battleEffects.push_back(std::make_unique<bfx::AuroraVeilDefSpdefBoost>(battle.attackerIsPlayer));
		}
	}
	else {
		std::string msg = "But it failed!";
		Events::WriteToScreen(msg);
	}
}

// Works the same was as the similar Terrain algorithm.
bool RaiseOrRefreshFieldGuard(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, bfx::FieldGuard::GuardType type) {
	auto it = std::find_if(battleEffects.begin(), battleEffects.end(), [&type](const std::unique_ptr<bfx::BattleEffect>& x)
		{ return
		x.get()->GetTypeOfEffect() == bfx::BattleEffect::FieldGuard &&
		dynamic_cast<bfx::FieldGuard*>(x.get())->GetGuardType() == type;
		});
	if (it != battleEffects.end()) {
		auto index = std::distance(battleEffects.begin(), it);
		dynamic_cast<bfx::FieldGuard*>(battleEffects[index].get())->RefreshDuration();
		std::map<bfx::FieldGuard::GuardType, std::string> guards = {
			{ bfx::FieldGuard::GuardType::Reflect, "Reflect" },
			{ bfx::FieldGuard::GuardType::LightScreen, "Light Screen" },
			{ bfx::FieldGuard::GuardType::AuroraVeil, "Aurora Veil" },
		};
		std::string msg = guards.at(type) + "'s duration was extended!";
		Events::WriteToScreen(msg);
		return false;
	}
	else { return true; }
}
#pragma endregion

#pragma region Terrain
void mv::ElectricField(bat::Battle& battle)
{
	if (EraseOrRefreshTerrain(battle.battleEffects, bfx::Terrain::ElectricTerrain)) {
		std::string msg = "An electric current ran across the battlefield!";
		Events::WriteToScreen(msg);
		battle.battleEffects.push_back(std::make_unique<bfx::ElectricTerrain>());
	}
}

void mv::MistField(bat::Battle& battle)
{
	if (EraseOrRefreshTerrain(battle.battleEffects, bfx::Terrain::MistyTerrain)) {
		std::string msg = "Mist swirls around the battlefield!";
		Events::WriteToScreen(msg);
		battle.battleEffects.push_back(std::make_unique<bfx::MistyTerrain>());
	}
}

void mv::PsychoField(bat::Battle& battle)
{
	if (EraseOrRefreshTerrain(battle.battleEffects, bfx::Terrain::PsychicTerrain)) {
		std::string msg = "The battlefield got weird!";
		Events::WriteToScreen(msg);
		battle.battleEffects.push_back(std::make_unique<bfx::PsychicTerrain>());
	}
}

void mv::GrassField(bat::Battle& battle)
{
	if (EraseOrRefreshTerrain(battle.battleEffects, bfx::Terrain::GrassyTerrain)) {
		std::string msg = "Grass grew to cover the battlefield!";
		Events::WriteToScreen(msg);
		battle.battleEffects.push_back(std::make_unique<bfx::GrassyTerrain>());
	}
}

// Return value indicates whether the terrain was made anew (true) or extended (false).
// If a terrain is refreshed, the operation is done so here since an iterator is already made.
bool EraseOrRefreshTerrain(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, bfx::Terrain::TerrainType type) {
	auto it = std::find_if(battleEffects.begin(), battleEffects.end(), [&type](const std::unique_ptr<bfx::BattleEffect>& x) 
		{ return 
			x.get()->GetTypeOfEffect() == bfx::BattleEffect::Terrain &&
			dynamic_cast<bfx::Terrain*>(x.get())->GetTerrainType() == type;
		});
	if (it != battleEffects.end()) {
		auto index = std::distance(battleEffects.begin(), it);
		dynamic_cast<bfx::Terrain*>(battleEffects[index].get())->RefreshDuration();
		std::map<bfx::Terrain::TerrainType, std::string> terrains = {
			{ bfx::Terrain::ElectricTerrain, "Electric" },
			{ bfx::Terrain::MistyTerrain, "Misty" },
			{ bfx::Terrain::PsychicTerrain, "Psychic" },
			{ bfx::Terrain::GrassyTerrain, "Grassy" },
		};
		std::string msg = terrains.at(type) + " Terrain's duration was extended!";
		Events::WriteToScreen(msg);
		return false;
	}
	else {
		std::erase_if(battleEffects, [](const std::unique_ptr<bfx::BattleEffect>& x) 
			{ return x.get()->GetTypeOfEffect() == bfx::BattleEffect::Terrain; });
		return true;
	}
}
#pragma endregion

#pragma region Weather
void mv::SunnyDay(bat::Battle& battle)
{
	if (EraseOrRefreshWeather(battle.battleEffects, bfx::Weather::HarshSunlight)) {
		std::string msg = "The sunlight turned harsh!";
		Events::WriteToScreen(msg);
		battle.battleEffects.push_back(std::make_unique<bfx::HarshSunlight>());
	}
}

void mv::RainDance(bat::Battle& battle)
{
	if (EraseOrRefreshWeather(battle.battleEffects, bfx::Weather::Rain)) {
		std::string msg = "It started to rain!";
		Events::WriteToScreen(msg);
		battle.battleEffects.push_back(std::make_unique<bfx::Rain>());
	}
}

void mv::Sandstorm(bat::Battle& battle)
{
	if (EraseOrRefreshWeather(battle.battleEffects, bfx::Weather::Sandstorm)) {
		std::string msg = "A sandstorm kicked up!";
		Events::WriteToScreen(msg);
		battle.battleEffects.push_back(std::make_unique<bfx::Sandstorm>());
		battle.battleEffects.push_back(std::make_unique<bfx::SandstormDamage>());
	}
}

void mv::Hail(bat::Battle& battle)
{
	if (EraseOrRefreshWeather(battle.battleEffects, bfx::Weather::Hail)) {
		std::string msg = "It started to hail!";
		Events::WriteToScreen(msg);
		battle.battleEffects.push_back(std::make_unique<bfx::Hail>());
		battle.battleEffects.push_back(std::make_unique<bfx::HailDamage>());
	}
}

void mv::Turbulence(bat::Battle& battle)
{
	if (EraseOrRefreshWeather(battle.battleEffects, bfx::Weather::StrongWinds)) {
		std::string msg = "String winds begin to blow!";
		Events::WriteToScreen(msg);
		battle.battleEffects.push_back(std::make_unique<bfx::StrongWinds>());
	}
}

void mv::Gravity(bat::Battle& battle)
{
	if (EraseOrRefreshWeather(battle.battleEffects, bfx::Weather::IntenseGravity)) {
		std::string msg = "Gravity intensified!";
		Events::WriteToScreen(msg);
		battle.battleEffects.push_back(std::make_unique<bfx::IntenseGravity>());
	}
}

bool EraseOrRefreshWeather(std::vector<std::unique_ptr<bfx::BattleEffect>>& battleEffects, bfx::Weather::WeatherType type) 
{
	auto it = std::find_if(battleEffects.begin(), battleEffects.end(), [&type](const std::unique_ptr<bfx::BattleEffect>& x)
		{ return
		x.get()->GetTypeOfEffect() == bfx::BattleEffect::Weather &&
		dynamic_cast<bfx::Weather*>(x.get())->GetWeatherType() == type;
		});
	if (it != battleEffects.end()) {
		auto index = std::distance(battleEffects.begin(), it);
		dynamic_cast<bfx::Weather*>(battleEffects[index].get())->RefreshDuration();
		std::map<bfx::Weather::WeatherType, std::string> weather = {
			{ bfx::Weather::HarshSunlight, "Harsh Sunlight was" },
			{ bfx::Weather::Rain, "Rain was" },
			{ bfx::Weather::Sandstorm, "Sandstorm was" },
			{ bfx::Weather::Hail, "Hail was" },
			{ bfx::Weather::StrongWinds, "Strong Winds were" },
			{ bfx::Weather::IntenseGravity, "Intense Gravity was" },
		};
		std::string msg = "The " + weather.at(type) + " extended!";
		Events::WriteToScreen(msg);
		return false;
	}
	else {
		std::erase_if(battleEffects, [](const std::unique_ptr<bfx::BattleEffect>& x)
			{ return x.get()->GetTypeOfEffect() == bfx::BattleEffect::Weather; });
		return true;
	}
}
#pragma endregion

#pragma region Rooms
// Extrapolate a method from these functions later.
void mv::TrickRoom(bat::Battle& battle)
{
	std::string msg;
	auto it = std::find_if(battle.battleEffects.begin(), battle.battleEffects.end(), [](const std::unique_ptr<bfx::BattleEffect>& x)
		{ return x.get()->GetTypeOfEffect() == bfx::BattleEffect::TrickRoom; });
	if (it != battle.battleEffects.end()) {
		msg = "But it was already in effect!";
	}
	else {
		std::erase_if(battle.battleEffects, [](const std::unique_ptr<bfx::BattleEffect>& x) // Overwrite Wonder Room if applicable.
			{ return x.get()->GetTypeOfEffect() == bfx::BattleEffect::WonderRoom; });
		battle.battleEffects.push_back(std::make_unique<bfx::TrickRoom>());
		msg = "The dimensions were twisted!";
	}
	Events::WriteToScreen(msg);
}

void mv::WonderRoom(bat::Battle& battle)
{
	std::string msg;
	auto it = std::find_if(battle.battleEffects.begin(), battle.battleEffects.end(), [](const std::unique_ptr<bfx::BattleEffect>& x)
		{ return x.get()->GetTypeOfEffect() == bfx::BattleEffect::WonderRoom; });
	if (it != battle.battleEffects.end()) {
		msg = "But it was already in effect!";
	}
	else {
		std::erase_if(battle.battleEffects, [](const std::unique_ptr<bfx::BattleEffect>& x) // Overwrite Trick Room if applicable.
			{ return x.get()->GetTypeOfEffect() == bfx::BattleEffect::TrickRoom; });
		battle.battleEffects.push_back(std::make_unique<bfx::TrickRoom>());
		msg = "It created a bizarre Defense and Special Defense swapping area!";
	}
	Events::WriteToScreen(msg);
}
#pragma endregion

#pragma region FieldEffects
// These aren't mutually exclusive. Pull a method out of these later.
void mv::MudSport(bat::Battle& battle)
{
	std::string msg;
	bool refreshed = false;
	for (const std::unique_ptr<bfx::BattleEffect>& b : battle.battleEffects) {
		if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::MudSport) {
			dynamic_cast<bfx::MudSport*>(b.get())->RefreshDuration();
			refreshed = true;
			msg = "Mud Sport's duration was extended!";
			break;
		}
	}
	if (not refreshed) { 
		battle.battleEffects.push_back(std::make_unique<bfx::MudSport>()); 
		msg = "Electricity's power was weakened!";
	}
	Events::WriteToScreen(msg);
}

void mv::SplashSport(bat::Battle& battle)
{
	std::string msg;
	bool refreshed = false;
	for (const std::unique_ptr<bfx::BattleEffect>& b : battle.battleEffects) {
		if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::SplashSport) {
			dynamic_cast<bfx::SplashSport*>(b.get())->RefreshDuration();
			refreshed = true;
			msg = "Splash Sport's duration was extended!";
			break;
		}
	}
	if (not refreshed) { 
		battle.battleEffects.push_back(std::make_unique<bfx::SplashSport>()); 
		msg = "Fire's power was weakened!";
	}
	Events::WriteToScreen(msg);
}

void mv::IonDeluge(bat::Battle& battle)
{
	std::string msg;
	bool refreshed = false;
	for (const std::unique_ptr<bfx::BattleEffect>& b : battle.battleEffects) {
		if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::IonDeluge) {
			dynamic_cast<bfx::IonDeluge*>(b.get())->RefreshDuration();
			refreshed = true;
			msg = "Ion Deluge's duration was extended!";
			break;
		}
	}
	if (not refreshed) { 
		battle.battleEffects.push_back(std::make_unique<bfx::IonDeluge>()); 
		msg = "A deluge of ions showers the battlefield!";
	}
	Events::WriteToScreen(msg);
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

void mv::HitTwice(bat::Battle& battle) // pre
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
	int damageDealt = battle.attackerIsPlayer ? battle.p_dmgThisRound : battle.r_dmgThisRound;
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

void mv::DoublePowerIfStatusEffect(bat::Battle& battle) // pre
{
	if (CheckForExistingStatus(battle.battleEffects, not battle.attackerIsPlayer)) {
		battle.battleEffects.push_back(std::make_unique<bfx::FlatDamageMod>(battle.attackerIsPlayer, 1, 2.0F));
	}
}

void mv::PassiveDamageTrap(bat::Battle& battle, std::string cause) // post
{
	bool noTrap = true;
	for (const std::unique_ptr<bfx::BattleEffect>& b : battle.battleEffects) { // Check for existing damage trap.
		if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::DamageTrap && 
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

#pragma region UniqueMoves
void mv::FuryCutter(bat::Battle& battle) // pre
{
	int dmg_mult = 1;
	for (int i = 0; i < 3; ++i) {
		if (battle.p_movesUsed.at(i) == 5) { ++dmg_mult; }
		else { dmg_mult = 1; break; }
	}
	battle.battleEffects.push_back(std::make_unique<bfx::FlatDamageMod>(battle.attackerIsPlayer, 1, (float)dmg_mult));
}

void mv::TwinNeedle(bat::Battle& battle)
{ // This move will use the HitTwice method in pre, and this unique TwinNeedle method in post. 
	ChancePoison(battle, 20);
	ChancePoison(battle, 20);
}

void mv::DarkestLariat(bat::Battle& battle)
{
	// NOT IMPLIMENTED
	// Reason: Relies on knowledge of other BEs.
}

void mv::Payback(bat::Battle& battle) // pre
{ // Temporary implementation of checking whether the defender has gone first, if they have more used moves in their deque, they already moved. 
	bool commit_payback;
	if (battle.attackerIsPlayer) { commit_payback = battle.r_movesUsed.size() > battle.p_movesUsed.size(); }
	else { commit_payback = battle.r_movesUsed.size() < battle.p_movesUsed.size(); }

	if (commit_payback) { battle.battleEffects.push_back(std::make_unique<bfx::FlatDamageMod>(battle.attackerIsPlayer, 1, 2.0F)); }
}

void mv::Punishment(bat::Battle& battle)
{
	// NOT IMPLIMENTED
	// Reason: Relies on knowledge of other BEs.
}

void mv::Obstruct(bat::Battle& battle) // post
{
	pkmn::Move d_move = battle.attackerIsPlayer ? battle.r_move : battle.p_move;

	if (d_move.GetCategory() != tc::Special) {
		battle.battleEffects.push_back(std::make_unique<bfx::InterruptMove>(not battle.attackerIsPlayer, 1));
		if (d_move.GetCategory() == tc::Physical) {
			battle.battleEffects.push_back(std::make_unique<bfx::ModDefense>(not battle.attackerIsPlayer, -2)); // This might display out of order. Needs testing.
		}
	}
}

void mv::Snatch(bat::Battle& battle) // pre
{
	std::string msg = "But nothing happened!";
	if (battle.p_movesUsed.size() == battle.r_movesUsed.size()) {
		pkmn::Move d_move = battle.attackerIsPlayer ? battle.r_move : battle.p_move;
		std::vector<int> valid_moves = { 19, 20, 21, 24, 44, 46, 47, 69, 87, 91, 92, 106, 108, 152, 153, 154, 155, 156, 164,
			166, 168, 172, 173, 178, 211, 212, 213, 234, 235, 247, 250, 252, 272, 286, 320, 321, 322, 323, 326, 345, 346,
			369, 370, 373, 378, 391, 429, 432, 434, 437, 439, 442, 446, 447, 449, 450, 452, 457, 458, 464, 465, 466, 468,
			470, 471, 476, 479, 487, 488, 492 };
		if (std::find(valid_moves.begin(), valid_moves.end(), d_move.GetID()) != valid_moves.end()) {
			battle.battleEffects.push_back(std::make_unique<bfx::InterruptMove>(not battle.attackerIsPlayer, 1));
			battle.battleEffects.push_back(std::make_unique<bfx::MoveOverride>(battle.attackerIsPlayer, d_move.GetID()));
			msg = battle.attackerIsPlayer ? battle.player.GetName() : battle.rival.GetName();
			msg += " stole " + d_move.GetName() + "!";
		}
	}
	Events::WriteToScreen(msg);
}

void mv::Taunt(bat::Battle& battle)
{
	// NOT IMPLEMENTED
	// Reason: Relies on move restrictions, which also isn't implemented. 
}

void mv::TopsyTurvy(bat::Battle& battle) //post
{
	int stages = 0;
	for (const std::unique_ptr<bfx::BattleEffect>& b : battle.battleEffects) {
		if (b.get()->GetTarget() == not battle.attackerIsPlayer) {
			switch (b.get()->GetTypeOfEffect()) {
			case bfx::BattleEffect::AttackMod:
				stages = dynamic_cast<bfx::ModAttack*>(b.get())->GetStages() * -1;
				dynamic_cast<bfx::ModAttack*>(b.get())->ModStages(stages);
				break;
			case bfx::BattleEffect::DefenseMod:
				stages = dynamic_cast<bfx::ModDefense*>(b.get())->GetStages() * -1;
				dynamic_cast<bfx::ModDefense*>(b.get())->ModStages(stages);
				break;
			case bfx::BattleEffect::SpecialAttackMod:
				stages = dynamic_cast<bfx::ModSpecialAttack*>(b.get())->GetStages() * -1;
				dynamic_cast<bfx::ModSpecialAttack*>(b.get())->ModStages(stages);
				break;
			case bfx::BattleEffect::SpecialDefenseMod:
				stages = dynamic_cast<bfx::ModSpecialDefense*>(b.get())->GetStages() * -1;
				dynamic_cast<bfx::ModSpecialDefense*>(b.get())->ModStages(stages);
				break;
			case bfx::BattleEffect::SpeedMod:
				stages = dynamic_cast<bfx::ModSpeed*>(b.get())->GetStages() * -1;
				dynamic_cast<bfx::ModSpeed*>(b.get())->ModStages(stages);
				break;
			case bfx::BattleEffect::AccuracyMod:
				stages = dynamic_cast<bfx::ModAccuracy*>(b.get())->GetStages() * -1;
				dynamic_cast<bfx::ModAccuracy*>(b.get())->ModStages(stages);
				break;
			case bfx::BattleEffect::EvasionMod:
				stages = dynamic_cast<bfx::ModEvasion*>(b.get())->GetStages() * -1;
				dynamic_cast<bfx::ModEvasion*>(b.get())->ModStages(stages);
				break;
			case bfx::BattleEffect::CriticalRatioMod:
				stages = dynamic_cast<bfx::ModCriticalRatio*>(b.get())->GetStages() * -1;
				dynamic_cast<bfx::ModCriticalRatio*>(b.get())->ModStages(stages);
				break;
			}
		}
	}

	std::string who = battle.attackerIsPlayer ? battle.rival.GetName() : battle.player.GetName();
	std::string msg = who + " had their stat changes reversed!";
	Events::WriteToScreen(msg);
}

void mv::Torment(bat::Battle& battle)
{
	// NOT IMPLEMENTED
	// Reason: Relies on move restrictions, which also isn't implemented. 
}

void mv::BoltBeak(bat::Battle& battle) // pre
{
	if ((battle.attackerIsPlayer && battle.playerGoesFirst) ||
		not battle.attackerIsPlayer && not battle.playerGoesFirst) {
		battle.battleEffects.push_back(std::make_unique<bfx::FlatDamageMod>(battle.attackerIsPlayer, 1, 2.0F));
	}
}

void mv::SparkFang(bat::Battle& battle) // post
{
	mv::ChanceParalyze(battle, 20);
	mv::ChanceFlinch(battle, 10);
}

void mv::VoltTackle(bat::Battle& battle) // post
{
	bool temp = battle.attackerIsPlayer;
	int count = std::erase_if(battle.battleEffects, [&temp](const std::unique_ptr<bfx::BattleEffect>& x)
		{ return (x.get()->GetTypeOfEffect() == bfx::BattleEffect::DrowsyStatus) && (x.get()->GetTarget() == temp); });
	if (count != 0) {
		std::string msg = battle.attackerIsPlayer ? battle.player.GetName() : battle.rival.GetName();
		msg += " was cured of their Drowsiness!";
		Events::WriteToScreen(msg);
	}

	mv::ChanceParalyze(battle, 30);
	battle.battleEffects.push_back(std::make_unique<bfx::Recoil>(battle.attackerIsPlayer, 33));
}

void mv::Charge(bat::Battle& battle) // post
{
	mv::ModSpecialDefense(battle, 100, 1, true);
	//
	// NEED SPECIAL BE FOR CHARGE
	//
}

void mv::MagnetRise(bat::Battle& battle)
{

}

void mv::NaturesMadness(bat::Battle& battle) // post
{
	int dmgVal = battle.attackerIsPlayer ? battle.rival.GetCurrentHP() : battle.player.GetCurrentHP();
	dmgVal /= 2;
	if (dmgVal == 0) { dmgVal = 1; }
	battle.battleEffects.push_back(std::make_unique<bfx::ExactDamageOverride>(battle.attackerIsPlayer, dmgVal));
}

void mv::CraftyShield(bat::Battle& battle) // pre
{
	pkmn::Move d_move = battle.attackerIsPlayer ? battle.r_move : battle.p_move;
	if (d_move.GetCategory() == tc::Status) {
		battle.battleEffects.push_back(std::make_unique<bfx::InterruptMove>(not battle.attackerIsPlayer, 1));
	}
}

void mv::FireFang(bat::Battle& battle) // post
{
	mv::ChanceBurn(battle, 20);
	mv::ChanceFlinch(battle, 10);
}

void mv::FlareWheel(bat::Battle& battle) // post
{
	
	bool temp = battle.attackerIsPlayer;
	int count = std::erase_if(battle.battleEffects, [&temp](const std::unique_ptr<bfx::BattleEffect>& x)
		{ return (x.get()->GetTypeOfEffect() == bfx::BattleEffect::FrostbiteStatus) && (x.get()->GetTarget() == temp); });
	if (count != 0) {
		std::string msg = battle.attackerIsPlayer ? battle.player.GetName() : battle.rival.GetName();
		msg += " was cured of their Frostbite!";
		Events::WriteToScreen(msg);
	}

	mv::ChanceBurn(battle, 20);
	battle.battleEffects.push_back(std::make_unique<bfx::Recoil>(battle.attackerIsPlayer, 33));
}

void mv::InfernoDrive(bat::Battle& battle) // post
{
	bool temp = battle.attackerIsPlayer;
	int count = std::erase_if(battle.battleEffects, [&temp](const std::unique_ptr<bfx::BattleEffect>& x)
		{ return (x.get()->GetTypeOfEffect() == bfx::BattleEffect::FrostbiteStatus) && (x.get()->GetTarget() == temp); });
	if (count != 0) {
		std::string msg = battle.attackerIsPlayer ? battle.player.GetName() : battle.rival.GetName();
		msg += " was cured of their Frostbite!";
		Events::WriteToScreen(msg);
	}

	mv::ChanceBurn(battle, 10);
	battle.battleEffects.push_back(std::make_unique<bfx::Recoil>(battle.attackerIsPlayer, 33));
}

void mv::VCreate(bat::Battle& battle) // post
{
	mv::ModDefense(battle, 100, -1, true);
	mv::ModSpecialDefense(battle, 100, -1, true);
	mv::ModSpeed(battle, 100, -1, true);
}

void mv::BurnUp(bat::Battle& battle) // post
{
	bool temp = battle.attackerIsPlayer;
	int count = std::erase_if(battle.battleEffects, [&temp](const std::unique_ptr<bfx::BattleEffect>& x)
		{ return (x.get()->GetTypeOfEffect() == bfx::BattleEffect::FrostbiteStatus) && (x.get()->GetTarget() == temp); });
	if (count != 0) {
		std::string msg = battle.attackerIsPlayer ? battle.player.GetName() : battle.rival.GetName();
		msg += " was cured of their Frostbite!";
		Events::WriteToScreen(msg);
	}

	battle.battleEffects.push_back(std::make_unique<bfx::ShedType>(battle.attackerIsPlayer, tc::Fire, -1));
}

void mv::BurningJealosy(bat::Battle& battle) // post
{
	bool commitBurn = false;
	for (const std::unique_ptr<bfx::BattleEffect>& b : battle.battleEffects) {
		if (dynamic_cast<bfx::ModStat*>(b.get()) != nullptr &&
			dynamic_cast<bfx::ModStat*>(b.get())->GetStages() > 0) {
			commitBurn = true;
			break;
		}
	}
	if (commitBurn) { mv::ChanceBurn(battle, 100); }
}

void mv::PsychicFang(bat::Battle& battle) // post
{
	bool targ = not battle.attackerIsPlayer;
	int res = std::erase_if(battle.battleEffects, [&targ](const std::unique_ptr<bfx::BattleEffect>& x)
		{ return x.get()->GetTypeOfEffect() == bfx::BattleEffect::FieldGuard && 
		x.get()->GetTarget() == targ; });
	if (res != 0) {
		std::string msg = battle.attackerIsPlayer ? battle.rival.GetName() : battle.player.GetName();
		msg += " had their field guards broken!";
		Events::WriteToScreen(msg);
	}
}

void mv::StoredPower(bat::Battle& battle)
{
	//
	// bepis lol
	// I don't feel like writing this right now
	// just don't forget it
	//
}

void mv::Extrasensory(bat::Battle& battle) // post
{
	mv::ModSpeed(battle, 100, -1, false);
	mv::ChanceFlinch(battle, 10);
}

void mv::MirrorCoat(bat::Battle& battle)
{
	//
	// Damage tracking function too flawed to implement this. 
	//
}

void mv::MysticalPower(bat::Battle& battle) // post
{
	if (battle.attackerIsPlayer) {
		if (battle.player.GetBaseSpAtk() > battle.player.GetBaseSpDef()) { mv::ModSpecialAttack(battle, 100, 1, true); }
		else { mv::ModSpecialDefense(battle, 100, 1, true); }
	}
	else {
		if (battle.rival.GetBaseSpAtk() > battle.rival.GetBaseSpDef()) { mv::ModSpecialAttack(battle, 100, 1, false); }
		else { mv::ModSpecialDefense(battle, 100, 1, false); }
	}
}

void mv::Psyshock(bat::Battle& battle) // pre
{
	int override = battle.attackerIsPlayer ? battle.rival.GetBaseDef() : battle.player.GetBaseDef();
	battle.battleEffects.push_back(
		std::make_unique<bfx::SpecialDefenseOverride>(not battle.attackerIsPlayer, 1, override)
	);
}

void mv::GuardSplit(bat::Battle& battle)
{
	//
	// ree
	//
}

void mv::GuardSwap(bat::Battle& battle)
{
	//
	// bengis
	// 
}

void mv::HealBlock(bat::Battle& battle)
{
	// NOT IMPLEMENTED
	// Reason: Relies on knowledge of other BEs.
}

void mv::MagicCoat(bat::Battle& battle)
{
	// get list of valid magic coat status moves, can't be fucked rn
}

void mv::Meditate(bat::Battle& battle)
{
	mv::ModAttack(battle, 100, 2, true); // why is this its own method??
}

void mv::PowerSplit(bat::Battle& battle)
{
	// stop with these moves
}

void mv::PowerSwap(bat::Battle& battle)
{
	// KYS IRL DELETE THESE FUCKING MOVES
}

void mv::Rest(bat::Battle& battle)
{
	mv::InstantHealSelf(battle, 75);
	std::string msg = battle.attackerIsPlayer ? battle.player.GetName() : battle.rival.GetName();
	if (CheckForTerrain(battle.battleEffects, bfx::Terrain::ElectricTerrain)) {
		msg += " couldn't become drowsy due to the Electric Terrain!";
	}
	else {
		battle.battleEffects.push_back(std::make_unique<bfx::Drowsy>(battle.attackerIsPlayer));
		battle.battleEffects.push_back(std::make_unique<bfx::DrowsyDamageBoost>(not battle.attackerIsPlayer));
		msg += " became drowsy!";
	}
}

void mv::SoulSwap(bat::Battle& battle)
{
	int atk, def, spatk, spdef, spd, acc, eva, crit;
	atk = def = spatk = spdef = spd = acc = eva = crit = 0;
	// finish this
}

void mv::SpeedSwap(bat::Battle& battle)
{
}

void mv::StatTrick(bat::Battle& battle)
{
}

void mv::StatusShift(bat::Battle& battle)
{
}

void mv::Telekenesis(bat::Battle& battle)
{
}

void mv::WakeUpSlap(bat::Battle& battle)
{
}

void mv::BodyPress(bat::Battle& battle)
{
}

void mv::BrickBreak(bat::Battle& battle)
{
}

void mv::Counter(bat::Battle& battle)
{
}

void mv::FocusPunch(bat::Battle& battle)
{
}

void mv::HighJumpKick(bat::Battle& battle)
{
}

void mv::SacredSword(bat::Battle& battle)
{
}

void mv::StormThrow(bat::Battle& battle)
{
}

void mv::MysterySword(bat::Battle& battle)
{
}

void mv::Acrobatics(bat::Battle& battle)
{
}

void mv::BraveBird(bat::Battle& battle)
{
}

void mv::Fly(bat::Battle& battle)
{
}

void mv::Freefall(bat::Battle& battle)
{
}

void mv::SkyAttack(bat::Battle& battle)
{
}

void mv::Defog(bat::Battle& battle)
{
}

void mv::MirrorMove(bat::Battle& battle)
{
}

void mv::Roost(bat::Battle& battle)
{
}

void mv::Tailwind(bat::Battle& battle)
{
}

void mv::BarbBarrage(bat::Battle& battle)
{
}

void mv::DireClaw(bat::Battle& battle)
{
}

void mv::VenomFang(bat::Battle& battle)
{
}

void mv::BanefulThread(bat::Battle& battle)
{
}

void mv::Pillbox(bat::Battle& battle)
{
}

void mv::Toxic(bat::Battle& battle)
{
}

void mv::VenomTrap(bat::Battle& battle)
{
}

void mv::Dig(bat::Battle& battle)
{
}

void mv::StompingTantrum(bat::Battle& battle)
{
}

void mv::SandsearStorm(bat::Battle& battle)
{
}

void mv::Rototiller(bat::Battle& battle)
{
}

void mv::SandAttack(bat::Battle& battle)
{
}

void mv::SoilGather(bat::Battle& battle)
{
}

void mv::HeadSmash(bat::Battle& battle)
{
}

void mv::Rollout(bat::Battle& battle)
{
}

void mv::SmackDown(bat::Battle& battle)
{
}

void mv::MeteorBeam(bat::Battle& battle)
{
}

void mv::Astonish(bat::Battle& battle)
{
}

void mv::Lick(bat::Battle& battle)
{
}

void mv::PhantomForce(bat::Battle& battle)
{
}

void mv::SpectralThief(bat::Battle& battle)
{
}

void mv::BitterMalice(bat::Battle& battle)
{
}

void mv::InfernalParade(bat::Battle& battle)
{
}

void mv::DoublePanzer(bat::Battle& battle)
{
}

void mv::MirrorBurst(bat::Battle& battle)
{
}

void mv::SteelRoller(bat::Battle& battle)
{
}

void mv::AssistGear(bat::Battle& battle)
{
}

void mv::Dive(bat::Battle& battle)
{
}

void mv::Brine(bat::Battle& battle)
{
}

void mv::Scald(bat::Battle& battle)
{
}

void mv::SparklingAria(bat::Battle& battle)
{
}

void mv::AquaRing(bat::Battle& battle)
{
}

void mv::Soak(bat::Battle& battle)
{
}

void mv::GForce(bat::Battle& battle)
{
}

void mv::GrassyGlide(bat::Battle& battle)
{
}

void mv::SedativeFang(bat::Battle& battle)
{
}

void mv::Chloroblast(bat::Battle& battle)
{
}

void mv::Aromatherapy(bat::Battle& battle)
{
}

void mv::Ingrain(bat::Battle& battle)
{
}

void mv::LeechSeed(bat::Battle& battle)
{
}

void mv::Synthesis(bat::Battle& battle)
{
}

void mv::Avalanche(bat::Battle& battle)
{
}

void mv::IceBall(bat::Battle& battle)
{
}

void mv::IceFang(bat::Battle& battle)
{
}

void mv::FreezeDry(bat::Battle& battle)
{
}

void mv::FrostBreath(bat::Battle& battle)
{
}

void mv::Mist(bat::Battle& battle)
{
}

void mv::Haze(bat::Battle& battle)
{
}

void mv::BodySlam(bat::Battle& battle)
{
}

void mv::Bravado(bat::Battle& battle)
{
}

void mv::Cut(bat::Battle& battle)
{
}

void mv::DoubleEdge(bat::Battle& battle)
{
}

void mv::Endeavor(bat::Battle& battle)
{
}

void mv::FalseSwipe(bat::Battle& battle)
{
}

void mv::RapidSpin(bat::Battle& battle)
{
}

void mv::SmellingSalts(bat::Battle& battle)
{
}

void mv::Strength(bat::Battle& battle)
{
}

void mv::Struggle(bat::Battle& battle)
{
}

void mv::SuperFang(bat::Battle& battle)
{
}

void mv::RevelationDance(bat::Battle& battle)
{
}

void mv::Swift(bat::Battle& battle)
{
}

void mv::TriAttack(bat::Battle& battle)
{
}

void mv::WeatherBall(bat::Battle& battle)
{
}

void mv::Acupressure(bat::Battle& battle)
{
}

void mv::BellyDrum(bat::Battle& battle)
{
}

void mv::Conversion(bat::Battle& battle)
{
}

void mv::CourtChange(bat::Battle& battle)
{
}

void mv::Disable(bat::Battle& battle)
{
}

void mv::Encore(bat::Battle& battle)
{
}

void mv::Endure(bat::Battle& battle)
{
}

void mv::Glare(bat::Battle& battle)
{

}

void mv::HealBell(bat::Battle& battle)
{
}

void mv::LaserFocus(bat::Battle& battle)
{
}

void mv::LockOn(bat::Battle& battle)
{
}

void mv::LuckyChant(bat::Battle& battle)
{
}

void mv::MeFirst(bat::Battle& battle)
{
}

void mv::Metronome(bat::Battle& battle)
{
}

void mv::NaturePower(bat::Battle& battle)
{
}

void mv::OdorSleuth(bat::Battle& battle)
{
}

void mv::PainSplit(bat::Battle& battle)
{
}

void mv::PowerShift(bat::Battle& battle)
{
}

void mv::Protect(bat::Battle& battle)
{
}

void mv::PsychUp(bat::Battle& battle)
{
}

void mv::Regeneration(bat::Battle& battle)
{
}

void mv::Safeguard(bat::Battle& battle)
{
}

void mv::ShellSmash(bat::Battle& battle)
{
}

void mv::Substitute(bat::Battle& battle)
{
}

void mv::SweetScent(bat::Battle& battle)
{
}

void mv::Wish(bat::Battle& battle)
{
}

void mv::DreamEater(bat::Battle& battle)
{
}

void mv::Corrode(bat::Battle& battle)
{
}

void mv::AncientPower(bat::Battle& battle)
{
}

void mv::OminousWind(bat::Battle& battle)
{
}

void mv::DisarmingVoice(bat::Battle& battle)
{
}

void mv::LightningStrike(bat::Battle& battle)
{
}

void mv::Blizzard(bat::Battle& battle)
{
}
#pragma endregion

bool AttemptProc(int proc) {
	std::random_device rd;
	std::mt19937_64 eng(rd());
	std::uniform_int_distribution<int> uniform_dist(1, 100);
	return uniform_dist(eng) <= proc;
}