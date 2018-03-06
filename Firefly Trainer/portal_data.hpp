#pragma once

#include "basic_types.hpp"

#include <string>
#include <vector>

namespace firefly
{
	class portal_data
	{
		friend std::ostream& operator <<(std::ostream& stream, portal_data const& portal);

		enum portal_type : int
		{
			PLACEHOLDER = 0, 		//no image, no use
			INVISIBLE, 				//no image, can use
			NORMAL, 				//has image, can use
			AUTOMATIC, 				//no image, automatically use
			STOCK_BLOCKING, 		//has image, no use with block
			USELESS_BLOCKING, 		//no image, no use with block
			USELESS, 				//no image, no use
			NORMAL_ALT, 			//has image, can enter
			INVISIBLE_SCRIPTED,		//no image, can use with script
			INVISIBLE_BLOCKING,		//no image, can use with block
			HIDDEN, 				//no image, can enter
			HIDDEN_ALT 				//no image, can enter
		};
		
		//enum $88DAE3A1145CB3CBA9F0BA5DCF0F0966
		//{
		//	PORTALTYPE_STARTPOINT = 0x0,
		//	PORTALTYPE_INVISIBLE = 0x1,
		//	PORTALTYPE_VISIBLE = 0x2,
		//	PORTALTYPE_COLLISION = 0x3,
		//	PORTALTYPE_CHANGABLE = 0x4,
		//	PORTALTYPE_CHANGABLE_INVISIBLE = 0x5,
		//	PORTALTYPE_TOWNPORTAL_POINT = 0x6,
		//	PORTALTYPE_SCRIPT = 0x7,
		//	PORTALTYPE_SCRIPT_INVISIBLE = 0x8,
		//	PORTALTYPE_COLLISION_SCRIPT = 0x9,
		//	PORTALTYPE_HIDDEN = 0xA,
		//	PORTALTYPE_SCRIPT_HIDDEN = 0xB,
		//	PORTALTYPE_COLLISION_VERTICAL_JUMP = 0xC,
		//	PORTALTYPE_COLLISION_CUSTOM_IMPACT = 0xD,
		//};

	public:
		portal_data(int id, int map, std::vector<std::string>& lines);
		~portal_data();

		bool good() const;

		int id() const;
		int map_id() const;
		int to_map() const;

		std::string const& from() const;
		std::string const& to() const;

		portal_type type() const;
		point const& position() const;

		operator std::string const() const;

	private:
		unsigned int id_;
		unsigned int map_id_;
		unsigned int to_map_;

		std::string from_;
		std::string to_;

		portal_type type_;
		point position_;
	};
}