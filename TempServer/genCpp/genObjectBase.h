//-----------------------------------------
// Auto Generated
//-----------------------------------------


#pragma once
#include "types.h"
#include "csv_parser.h"
namespace Data
{


	struct ObjectBase
	{
		Int32 id;
		Int64 max_hp;
		Int64 damage;
		Int64 exp;
		Single base_speed;
		Vector<uint32_t> drop_table;
	};


	class ObjectBaseTable
	{
	private:
		UnorderedMap<Int32, ObjectBase> ObjectBases;

	public:
		bool HasKey(Int32 id)
	{
		return ObjectBases.contains(id);
	}
		ObjectBase& operator[](Int32 id)
	{
		return ObjectBases.at(id);
	}

		virtual void Load(StringView path)
		{
			CsvParser reader(path.data());
			for (auto& row : reader)
			{
				ObjectBase t;
				t.id = row["id"].get<Int32>();
				t.max_hp = row["max_hp"].get<Int64>();
				t.damage = row["damage"].get<Int64>();
				t.exp = row["exp"].get<Int64>();
				t.base_speed = row["base_speed"].get<Single>();
				{
					String str = row["drop_table"].get<String>();
					std::stringstream ss(str);
					String token;
					while (std::getline(ss, token, ':'))
					{
						t.drop_table.emplace_back(std::stod(token));
					}
				}
					ObjectBases[t.id] = std::move(t);
			}
		}

	};
}
