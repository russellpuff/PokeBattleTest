#include "Pokemon.h"
#include "EventListener.h"
#include "PBTExceptions.h"
#include "TypeFixer.h"
#include "sqlite3.h"
sqlite3_stmt* RunQuery(sqlite3* db, std::string q, std::string currentDB);

mon::Move::Move(int _moveID) {
	sqlite3* movedb;
	int rc = sqlite3_open("/resource/move.db", &movedb); // Load move.db
	if (rc) {
		std::string e = sqlite3_errmsg(movedb);
		std::string error = "Couldn't open move.db, " + e; // Problem with opening. This is almost always because the db is in use by another application.
		throw ex::MoveConstructionException(const_cast<char*>(error.c_str()));
	}

	std::string query = "SELECT * FROM MOVE WHERE MOVE_ID = " + std::to_string(_moveID);
	sqlite3_stmt* st = RunQuery(movedb, query, "move.db");
	int tc = sqlite3_step(st); // Shift the results to the first (and only, since the MOVE_ID row is set to unique) row.

	// Check for ok result before doing anything.
	if (tc != SQLITE_DONE && tc != SQLITE_ROW) {
		std::string e = sqlite3_errmsg(movedb);
		std::string error = "Major error encountered when parsing results from move.db, " + e; // Omega fuckery occured. I have no idea what can cause this problem, but I do know it can happen and that it's bad.
		sqlite3_finalize(st);
		throw ex::MoveConstructionException(const_cast<char*>(error.c_str()));
	}
	if (tc == SQLITE_DONE || sqlite3_column_type(st, 0) == SQLITE_NULL) { // There were no results from the query for some reason. 
		std::string error = "The supplied Move ID was invalid.";
		sqlite3_finalize(st);
		throw ex::MoveConstructionException(const_cast<char*>(error.c_str()));
	}

	moveID = _moveID;
	name = reinterpret_cast<const char*>(sqlite3_column_text(st, 1));
	std::string foo = reinterpret_cast<const char*>(sqlite3_column_text(st, 2));
	type = tf::strToType[foo]; // move.db has a foreign key constraint that restricts the type column to one of the existing types, this will never go wrong.
	foo = reinterpret_cast<const char*>(sqlite3_column_text(st, 3));
	category = tf::stfToCategory[foo]; // Same as above.
	powerPoints = reinterpret_cast<int>(sqlite3_column_text(st, 4));
	power = reinterpret_cast<int>(sqlite3_column_text(st, 5));
	accuracy = reinterpret_cast<int>(sqlite3_column_text(st, 6));
	priority = reinterpret_cast<int>(sqlite3_column_text(st, 7));

	sqlite3_finalize(st);
}

mon::Pokemon::Pokemon(bool player) { // player == true means the pokemon being loaded is the players, false is the opponent. This application only cares about those two.
	// load base file
	// check if template data is used
	// if template comes from db, load db and grab template data
	// if template comes from user, load template file
	

	form = "default";
}

mon::Stats::Stats(int _dexID) {
	// Get stats from db by dexID
	sqlite3* speciesdb;
	int rc = sqlite3_open("/resource/species.db", &speciesdb); // Load move.db
	if (rc) {
		std::string e = sqlite3_errmsg(speciesdb);
		std::string error = "Couldn't open species.db, " + e; // Problem with opening. This is almost always because the db is in use by another application.
		throw ex::StatsConstructionException(const_cast<char*>(error.c_str()));
	}
}

mon::Stats::Stats() { // Fake constructor because C++ is a load of bullshit. 
	hpBase = atkBase = defBase = spAtkBase = spDefBase = spdBase = hpCurrent = 
		atkCurrent = defCurrent = spAtkCurrent = spDefCurrent = spdCurrent = 1;
}

mon::Move::Move() {
	moveID = 0;
	name = "NoMove";
	type = Type::Fairy;
	category = MoveCategory::Status;
	powerPoints = power = accuracy = priority = 0;
}

sqlite3_stmt* RunQuery(sqlite3* db, std::string q, std::string currentDB) {
	sqlite3_stmt* s;
	if (sqlite3_prepare_v2(db, q.c_str(), -1, &s, NULL) != SQLITE_OK)
	{
		std::string e = sqlite3_errmsg(db);
		sqlite3_finalize(s);
		std::string error = "Error reading " + currentDB + ": " + e;
		throw ex::DatabaseReadException(const_cast<char*>(error.c_str()));
		return nullptr;
	}
	else { return s; }
}