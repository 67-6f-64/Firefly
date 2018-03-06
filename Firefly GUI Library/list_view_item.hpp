#pragma once	

#include "list_view_base_item.hpp"

namespace firefly
{
	class list_view_item : public list_view_base_item
	{
	public:
		list_view_item();
		~list_view_item();

		template <typename... Args>
		bool bind_data(Args&&... args);

		template <typename T, typename... Args>
		bool bind_helper(int counter, T const& value, Args... args);

		bool bind_helper(int index, std::string const& value);
		bool bind_helper(int index, char const* value);
		bool bind_helper(int index, int value);

	public:
		std::vector<std::string const> subitems;
	};
	
	template <typename... Args>
	bool list_view_item::bind_data(Args&&... args)
	{
		return this->bind_helper(0, args...);
	}
		
	template <typename T, typename... Args>
	bool list_view_item::bind_helper(int index, T const& value, Args... args)
	{
		return (this->bind_helper(index, value) && this->bind_helper(index + 1, args...));
	}
}