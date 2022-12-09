// File contains information on creating BattleEffects when a move is executed. 
#pragma once
#include <string>
#include "TypeCategory.h"
namespace bat { class Battle; }

namespace mv {
	enum StatusEffect {
		Burn,
		Paralysis,
		Frostbite,
		Drowsy,
		Poison,
		BadlyPoison,
		Confusion
	};
	void ChanceStatusEffect(bat::Battle& battle, StatusEffect status, tc::Type typeImmune, int proc);
	// Attack Order
	// X-Scissor
	// Night Slash
	// Talon Strike
	// Aeroblast
	// Air Cutter
	// Drill Run
	// Leaf Blade
	// Blaze Kick
	// Psycho Cut
	// Esper Wing
	// Cross Poison
	// Rupture
	// Shadow Claw
	void IncreasedCriticalOneStage(bat::Battle& battle); // pre

	// Pin Missile
	// Harass
	// Scale Shot
	// Arm Thrust
	// Stone Blast
	// Water Shuriken
	// Bullet Seed
	// Fury Attack
	void Hit2to5Times(bat::Battle& battle); // pre

	// Dual Chop
	// Double Kick
	// Dual Wingbeat
	// Double Hit
	void HitTwice(bat::Battle& battle); // pre

	// Heal Order - 50%
	// Floral Healing - 50%
	// Life Dew - 25%
	// Milk Drink - 50%
	void InstantHealSelf(bat::Battle& battle, int percent); // post

	// Leech Life - 50%
	// Parabolic Charge - 50%
	// Draining Kiss - 75%
	// Drain Punch - 50%
	// Oblivion Wing - 75%
	// Horn Leech - 50%
	// Giga Drain - 50%
	void HealOnDamage(bat::Battle& battle, int percent); // post

	// Steam Roller - 30%
	// Bite - 30%
	// Dark Pulse - 20%
	// Mythic Rush - 20%
	// Twister - 20%
	// Heart Stamp - 30%
	// Zen Headbutt - 20%
	// Air Slash - 20%
	// Rock Slide - 30%
	// Iron Head - 30%
	// Waterfall - 20%
	// Seed Bomb - 10%
	// Icicle Crash - 30%
	// Headbutt - 30%
	// Hyper Fang - 30%
	void ChanceFlinch(bat::Battle& battle, int proc); // post

	// Blaze Kick - 10%
	// Fire Punch - 10%
	// Ember - 10%
	// Flamethrower - 20%
	// Heatwave - 30%
	// Will-O-Wisp - 100%
	// Cold Flare - 30%
	void ChanceBurn(bat::Battle& battle, int proc); // post

	// Dragon Breath - 30%
	// Bolt Strike - 20%
	// Jolt - 30%
	// Nuzzle - 100%
	// Spark Punch - 20%
	// Discharge - 30%
	// Lightning Strike - 30%
	// Thunderbolt - 10%
	// Zap Cannon - 100%
	// Stun Wave - 100%
	// Force Palm - 30%
	// Stun Spore - 100%
	void ChanceParalyze(bat::Battle& battle, int proc); // post

	// Hypnosis - 100%
	// Sleep Powder - 100%
	// Relic Song - 20%
	void ChanceDrowsy(bat::Battle& battle, int proc); // post

	// Bleakwind Storm - 30%
	// Freeze Punch - 20%
	// Blizzard - 20%
	// Ice Beam - 20%
	void ChanceFrostbite(bat::Battle& battle, int proc); // post

	// BarbedStrike - 30%
	// Cross Poison - 10%
	// Gunk Shot - 30%
	// Mire - 40%
	// Sludge Bomb - 30%
	// Sludge Wave - 10%
	// Poison Gas - 100%
	void ChancePoison(bat::Battle& battle, int proc); // post

	void ChanceBadlyPoison(bat::Battle& battle, int proc);

	// Signal Beam - 30%
	// Strange Stream - 20%
	// Sweet Kiss - 100%
	// Confusion - 10%
	// Dynamic Strike - 100%
	// Confuse Ray - 100%
	// Liquid Pulse - 20%
	// Dizzy Punch - 20%
	// Supersonic - 100%
	// Teeter Dance - 100%
	void ChanceConfusion(bat::Battle& battle, int proc); // post

	// Breaking Swipe - 100%, -1 stage, defender
	// Play Rough - 10%, -1 stage, defender
	// Charm - 100%, -2 stages, defender
	// Power Up Punch - 100%, +1 stage, attacker
	// Feather Dance - 100%, -2 stages, defender
	// Metal Claw - 10%, +1 stage, attacker
	// Meteor Mash - 20%, +1 stage, attacker
	// Aurora Beam - 20%, -1 stage, defender
	// Growl - 100%, -2 stages, defender
	void ModAttack(bat::Battle& battle, int proc, int stages, bool targetAttacker); // post

	// Crunch - 20%, -1 stage, defender
	// Flame Whip - 100%, -1 stage, defender
	// Barrier - 100%, +2 stages, attacker
	// Acid Armor - 100%, +2 stages, attacker
	// Diamond Storm - 50%, +2 stages, attacker
	// Steel Wing - 10%, +1 stage, attacker
	// Tail Slam - 30%, -1 stage, defender
	// Iron Wall - 100%, +2 stages, attacker
	// Liquidation - 20%, -1 stage, defender
	// Razor Shell - 50%, -1 stage, defender
	// Cotton Guard - 100%, +3 stages, attacker
	// Crush Claw - 50%, -1 stage, defender
	// Harden - 100%, +2 stages, attacker
	// Leer - 100%, -1 stage, defender
	// Unpleasant Tone - 100%, -2 stages, defender
	void ModDefense(bat::Battle& battle, int proc, int stages, bool targetAttacker); // post

	// Tail Glow - 100%, +2 stages, attacker
	// Nasty Plot - 100%, +2 stages, attacker
	// Draco Meteor - 100%, -2 stages, attacker
	// Eerie Impulse - 100%, -2 stages, defender
	// Spirit Break - 100%, -1 stage, defender
	// Moonblast - 30%, -1 stage, defender
	// Captivate - 100%, -2 stages, defender
	// Fiery Dance - 50%, +1 stage, attacker
	// Autumn Storm - 100%, -2 stages, attacker
	// Tearful Look - 100%, -2 stages, defender
	void ModSpecialAttack(bat::Battle& battle, int proc, int stages, bool targetAttacker); // post

	// Bug Buzz - 20%, -2 stages, defender
	// Fake Tears - 100%, -2 stages, defender
	// Psychic - 10%, -1 stage, defender
	// Amnesia - 100%, +2 stages, attacker
	// Focus Blast - 10%, -1 stage, defender
	// Acid Bomb - 100%, -2 stages, defender
	// Earth Power - 100%, -1 stage, defender
	// Shadow Ball - 20%, -1 stage, defender
	// Flash Cannon - 10%, -1 stage, defender
	// Screeching Sound - 100%, -2 stages, defender
	// Energy Ball - 10%, -1 stage, defender
	// Malic Acid - 100%, -1 stage, defender
	// Tail Whip - 100%, -1 stage, defender
	void ModSpecialDefense(bat::Battle& battle, int proc, int stages, bool targetAttacker); // post

	// String Shot - 100%, -2 stages, defender
	// Electroweb - 100%, -1 stage, defender
	// Nitro Charge - 100%, +1 stage, attacker
	// Esper Wing - 100%, +1 stage, attacker
	// Agility - 100%, +2 stages, attacker
	// Hammer Arm - 100%, -1 stage, attacker
	// Low Sweep - 100%, -1 stage, defender
	// Bulldoze - 100%, -1 stage, defender
	// Boulder Tomb - 100%, -1 stage, defender
	// Rock Polish - 100%, +2 stages, attacker
	// Scary Face - 100%, -2 stages, defender
	// Automize - 100%, +2 stages, attacker
	// Bubble Beam - 20%, -1 stage, defender
	// Soft Spore - 100%, -2 stages, defender
	void ModSpeed(bat::Battle& battle, int proc, int stages, bool targetAttacker); // post

	// Night Daze - 40%, -1 stage, defender
	// Mud Bomb - 30%, -1 stage, defender
	// Reflecting Shot - 30%, -1 stage, defender
	// Flash - 100%, -1 stage, defender
	void ModAccuracy(bat::Battle& battle, int proc, int stages, bool targetAttacker);

	// Double Team - 100%, +1 stage, attacker
	// Minimize - 100%, +2 stages, attacker
	void ModEvasion(bat::Battle& battle, int proc, int stages, bool targetAttacker);

	// Triple Arrows - 100%, +2 stages, attacker
	// Focus Energy - 100%, +2 stages, attacker
	void ModCritical(bat::Battle& battle, int proc, int stages, bool targetAttacker);

	// Struggle Bug : 0/0/-1/-1/0/0/0/0, defender
	// Defend Order : 0/1/0/1/0/0/0/0, attacker
	// Quiver Dance : 0/0/1/1/1/0/0/0, attacker
	// Snarl : -1/0/-1/0/0/0/0/0, defender
	// Hone Claws : 1/0/0/0/0/1/0/0, attacker
	// Dragon Dance : 1/0/0/0/1/0/0/0, attacker
	// Magnetic Flux : 0/1/0/1/0/0/0/0, attacker
	// Baby Doll Eyes : -1/0/-1/0/0/0/0/0, defender
	// Mystical Flame :  -1/0/-1/0/0/0/0/0, defender
	// Barrier Rush : 0/1/0/1/0/0/0/0, attacker
	// Calm Mind : 0/0/1/1/0/0/0/0, attacker
	// Cosmic Power : 0/1/0/1/0/0/0/0, attacker
	// Close Combat : 0/-1/0/-1/0/0/0/0, attacker
	// Bulk Up: 1/1/0/1/0/0/0/0, attacker
	// No Retreat: 1/1/1/1/1/0/0/0, attacker
	// Victory Dance: 2/1/0/0/0/0/0/0, attacker
	// Coil : 1/1/0/0/0/1/0/0, attacker
	// Headlong Rush : 0/-1/0/-1/0/0/0/0, attacker
	// Barricade : 0/1/0/0/0/0/1/0, attacker
	// Shift Gear : 1/0/0/0/2/0/0/0, attacker
	// Confide : -1/0/-1/0/0/0/0/0, defender
	// Howl : 1/0/1/0/0/0/0/0, attacker
	// Noble Roar : -2/0/-2/0/0/0/0/0, defender
	// Swords Dance : 2/0/2/0/0/0/0/0, attacker
	// Tickle : -1/-1/0/0/0/0/0/0, defender
	// Work Up : 1/0/0/0/1/0/0/0, attacker
	void ModMultipleStats(bat::Battle& battle, int proc, int atk, int def, int spatk, int spdef, int spd, int acc, int eva, int crit, bool targetAttacker);

	// Steam Roller
	// Mythic Rush
	// Feint Attack
	// Shock Wave
	// Aura Sphere
	// Aerial Ace
	// Umbral Punch
	// Magnet Bomb
	// Magical Leaf
	void CheckTargetMinimized(bat::Battle& battle); // pre

	// Twister
	// Sky Uppercut
	// Gust
	// Hurricane
	void CanHitDuringFlyFreefall(bat::Battle& battle, bool moveIsSwift); // pre

	// Earthquake
	void CanHitDuringDig(bat::Battle& battle, bool moveIsSwift); // pre

	// Surf
	void CanHitDuringDive(bat::Battle& battle, bool moveIsSwift); // pre
	void CanHitDuringPhantomForce(bat::Battle& battle); // pre

	// Solar Blade
	// Solar Beam
	void SolarMoveCharge(bat::Battle& battle); // pre
	void PreCharge(bat::Battle& battle); // pre

	// Blast Burn
	// Hydro Cannon
	// Hyper Beam
	void PostCharge(bat::Battle& battle); // post

	// Hex
	void DoublePowerIfStatusEffect(bat::Battle& battle);

	// Outrage
	// Petal Dance
	// Raging Fury
	void Fixated(bat::Battle& battle); // post

	// Infestation
	// Thunder Cage
	// Heat Swirl
	// Sand Tomb
	// Whirlpool
	// Bind
	void PassiveDamageTrap(bat::Battle& battle); // post

	void LightningStrikeCheckRain(bat::Battle& battle); // pre
	void BlizzardCheckForHail(bat::Battle& battle); // pre

	void ElectricField(bat::Battle& battle); // Creates Electric terrain for 5 turns.
	void IonDeluge(bat::Battle& battle); // Causes all Normal moves to become Electric moves for 3 turns. 
	void SunnyDay(bat::Battle& battle); // Creates harsh sunlight for 5 turns. 
	void MistyTerrain(bat::Battle& battle); // Creates Misty terrain for 5 turns.
	void Gravity(bat::Battle& battle); // Creates intense gravity for 5 turns.
	void MagicRoom(bat::Battle& battle); // Creates magic room for 5 turns. 
	void PsychoField(bat::Battle& battle); // Creates psychic terrain for 5 turns. 
	void TrickRoom(bat::Battle& battle); // Creates trick room for 5 turns. 
	void WonderRoom(bat::Battle& battle); // Creates wonder room for 5 turns.
	void Sandstorm(bat::Battle& battle); // Creates sandstorm for 5 turns.
	void MudSport(bat::Battle& battle); // Base power of all Electric-type moves is reduced by 67% for 5 turns.
	void RainDance(bat::Battle& battle); // Creates rain for 5 turns. 
	void SplashSport(bat::Battle& battle); // Base power of all Fire-type moves is reduced by 67% for 5 turns. 
	void Hail(bat::Battle& battle); // Creates hail for 5 turns. 
	void GrassField(bat::Battle& battle); // Creates grassy terrain for 5 turns. 
	void Turbulence(bat::Battle& battle); // Creates Strong Winds for 5 turns.

	void LightScreen(bat::Battle& battle); // Halves damage done to user and allies from Special moves. 
	void Reflect(bat::Battle& battle); // Halves damage done to user and allies from Physical moves. 
	void AuroraVeil(bat::Battle& battle); // Halves damage done to user from Physical and Special moves. If allies are present, all benefit but only reduces damage by 33%

	void FuryCutter(bat::Battle& battle); // Power doubles on consecutive hits to a maximum of 160.
	void TwinNeedle(bat::Battle& battle); // Hits twice. Each strike has a 20% chance to Poison.
	void DarkestLariat(bat::Battle& battle); // Ignores any changes to the target's Defense and Evasion.
	void Payback(bat::Battle& battle); // Power doubles if the user moves after the target, or if the target switches out. 
	void Punishment(bat::Battle& battle); // Increases power by 20 for each positive stat change the target has, to a maximum of 200. 
	void Obstruct(bat::Battle& battle); // The user is protected from all except status moves. If the move blocks a contact move, the attacker's Defense is lowered by 2 stages. 
	void Snatch(bat::Battle& battle); // The user steals and uses the first beneficial status move used by any other target on the field. 
	void Taunt(bat::Battle& battle); // The target becomes unable to use status moves. 
	void TopsyTurvy(bat::Battle& battle); // Reverses all stat changes on the target. Does not trigger effects based on stat changes.
	void Torment(bat::Battle& battle); // Prevents the target from using the same move twice in a row. 
	void BoltBeak(bat::Battle& battle); // If the user attacks before the target, or if the target switches, base power is doubled. 
	void SparkFang(bat::Battle& battle); // 20% chance to paralyze the target, separate 10% chance to flinch the target.
	void VoltTackle(bat::Battle& battle); // 30% chance to paralyze the target. User takes 1/3 recoil damage. Curse user's drowsiness.
	void Charge(bat::Battle& battle); // User's Special Defense is raised by 1 stage. If the user's next move is Electric-type, its power is doubled. 
	void MagnetRise(bat::Battle& battle); // User becomes immune to Ground attacks, Spikes, Toxic Spikes, and Arena Trap. 
	void NaturesMadness(bat::Battle& battle); // Deals damage equal to 50% of the opponent's remaining HP. 
	void CraftyShield(bat::Battle& battle); // Protects all allies from status moves. 
	void FireFang(bat::Battle& battle); // 20% chance to burn the target, separate 10% chance to flinch the target.
	void FlareWheel(bat::Battle& battle); // 10% chance to burn the target. Removes frostbite from user. 
	void InfernoDrive(bat::Battle& battle); // 10% chance to burn the target. Removes frostbite from user. User takes 1/3 recoil damage.
	void VCreate(bat::Battle& battle); // Lowers user's Defense, Special Defense, and Speed by 1 stage each.
	void BurnUp(bat::Battle& battle); // Removes frostbite from user. User loses Fire typing.
	void BurningJealosy(bat::Battle& battle); // Hits multiple targets. Burn all targets with stat increases.
	void PsychicFang(bat::Battle& battle); // Removes Light Screen, Reflect, and Aurora Veil from the target's side of the field.
	void StoredPower(bat::Battle& battle); // Power increases by 20 for each positive stat change the user has. 
	void Extrasensory(bat::Battle& battle); // Lowers the target's Speed by 1 stage. 10% chance to cause the target to flinch. 
	void MirrorCoat(bat::Battle& battle); // Deals twice as much damage to any Pokemon that damages it with a Special move in the same turn. 
	void MysticalPower(bat::Battle& battle); // Raises user's Special Attack or Special Defense by 1 stage depending on which has a higher base stat. 
	void Psyshock(bat::Battle& battle); // Uses the target's Defense to calculate damage.
	void GuardSplit(bat::Battle& battle); // User and target have their Defense and Special Defense averaged with each other. 
	void GuardSwap(bat::Battle& battle); // User and target swap Defense and Special Defense raw stats.
	void HealBlock(bat::Battle& battle); // Target becomes unable to regain HP for 5 turns.
	void MagicCoat(bat::Battle& battle); // Reflects most status moves targeting the user back to the Pokemon that cast them. 
	void Meditate(bat::Battle& battle); // Raises user's Attack by 2 stages.
	void PowerSplit(bat::Battle& battle); // User and target have their Attack and Special Attack averaged with each other. 
	void PowerSwap(bat::Battle& battle); // User and target swap their Attack and Special Attack raw stats.
	void Rest(bat::Battle& battle); // User heals 75% of their maximum HP and becomes drowzy.
	void SoulSwap(bat::Battle& battle); // User and target swap stat changes.
	void SpeedSwap(bat::Battle& battle); // User and target swap Speed raw stats. 
	void StatTrick(bat::Battle& battle); // User's Attack and Defense raw stats are swapped. 
	void StatusShift(bat::Battle& battle); // Cures the user of any status effect and afflicts the effect onto the target. 
	void Telekenesis(bat::Battle& battle); // Target becomes immune to Ground-type moves and entry hazards. All moves used against the target bypass accuracy checks. Lasts 3 turns.
	void WakeUpSlap(bat::Battle& battle); // Cures target's drowziness. 
	void BodyPress(bat::Battle& battle); // Uses the user's Defense to calculate damage. 
	void BrickBreak(bat::Battle& battle); // Removes Reflect, Light Screen, and Aurora Veil before doing damage. 
	void Counter(bat::Battle& battle); // Deals twice as much damage to any Pokemon that damages it with a Physical move in the same turn. 
	void FocusPunch(bat::Battle& battle); // If the user takes damage before it can execute the attack, the move fails. 
	void HighJumpKick(bat::Battle& battle); // If the attack misses, the user takes damage equal to 50% of their max HP. 
	void SacredSword(bat::Battle& battle); // Ignores any changes to the target's Defense and Evasion.
	void StormThrow(bat::Battle& battle); // Will always be a critical hit. 
	void MysterySword(bat::Battle& battle); // Uses the target's Defense to calculate damage. 
	void Acrobatics(bat::Battle& battle); // Has double power if the user isn't holding an item. 
	void BraveBird(bat::Battle& battle); // User takes 1/3 the damage dealt as recoil damage. 
	void Fly(bat::Battle& battle); // User becomes semi-invulnerable on the turn this move is used, on the following turn it does damage. 
	void Freefall(bat::Battle& battle); // User and target become semi-invulnerable on the turn this move is used, on the following turn it does damage. 
	void SkyAttack(bat::Battle& battle); // 30% chance to cause the target to flinch. Increased critical hit ratio by 1 stage. User charges on the first turn. On the second turn deals damage. 
	void Defog(bat::Battle& battle); // Removes Spikes, Toxic Spikes, Stealth Rock, Sticky Web from both sides of the field. Removes Aurora Veil, Light Screen, Reflect, Safeguard, and Mist from the target's side of the field. 
	void MirrorMove(bat::Battle& battle); // User copies the last move that targeted them and uses it against the target.
	void Roost(bat::Battle& battle); // User heals 50% of their maximum HP. User loses their Flying-typing until the end of the turn.
	void Tailwind(bat::Battle& battle); // User and all allies have their Speed raw stat doubled for 4 turns. 
	void BarbBarrage(bat::Battle& battle); // Hits multiple targets. 30% chance to poison the target. Power is doubled if the target has a status effect.
	void DireClaw(bat::Battle& battle); // 50% chance to inflict a status effect, randomly chosen from poison, paralysis, and drowziness. 
	void VenomFang(bat::Battle& battle); // 30% chance to badly poison the target, separate 10% chance to flinch the target. 
	void BanefulThread(bat::Battle& battle); // Poisons the target. Target's Speed is lowered by 1 stage.
	void Pillbox(bat::Battle& battle); // Protects the user from damaging and status moves. If a blocked move would have made contact, the Pokemon that used it is poisoned. 
	void Toxic(bat::Battle& battle); // Badly poisons the target. Bypasses accuracy checks if the user is Poison-type.
	void VenomTrap(bat::Battle& battle); // Hits multiple targets. Any target that is poisoned has their Attack, Special Attack, and Speed lowered by 1 stage each. 
	void Dig(bat::Battle& battle); // User becomes semi-invulnerable on the turn this move is used, on the following turn it does damage. 
	void StompingTantrum(bat::Battle& battle); // If on the previous turn, the user's last move missed or failed, power is doubled.
	void SandsearStorm(bat::Battle& battle); // 30% chance to burn the target. Cures user's and target's frostbite. 
	void Rototiller(bat::Battle& battle); // Raises the Attack and Special Attack of all Grass-type Pokemon by 1 stage each. 
	void SandAttack(bat::Battle& battle); // Lowers the target's Accuracy by 1 stage. Can hit Pokemon normally immune to Ground-type moves. 
	void SoilGather(bat::Battle& battle); // User heals 50% of their maximum HP. If sandstorm is active, instead regain 67% of their maximum HP.
	void HeadSmash(bat::Battle& battle); // User takes 50% the damage dealt as recoil damage. 
	void Rollout(bat::Battle& battle); // Power doubles on consecutive hits to a maximum of 480. Power resets if the move is interrupted. 
	void SmackDown(bat::Battle& battle); // Target becomes grounded. 
	void MeteorBeam(bat::Battle& battle); // User charges on the first turn and raises user's Special Attack by 1 stage. On the second turn, the move does damage. Status effects and flinching delays the damage until the next turn.
	void Astonish(bat::Battle& battle); // 30% chance to cause the target to flinch. Lowers the target's Speed by 1 stage. 
	void Lick(bat::Battle& battle); // 30% chance to paralyze the target. Cannot paralyze Ghost-type pokemon. 
	void PhantomForce(bat::Battle& battle); // User becomes semi-invulnerable on the turn this move is used, on the following turn it does damage. 
	void SpectralThief(bat::Battle& battle); // Removes all positive stat changes from the target and applies them to the user before dealing damage. 
	void BitterMalice(bat::Battle& battle); // 30% chance to frostbite the target. Power is doubled if the target has a status effect. 
	void InfernalParade(bat::Battle& battle); // 30% chance to burn the target. Power is doubled if the target has a status effect. 
	void DoublePanzer(bat::Battle& battle); // Hits twice. 30% chance per hit to flinch the target. 
	void MirrorBurst(bat::Battle& battle); // Deals 150% the damage dealt to the user by the target's last attack to the target. 
	void SteelRoller(bat::Battle& battle); // Removes active terrain. 
	void AssistGear(bat::Battle& battle); // Raises the Attack and Special Attack of all Steel-type and Electric-type pokemon by 1 stage. 
	void Dive(bat::Battle& battle); // User becomes semi-invulnerable on the turn this move is used, on the following turn it does damage.
	void Brine(bat::Battle& battle); // Power is doubled if the target has 50% or less of their maximum HP.
	void Scald(bat::Battle& battle); // 30% chance to burn the target. Cures user's and target's frostbite. 
	void SparklingAria(bat::Battle& battle); // Hits multiple targets, including allies. Targets are cured of their burn. 
	void AquaRing(bat::Battle& battle); // User heals 1/16 max HP every turn. 
	void Soak(bat::Battle& battle); // Target becomes pure Water-type.
	void GForce(bat::Battle& battle); // Lowers the target's Defense by 1 stage. If intense gravity is in effect, power is 120.
	void GrassyGlide(bat::Battle& battle); // If grassy terrain is in effect, the move has a priority of +1.
	void SedativeFang(bat::Battle& battle); // 20% chance to make the target drowsy, separate 10% chance to flinch the target.
	void Chloroblast(bat::Battle& battle); // User takes 50% of their maximum HP as recoil damage. 
	void Aromatherapy(bat::Battle& battle); // User and all allies are cured of their status effects.
	void Ingrain(bat::Battle& battle); // User becomes grounded and heals 1/16 their maximum HP each turn. User becomes unable to switch out. 
	void LeechSeed(bat::Battle& battle); // Target takes 1/8 their maximum HP per turn, user heals the damage dealt. If the user switches out, their replacement benefits from the healing. 
	void Syntehsis(bat::Battle& battle); // User heals HP depending on weather. In harsh sunlight, heal 75% HP. During no weather, heal 25% maximum HP. In any other weather, heal 12.5% maximum HP. 
	void Avalanche(bat::Battle& battle); // If the target has already damaged the user when this move executes, power is doubled.  
	void IceBall(bat::Battle& battle); // Power doubles on consecutive hits to a maximum of 480. Power resets if the move is interrupted. 
	void IceFang(bat::Battle& battle); // 20% chance to frostbite the target, separate 10% chance to flinch the target. 
	void FreezeDry(bat::Battle& battle); // 10% chance to freeze the target. Supereffective against Water-type Pokemon. 
	void FrostBreath(bat::Battle& battle); // Always a critical hit. 
	void Mist(bat::Battle& battle); // User and allies are protected from having their stats lowered. Does not affect stat changes caused by the user or allies. 
	void Haze(bat::Battle& battle); // All Pokemon have their stat changes removed. 
	void BodySlam(bat::Battle& battle); // 30% chance to paralyze the target. Normal-type pokemon cannot be paralyzed. 
	void Bravado(bat::Battle& battle); // Power is doubled if the user has a status effect. 
	void Cut(bat::Battle& battle); // Supereffective against Grass-type targets. 
	void DoubleEdge(bat::Battle& battle); // User takes 33% the damage dealt as recoil damage.
	void Endeavor(bat::Battle& battle); // Target's HP becomes the same as the user's HP. 
	void FalseSwipe(bat::Battle& battle); // Target will be left with 1 HP if the move would otherwise cause it to faint.
	void RapidSpin(bat::Battle& battle); // Raises the user's Speed by 1 stage. Removes the effects of binding moves, Leech Seed, and entry hazards from the user's side of the field after dealing damage. 
	void SmellingSalts(bat::Battle& battle); // Power is doubled against a paralyzed target. Curse target's paralysis. 
	void Strength(bat::Battle& battle); // Supereffective against Rock-type targets. 
	void Struggle(bat::Battle& battle); // Cannot be learned. Is used automatically when the user has no usable moves. User takes 1/4 the damage dealt as recoil damage. 
	void SuperFang(bat::Battle& battle); // Deals damage equal to 50% of the opponent's remaining HP.
	void RevelationDance(bat::Battle& battle); // Type changes to match the user's type. 
	void Swift(bat::Battle& battle); // Hits multiple targets. Can hit targets during the semi-invulnerable turns of Fly, Freefall, Dig, Phantom Force, and Dive. 
	void TriAttack(bat::Battle& battle); // 60% chance for a random status effect, chosen from paralysis, frostbite, or burn. 
	void WeatherBall(bat::Battle& battle); // Move changes type based on the weather. 
	void Acupressure(bat::Battle& battle); // Randomly raises the users Attack, Defense, Special Attack, Special Defense, Speed, Accuracy, or Evasion by 3 stages.
	void BellyDrum(bat::Battle& battle); // Raises user's Attack by 6 stages. User takes 50% max HP as recoil damage. Move fails if the user's current HP is less than 50% its maximum. 
	void Conversion(bat::Battle& battle); // User's current type combination changes to match the typing of the moves the user has in its first two move slots. 
	void CourtChange(bat::Battle& battle); // All active one-sided battlefield effects switch sides. 
	void Disable(bat::Battle& battle); // Disables the last move the target used for 4 turns. 
	void Encore(bat::Battle& battle); // Target is prevented from using any move except the last move that it used for 3 turns. 
	void Endure(bat::Battle& battle); // Allows the user to survive any attack that would cause it to faint, instead leaving it with 1 HP instead.
	void Glare(bat::Battle& battle); // Paralyzes the target. Affects Ghost-type targets.
	void HealBell(bat::Battle& battle); // User, all allies, and all Pokemon in the user's party are cured of all status conditions. 
	void LaserFocus(bat::Battle& battle); // The user's moves are critical hits until the end of the next turn. 
	void LockOn(bat::Battle& battle); // The user's moves bypass accuracy checks until the end of the next turn. 
	void LuckyChant(bat::Battle& battle); // The user's party becomes immune to critical hits for 5 turns. 
	void MeFirst(bat::Battle& battle); // If the target has not moved, but has selected a damage-dealing move, the user copies that move and uses it against the target with 50% extra power and bypassing accuracy checks. 
	void Metronome(bat::Battle& battle); // User randomly selects a move from all possible moves in the game and executes it. 
	void NaturePower(bat::Battle& battle); // User executes a move based on the current battlefield effect. 
	void OdorSleuth(bat::Battle& battle); // Causes all accuracy checks against the target to ignore any changes to the target's Evasion. If the target is Ghost-type, it loses its immunity to Normal-type and Fighting-type moves. 
	void PainSplit(bat::Battle& battle); // User and target's current HP is averaged. 
	void PowerShift(bat::Battle& battle); // Swaps users Attack and Special Attack with their Defense and Special Defense. 
	void Protect(bat::Battle& battle); // Protects user from all moves. 
	void PsychUp(bat::Battle& battle); // User's stat changes are reset and copies all the target's stat changes on the user, including critical hit ratio. 
	void Regeneration(bat::Battle& battle); // User heals 20% their maximum HP per turn for 3 turns. On the second and third turn, the user can use other moves. 
	void Safeguard(bat::Battle& battle); // User and allies become immune to status effects for 5 turns. 
	void ShellSmash(bat::Battle& battle); // Raises user's Attack, Special Attack, and Speed by 2 stages each. Lowers user's Defense and Special Defense by 1 stage each. 
	void Substitute(bat::Battle& battle); // User cuts own HP by 25% its max value to create a substitute to take damage for the user. Has several effects. 
	void SweetScent(bat::Battle& battle); // Hits multiple targets. Lowers the target's Evasion by 2 stages. Cannot be used under battlefield terrain effects. 
	void Wish(bat::Battle& battle); // At the end of the next turn, whatever Pokemon occupies the slot the user did when this move was used regains 50% of their maximum HP. 
	void DreamEater(bat::Battle& battle); // User heals 50% of damage dealt. Only works on a target that is drowsy.
	void Corrode(bat::Battle& battle); // 10% chance to lower the target's Defense and Special Defense by 1 stage each. 
	void AncientPower(bat::Battle& battle); // 10% chance to raise the user's Attack, Defense, Special Attack, Special Defense, and Speed by 1 stage each. 
	void OminousWind(bat::Battle& battle); // 10% chance to raise the user's Attack, Defense, Special Attack, Special Defense, and Speed by 1 stage each. 
	void DisarmingVoice(bat::Battle& battle); // 10% chance to flinch the target, is surehit so it checks target minimized.
}