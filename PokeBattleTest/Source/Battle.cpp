#include <random>
#include "Battle.h"
#include "TypeCategory.h"
#include "EventListener.h"
float StageMultiplier(int stageVal);

bat::Battle::Battle(mon::Pokemon& _player, mon::Pokemon& _rival) :
	player(_player), rival(_rival), p_move(0), r_move(0)
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
	post_moveFuncs.emplace(6, std::bind(&mv::HealOnDamage, std::placeholders::_1, 50)); // Leech Life
	post_moveFuncs.emplace(81, std::bind(&mv::HealOnDamage, std::placeholders::_1, 50)); // Parabolic Charge
	post_moveFuncs.emplace(98, std::bind(&mv::HealOnDamage, std::placeholders::_1, 75)); // Draining Kiss
	post_moveFuncs.emplace(192, std::bind(&mv::HealOnDamage, std::placeholders::_1, 50)); // Drain Punch
	post_moveFuncs.emplace(230, std::bind(&mv::HealOnDamage, std::placeholders::_1, 50)); // Oblivion Wing
	post_moveFuncs.emplace(354, std::bind(&mv::HealOnDamage, std::placeholders::_1, 50)); // Horn Leech
	post_moveFuncs.emplace(364, std::bind(&mv::HealOnDamage, std::placeholders::_1, 50)); // Giga Drain
	post_moveFuncs.emplace(9, std::bind(&mv::ChanceFlinch, std::placeholders::_1, 30)); // Steamroller
	post_moveFuncs.emplace(25, std::bind(&mv::ChanceFlinch, std::placeholders::_1, 30)); // Bite
	post_moveFuncs.emplace(36, std::bind(&mv::ChanceFlinch, std::placeholders::_1, 20)); // Dark Pulse
	post_moveFuncs.emplace(57, std::bind(&mv::ChanceFlinch, std::placeholders::_1, 20)); // Mythic Rush
	post_moveFuncs.emplace(67, std::bind(&mv::ChanceFlinch, std::placeholders::_1, 20)); // Twister
	post_moveFuncs.emplace(97, std::bind(&mv::ChanceFlinch, std::placeholders::_1, 10)); // Disarming Voice
	post_moveFuncs.emplace(137, std::bind(&mv::ChanceFlinch, std::placeholders::_1, 30)); // Heart Stamp
	post_moveFuncs.emplace(141, std::bind(&mv::ChanceFlinch, std::placeholders::_1, 20)); // Zen Headbutt
	post_moveFuncs.emplace(226, std::bind(&mv::ChanceFlinch, std::placeholders::_1, 20)); // Air Slash
	post_moveFuncs.emplace(277, std::bind(&mv::ChanceFlinch, std::placeholders::_1, 30)); // Rock Slide
	post_moveFuncs.emplace(310, std::bind(&mv::ChanceFlinch, std::placeholders::_1, 30)); // Iron Head
	post_moveFuncs.emplace(334, std::bind(&mv::ChanceFlinch, std::placeholders::_1, 20)); // Waterfall
	post_moveFuncs.emplace(359, std::bind(&mv::ChanceFlinch, std::placeholders::_1, 10)); // Seed Bomb
	post_moveFuncs.emplace(384, std::bind(&mv::ChanceFlinch, std::placeholders::_1, 30)); // Icicle Crash
	post_moveFuncs.emplace(410, std::bind(&mv::ChanceFlinch, std::placeholders::_1, 30)); // Headbutt
	post_moveFuncs.emplace(411, std::bind(&mv::ChanceFlinch, std::placeholders::_1, 30)); // Hyper Fang
	post_moveFuncs.emplace(112, std::bind(&mv::ChanceBurn, std::placeholders::_1, 10)); // Blaze Kick
	post_moveFuncs.emplace(114, std::bind(&mv::ChanceBurn, std::placeholders::_1, 10)); // Fire Punch
	post_moveFuncs.emplace(126, std::bind(&mv::ChanceBurn, std::placeholders::_1, 10)); // Ember
	post_moveFuncs.emplace(128, std::bind(&mv::ChanceBurn, std::placeholders::_1, 20)); // Flamethrower
	post_moveFuncs.emplace(130, std::bind(&mv::ChanceBurn, std::placeholders::_1, 30)); // Heatwave
	post_moveFuncs.emplace(135, std::bind(&mv::ChanceBurn, std::placeholders::_1, 100)); // Will-O-Wisp
	post_moveFuncs.emplace(387, std::bind(&mv::ChanceBurn, std::placeholders::_1, 30)); // Cold Flare
	post_moveFuncs.emplace(63, std::bind(&mv::ChanceParalyze, std::placeholders::_1, 30)); // Dragon Breath
	post_moveFuncs.emplace(71, std::bind(&mv::ChanceParalyze, std::placeholders::_1, 20)); // Bolt Strike
	post_moveFuncs.emplace(72, std::bind(&mv::ChanceParalyze, std::placeholders::_1, 30)); // Jolt
	post_moveFuncs.emplace(73, std::bind(&mv::ChanceParalyze, std::placeholders::_1, 100)); // Nuzzle
	post_moveFuncs.emplace(75, std::bind(&mv::ChanceParalyze, std::placeholders::_1, 20)); // Spark Punch
	post_moveFuncs.emplace(77, std::bind(&mv::ChanceParalyze, std::placeholders::_1, 30)); // Discharge
	post_moveFuncs.emplace(79, std::bind(&mv::ChanceParalyze, std::placeholders::_1, 30)); // Lightning Strike
	post_moveFuncs.emplace(84, std::bind(&mv::ChanceParalyze, std::placeholders::_1, 10)); // Thunderbolt
	post_moveFuncs.emplace(86, std::bind(&mv::ChanceParalyze, std::placeholders::_1, 100)); // Zap Cannon
	post_moveFuncs.emplace(93, std::bind(&mv::ChanceParalyze, std::placeholders::_1, 100)); // Stun Wave
	post_moveFuncs.emplace(195, std::bind(&mv::ChanceParalyze, std::placeholders::_1, 30)); // Force Palm
	post_moveFuncs.emplace(377, std::bind(&mv::ChanceParalyze, std::placeholders::_1, 100)); // Stun Spore
	post_moveFuncs.emplace(163, std::bind(&mv::ChanceDrowsy, std::placeholders::_1, 100)); // Hypnosis
	post_moveFuncs.emplace(375, std::bind(&mv::ChanceDrowsy, std::placeholders::_1, 100)); // Sleep Powder
	post_moveFuncs.emplace(423, std::bind(&mv::ChanceDrowsy, std::placeholders::_1, 20)); // Relic Song
	post_moveFuncs.emplace(227, std::bind(&mv::ChanceFrostbite, std::placeholders::_1, 30)); // Bleakwind Storm
	post_moveFuncs.emplace(380, std::bind(&mv::ChanceFrostbite, std::placeholders::_1, 20)); // Freeze Punch
	post_moveFuncs.emplace(386, std::bind(&mv::ChanceFrostbite, std::placeholders::_1, 20)); // Blizzard
	post_moveFuncs.emplace(390, std::bind(&mv::ChanceFrostbite, std::placeholders::_1, 20)); // Ice beam
	post_moveFuncs.emplace(237, std::bind(&mv::ChancePoison, std::placeholders::_1, 30)); // Barbed Strike
	post_moveFuncs.emplace(238, std::bind(&mv::ChancePoison, std::placeholders::_1, 10)); // Cross Poison
	post_moveFuncs.emplace(240, std::bind(&mv::ChancePoison, std::placeholders::_1, 30)); // Gunk Shot
	post_moveFuncs.emplace(244, std::bind(&mv::ChancePoison, std::placeholders::_1, 40)); // Mire
	post_moveFuncs.emplace(245, std::bind(&mv::ChancePoison, std::placeholders::_1, 30)); // Sludge Bomb
	post_moveFuncs.emplace(246, std::bind(&mv::ChancePoison, std::placeholders::_1, 10)); // Sludge Wave
	post_moveFuncs.emplace(253, std::bind(&mv::ChancePoison, std::placeholders::_1, 100)); // Poison Gas
} 

void bat::Battle::Round(mon::Move& playerMove, mon::Move& rivalMove) // Battle::Round() and Turn::Act() cover all the logic for a single round of Battle.
{
	p_move = playerMove;
	r_move = rivalMove;
	p_dmgThisTurn = r_dmgThisTurn = 0;
	// Upkeep phase, remove all expired BattleEffects
	std::erase_if(battleEffects, [](const std::unique_ptr<bfx::BattleEffect>& x) { return x.get()->GetDuration() == 0; });

	bool playerGoesFirst;
	// Determine turn order.
	if (playerMove.GetPriority() == rivalMove.GetPriority()) {
		// Calculate speed
		for (const std::unique_ptr<bfx::BattleEffect>& b : battleEffects) {
			// Check for paralysis.
			if (dynamic_cast<bfx::Paralysis*>(b.get()) != nullptr && b.get()->GetTarget() == true) { p_speedMult *= 0.5F; }
			if (dynamic_cast<bfx::Paralysis*>(b.get()) != nullptr && b.get()->GetTarget() == false) { r_speedMult *= 0.5F; }
			// Check for speed mods.
			if (dynamic_cast<bfx::ModSpeed*>(b.get()) != nullptr && b.get()->GetTarget() == true)
			{ p_speedMult *= StageMultiplier(dynamic_cast<bfx::ModSpeed*>(b.get())->GetStages()); }
			if (dynamic_cast<bfx::ModSpeed*>(b.get()) != nullptr && b.get()->GetTarget() == false)
			{ r_speedMult *= StageMultiplier(dynamic_cast<bfx::ModSpeed*>(b.get())->GetStages()); }
		}

		int p_speed = std::floor(player.GetFinalSpd() * p_speedMult);
		int r_speed = std::floor(player.GetFinalSpd() * p_speedMult);

		if (p_speed == r_speed) { // Speed tie is randomly broken. 
			std::random_device rd;
			std::mt19937_64 eng(rd());
			std::uniform_int_distribution<int> uniform_dist(1, 2);
			playerGoesFirst = uniform_dist(eng) == 1;
		}
		else { playerGoesFirst = p_speed > r_speed; }
	}
	else { playerGoesFirst = playerMove.GetPriority() > rivalMove.GetPriority(); }

	Turn p_turn(player, rival, p_move, true);
	Turn r_turn(rival, player, r_move, false);
	auto* first = playerGoesFirst ? &p_turn : &r_turn;
	auto* second = playerGoesFirst ? &r_turn :  &p_turn;
	if (!first->Act(*this) || !second->Act(*this)) { victorDeclared = true; return; }
	
	// Post move phase, decrement duration of all BattleEffects
	for (const std::unique_ptr<bfx::BattleEffect>& b : battleEffects) { b.get()->DecrementDuration(); }
}


bool bat::Turn::Act(Battle& battle)
{
	battle.attackerIsPlayer = attackerIsPlayer; // Clunky. Both battle and turn need to track this, but Turn can't access battle outside of this method.

	// Pre-move phase. First checks for a pre-move function, and runs it, then Executes all battleeffects.
	if (battle.pre_moveFuncs.count(move.GetID())) { battle.pre_moveFuncs[move.GetID()](battle); }

	for (const std::unique_ptr<bfx::BattleEffect>& b : battle.battleEffects) {
		if (b.get()->GetTurnPhase() == bfx::BattleEffect::BeforeMoveExecuted) { b.get()->Execute(*this); }
	}

	// Action phase
	if (!interruptTurn) { 
		bool autoMiss = false;
		// Check for grounded if applicable
		// Check here if move is noneffective.
		
		interruptTurn = autoMiss;
	}

	if (!interruptTurn) { // True means something like Paralysis or Drowsy skipped the Action phase.
		

		if (move.GetIsProcMutable() && !moveHits) { // moveHits starts out at false, if a battleEffect sets it to true prematurely, skip accuracy check. 
			int netAccuracy = a_accuracyStages - d_evasionStages;
			int accModified = move.GetAccuracy();
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
			std::string msg = attacker.GetName() + "'s attack missed!";
			Events::WriteToScreen(msg);
		}
	}
	
	// Post Action phase. Check for function code if applicable and battle effects.
	if (moveHits && battle.post_moveFuncs.count(move.GetID())) { battle.post_moveFuncs[move.GetID()](battle); }

	for (const std::unique_ptr<bfx::BattleEffect>& b : battle.battleEffects) {
		if (b.get()->GetTurnPhase() == bfx::BattleEffect::AfterMoveExecuted &&
			b.get()->GetTarget() == attackerIsPlayer) { b.get()->Execute(*this); }
	}

	// Check for death. True means no death and continue.
	return attacker.GetCurrentHP() > 0 && defender.GetCurrentHP() > 0;
}

float StageMultiplier(int stageVal) {
	if (stageVal >= 0) { return (2.0F + stageVal) / 2.0F; }
	else { return 2.0F / (2.0F - stageVal); }
}
