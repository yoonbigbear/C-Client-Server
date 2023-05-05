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
		String name;
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
				t.name = row["name:c"].get<String>();
				t.spawn_pos = row["spawn_pos"].get<Vector<float>>();
					Worlds[t.id] = std::move(t);
			}
		}

	};
}
