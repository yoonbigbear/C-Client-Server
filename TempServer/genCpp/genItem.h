//-----------------------------------------
// Auto Generated
//-----------------------------------------


#pragma once
#include "types.h"
#include "csv_parser.h"
namespace Data
{

	enum Usage : SByte
	{
		None = 0,
		weapon = 1,
		defence = 2,
		consume = 3,
		accessory = 4,
		misc = 5,
	};

	enum Level : SByte
	{
		None = 0,
		normal = 1,
		magic = 2,
		rare = 3,
		unique = 4,
		legend = 5,
	};


	struct Item
	{
		Int32 id;
		Int32 maxstack;
		SByte level;
		SByte usage;
		String name;
	};


	class ItemTable
	{
	private:
		UnorderedMap<Int32, Item> Items;

	public:
		bool HasKey(Int32 id)
	{
		return Items.contains(id);
	}
		Item& operator[](Int32 id)
	{
		return Items.at(id);
	}

		virtual void Load(StringView path)
		{
			CsvParser reader(path.data());
			for (auto& row : reader)
			{
				Item t;
				t.id = row["id"].get<Int32>();
				t.maxstack = row["maxstack"].get<Int32>();
				t.level = row["level"].get<SByte>();
				t.usage = row["usage"].get<SByte>();
				t.name = row["name"].get<String>();
					Items[t.id] = std::move(t);
			}
		}

	};
}
