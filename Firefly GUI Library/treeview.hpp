#pragma once

#include "native.hpp"
#include "widget.hpp"

#include <vector>

namespace firefly
{
	typedef std::function<void(bool)> treeview_function_t;
	
	enum class tree_level
	{
		root,
		parent,
		child
	};
	
	struct treeview_param
	{
		HTREEITEM tree_item;

		bool is_editable;
		bool has_checkbox;
		treeview_function_t on_check_function;
	};

	class treeview : public widget
	{
		void initialize();

	public:
		treeview(window& parent_window);
		treeview(widget& parent_widget);
		~treeview();

		HTREEITEM add_item(std::string const& caption, tree_level level, bool is_editable = false, bool has_checkbox = false, treeview_function_t on_check_function = [](bool) -> void {  });
		bool remove_item(HTREEITEM tree_item);
		bool clear_items();

		void create(rectangle& rect = rectangle(), bool has_checkboxes = false);

	private:
		bool delete_item(HTREEITEM tree_item);
		bool has_children(HTREEITEM tree_item);
		void traverse_all_nodes(HTREEITEM tree_item, std::vector<std::pair<HTREEITEM, int>>& treeview_nodes, int level = 0, std::function<bool(HTREEITEM)> validate_func = [](HTREEITEM) -> bool { return true; });

	private:
		void set_default_message_handlers();

		LRESULT try_custom_draw_item(NMTVCUSTOMDRAW* custom_draw);

	private:
		bool has_checkboxes;
		HFONT root_font;

		HTREEITEM prev_tree_item;
		HTREEITEM prev_root_item;
		HTREEITEM prev_child_item;

		HBITMAP collapsed_button_image;
		HBITMAP collapsible_button_image;
	};
}