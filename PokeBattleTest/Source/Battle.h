#pragma once
#include <string>
#include <vector>
#include <queue>
#include <map>
#include "Pokemon.h"
#include "BattleEffect.h"
#include "MoveLogic.h"

namespace bat {
	class Battle;

	class Turn {
	private:
		pkmn::Pokemon attacker;
		pkmn::Pokemon defender;
		pkmn::Move move;
		bool attackerIsPlayer;
		bool moveHits = false;
		// Mods in terms of a flat multiplier.
		float a_atkMod = 1.0F;
		float a_defMod = 1.0F;
		float a_spAtkMod = 1.0F;
		float a_spDefMod = 1.0F;
		float a_spdMod = 1.0F;
		float a_accMod = 1.0F;
		float d_atkMod = 1.0F;
		float d_defMod = 1.0F;
		float d_spAtkMod = 1.0F;
		float d_spDefMod = 1.0F;
		float d_spdMod = 1.0F;
		float a_absoluteDmgMod = 1.0F;
		// Mods in terms of an absolute override. These are factored in before the multiplier. The default -1 indicates they're not used.
		int a_defOverride = -1;
		int a_spDefOverride = -1;
		int d_defOverride = -1;
		int d_spDefOverride = -1;
		// Non-grounded pokemon are immune to ground-type moves and terrain. Constructor discerns who is grounded.
		bool a_grounded;
		bool d_grounded;
		// Accuracy and evasion stages, resolved as a part of the Action phase.
		int a_accuracyStages = 0;
		int d_evasionStages = 0;
		// Other mods
		bool interruptTurn = false; // If this is true, such as from a paralysis battle effect, the pokemon skips their action phase.
		bool interruptBecauseConfused = false; // A temporary? Workaround to a problem with ConfusionAttackSelf not being able to selectively execute. 
		bool a_bypassInvulnerable = false;
		bool d_SemiInvulnerable = false;
		tc::Type a_type1Override = tc::Type::NoType;
		tc::Type a_type2Override = tc::Type::NoType;
		tc::Type d_type1Override = tc::Type::NoType;
		tc::Type d_type2Override = tc::Type::NoType;
		tc::Type a_moveTypeOverride = tc::Type::NoType;
		int critChance = 24; // Algorithm will generate a number between 1 and this to determine crit. Default 1/24, mod can be 1/8, 1/2, or 1/1 (always crit) 
		int numAttacks = 1; // Some BattleEffects hit twice, others 2-5 times.
	public:
		bool Act(Battle& battle); // If this returns false, a pokemon was knocked out and the Round() method instantly ends.
		Turn(pkmn::Pokemon& _attacker, pkmn::Pokemon& _defender, pkmn::Move _move, bool _atkr); // The move isn't passed by reference because it may be modified dynamically.
		// BattleEffect friends
		friend void bfx::ModAttack::Execute(Turn& turn);
		friend void bfx::ModDefense::Execute(Turn& turn);
		friend void bfx::ModSpecialAttack::Execute(Turn& turn);
		friend void bfx::ModSpecialDefense::Execute(Turn& turn);
		friend void bfx::ModSpeed::Execute(Turn& turn);
		friend void bfx::ModCriticalRatio::Execute(Turn& turn);
		friend void bfx::TemporaryCritIncrease::Execute(bat::Turn& turn);
		friend void bfx::ModAccuracy::Execute(Turn& turn);
		friend void bfx::ModEvasion::Execute(Turn& turn);
		friend void bfx::BurnAttackReduction::Execute(Turn& turn);
		friend void bfx::BurnDamage::Execute(Turn& turn);
		friend void bfx::Drowsy::Execute(Turn& turn);
		friend void bfx::DrowsyDamageBoost::Execute(Turn& turn);
		friend void bfx::Paralysis::Execute(Turn& turn);
		friend void bfx::FrostbiteDamage::Execute(Turn& turn);
		friend void bfx::FrostbiteSpecialAttackReduction::Execute(Turn& turn);
		friend void bfx::Poisoned::Execute(Turn& turn);
		friend void bfx::BadlyPoisoned::Execute(Turn& turn);
		friend void bfx::Confusion::Execute(Turn& turn);
		friend void bfx::ConfusionAttackSelf::Execute(Turn& turn);
		friend void bfx::Flinch::Execute(Turn& turn);

		friend void bfx::ElectricTerrain::Execute(bat::Turn& turn);
		friend void bfx::MistyTerrain::Execute(bat::Turn& turn);
		friend void bfx::PsychicTerrain::Execute(bat::Turn& turn);
		friend void bfx::GrassyTerrain::Execute(bat::Turn& turn);
		friend void bfx::GrassyTerrainHeal::Execute(bat::Turn& turn);

		friend void bfx::HarshSunlight::Execute(bat::Turn& turn);
		friend void bfx::Rain::Execute(bat::Turn& turn);
		friend void bfx::Sandstorm::Execute(bat::Turn& turn);
		friend void bfx::SandstormDamage::Execute(bat::Turn& turn);
		friend void bfx::Hail::Execute(bat::Turn& turn);
		friend void bfx::HailDamage::Execute(bat::Turn& turn);
		friend void bfx::StrongWinds::Execute(bat::Turn& turn);
		friend void bfx::IntenseGravity::Execute(bat::Turn& turn);

		friend void bfx::MudSport::Execute(bat::Turn& turn);
		friend void bfx::SplashSport::Execute(bat::Turn& turn);
		friend void bfx::IonDeluge::Execute(bat::Turn& turn);

		friend void bfx::MultiHit::Execute(bat::Turn& turn);
		friend void bfx::MoveAutoHit::Execute(bat::Turn& turn);
		friend void bfx::FlatDamageMod::Execute(bat::Turn& turn);
		friend void bfx::BypassSemiInvulnerable::Execute(bat::Turn& turn);
		friend void bfx::InterruptMove::Execute(bat::Turn& turn);
		friend void bfx::DamageTrap::Execute(bat::Turn& turn);
		friend void bfx::ReflectDefenseBoost::Execute(bat::Turn& turn);
		friend void bfx::LightScreenSpecialDefenseBoost::Execute(bat::Turn& turn);
		friend void bfx::AuroraVeilDefSpdefBoost::Execute(bat::Turn& turn);
		friend void bfx::WonderRoom::Execute(bat::Turn& turn);
		friend void bfx::MoveOverride::Execute(bat::Turn& turn);
	};

	class Battle {
		typedef std::function<void(Battle& battle)> m_func;
	private:
		std::vector<std::unique_ptr<bfx::BattleEffect>> battleEffects;
		std::deque<int> p_movesUsed;
		std::deque<int> r_movesUsed;
		std::map<int, m_func> pre_moveFuncs;
		std::map<int, m_func> post_moveFuncs;
		pkmn::Pokemon player;
		pkmn::Pokemon rival;
		pkmn::Move p_move;
		pkmn::Move r_move;
		float p_speedMult = 1.0F;
		float r_speedMult = 1.0F;
		int p_dmgThisTurn = 0;
		int r_dmgThisTurn = 0;
		bool attackerIsPlayer = false;
		bool playerGoesFirst = false;
		bool victorDeclared = false;
	public:
		bool GetVictorDeclared() { return victorDeclared; }
		void Round(pkmn::Move& playerMove, pkmn::Move& rivalMove);
		friend bool Turn::Act(Battle& battle);
		Battle(pkmn::Pokemon& _player, pkmn::Pokemon& _rival);
		// MoveLogic friends
		friend void mv::IncreasedCriticalOneStage(bat::Battle& battle);
		friend void mv::Hit2to5Times(bat::Battle& battle);
		friend void mv::HitTwice(bat::Battle& battle);
		friend void mv::InstantHealSelf(bat::Battle& battle, int percent);
		friend void mv::HealOnDamage(bat::Battle& battle, int percent);
		friend void mv::ChanceFlinch(bat::Battle& battle, int proc);
		friend void mv::ChanceStatusEffect(bat::Battle& battle, StatusEffect status, tc::Type typeImmune, int proc);
		friend void mv::ModMultipleStats(bat::Battle& battle, int proc, int atk, int def, int spatk, int spdef, int spd, int acc, int eva, int crit, bool targetAttacker);
		friend void mv::CheckTargetMinimized(bat::Battle& battle);
		friend void mv::CanHitDuringFlyFreefall(bat::Battle& battle, bool moveIsSwift);
		friend void mv::CanHitDuringSemiInvulnerable(bat::Battle& battle, int moveID, bool moveIsSwift);
		friend void mv::DoublePowerIfStatusEffect(bat::Battle& battle);
		friend void mv::PassiveDamageTrap(bat::Battle& battle, std::string cause);
		friend void mv::Reflect(bat::Battle& battle);
		friend void mv::LightScreen(bat::Battle& battle);
		friend void mv::AuroraVeil(bat::Battle& battle);
		friend void mv::ElectricField(bat::Battle& battle);
		friend void mv::MistField(bat::Battle& battle);
		friend void mv::PsychoField(bat::Battle& battle);
		friend void mv::GrassField(bat::Battle& battle);
		friend void mv::SunnyDay(bat::Battle& battle);
		friend void mv::RainDance(bat::Battle& battle);
		friend void mv::Sandstorm(bat::Battle& battle);
		friend void mv::Hail(bat::Battle& battle);
		friend void mv::Turbulence(bat::Battle& battle);
		friend void mv::Gravity(bat::Battle& battle);
		friend void mv::TrickRoom(bat::Battle& battle); 
		friend void mv::WonderRoom(bat::Battle& battle);
		friend void mv::MudSport(bat::Battle& battle);
		friend void mv::SplashSport(bat::Battle& battle);
		friend void mv::IonDeluge(bat::Battle& battle);
		friend void mv::FuryCutter(bat::Battle& battle);
		friend void mv::TwinNeedle(bat::Battle& battle);
		friend void mv::DarkestLariat(bat::Battle& battle);
		friend void mv::Payback(bat::Battle& battle);
		friend void mv::Punishment(bat::Battle& battle);
		friend void mv::Obstruct(bat::Battle& battle);
		friend void mv::Snatch(bat::Battle& battle);
		friend void mv::Taunt(bat::Battle& battle);
		friend void mv::TopsyTurvy(bat::Battle& battle);
		friend void mv::Torment(bat::Battle& battle);
		friend void mv::BoltBeak(bat::Battle& battle);
		friend void mv::SparkFang(bat::Battle& battle);
		friend void mv::VoltTackle(bat::Battle& battle);
		friend void mv::Charge(bat::Battle& battle);
		friend void mv::MagnetRise(bat::Battle& battle);
		friend void mv::NaturesMadness(bat::Battle& battle);
		friend void mv::CraftyShield(bat::Battle& battle);
		friend void mv::FireFang(bat::Battle& battle);
		friend void mv::FlareWheel(bat::Battle& battle);
		friend void mv::InfernoDrive(bat::Battle& battle);
		friend void mv::VCreate(bat::Battle& battle);
		friend void mv::BurnUp(bat::Battle& battle);
		friend void mv::BurningJealosy(bat::Battle& battle);
		friend void mv::PsychicFang(bat::Battle& battle);
		friend void mv::StoredPower(bat::Battle& battle);
		friend void mv::Extrasensory(bat::Battle& battle);
		friend void mv::MirrorCoat(bat::Battle& battle);
		friend void mv::MysticalPower(bat::Battle& battle);
		friend void mv::Psyshock(bat::Battle& battle);
		friend void mv::GuardSplit(bat::Battle& battle);
		friend void mv::GuardSwap(bat::Battle& battle);
		friend void mv::HealBlock(bat::Battle& battle);
		friend void mv::MagicCoat(bat::Battle& battle);
		friend void mv::Meditate(bat::Battle& battle);
		friend void mv::PowerSplit(bat::Battle& battle);
		friend void mv::PowerSwap(bat::Battle& battle);
		friend void mv::Rest(bat::Battle& battle);
		friend void mv::SoulSwap(bat::Battle& battle);
		friend void mv::SpeedSwap(bat::Battle& battle);
		friend void mv::StatTrick(bat::Battle& battle);
		friend void mv::StatusShift(bat::Battle& battle);
		friend void mv::Telekenesis(bat::Battle& battle);
		friend void mv::WakeUpSlap(bat::Battle& battle);
		friend void mv::BodyPress(bat::Battle& battle);
		friend void mv::BrickBreak(bat::Battle& battle);
		friend void mv::Counter(bat::Battle& battle);
		friend void mv::FocusPunch(bat::Battle& battle);
		friend void mv::HighJumpKick(bat::Battle& battle);
		friend void mv::SacredSword(bat::Battle& battle);
		friend void mv::StormThrow(bat::Battle& battle);
		friend void mv::MysterySword(bat::Battle& battle);
		friend void mv::Acrobatics(bat::Battle& battle);
		friend void mv::BraveBird(bat::Battle& battle);
		friend void mv::Fly(bat::Battle& battle);
		friend void mv::Freefall(bat::Battle& battle);
		friend void mv::SkyAttack(bat::Battle& battle);
		friend void mv::Defog(bat::Battle& battle);
		friend void mv::MirrorMove(bat::Battle& battle);
		friend void mv::Roost(bat::Battle& battle);
		friend void mv::Tailwind(bat::Battle& battle);
		friend void mv::BarbBarrage(bat::Battle& battle);
		friend void mv::DireClaw(bat::Battle& battle);
		friend void mv::VenomFang(bat::Battle& battle);
		friend void mv::BanefulThread(bat::Battle& battle);
		friend void mv::Pillbox(bat::Battle& battle);
		friend void mv::Toxic(bat::Battle& battle);
		friend void mv::VenomTrap(bat::Battle& battle);
		friend void mv::Dig(bat::Battle& battle);
		friend void mv::StompingTantrum(bat::Battle& battle);
		friend void mv::SandsearStorm(bat::Battle& battle);
		friend void mv::Rototiller(bat::Battle& battle);
		friend void mv::SandAttack(bat::Battle& battle);
		friend void mv::SoilGather(bat::Battle& battle);
		friend void mv::HeadSmash(bat::Battle& battle);
		friend void mv::Rollout(bat::Battle& battle);
		friend void mv::SmackDown(bat::Battle& battle);
		friend void mv::MeteorBeam(bat::Battle& battle);
		friend void mv::Astonish(bat::Battle& battle);
		friend void mv::Lick(bat::Battle& battle);
		friend void mv::PhantomForce(bat::Battle& battle);
		friend void mv::SpectralThief(bat::Battle& battle);
		friend void mv::BitterMalice(bat::Battle& battle);
		friend void mv::InfernalParade(bat::Battle& battle);
		friend void mv::DoublePanzer(bat::Battle& battle);
		friend void mv::MirrorBurst(bat::Battle& battle);
		friend void mv::SteelRoller(bat::Battle& battle);
		friend void mv::AssistGear(bat::Battle& battle);
		friend void mv::Dive(bat::Battle& battle);
		friend void mv::Brine(bat::Battle& battle);
		friend void mv::Scald(bat::Battle& battle);
		friend void mv::SparklingAria(bat::Battle& battle);
		friend void mv::AquaRing(bat::Battle& battle);
		friend void mv::Soak(bat::Battle& battle);
		friend void mv::GForce(bat::Battle& battle);
		friend void mv::GrassyGlide(bat::Battle& battle);
		friend void mv::SedativeFang(bat::Battle& battle);
		friend void mv::Chloroblast(bat::Battle& battle);
		friend void mv::Aromatherapy(bat::Battle& battle);
		friend void mv::Ingrain(bat::Battle& battle);
		friend void mv::LeechSeed(bat::Battle& battle);
		friend void mv::Synthesis(bat::Battle& battle);
		friend void mv::Avalanche(bat::Battle& battle);
		friend void mv::IceBall(bat::Battle& battle);
		friend void mv::IceFang(bat::Battle& battle);
		friend void mv::FreezeDry(bat::Battle& battle);
		friend void mv::FrostBreath(bat::Battle& battle);
		friend void mv::Mist(bat::Battle& battle);
		friend void mv::Haze(bat::Battle& battle);
		friend void mv::BodySlam(bat::Battle& battle);
		friend void mv::Bravado(bat::Battle& battle);
		friend void mv::Cut(bat::Battle& battle);
		friend void mv::DoubleEdge(bat::Battle& battle);
		friend void mv::Endeavor(bat::Battle& battle);
		friend void mv::FalseSwipe(bat::Battle& battle);
		friend void mv::RapidSpin(bat::Battle& battle);
		friend void mv::SmellingSalts(bat::Battle& battle);
		friend void mv::Strength(bat::Battle& battle);
		friend void mv::Struggle(bat::Battle& battle);
		friend void mv::SuperFang(bat::Battle& battle);
		friend void mv::RevelationDance(bat::Battle& battle);
		friend void mv::Swift(bat::Battle& battle);
		friend void mv::TriAttack(bat::Battle& battle);
		friend void mv::WeatherBall(bat::Battle& battle);
		friend void mv::Acupressure(bat::Battle& battle);
		friend void mv::BellyDrum(bat::Battle& battle);
		friend void mv::Conversion(bat::Battle& battle);
		friend void mv::CourtChange(bat::Battle& battle);
		friend void mv::Disable(bat::Battle& battle);
		friend void mv::Encore(bat::Battle& battle);
		friend void mv::Endure(bat::Battle& battle);
		friend void mv::Glare(bat::Battle& battle);
		friend void mv::HealBell(bat::Battle& battle);
		friend void mv::LaserFocus(bat::Battle& battle);
		friend void mv::LockOn(bat::Battle& battle);
		friend void mv::LuckyChant(bat::Battle& battle);
		friend void mv::MeFirst(bat::Battle& battle);
		friend void mv::Metronome(bat::Battle& battle);
		friend void mv::NaturePower(bat::Battle& battle);
		friend void mv::OdorSleuth(bat::Battle& battle);
		friend void mv::PainSplit(bat::Battle& battle);
		friend void mv::PowerShift(bat::Battle& battle);
		friend void mv::Protect(bat::Battle& battle);
		friend void mv::PsychUp(bat::Battle& battle);
		friend void mv::Regeneration(bat::Battle& battle);
		friend void mv::Safeguard(bat::Battle& battle);
		friend void mv::ShellSmash(bat::Battle& battle);
		friend void mv::Substitute(bat::Battle& battle);
		friend void mv::SweetScent(bat::Battle& battle);
		friend void mv::Wish(bat::Battle& battle);
		friend void mv::DreamEater(bat::Battle& battle);
		friend void mv::Corrode(bat::Battle& battle);
		friend void mv::AncientPower(bat::Battle& battle);
		friend void mv::OminousWind(bat::Battle& battle);
		friend void mv::DisarmingVoice(bat::Battle& battle);
		friend void mv::LightningStrike(bat::Battle& battle);
		friend void mv::Blizzard(bat::Battle& battle);
	};
}