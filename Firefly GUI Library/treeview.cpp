#include "treeview.hpp"
#include "resource.hpp"

namespace firefly
{
	void treeview::initialize()
	{
		this->has_checkboxes = false;
		this->root_font = NULL;
		
		this->prev_tree_item = TVI_FIRST;
		this->prev_root_item = NULL;
		this->prev_child_item = NULL;
		
		this->collapsed_button_image = this->make_bitmap_from_png(this->instance(), png_treeview_arrow_collapsed);
		this->collapsible_button_image = this->make_bitmap_from_png(this->instance(), png_treeview_arrow_collapsible);
	}

	treeview::treeview(window& parent_window)
		: widget(parent_window)
	{
		this->initialize();
		this->set_default_message_handlers();
	}

	treeview::treeview(widget& parent_widget)
		: widget(parent_widget)
	{
		this->initialize();
		this->set_default_message_handlers();
	}

	treeview::~treeview()
	{
		if (this->collapsed_button_image)
			DeleteObject(this->collapsed_button_image);

		if (this->collapsible_button_image)
			DeleteObject(this->collapsible_button_image);

		this->clear_items();
	}

	HTREEITEM treeview::add_item(std::string const& caption, tree_level level, bool is_editable, bool has_checkbox, treeview_function_t on_check_function)
	{
		TVINSERTSTRUCT treeview_insert_struct;
		memset(&treeview_insert_struct, 0, sizeof(TVINSERTSTRUCT));
		memset(&treeview_insert_struct.item, 0, sizeof(TVITEM));

		treeview_insert_struct.item.mask = TVIF_TEXT;
		treeview_insert_struct.item.pszText = const_cast<char*>(caption.c_str());
		treeview_insert_struct.item.cchTextMax = caption.length() + 1;
		treeview_insert_struct.hInsertAfter = prev_tree_item;

		switch (level)
		{
		case tree_level::root:
			treeview_insert_struct.hParent = TVI_ROOT;
			break;

		case tree_level::parent:
			treeview_insert_struct.hParent = this->prev_root_item;
			break;

		default:
			treeview_insert_struct.hParent = this->prev_child_item;
			break;
		}

		if ((this->prev_tree_item = TreeView_InsertItem(this->handle(), &treeview_insert_struct)) == NULL)
			return NULL;

		switch (level)
		{
		case tree_level::root:
			this->prev_root_item = this->prev_tree_item;
			break;

		case tree_level::parent:
			TreeView_Expand(this->handle(), this->prev_root_item, TVE_EXPAND);
			this->prev_child_item = this->prev_tree_item;
			break;

		default:
			TreeView_Expand(this->handle(), this->prev_child_item, TVE_EXPAND);
			break;
		}

		treeview_param* param = new treeview_param;
		param->tree_item = this->prev_tree_item;
		param->is_editable = is_editable;
		param->has_checkbox = has_checkbox;
		param->on_check_function = on_check_function;

		treeview_insert_struct.item.mask = TVIF_PARAM;
		treeview_insert_struct.item.hItem = this->prev_tree_item;
		treeview_insert_struct.item.lParam = reinterpret_cast<LPARAM>(param);

		if (!TreeView_SetItem(this->handle(), &treeview_insert_struct.item))
			return NULL;

		return this->prev_tree_item;
	}

	bool treeview::remove_item(HTREEITEM tree_item)
	{
		if (!this->delete_item(tree_item))
			return false;

		return (TreeView_DeleteItem(this->handle(), tree_item) != FALSE);
	}

	bool treeview::clear_items()
	{
		HTREEITEM root_item = TreeView_GetRoot(this->handle());

		if (!root_item)
			return false;

		std::vector<std::pair<HTREEITEM, int>> treeview_nodes;
		this->traverse_all_nodes(root_item, treeview_nodes);

		for (int i = 2; i >= 0; i--)
		{
			for (std::pair<HTREEITEM, int> node : treeview_nodes)
			{
				if (node.second == i)
				{
					if (!this->remove_item(node.first))
						return false;
				}
			}
		}

		return true;
	}

	void treeview::create(rectangle& rect, bool has_checkboxes)
	{
		if (!this->build(WC_TREEVIEW, "", rect, WS_VISIBLE | WS_CHILD | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_EDITLABELS | TVS_INFOTIP | TVS_FULLROWSELECT | (has_checkboxes ? TVS_CHECKBOXES : 0)))
			throw std::exception("Failed to create 'treeview' widget");

		this->has_checkboxes = has_checkboxes;
		this->root_font = this->create_font("Arial", 15, 0, FW_BOLD);
	}

	bool treeview::delete_item(HTREEITEM tree_item)
	{
		TVITEM treeview_item;
		memset(&treeview_item, 0, sizeof(TVITEM));

		treeview_item.mask = TVIF_PARAM;
		treeview_item.hItem = tree_item;

		if (!TreeView_GetItem(this->handle(), &treeview_item))
			return false;

		delete reinterpret_cast<treeview_param*>(treeview_item.lParam);
		return true;
	}

	bool treeview::has_children(HTREEITEM tree_item)
	{
		TVITEM treeview_item;
		memset(&treeview_item, 0, sizeof(TVITEM));

		treeview_item.mask = TVIF_CHILDREN;
		treeview_item.hItem = tree_item;

		if (!TreeView_GetItem(this->handle(), &treeview_item))
			return false;

		return (treeview_item.cChildren > 0);
	}

	void treeview::traverse_all_nodes(HTREEITEM tree_item, std::vector<std::pair<HTREEITEM, int>>& treeview_nodes, int level, std::function<bool(HTREEITEM)> validate_func)
	{
		if (tree_item != NULL)
		{
			if (validate_func(tree_item))
				treeview_nodes.push_back(std::make_pair(tree_item, level));

			if (this->has_children(tree_item))
				this->traverse_all_nodes(TreeView_GetChild(this->handle(), tree_item), treeview_nodes, level + 1);

			return this->traverse_all_nodes(TreeView_GetNextSibling(this->handle(), tree_item), treeview_nodes, level);
		}
	}

	void treeview::set_default_message_handlers()
	{
		this->add_message_handler(WM_ERASEBKGND, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			return 1;
		});

		this->add_message_handler(CUSTOM_NOTIFY, [this](HWND hWnd, WPARAM wParam, LPARAM lParam) -> LRESULT
		{
			NMHDR* custom_notify = reinterpret_cast<NMHDR*>(lParam);

			switch (custom_notify->code)
			{
			case TVN_BEGINLABELEDIT:
				{
					NMTVDISPINFO* treeview_display_info = reinterpret_cast<NMTVDISPINFO*>(lParam);
					treeview_param* param = reinterpret_cast<treeview_param*>(treeview_display_info->item.lParam);

					if (!param->is_editable)
						return TRUE;

					HWND treeview_edit_control = TreeView_GetEditControl(this->handle());

					if (treeview_edit_control)
						SetFocus(treeview_edit_control);
				}
				break;

			case TVN_ENDLABELEDIT:
				{
					NMTVDISPINFO* treeview_display_info = reinterpret_cast<NMTVDISPINFO*>(lParam);

					if (treeview_display_info->item.pszText != NULL)
						return TRUE;
				}
				break;

			case NM_TVSTATEIMAGECHANGING:
				{
					NMTVSTATEIMAGECHANGING* state_image_changing = reinterpret_cast<NMTVSTATEIMAGECHANGING*>(lParam);

					TV_ITEM treeview_item;
					memset(&treeview_item, 0, sizeof(TV_ITEM));

					treeview_item.mask = TVIF_PARAM;
					treeview_item.hItem = state_image_changing->hti;

					if (!TreeView_GetItem(this->handle(), &treeview_item))
						return 0;

					treeview_param* param = reinterpret_cast<treeview_param*>(treeview_item.lParam);

					if (param->has_checkbox && param->on_check_function)
						param->on_check_function(state_image_changing->iNewStateImageIndex == 2);
					else
						TreeView_SelectItem(this->handle(), state_image_changing->hti);

					return 0;
				}

			case NM_CUSTOMDRAW:
				return this->try_custom_draw_item(reinterpret_cast<NMTVCUSTOMDRAW*>(lParam));

			default:
				break;
			}

			return 0;
		});
	}

	LRESULT treeview::try_custom_draw_item(NMTVCUSTOMDRAW* custom_draw)
	{
		if (custom_draw->nmcd.dwDrawStage == CDDS_PREPAINT)
		{
			this->draw_rectangle(&custom_draw->nmcd.rc, this->background());
			this->commit_drawing(custom_draw->nmcd.hdc, &custom_draw->nmcd.rc);
			return CDRF_NOTIFYITEMDRAW;
		}
		else if (custom_draw->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
		{
			treeview_param* param = reinterpret_cast<treeview_param*>(custom_draw->nmcd.lItemlParam);

			if (param)
			{
				if (custom_draw->nmcd.uItemState & CDIS_SELECTED)
					this->draw_rectangle(&custom_draw->nmcd.rc, RGB(60, 70, 80), true, RGB(73, 90, 97));
				else
					this->draw_rectangle(&custom_draw->nmcd.rc, this->background());
				
				/* retrieve treeview item information */
				TV_ITEM treeview_item;
				memset(&treeview_item, 0, sizeof(TV_ITEM));

				char treeview_item_caption[256];
				memset(treeview_item_caption, 0, sizeof(treeview_item_caption));

				treeview_item.mask = TVIF_TEXT | TVIF_CHILDREN | TVIF_STATE;
				treeview_item.pszText = treeview_item_caption;
				treeview_item.cchTextMax = sizeof(treeview_item_caption);
				treeview_item.hItem = param->tree_item;

				if (!TreeView_GetItem(this->handle(), &treeview_item))
					return CDRF_SKIPDEFAULT;

				/* draw expansion-icon */
				if (treeview_item.cChildren > 0)
				{
					RECT rc_expand_button(custom_draw->nmcd.rc);
					rc_expand_button.left += 5 + (19 * custom_draw->iLevel) + ((treeview_item.state & TVIS_EXPANDED) ? 0 : 2);
					rc_expand_button.top += 3 + ((treeview_item.state & TVIS_EXPANDED) ? 2 : 0);
					rc_expand_button.right = rc_expand_button.left + 11;
					rc_expand_button.bottom -= 2;

					this->draw_alpha_bitmap((treeview_item.state & TVIS_EXPANDED) ? this->collapsed_button_image : this->collapsible_button_image, point(rc_expand_button.left, rc_expand_button.top));
				}

				/* draw checkbox */
				if (this->has_checkboxes && param->has_checkbox)
				{
					RECT rc_checkbox(custom_draw->nmcd.rc);
					rc_checkbox.left += 22 + (19 * custom_draw->iLevel);
					rc_checkbox.top += 1;
					rc_checkbox.right = rc_checkbox.left + 16;
					rc_checkbox.bottom -= 1;
								
					this->draw_round_rectangle(&rc_checkbox, size(2, 2), RGB(76, 81, 87), true, RGB(160, 160, 160));

					if (TreeView_GetCheckState(this->handle(), param->tree_item))
					{
						InflateRect(&rc_checkbox, -2, -2);

						HBITMAP bitmap = LoadBitmap(this->instance(), MAKEINTRESOURCE(bmp_checkbox_check));
						this->draw_bitmap(bitmap, point(rc_checkbox.left, rc_checkbox.top));
					}
				}

				/* draw text */
				RECT rc_text(custom_draw->nmcd.rc);
				rc_text.left += 24 + (((this->has_checkboxes && param->has_checkbox) || param->is_editable) ? 16 : 0) + (19 * custom_draw->iLevel);

				this->draw_text(treeview_item_caption, &rc_text, this->foreground(), DT_LEFT | DT_SINGLELINE | DT_VCENTER, custom_draw->iLevel == 0 ? this->root_font : NULL);
				this->commit_drawing(custom_draw->nmcd.hdc, &custom_draw->nmcd.rc);
			}
			
			return CDRF_SKIPDEFAULT;
		}

		return CDRF_DODEFAULT;
	}
}