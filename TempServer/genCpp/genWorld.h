//-----------------------------------------
// Auto Generated
//-----------------------------------------


#pragma once
#include "types.h"
#include "csv_parser.h"
namespace Data
{


	struct World
	{
		Int32 id;
		Vector<float> spawn_pos;
	};


	class WorldTable
	{
	private:
		UnorderedMap<Int32, World> Worlds;

	public:
		bool HasKey(Int32 id)
	{
		return Worlds.contains(id);
	}
		World& operator[](Int32 id)
	{
		return Worlds.at(id);
	}

		virtual void Load(StringView path)
		{
			CsvParser reader(path.data());
			for (auto& row : reader)
			{
				World t;
				t.id = row["id"].get<Int32>();
				{
					String str = row["spawn_pos"].get<String>();
					std::stringstream ss(str);
					String token;
					while (std::getline(ss, token, ':'))
					{
						t.spawn_pos.emplace_back(std::stod(token));
					}
				}
					Worlds[t.id] = std::move(t);
			}
		}

	};
}
