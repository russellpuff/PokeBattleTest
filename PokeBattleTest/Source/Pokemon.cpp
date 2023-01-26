#include <cmath>
#include "Pokemon.h"
#include "EventListener.h"
#include "PBTExceptions.h"
#include "TypeCategory.h"
#include "sqlite3.h"
#include "json.hpp"
sqlite3_stmt* RunQuery(sqlite3* db, std::string q, std::string currentDB);
int CalculateCurrentStats(int base, int ev, int level, bool isHP, float mod);

pkmn::Move::Move(int _moveID) {
	sqlite3* movedb;
	int rc = sqlite3_open("resource\\move.db", &movedb); // Load move.db
	if (rc) {
		std::string e = sqlite3_errmsg(movedb);
		std::string error = "Couldn't open move.db, " + e; // Problem with opening. This is almost always because the db is in use by another application.
		const char* er = error.c_str();
		throw ex::MoveConstructionException(er);
	}

	std::string query = "SELECT NAME, TYPE, CATEGORY, POWER_POINTS, POWER, ACCURACY, ACCURACY_MUTABLE, PRIORITY ";
		query += "FROM MOVE WHERE MOVE_ID = " + std::to_string(_moveID);
	sqlite3_stmt* st = RunQuery(movedb, query, "move.db");
	int tc = sqlite3_step(st); // Shift the results to the first (and only, since the MOVE_ID row is set to unique) row.

	// Check for ok result before doing anything.
	if (tc != SQLITE_DONE && tc != SQLITE_ROW) {
		std::string e = sqlite3_errmsg(movedb);
		std::string error = "Major error encountered when parsing results from move.db, " + e; // Omega fuckery occured. I have no idea what can cause this problem, but I do know it can happen and that it's bad.
		sqlite3_finalize(st);
		const char* er = error.c_str();
		throw ex::MoveConstructionException(const_cast<char*>(error.c_str()));
	}
	if (tc == SQLITE_DONE || sqlite3_column_type(st, 0) == SQLITE_NULL) { // There were no results from the query for some reason. 
		std::string error = "The supplied Move ID was invalid.";
		sqlite3_finalize(st);
		const char* er = error.c_str();
		throw ex::MoveConstructionException(const_cast<char*>(error.c_str()));
	}

	moveID = _moveID;
	name = reinterpret_cast<const char*>(sqlite3_column_text(st, 0));
	std::string foo = reinterpret_cast<const char*>(sqlite3_column_text(st, 1));
	type = tc::strToType.at(foo); // move.db has a foreign key constraint that restricts the type column to one of the existing types, this will never go wrong.
	foo = reinterpret_cast<const char*>(sqlite3_column_text(st, 2));
	category = tc::strToCategory.at(foo); // Same as above.
	powerPoints = sqlite3_column_int(st, 3);
	power = sqlite3_column_int(st, 4);
	accuracy = sqlite3_column_int(st, 5);
	procMutable = sqlite3_column_int(st, 6);
	priority = sqlite3_column_int(st, 7);

	sqlite3_finalize(st);
}

pkmn::Pokemon::Pokemon(jt::JsonPkmn p, jt::JsonTemplate t, std::array<pkmn::Move, 4>& m) : moves(m)
{
	sqlite3* speciesdb;
	int rc = sqlite3_open("resource\\species.db", &speciesdb); // Load species.db
	if (rc) {
		std::string e = sqlite3_errmsg(speciesdb);
		std::string error = "Couldn't open species.db, " + e; // Same as move from above.
		const char* er = error.c_str();
		throw ex::MoveConstructionException(er);
	}

	form = p.Form;
	level = p.Level;
	std::string query;
	if (p.Form == "custom") {
		query = "SELECT NAME FROM SPECIES WHERE DEX_ID = ";
		query += std::to_string(p.DexID);
		sqlite3_stmt* st = RunQuery(speciesdb, query, "species.db");
		sqlite3_step(st);
		name = reinterpret_cast<const char*>(sqlite3_column_text(st, 0));
		sqlite3_finalize(st);

		firstType = tc::strToType.at(t.Type1);
		secondType = tc::strToType.at(t.Type2);
		stats.hpBase = t.HP;
		stats.atkBase = t.ATK;
		stats.defBase = t.DEF;
		stats.spAtkBase = t.SPATK;
		stats.spDefBase = t.SPDEF;
		stats.spdBase = t.SPD;
	}
	else {
		
		if (p.Form == "default") {
			query = "SELECT SPECIES.NAME, SPECIES.TYPE_1, SPECIES.TYPE_2, BASE_STATS.HP, BASE_STATS.ATTACK, BASE_STATS.DEFENSE, ";
			query += "BASE_STATS.SP_ATTACK, BASE_STATS.SP_DEFENSE, BASE_STATS.SPEED from SPECIES INNER JOIN BASE_STATS";
			query += "on SPECIES.DEX_ID = BASE_STATS.DEX_ID and SPECIES.DEX_ID = ";
			query += std::to_string(p.DexID);
		}
		else {
			query = "SELECT SPECIES.NAME, SPECIES_OVERRIDE.TYPE_1, SPECIES_OVERRIDE.TYPE_2,";
			query += "STATS_OVERRIDE.HP, STATS_OVERRIDE.ATTACK, STATS_OVERRIDE.DEFENSE, ";
			query += "STATS_OVERRIDE.SP_ATTACK, STATS_OVERRIDE.SP_DEFENSE, STATS_OVERRIDE.SPEED FROM SPECIES ";
			query += "LEFT JOIN SPECIES_OVERRIDE ON SPECIES.DEX_ID = SPECIES_OVERRIDE.DEX_ID ";
			query += "LEFT JOIN STATS_OVERRIDE ON SPECIES_OVERRIDE.OVERRIDE_ID = STATS_OVERRIDE.OVERRIDE_ID ";
			query += "WHERE SPECIES_OVERRIDE.FORM_NAME = \"";
			query += p.Form + "\"";
		}

		sqlite3_stmt* st = RunQuery(speciesdb, query, "species.db");
		int tc = sqlite3_step(st); // Similar to move, this should only produce a single result. 
		// Check for ok result before doing anything.
		if (tc != SQLITE_DONE && tc != SQLITE_ROW) {
			std::string e = sqlite3_errmsg(speciesdb);
			std::string error = "Major error encountered when parsing results from move.db, " + e;
			sqlite3_finalize(st);
			const char* er = error.c_str();
			throw ex::MoveConstructionException(er);
		}
		if (tc == SQLITE_DONE || sqlite3_column_type(st, 0) == SQLITE_NULL) { // There were no results from the query for some reason. 
			std::string error = "The supplied Dex ID was invalid.";
			sqlite3_finalize(st);
			const char* er = error.c_str();
			throw ex::MoveConstructionException(er);
		}

		name = reinterpret_cast<const char*>(sqlite3_column_text(st, 0));
		firstType = tc::strToType.at(reinterpret_cast<const char*>(sqlite3_column_text(st, 1)));
		stats.hpBase = sqlite3_column_int(st, 3);
		stats.atkBase = sqlite3_column_int(st, 4);
		stats.defBase = sqlite3_column_int(st, 5);
		stats.spAtkBase = sqlite3_column_int(st, 6);
		stats.spDefBase = sqlite3_column_int(st, 7);
		stats.spdBase = sqlite3_column_int(st, 8);

		if (sqlite3_column_type(st, 2) == SQLITE_NULL) { secondType = tc::NoType; }
		else { secondType = tc::strToType.at(reinterpret_cast<const char*>(sqlite3_column_text(st, 2))); }
		sqlite3_finalize(st);
	}

	query = "SELECT INCREASED_STAT, DECREASED_STAT FROM NATURE WHERE NATURE_ID = " + std::to_string(p.Nature);
	sqlite3_stmt* st = RunQuery(speciesdb, query, "species.db");
	sqlite3_step(st);
	std::string inc = reinterpret_cast<const char*>(sqlite3_column_text(st, 0));
	std::string dec = reinterpret_cast<const char*>(sqlite3_column_text(st, 1));
	sqlite3_finalize(st);

	int mods[5] = { 10, 10, 10, 10, 10 };
	std::string stat[5] = { "Attack", "Defense", "Sp. Attack", "Sp. Defense", "Speed" };
	for (int i = 0; i < 5; ++i) {
		if (stat[i] == inc) { mods[i] = 11; continue; }
		if (stat[i] == dec) { mods[i] = 9; continue; }
	}

	// EVs are never read from again so they're not saved. In a more elaborate project, they would be. Nature isn't read from again either.
	stats.hpFinal = stats.hpCurrent = CalculateCurrentStats(stats.hpBase, p.HPEV, level, true, 1.0); // HP is never affected by nature.
	stats.atkFinal = CalculateCurrentStats(stats.atkBase, p.AtkEV, level, false, mods[0]);
	stats.defFinal = CalculateCurrentStats(stats.defBase, p.DefEV, level, false, mods[1]);
	stats.spAtkFinal = CalculateCurrentStats(stats.spAtkBase, p.SpAtkEV, level, false, mods[2]);
	stats.spDefFinal = CalculateCurrentStats(stats.spDefBase, p.SpDefEV, level, false, mods[3]);
	stats.spdFinal = CalculateCurrentStats(stats.spdBase, p.SpdEV, level, false, mods[4]);

	if (name == "Shedinja") { stats.hpFinal = 1; } // Special exception. 
}

pkmn::Stats::Stats() { // Fake constructor because C++ is a load of bullshit. 
	hpBase = atkBase = defBase = spAtkBase = spDefBase = spdBase = hpFinal = hpCurrent =
		atkFinal = defFinal = spAtkFinal = spDefFinal = spdFinal = 1;
}

sqlite3_stmt* RunQuery(sqlite3* db, std::string q, std::string currentDB) {
	sqlite3_stmt* s;
	if (sqlite3_prepare_v2(db, q.c_str(), -1, &s, NULL) != SQLITE_OK)
	{
		std::string e = sqlite3_errmsg(db);
		sqlite3_finalize(s);
		std::string error = "Error reading " + currentDB + ": " + e;
		const char* er = error.c_str();
		throw ex::DatabaseReadException(er);
		return nullptr;
	}
	else { return s; }
}

int CalculateCurrentStats(int base, int ev, int level, bool isHP, float mod)
{
	int res = (((2 * base) + 31 + (ev / 4)) * level) / 100; // https://bulbapedia.bulbagarden.net/wiki/Stat#Generation_III_onward
	res += isHP ? level + 10 : 5;
	if (!isHP) { res = std::floor((res * mod) / 10); }
	return res;
}
