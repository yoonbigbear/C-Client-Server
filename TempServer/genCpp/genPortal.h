//-----------------------------------------
// Auto Generated
//-----------------------------------------


#pragma once
#include "types.h"
#include "csv_parser.h"
namespace Data
{


	struct Portal
	{
		Int32 id;
		UInt32 world;
		UInt32 destination;
		Vector<float> portal_pos;
	};


	class PortalTable
	{
	private:
		UnorderedMap<Int32, Portal> Portals;

	public:
		bool HasKey(Int32 id)
	{
		return Portals.contains(id);
	}
		Portal& operator[](Int32 id)
	{
		return Portals.at(id);
	}

		virtual void Load(StringView path)
		{
			CsvParser reader(path.data());
			for (auto& row : reader)
			{
				Portal t;
				t.id = row["id"].get<Int32>();
				t.world = row["world"].get<UInt32>();
				t.destination = row["destination"].get<UInt32>();
				{
					String str = row["portal_pos"].get<String>();
					std::stringstream ss(str);
					String token;
					while (std::getline(ss, token, ':'))
					{
						t.portal_pos.emplace_back(std::stod(token));
					}
				}
					Portals[t.id] = std::move(t);
			}
		}

	};
}
