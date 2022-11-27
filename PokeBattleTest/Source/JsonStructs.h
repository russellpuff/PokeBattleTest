#pragma once
#include <string>
#include <json.hpp>
namespace jt {
	struct JsonPkmn {
		int DexID;
		int Level;
		std::string Form;
		int Nature;
		int Move1;
		int Move2;
		int Move3;
		int Move4;
		int HPEV;
		int AtkEV;
		int DefEV;
		int SpAtkEV;
		int SpDefEV;
		int SpdEV;
		JsonPkmn(int _d, int _l, std::string _f, int _n, int _m1, int _m2, int _m3, int _m4,
			int _hev, int _aev, int _dev, int _saev, int _sdev, int _sev) :
			DexID(_d), Level(_l), Form(_f), Nature(_n), 
			Move1(_m1), Move2(_m2), Move3(_m3), Move4(_m4),
			HPEV(_hev), AtkEV(_aev), DefEV(_dev), 
			SpAtkEV(_saev), SpDefEV(_sdev), SpdEV(_sev) {}
	};

	struct JsonTemplate {
		std::string Type1;
		std::string Type2;
		int HP;
		int ATK;
		int DEF;
		int SPATK;
		int SPDEF;
		int SPD;
		JsonTemplate(std::string _t1, std::string _t2, int _h, int _a, int _d, int _sa, int _sd, int _s) :
			Type1(_t1), Type2(_t2), 
			HP(_h), ATK(_a), DEF(_d), 
			SPATK(_sa), SPDEF(_sd), SPD(_s) {}

	};
}

namespace nlohmann { // Stuff for json deserialization. 
	template <>
	struct adl_serializer<jt::JsonPkmn>
	{
		static jt::JsonPkmn from_json(const json& j) {
			return { j.at("DexID"), 
				j.at("Level"), 
				j.at("Form"), 
				j.at("Nature"), 
				j.at("Move1"), j.at("Move2"), j.at("Move3"), j.at("Move4"),
			j.at("HPEV"), j.at("AtkEV"), j.at("DefEV"), j.at("SpAtkEV"), j.at("SpDefEV"), j.at("SpdEV") };
		}
	};
	template <>
	struct adl_serializer<jt::JsonTemplate>
	{
		static jt::JsonTemplate from_json(const json& j) {
			return { 
				j.at("Type1"), j.at("Type2"), 
				j.at("HP"), j.at("ATK"), j.at("DEF"),
				j.at("SPATK"), j.at("SPDEF"), j.at("SPD") };
		}
	};
}