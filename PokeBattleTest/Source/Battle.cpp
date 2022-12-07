#include <random>
#include "Battle.h"
#include "Pokemon.h"
#include "TypeCategory.h"
#include "BattleEffect.h"
#include "EventListener.h"
float StageMultiplier(int stageVal);

bat::Battle::Battle(mon::Pokemon& _player, mon::Pokemon& _rival) :
	player(_player), rival(_rival), p_move(0), r_move(0),
	playerCurrentHP(_player.GetCurrentHP()), rivalCurrentHP(_rival.GetCurrentHP())
{
	pre_moveFuncs.emplace(1, std::bind(&mv::IncreasedCriticalOneStage, std::placeholders::_1)); // Attack Order
	pre_moveFuncs.emplace(12, std::bind(&mv::IncreasedCriticalOneStage, std::placeholders::_1)); // X-Scissor
	pre_moveFuncs.emplace(30, std::bind(&mv::IncreasedCriticalOneStage, std::placeholders::_1)); // Night Slash
	pre_moveFuncs.emplace(61, std::bind(&mv::IncreasedCriticalOneStage, std::placeholders::_1)); // Talon Strike
	pre_moveFuncs.emplace(224, std::bind(&mv::IncreasedCriticalOneStage, std::placeholders::_1)); // Aeroblast
	pre_moveFuncs.emplace(225, std::bind(&mv::IncreasedCriticalOneStage, std::placeholders::_1)); // Air Cutter
	pre_moveFuncs.emplace(259, std::bind(&mv::IncreasedCriticalOneStage, std::placeholders::_1)); // Drill Run
	pre_moveFuncs.emplace(355, std::bind(&mv::IncreasedCriticalOneStage, std::placeholders::_1)); // Leaf Blade
	pre_moveFuncs.emplace(112, std::bind(&mv::IncreasedCriticalOneStage, std::placeholders::_1)); // Blaze Kick
	pre_moveFuncs.emplace(139, std::bind(&mv::IncreasedCriticalOneStage, std::placeholders::_1)); // Psycho Cut
	pre_moveFuncs.emplace(144, std::bind(&mv::IncreasedCriticalOneStage, std::placeholders::_1)); // Esper Wing
	pre_moveFuncs.emplace(238, std::bind(&mv::IncreasedCriticalOneStage, std::placeholders::_1)); // Cross-Poison
	pre_moveFuncs.emplace(279, std::bind(&mv::IncreasedCriticalOneStage, std::placeholders::_1)); // Rupture
	pre_moveFuncs.emplace(294, std::bind(&mv::IncreasedCriticalOneStage, std::placeholders::_1)); // Shadow Claw

	post_moveFuncs.emplace(8, std::bind(&mv::Hit2to5Times, std::placeholders::_1)); // Pin Missile
	post_moveFuncs.emplace(38, std::bind(&mv::Hit2to5Times, std::placeholders::_1)); // Harass
	post_moveFuncs.emplace(59, std::bind(&mv::Hit2to5Times, std::placeholders::_1)); // Scale Shot
	post_moveFuncs.emplace(184, std::bind(&mv::Hit2to5Times, std::placeholders::_1)); // Arm Thrust
	post_moveFuncs.emplace(282, std::bind(&mv::Hit2to5Times, std::placeholders::_1)); // Stone Blast
	post_moveFuncs.emplace(333, std::bind(&mv::Hit2to5Times, std::placeholders::_1)); // Water Shuriken
	post_moveFuncs.emplace(350, std::bind(&mv::Hit2to5Times, std::placeholders::_1)); // Bullet Seed
	post_moveFuncs.emplace(409, std::bind(&mv::Hit2to5Times, std::placeholders::_1)); // Fury Attack
	post_moveFuncs.emplace(56, std::bind(&mv::HitTwice, std::placeholders::_1)); // Dual Chop
	post_moveFuncs.emplace(191, std::bind(&mv::HitTwice, std::placeholders::_1)); // Double Kick
	post_moveFuncs.emplace(218, std::bind(&mv::HitTwice, std::placeholders::_1)); // Dual Wingbeat
	post_moveFuncs.emplace(402, std::bind(&mv::HitTwice, std::placeholders::_1)); // Double Hit
	post_moveFuncs.emplace(20, std::bind(&mv::InstantHealSelf, std::placeholders::_1, 50)); // Heal Order
	post_moveFuncs.emplace(108, std::bind(&mv::InstantHealSelf, std::placeholders::_1, 50)); // Floral Healing
	post_moveFuncs.emplace(346, std::bind(&mv::InstantHealSelf, std::placeholders::_1, 25)); // Life Dew
	post_moveFuncs.emplace(457, std::bind(&mv::InstantHealSelf, std::placeholders::_1, 50)); // Milk Drink
}

void bat::Battle::Round(mon::Move& playerMove, mon::Move& rivalMove) // Battle::Round() and Turn::Act() cover all the logic for a single round of Battle.
{
	p_move = playerMove;
	r_move = rivalMove;
	p_dmgThisTurn = r_dmgThisTurn = 0;
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
			// Check for speed mods.
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
	battle.attackerIsPlayer = attackerIsPlayer; // Clunky. Both battle and turn need to track this, but Turn can't access battle outside of this method.
	//
	// 
	// 
	// Figure out how to note things being grounded. 
	// 
	// 
	// 
	// Pre-move phase. Modifiers get loaded and any other shmancy effects are executed here. 
	//
	//
	// BEFORE THIS RUNS, ALL PRE-MOVE BATTLEEFFECTS NEED TO TO BE LOADED
	// Requires a library of "move functions" that simply prime the battleeffect
	// Need to differentiate between pre-move effects and postmove effects...?
	//
	for (bfx::BattleEffect* b : battle.battleEffects) {
		if (b->GetTurnPhase() == bfx::BattleEffect::BeforeMoveExecuted) { b->Execute(*this); }
	}
	if (interruptTurn) { return; } // This means something like Paralysis or Drowsy skipped the Action phase.
	
	// Action phase, this is divided into two parts, the first determines whether the move hits, the second determines result.
	if (move.GetIsProcMutable() && !moveHits) { // moveHits starts out at false, if a battleEffect sets it to true prematurely, skip accuracy check. 
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
		//
		//
		// check for status move, don't bother doing damage calc if so
		//
		//
		//
	}
	else {
		std::string msg = attackerIsPlayer ? battle.player.GetName() : battle.rival.GetName();
		msg += "'s attack missed!";
		battle.WriteToScreen(msg);
	}
}

void bat::Battle::WriteToScreen(std::string& message)
{
	Events::Log(message);
}

float StageMultiplier(int stageVal) {
	if (stageVal >= 0) { return (2.0F + stageVal) / 2.0F; }
	else { return 2.0F / (2.0F - stageVal); }
}
