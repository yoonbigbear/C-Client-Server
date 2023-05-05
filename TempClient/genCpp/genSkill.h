//-----------------------------------------
// Auto Generated
//-----------------------------------------


#pragma once
#include "types.h"
#include "csv_parser.h"
namespace Data
{

	enum skilltype : Int32
	{
		None = 0,
		active = 0,
		end = 1,
	};


	struct Skill
	{
		Int32 id;
		Int32 skilltype;
		Single max_range;
		Int32 halfangle;
		Int64 damage;
		Int32 cooldown;
		Int32 ani_time;
		Int32 hit_time;
		String name;
	};


	class SkillTable
	{
	private:
		UnorderedMap<Int32, Skill> Skills;

	public:
		bool HasKey(Int32 id)
	{
		return Skills.contains(id);
	}
		Skill& operator[](Int32 id)
	{
		return Skills.at(id);
	}

		virtual void Load(StringView path)
		{
			CsvParser reader(path.data());
			for (auto& row : reader)
			{
				Skill t;
				t.id = row["id"].get<Int32>();
				t.skilltype = row["skilltype"].get<Int32>();
				t.max_range = row["max_range"].get<Single>();
				t.halfangle = row["halfangle"].get<Int32>();
				t.damage = row["damage"].get<Int64>();
				t.cooldown = row["cooldown"].get<Int32>();
				t.ani_time = row["ani_time"].get<Int32>();
				t.hit_time = row["hit_time"].get<Int32>();
				t.name = row["name:c"].get<String>();
					Skills[t.id] = std::move(t);
			}
		}

	};
}
