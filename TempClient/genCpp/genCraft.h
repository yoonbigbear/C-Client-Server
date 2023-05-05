//-----------------------------------------
// Auto Generated
//-----------------------------------------


#pragma once
#include "types.h"
#include "csv_parser.h"
namespace Data
{


	struct Craft
	{
		Int32 id;
		Vector<uint32_t> ingredients;
		Vector<int32_t> counts;
		String resource;
	};


	class CraftTable
	{
	private:
		UnorderedMap<Int32, Craft> Crafts;

	public:
		bool HasKey(Int32 id)
	{
		return Crafts.contains(id);
	}
		Craft& operator[](Int32 id)
	{
		return Crafts.at(id);
	}

		virtual void Load(StringView path)
		{
			CsvParser reader(path.data());
			for (auto& row : reader)
			{
				Craft t;
				t.id = row["id"].get<Int32>();
				t.ingredients = row["ingredients"].get<Vector<uint32_t>>();
				t.counts = row["counts"].get<Vector<int32_t>>();
				t.resource = row["resource:c"].get<String>();
					Crafts[t.id] = std::move(t);
			}
		}

	};
}
