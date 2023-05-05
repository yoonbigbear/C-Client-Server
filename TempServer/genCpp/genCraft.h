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
				{
					String str = row["ingredients"].get<String>();
					std::stringstream ss(str);
					String token;
					while (std::getline(ss, token, ':'))
					{
						t.ingredients.emplace_back(std::stod(token));
					}
				}
				{
					String str = row["counts"].get<String>();
					std::stringstream ss(str);
					String token;
					while (std::getline(ss, token, ':'))
					{
						t.counts.emplace_back(std::stod(token));
					}
				}
					Crafts[t.id] = std::move(t);
			}
		}

	};
}
