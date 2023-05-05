//-----------------------------------------
// Auto Generated
//-----------------------------------------


#pragma once
#include "types.h"
#include "csv_parser.h"
namespace Data
{


	struct Drop
	{
		Int32 id;
		UInt32 item_tid;
		Int32 min;
		Int32 max;
	};


	class DropTable
	{
	private:
		UnorderedMap<Int32, Drop> Drops;

	public:
		bool HasKey(Int32 id)
	{
		return Drops.contains(id);
	}
		Drop& operator[](Int32 id)
	{
		return Drops.at(id);
	}

		virtual void Load(StringView path)
		{
			CsvParser reader(path.data());
			for (auto& row : reader)
			{
				Drop t;
				t.id = row["id"].get<Int32>();
				t.item_tid = row["item_tid"].get<UInt32>();
				t.min = row["min"].get<Int32>();
				t.max = row["max"].get<Int32>();
					Drops[t.id] = std::move(t);
			}
		}

	};
}
