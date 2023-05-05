//-----------------------------------------
// Auto Generated
//-----------------------------------------


#pragma once
#include "types.h"
#include "csv_parser.h"
namespace Data
{


	struct Debug
	{
		Int32 id;
		SByte collider_type;
		Single half_width;
		Single half_length;
		Int16 half_angle;
	};


	class DebugTable
	{
	private:
		UnorderedMap<Int32, Debug> Debugs;

	public:
		bool HasKey(Int32 id)
	{
		return Debugs.contains(id);
	}
		Debug& operator[](Int32 id)
	{
		return Debugs.at(id);
	}

		virtual void Load(StringView path)
		{
			CsvParser reader(path.data());
			for (auto& row : reader)
			{
				Debug t;
				t.id = row["id"].get<Int32>();
				t.collider_type = row["collider_type"].get<SByte>();
				t.half_width = row["half_width"].get<Single>();
				t.half_length = row["half_length"].get<Single>();
				t.half_angle = row["half_angle"].get<Int16>();
					Debugs[t.id] = std::move(t);
			}
		}

	};
}
