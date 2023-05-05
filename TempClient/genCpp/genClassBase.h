//-----------------------------------------
// Auto Generated
//-----------------------------------------


#pragma once
#include "types.h"
#include "csv_parser.h"
namespace Data
{

	enum ClassId : Int32
	{
		None = 0,
		novice = 1,
		traveler = 2,
	};


	struct ClassBase
	{
		Int32 id;
		Int32 classid;
		Int32 hp;
		Int32 damage;
		Single base_speed;
		String desc;
	};


	class ClassBaseTable
	{
	private:
		UnorderedMap<Int32, ClassBase> ClassBases;

	public:
		bool HasKey(Int32 id)
	{
		return ClassBases.contains(id);
	}
		ClassBase& operator[](Int32 id)
	{
		return ClassBases.at(id);
	}

		virtual void Load(StringView path)
		{
			CsvParser reader(path.data());
			for (auto& row : reader)
			{
				ClassBase t;
				t.id = row["id"].get<Int32>();
				t.classid = row["classid"].get<Int32>();
				t.hp = row["hp"].get<Int32>();
				t.damage = row["damage"].get<Int32>();
				t.base_speed = row["base_speed"].get<Single>();
				t.desc = row["desc:c"].get<String>();
					ClassBases[t.id] = std::move(t);
			}
		}

	};
}
