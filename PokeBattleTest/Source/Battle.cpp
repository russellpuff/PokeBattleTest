#include <random>
#include "Battle.h"
#include "Pokemon.h"
#include "TypeCategory.h"
#include "BattleEffect.h"
float StageMultiplier(int stageVal);

void bat::Battle::Round(mon::Move& playerMove, mon::Move& rivalMove) // Battle::Round() and Turn::Act() cover all the logic for a single round of Battle.
{
	p_move = playerMove;
	r_move = rivalMove;
	// Upkeep phase, remove all expired BattleEffects
	std::erase_if(battleEffects, [](bfx::BattleEffect* x) { return x->GetDuration() == 0; });

	bool playerGoesFirst;
	// Determine turn order.
	if (playerMove.GetPriority() == rivalMove.GetPriority()) {
		// Calculate speed
		for (bfx::BattleEffect* b : battleEffects) { 
			// Check for paralysis.
			if (dynamic_cast<bfx::Paralysis*>(b) != nullptr && b->GetTarget() == true) { p_speedMult *= 0.5F; }
			if (dynamic_cast<bfx::Paralysis*>(b) != nullptr && b->GetTarget() == false) { r_speedMult *= 0.5F; }
			// Check for speed mods
			if (dynamic_cast<bfx::ModSpeed*>(b) != nullptr && b->GetTarget() == true) 
			{ p_speedMult *= StageMultiplier(dynamic_cast<bfx::ModSpeed*>(b)->GetStages()); }
			if (dynamic_cast<bfx::ModSpeed*>(b) != nullptr && b->GetTarget() == false) 
			{ r_speedMult *= StageMultiplier(dynamic_cast<bfx::ModSpeed*>(b)->GetStages()); }
		}

		int p_speed = std::floor(player.GetCurrentSpd() * p_speedMult);
		int r_speed = std::floor(player.GetCurrentSpd() * p_speedMult);

		playerGoesFirst = p_speed > r_speed;
	}
	else { playerGoesFirst = playerMove.GetPriority() > rivalMove.GetPriority(); }

	Turn p_turn(p_move, true);
	Turn r_turn(r_move, false);
	if (playerGoesFirst) { p_turn.Act(*this); r_turn.Act(*this); }
	else { r_turn.Act(*this); p_turn.Act(*this); }

	// Post move phase, decrement duration of all BattleEffects
	for (bfx::BattleEffect* b : battleEffects) { if (b->GetDuration() != -1) { --b; } }
}

void bat::Turn::Act(Battle& battle)
{
	//
	// 
	// 
	// Figure out how to note things being grounded. 
	// 
	// 
	// 
	// Pre-move phase. Modifiers get loaded and any other shmancy effects are executed here. 
	for (bfx::BattleEffect* b : battle.battleEffects) {
		if (b->GetTurnPhase() == bfx::BattleEffect::BeforeMoveExecuted) { b->Execute(*this); }
	}
	if (interruptTurn) { return; } // This means something like Paralysis or Drowsy skipped the Action phase.
	
	// Action phase, this is divided into two parts, the first determines whether the move hits, the second determines result.
	if (move.GetIsProcMutable() || !moveHits) { // moveHits starts out at false, if a battleEffect sets it to true prematurely, skip accuracy check. 
		int netAccuracy = a_accuracyStages - d_evasionStages;
		int accModified = attackerIsPlayer ? battle.p_move.GetAccuracy() : battle.r_move.GetAccuracy();
		if (netAccuracy >= 0) { accModified = std::floor(a_accMod * ((3 + netAccuracy)) / 3); }
		else { accModified = std::floor((a_accMod * 3) / (3 - netAccuracy)); }

		std::random_device rd;
		std::mt19937_64 eng(rd());
		std::uniform_int_distribution<int> uniform_dist(1, 100);
		moveHits = uniform_dist(eng) <= accModified;
	}
	else { moveHits = true; } // Non proc mutable moves are surehit and bypass accuracy checks. 

	if (moveHits) {

	}
	else {
		std::string msg = attackerIsPlayer ? battle.player.GetName() : battle.rival.GetName();
		msg += "'s attack missed!";
		WriteToScreen(msg);
	}
}

float StageMultiplier(int stageVal) {
	if (stageVal >= 0) { return (2.0F + stageVal) / 2.0F; }
	else { return 2.0F / (2.0F - stageVal); }
}
