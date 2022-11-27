#pragma once
#include <string>

namespace ex {
	class PokemonConstructionException : public std::exception {
	private:
		const char* message;
	public:
		const char* what() { return message; }
		PokemonConstructionException(const char* _msg) : message(_msg) {}
	};

	class MoveConstructionException : public std::exception {
	private:
		const char* message;
	public:
		const char* what() { return message; }
		MoveConstructionException(const char* _msg) : message(_msg) {}
	};

	class StatsConstructionException : public std::exception {
	private:
		const char* message;
	public:
		const char* what() { return message; }
		StatsConstructionException(const char* _msg) : message(_msg) {}
	};

	class DatabaseReadException : public std::exception {
	private:
		const char* message;
	public:
		const char* what() { return message; }
		DatabaseReadException(const char* _msg) : message(_msg) {}
	};
}