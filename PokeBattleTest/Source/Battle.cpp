#include <random>
#include "Battle.h"
#include "TypeCategory.h"
#include "EventListener.h"
float StageMultiplier(int stageVal);

void bat::Battle::Round(pkmn::Move& playerMove, pkmn::Move& rivalMove) // Battle::Round() and Turn::Act() cover all the logic for a single round of Battle.
{
	bool trickRoomActive = false;
	p_move = playerMove;
	r_move = rivalMove;
	p_dmgThisRound = r_dmgThisRound = 0;
	// Upkeep phase, remove all expired BattleEffects
	std::erase_if(battleEffects, [](const std::unique_ptr<bfx::BattleEffect>& x) { return x.get()->GetDuration() == 0; });

	// Determine turn order.
	if (playerMove.GetPriority() == rivalMove.GetPriority()) {
		// Calculate speed
		for (const std::unique_ptr<bfx::BattleEffect>& b : battleEffects) {
			// Check for paralysis.
			if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::ParalysisStatus) {
				if (b.get()->GetTarget()) { p_speedMult *= 0.5F; } else { r_speedMult *= 0.5F; }
			}
			// Check for speed mods.
			if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::SpeedMod) {
				if (b.get()->GetTarget()) { p_speedMult *= StageMultiplier(dynamic_cast<bfx::ModSpeed*>(b.get())->GetStages()); }
				else { r_speedMult *= StageMultiplier(dynamic_cast<bfx::ModSpeed*>(b.get())->GetStages()); }
			}
			// Check for trick room.
			if (b.get()->GetTypeOfEffect() == bfx::BattleEffect::TrickRoom) { trickRoomActive = true; }
		}

		int p_speed = std::floor(player.GetFinalSpd() * p_speedMult);
		int r_speed = std::floor(player.GetFinalSpd() * p_speedMult);

		if (p_speed == r_speed) { // Speed tie is randomly broken. 
			std::random_device rd;
			std::mt19937_64 eng(rd());
			std::uniform_int_distribution<int> uniform_dist(1, 2);
			playerGoesFirst = uniform_dist(eng) == 1;
		}
		else { 
			if (trickRoomActive) { playerGoesFirst = r_speed > p_speed; }
			else { playerGoesFirst = p_speed > r_speed; }
		}
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
	if (not interruptTurn) { 
		bool autoMiss = false;
		// Check for grounded if applicable
		// Check here if move is noneffective.
		
		interruptTurn = autoMiss;
	}

	if (not interruptTurn) { // True means something like Paralysis or Drowsy skipped the Action phase.
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
		if (b.get()->GetTurnPhase() == bfx::BattleEffect::AfterMoveExecuted) { b.get()->Execute(*this); }
	}

	// Check for death. True means no death and continue.
	return attacker.GetCurrentHP() > 0 && defender.GetCurrentHP() > 0;
}

bat::Turn::Turn(pkmn::Pokemon& _attacker, pkmn::Pokemon& _defender, pkmn::Move _move, bool _atkr) :
	attacker(_attacker), defender(_defender), move(_move), attackerIsPlayer(_atkr)
{
	// Discern whether the attacker/defender is grounded or not. 
	a_grounded = _attacker.GetType1() == tc::Flying || _attacker.GetType2() == tc::Flying;
	d_grounded = _defender.GetType1() == tc::Flying || _defender.GetType2() == tc::Flying;
	//
	// Look for grounded related battle effects. 
	//
}

float StageMultiplier(int stageVal) {
	if (stageVal >= 0) { return roundf(((2.0F + stageVal) / 2.0F) * 1000.0F) / 1000.0F; }
	else { return roundf((2.0F / (2.0F - stageVal)) * 1000.0F) / 1000.0F; }
}

bat::Battle::Battle(pkmn::Pokemon& _player, pkmn::Pokemon& _rival) :
	player(_player), rival(_rival), p_move(0), r_move(0)
{
	p_movesUsed.push_front(0);
	r_movesUsed.push_front(0);

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
	pre_moveFuncs.emplace(9, std::bind(&mv::CheckTargetMinimized, std::placeholders::_1)); // Steam Roller
	pre_moveFuncs.emplace(57, std::bind(&mv::CheckTargetMinimized, std::placeholders::_1)); // Mythic Rush
	pre_moveFuncs.emplace(28, std::bind(&mv::CheckTargetMinimized, std::placeholders::_1)); // Feint Attack
	pre_moveFuncs.emplace(82, std::bind(&mv::CheckTargetMinimized, std::placeholders::_1)); // Shock Wave
	pre_moveFuncs.emplace(209, std::bind(&mv::CheckTargetMinimized, std::placeholders::_1)); // Aura Sphere
	pre_moveFuncs.emplace(215, std::bind(&mv::CheckTargetMinimized, std::placeholders::_1)); // Aerial Ace
	pre_moveFuncs.emplace(297, std::bind(&mv::CheckTargetMinimized, std::placeholders::_1)); // Umbral Punch
	pre_moveFuncs.emplace(311, std::bind(&mv::CheckTargetMinimized, std::placeholders::_1)); // Magnet Bomb
	pre_moveFuncs.emplace(365, std::bind(&mv::CheckTargetMinimized, std::placeholders::_1)); // Magical Leaf
	pre_moveFuncs.emplace(67, std::bind(&mv::CanHitDuringFlyFreefall, std::placeholders::_1, false)); // Twister
	pre_moveFuncs.emplace(202, std::bind(&mv::CanHitDuringFlyFreefall, std::placeholders::_1, false)); // Sky Uppercut
	pre_moveFuncs.emplace(228, std::bind(&mv::CanHitDuringFlyFreefall, std::placeholders::_1, false)); // Gust
	pre_moveFuncs.emplace(229, std::bind(&mv::CanHitDuringFlyFreefall, std::placeholders::_1, false)); // Hurricane
	pre_moveFuncs.emplace(260, std::bind(&mv::CanHitDuringSemiInvulnerable, std::placeholders::_1, 258, false)); // Earthquake
	pre_moveFuncs.emplace(343, std::bind(&mv::CanHitDuringSemiInvulnerable, std::placeholders::_1, 328, false)); // Surf
	pre_moveFuncs.emplace(299, std::bind(&mv::DoublePowerIfStatusEffect, std::placeholders::_1)); // Hex
	pre_moveFuncs.emplace(56, std::bind(&mv::HitTwice, std::placeholders::_1)); // Dual Chop
	pre_moveFuncs.emplace(191, std::bind(&mv::HitTwice, std::placeholders::_1)); // Double Kick
	pre_moveFuncs.emplace(218, std::bind(&mv::HitTwice, std::placeholders::_1)); // Dual Wingbeat
	pre_moveFuncs.emplace(402, std::bind(&mv::HitTwice, std::placeholders::_1)); // Double Hit
	pre_moveFuncs.emplace(10, std::bind(&mv::HitTwice, std::placeholders::_1)); // Twin Needle
	pre_moveFuncs.emplace(31, std::bind(&mv::Payback, std::placeholders::_1));

	post_moveFuncs.emplace(8, std::bind(&mv::Hit2to5Times, std::placeholders::_1)); // Pin Missile
	post_moveFuncs.emplace(38, std::bind(&mv::Hit2to5Times, std::placeholders::_1)); // Harass
	post_moveFuncs.emplace(59, std::bind(&mv::Hit2to5Times, std::placeholders::_1)); // Scale Shot
	post_moveFuncs.emplace(184, std::bind(&mv::Hit2to5Times, std::placeholders::_1)); // Arm Thrust
	post_moveFuncs.emplace(282, std::bind(&mv::Hit2to5Times, std::placeholders::_1)); // Stone Blast
	post_moveFuncs.emplace(333, std::bind(&mv::Hit2to5Times, std::placeholders::_1)); // Water Shuriken
	post_moveFuncs.emplace(350, std::bind(&mv::Hit2to5Times, std::placeholders::_1)); // Bullet Seed
	post_moveFuncs.emplace(409, std::bind(&mv::Hit2to5Times, std::placeholders::_1)); // Fury Attack
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
	post_moveFuncs.emplace(390, std::bind(&mv::ChanceFrostbite, std::placeholders::_1, 20)); // Ice beam
	post_moveFuncs.emplace(237, std::bind(&mv::ChancePoison, std::placeholders::_1, 30)); // Barbed Strike
	post_moveFuncs.emplace(238, std::bind(&mv::ChancePoison, std::placeholders::_1, 10)); // Cross Poison
	post_moveFuncs.emplace(240, std::bind(&mv::ChancePoison, std::placeholders::_1, 30)); // Gunk Shot
	post_moveFuncs.emplace(244, std::bind(&mv::ChancePoison, std::placeholders::_1, 40)); // Mire
	post_moveFuncs.emplace(245, std::bind(&mv::ChancePoison, std::placeholders::_1, 30)); // Sludge Bomb
	post_moveFuncs.emplace(246, std::bind(&mv::ChancePoison, std::placeholders::_1, 10)); // Sludge Wave
	post_moveFuncs.emplace(253, std::bind(&mv::ChancePoison, std::placeholders::_1, 100)); // Poison Gas
	post_moveFuncs.emplace(53, std::bind(&mv::ModAttack, std::placeholders::_1, 100, -1, false)); // Breaking Swipe
	post_moveFuncs.emplace(94, std::bind(&mv::ModAttack, std::placeholders::_1, 10, -1, false)); // Play Rough
	post_moveFuncs.emplace(105, std::bind(&mv::ModAttack, std::placeholders::_1, 100, -2, false)); // Charm
	post_moveFuncs.emplace(200, std::bind(&mv::ModAttack, std::placeholders::_1, 100, 1, true)); // Power-up Punch
	post_moveFuncs.emplace(232, std::bind(&mv::ModAttack, std::placeholders::_1, 100, -2, false)); // Feather Dance
	post_moveFuncs.emplace(312, std::bind(&mv::ModAttack, std::placeholders::_1, 10, 1, true)); // Metal Claw
	post_moveFuncs.emplace(313, std::bind(&mv::ModAttack, std::placeholders::_1, 20, 1, true)); // Meteor Mash
	post_moveFuncs.emplace(385, std::bind(&mv::ModAttack, std::placeholders::_1, 20, -1, false)); // Aurora Beam
	post_moveFuncs.emplace(445, std::bind(&mv::ModAttack, std::placeholders::_1, 100, -2, false)); // Growl
	post_moveFuncs.emplace(26, std::bind(&mv::ModDefense, std::placeholders::_1, 20, -1, false)); // Crunch
	post_moveFuncs.emplace(115, std::bind(&mv::ModDefense, std::placeholders::_1, 100, -1, false)); // Flame Whip
	post_moveFuncs.emplace(154, std::bind(&mv::ModDefense, std::placeholders::_1, 100, 2, true)); // Barrier
	post_moveFuncs.emplace(247, std::bind(&mv::ModDefense, std::placeholders::_1, 100, 2, true)); // Acid Armor
	post_moveFuncs.emplace(275, std::bind(&mv::ModDefense, std::placeholders::_1, 50, 2, true)); // Diamond Storm
	post_moveFuncs.emplace(316, std::bind(&mv::ModDefense, std::placeholders::_1, 10, 1, true)); // Steel Wing
	post_moveFuncs.emplace(317, std::bind(&mv::ModDefense, std::placeholders::_1, 30, -1, false)); // Tail Slam
	post_moveFuncs.emplace(323, std::bind(&mv::ModDefense, std::placeholders::_1, 100, 2, true)); // Iron Wall
	post_moveFuncs.emplace(329, std::bind(&mv::ModDefense, std::placeholders::_1, 20, -1, false)); // Liquidation
	post_moveFuncs.emplace(331, std::bind(&mv::ModDefense, std::placeholders::_1, 50, -1, false)); // Razor Shell
	post_moveFuncs.emplace(370, std::bind(&mv::ModDefense, std::placeholders::_1, 100, 3, true)); // Cotton Guard
	post_moveFuncs.emplace(399, std::bind(&mv::ModDefense, std::placeholders::_1, 50, -1, false)); // Crush Claw
	post_moveFuncs.emplace(446, std::bind(&mv::ModDefense, std::placeholders::_1, 100, 2, true)); // Harden
	post_moveFuncs.emplace(451, std::bind(&mv::ModDefense, std::placeholders::_1, 100, -1, false)); // Leer
	post_moveFuncs.emplace(486, std::bind(&mv::ModDefense, std::placeholders::_1, 100, -2, false)); // Unpleasant Tone
	post_moveFuncs.emplace(24, std::bind(&mv::ModSpecialAttack, std::placeholders::_1, 100, 2, true)); // Tail Glow
	post_moveFuncs.emplace(46, std::bind(&mv::ModSpecialAttack, std::placeholders::_1, 100, 2, true)); // Nasty Plot
	post_moveFuncs.emplace(54, std::bind(&mv::ModSpecialAttack, std::placeholders::_1, 100, -2, true)); // Draco Meteor
	post_moveFuncs.emplace(88, std::bind(&mv::ModSpecialAttack, std::placeholders::_1, 100, -2, false)); // Eerie Impulse
	post_moveFuncs.emplace(95, std::bind(&mv::ModSpecialAttack, std::placeholders::_1, 100, -1, false)); // Spirit Break
	post_moveFuncs.emplace(99, std::bind(&mv::ModSpecialAttack, std::placeholders::_1, 30, -1, false)); // Moonblast
	post_moveFuncs.emplace(104, std::bind(&mv::ModSpecialAttack, std::placeholders::_1, 100, -2, false)); // Captivate
	post_moveFuncs.emplace(127, std::bind(&mv::ModSpecialAttack, std::placeholders::_1, 50, 1, true)); // Fiery Dance
	post_moveFuncs.emplace(361, std::bind(&mv::ModSpecialAttack, std::placeholders::_1, 100, -2, true)); // Autumn Storm
	post_moveFuncs.emplace(481, std::bind(&mv::ModSpecialAttack, std::placeholders::_1, 100, -2, false)); // Tearful Look
	post_moveFuncs.emplace(13, std::bind(&mv::ModSpecialDefense, std::placeholders::_1, 20, -2, false)); // Bug Buzz
	post_moveFuncs.emplace(43, std::bind(&mv::ModSpecialDefense, std::placeholders::_1, 100, -2, false)); // Fake Tears
	post_moveFuncs.emplace(150, std::bind(&mv::ModSpecialDefense, std::placeholders::_1, 10, -1, false)); // Psychic
	post_moveFuncs.emplace(153, std::bind(&mv::ModSpecialDefense, std::placeholders::_1, 100, 2, true)); // Amnesia
	post_moveFuncs.emplace(207, std::bind(&mv::ModSpecialDefense, std::placeholders::_1, 10, -1, false)); // Focus Blast
	post_moveFuncs.emplace(242, std::bind(&mv::ModSpecialDefense, std::placeholders::_1, 100, -2, false)); // Acid Bomb
	post_moveFuncs.emplace(266, std::bind(&mv::ModSpecialDefense, std::placeholders::_1, 100, -1, false)); // Earth Power
	post_moveFuncs.emplace(302, std::bind(&mv::ModSpecialDefense, std::placeholders::_1, 20, -1, false)); // Shadow Ball
	post_moveFuncs.emplace(318, std::bind(&mv::ModSpecialDefense, std::placeholders::_1, 10, -1, false)); // Flash Cannon
	post_moveFuncs.emplace(325, std::bind(&mv::ModSpecialDefense, std::placeholders::_1, 100, -2, false)); // Screeching Sound
	post_moveFuncs.emplace(363, std::bind(&mv::ModSpecialDefense, std::placeholders::_1, 10, -1, false)); // Energy Ball
	post_moveFuncs.emplace(366, std::bind(&mv::ModSpecialDefense, std::placeholders::_1, 100, -1, false)); // Malic Acid
	post_moveFuncs.emplace(480, std::bind(&mv::ModSpecialDefense, std::placeholders::_1, 100, -1, false)); // Tail Whip
	post_moveFuncs.emplace(23, std::bind(&mv::ModSpeed, std::placeholders::_1, 100, -2, false)); // String Shot
	post_moveFuncs.emplace(78, std::bind(&mv::ModSpeed, std::placeholders::_1, 100, -1, false)); // Electroweb
	post_moveFuncs.emplace(180, std::bind(&mv::ModSpeed, std::placeholders::_1, 100, 1, true)); // Nitro Charge
	post_moveFuncs.emplace(144, std::bind(&mv::ModSpeed, std::placeholders::_1, 100, 1, true)); // Esper Wing
	post_moveFuncs.emplace(152, std::bind(&mv::ModSpeed, std::placeholders::_1, 100, 2, true)); // Agility
	post_moveFuncs.emplace(196, std::bind(&mv::ModSpeed, std::placeholders::_1, 100, -1, true)); // Hammer Arm
	post_moveFuncs.emplace(198, std::bind(&mv::ModSpeed, std::placeholders::_1, 100, -1, false)); // Low Sweep
	post_moveFuncs.emplace(257, std::bind(&mv::ModSpeed, std::placeholders::_1, 100, -1, false)); // Bulldoze
	post_moveFuncs.emplace(274, std::bind(&mv::ModSpeed, std::placeholders::_1, 100, -1, false)); // Boulder Tomb
	post_moveFuncs.emplace(286, std::bind(&mv::ModSpeed, std::placeholders::_1, 100, 2, true)); // Rock Polish
	post_moveFuncs.emplace(306, std::bind(&mv::ModSpeed, std::placeholders::_1, 100, -2, false)); // Scary Face
	post_moveFuncs.emplace(321, std::bind(&mv::ModSpeed, std::placeholders::_1, 100, 2, true)); // Automize
	post_moveFuncs.emplace(336, std::bind(&mv::ModSpeed, std::placeholders::_1, 20, -1, false)); // Bubble Beam
	post_moveFuncs.emplace(376, std::bind(&mv::ModSpeed, std::placeholders::_1, 100, -2, false)); // Soft Spore
	post_moveFuncs.emplace(39, std::bind(&mv::ModAccuracy, std::placeholders::_1, 40, -1, false)); // Night Daze
	post_moveFuncs.emplace(267, std::bind(&mv::ModAccuracy, std::placeholders::_1, 30, -1, false)); // Mud Bomb
	post_moveFuncs.emplace(319, std::bind(&mv::ModAccuracy, std::placeholders::_1, 30, -1, false)); // Reflecting Shot
	post_moveFuncs.emplace(441, std::bind(&mv::ModAccuracy, std::placeholders::_1, 100, -1, false)); // Flash
	post_moveFuncs.emplace(437, std::bind(&mv::ModEvasion, std::placeholders::_1, 100, 1, true)); // Double Team
	post_moveFuncs.emplace(458, std::bind(&mv::ModEvasion, std::placeholders::_1, 100, 2, true)); // Minimize
	post_moveFuncs.emplace(205, std::bind(&mv::ModCritical, std::placeholders::_1, 100, 2, true)); // Triple Arrows
	post_moveFuncs.emplace(442, std::bind(&mv::ModCritical, std::placeholders::_1, 100, 2, true)); // Focus Energy
	post_moveFuncs.emplace(18, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 0, 0, -1, -1, 0, 0, 0, 0, false)); // Struggle Bug
	post_moveFuncs.emplace(19, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 0, 1, 0, 1, 0, 0, 0, 0, true)); // Defend Order
	post_moveFuncs.emplace(21, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 0, 0, 1, 1, 1, 0, 0, 0, true)); // Quiver Dance
	post_moveFuncs.emplace(40, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, -1, 0, -1, 0, 0, 0, 0, 0, false)); // Snarl
	post_moveFuncs.emplace(44, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 1, 0, 0, 0, 0, 1, 0, 0, true)); // Hone Claws
	post_moveFuncs.emplace(69, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 1, 0, 0, 0, 1, 0, 0, 0, true)); // Dragon Dance
	post_moveFuncs.emplace(92, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 0, 1, 0, 1, 0, 0, 0, 0, true)); // Magnetic Flux
	post_moveFuncs.emplace(103, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, -1, 0, -1, 0, 0, 0, 0, 0, false)); // Baby Doll Eyes
	post_moveFuncs.emplace(133, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, -1, 0, -1, 0, 0, 0, 0, 0, false)); // Mystical Flame
	post_moveFuncs.emplace(136, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 0, 1, 0, 1, 0, 0, 0, 0, true)); // Barrier Rush
	post_moveFuncs.emplace(155, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 0, 0, 1, 1, 0, 0, 0, 0, true)); // Calm Mind
	post_moveFuncs.emplace(156, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 0, 1, 0, 1, 0, 0, 0, 0, true)); // Cosmic Power
	post_moveFuncs.emplace(189, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 0, -1, 0, -1, 0, 0, 0, 0, true)); // Close Combat
	post_moveFuncs.emplace(211, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 1, 1, 0, 1, 0, 0, 0, 0, true)); // Bulk Up
	post_moveFuncs.emplace(212, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 1, 1, 1, 1, 1, 0, 0, 0, true)); // No Retreat
	post_moveFuncs.emplace(213, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 2, 1, 0, 0, 0, 0, 0, 0, true)); // Victory Dance
	post_moveFuncs.emplace(250, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 1, 1, 0, 0, 0, 1, 0, 0, true)); // Coil
	post_moveFuncs.emplace(261, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 0, -1, 0, -1, 0, 0, 0, 0, true)); // Headlong Rush
	post_moveFuncs.emplace(322, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 0, 1, 0, 0, 0, 0, 1, 0, true)); // Barricade
	post_moveFuncs.emplace(326, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 1, 0, 0, 0, 2, 0, 0, 0, true)); // Shift Gear 
	post_moveFuncs.emplace(433, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, -1, 0, -1, 0, 0, 0, 0, 0, false)); // Confide
	post_moveFuncs.emplace(449, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 1, 0, 1, 0, 0, 0, 0, 0, true)); // Howl
	post_moveFuncs.emplace(460, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, -2, 0, -2, 0, 0, 0, 0, 0, false)); // Noble Roar
	post_moveFuncs.emplace(479, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 2, 0, 2, 0, 0, 0, 0, 0, true)); // Swords Dance
	post_moveFuncs.emplace(484, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, -1, -1, 0, 0, 0, 0, 0, 0, false)); // Tickle
	post_moveFuncs.emplace(488, std::bind(&mv::ModMultipleStats, std::placeholders::_1, 100, 1, 0, 0, 0, 1, 0, 0, 0, true)); // Work Up
	post_moveFuncs.emplace(15, std::bind(&mv::PassiveDamageTrap, std::placeholders::_1, "Infestation"));
	post_moveFuncs.emplace(83, std::bind(&mv::PassiveDamageTrap, std::placeholders::_1, "Thunder Cage"));
	post_moveFuncs.emplace(129, std::bind(&mv::PassiveDamageTrap, std::placeholders::_1, "Heat Swirl"));
	post_moveFuncs.emplace(264, std::bind(&mv::PassiveDamageTrap, std::placeholders::_1, "Sand Tomb"));
	post_moveFuncs.emplace(344, std::bind(&mv::PassiveDamageTrap, std::placeholders::_1, "Whirlpool"));
	post_moveFuncs.emplace(395, std::bind(&mv::PassiveDamageTrap, std::placeholders::_1, "Bind"));
	post_moveFuncs.emplace(172, std::bind(&mv::Reflect, std::placeholders::_1));
	post_moveFuncs.emplace(164, std::bind(&mv::LightScreen, std::placeholders::_1));
	post_moveFuncs.emplace(492, std::bind(&mv::AuroraVeil, std::placeholders::_1));
	post_moveFuncs.emplace(89, std::bind(&mv::ElectricField, std::placeholders::_1));
	post_moveFuncs.emplace(110, std::bind(&mv::MistField, std::placeholders::_1));
	post_moveFuncs.emplace(171, std::bind(&mv::PsychoField, std::placeholders::_1));
	post_moveFuncs.emplace(372, std::bind(&mv::GrassField, std::placeholders::_1));
	post_moveFuncs.emplace(134, std::bind(&mv::SunnyDay, std::placeholders::_1));
	post_moveFuncs.emplace(347, std::bind(&mv::RainDance, std::placeholders::_1));
	post_moveFuncs.emplace(287, std::bind(&mv::Sandstorm, std::placeholders::_1));
	post_moveFuncs.emplace(393, std::bind(&mv::Hail, std::placeholders::_1));
	post_moveFuncs.emplace(489, std::bind(&mv::Turbulence, std::placeholders::_1));
	post_moveFuncs.emplace(157, std::bind(&mv::Gravity, std::placeholders::_1));
	post_moveFuncs.emplace(182, std::bind(&mv::TrickRoom, std::placeholders::_1));
	post_moveFuncs.emplace(183, std::bind(&mv::WonderRoom, std::placeholders::_1));
	post_moveFuncs.emplace(269, std::bind(&mv::MudSport, std::placeholders::_1));
	post_moveFuncs.emplace(349, std::bind(&mv::SplashSport, std::placeholders::_1));
	post_moveFuncs.emplace(10, std::bind(&mv::TwinNeedle, std::placeholders::_1));
}
