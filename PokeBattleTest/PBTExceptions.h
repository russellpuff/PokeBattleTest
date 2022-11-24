#pragma once
#include <string>

namespace ex {
	class PokemonConstructionException : public std::exception {
	private:
		char* message;
	public:
		char* what() { return message; }
		PokemonConstructionException(char* _msg) : message(_msg) {}
	};

	class MoveConstructionException : public std::exception {
	private:
		char* message;
	public:
		char* what() { return message; }
		MoveConstructionException(char* _msg) : message(_msg) {}
	};

	class StatsConstructionException : public std::exception {
	private:
		char* message;
	public:
		char* what() { return message; }
		StatsConstructionException(char* _msg) : message(_msg) {}
	};

	class DatabaseReadException : public std::exception {
	private:
		char* message;
	public:
		char* what() { return message; }
		DatabaseReadException(char* _msg) : message(_msg) {}
	};
}