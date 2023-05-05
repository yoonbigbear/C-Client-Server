//-----------------------------------------
// Auto Generated
//-----------------------------------------


#pragma once
#include "types.h"
#include "csv_parser.h"
namespace Data
{


	struct DropGroup
	{
		Int32 id;
		UInt32 groupid;
		Int32 group_chance;
		UInt32 drop_tid;
		Int32 possibility;
	};


	class DropGroupTable
	{
	private:
		UnorderedMap<Int32, DropGroup> DropGroups;

	public:
		bool HasKey(Int32 id)
	{
		return DropGroups.contains(id);
	}
		DropGroup& operator[](Int32 id)
	{
		return DropGroups.at(id);
	}

		virtual void Load(StringView path)
		{
			CsvParser reader(path.data());
			for (auto& row : reader)
			{
				DropGroup t;
				t.id = row["id"].get<Int32>();
				t.groupid = row["groupid"].get<UInt32>();
				t.group_chance = row["group_chance"].get<Int32>();
				t.drop_tid = row["drop_tid"].get<UInt32>();
				t.possibility = row["possibility"].get<Int32>();
					DropGroups[t.id] = std::move(t);
			}
		}

	};
}
