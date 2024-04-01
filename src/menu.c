#include "menu.h"
#include <stdlib.h>


int menu_maxstr, menu_top_idx;
MENU_DRAW_PROC menu_draw_proc = NULL;

struct menu_stack g_MenuStack[MENU_MAX_DEPTH];
int menu_curr_level = -1;

//------------------------------------------------------------------------------
//internal prototpes

int menu_push(struct menu_def *menu);
int menu_pop(int num);
int menu_go();
uint32_t menu_get_DIS_flag(struct menu_item *item);
uint32_t menu_get_SEL_flag(int idx);
void menu_repaint(int previous);
void menu_updown(int direction);

//------------------------------------------------------------------------------
void menu_init(struct menu_def *menu, int maxstr, MENU_DRAW_PROC proc)
{
	// menu - ptr to main menu
	// maxstr - maximum strings (items) on screen
	// proc - callback to draw 1 menu item

	menu_curr_level = -1;

	if(menu_push(menu)) return;

	menu_maxstr = maxstr;
	menu_draw_proc = proc;
}
//------------------------------------------------------------------------------
int menu_push(struct menu_def *menu)
{
	// posh current menu and set submenu

	if(menu_curr_level >= (MENU_MAX_DEPTH - 1))
	{
		 return 1;
	}
	menu_curr_level++;

	g_MenuStack[menu_curr_level].parent = menu;
	g_MenuStack[menu_curr_level].idx = 0;
	menu_top_idx = 0;
	return 0;
}
//------------------------------------------------------------------------------
int menu_pop(int num)
{
	// return from current menu to parent
	if((menu_curr_level - num) >= -1)
	{
		menu_curr_level -= num;
		return 0;
	}
	else
	{
		return 1;
	}
}
//------------------------------------------------------------------------------
int menu_go()
{
	if(menu_curr_level < 0) return 1;

	int ret = 0;
	struct menu_def *sub;
	MENU_ACT_PROC ap;
	struct menu_item * item = g_MenuStack[menu_curr_level].parent->items + g_MenuStack[menu_curr_level].idx;
	switch(item->type)
	{
		case MENU_SUBMENU:
			sub = (struct menu_def *)item->action;
			if(sub)
			{
				ret = menu_push(sub);
				if(!ret)
				{
					menu_repaint(-1);
				}
			}
		break;
		case MENU_ACTION:
			ap = (MENU_ACT_PROC)item->action;
			if(ap)
			{
				// run action procedure
				if(ap(item->act_arg))
				{
					menu_repaint(-1);
				}
			}
		break;
	}
	return ret;
}
//------------------------------------------------------------------------------
uint32_t menu_get_DIS_flag(struct menu_item *item)
{
	return (item->type == MENU_SUBMENU || item->type == MENU_ACTION) ? 0 : MENU_FLAG_DIS;
}
//------------------------------------------------------------------------------
uint32_t menu_get_SEL_flag(int idx)
{
	return (g_MenuStack[menu_curr_level].idx == idx) ? MENU_FLAG_SEL : 0;
}
//------------------------------------------------------------------------------
void menu_repaint(int previous)
{
	// repaint manu items
	// depend force:
	// >= 0  - only current (g_MenuStack[menu_curr_level].idx) and prevous (previous) items
	// < 0   - all items on screen
	struct menu_item *item;
	uint32_t flag, i;


	if(!menu_draw_proc) return;

	if(previous < 0)
	{
		item = g_MenuStack[menu_curr_level].parent->items + menu_top_idx;
		for(i = 0; i < menu_maxstr; i++)
		{
			uint32_t flag = menu_get_DIS_flag(item) | menu_get_SEL_flag(i + menu_top_idx);
			menu_draw_proc(item, flag, i);
			item++;
		}
	}
	else
	{
		item = g_MenuStack[menu_curr_level].parent->items + previous;
		flag = menu_get_DIS_flag(item);
		menu_draw_proc(item, flag, previous - menu_top_idx);  // redraw unselected previous item

		i = g_MenuStack[menu_curr_level].idx;
		item = g_MenuStack[menu_curr_level].parent->items + i;
		flag = menu_get_DIS_flag(item) | MENU_FLAG_SEL;
		menu_draw_proc(item, MENU_FLAG_SEL, i - menu_top_idx);  // redraw selected new item
	}
}
//------------------------------------------------------------------------------
void menu_updown(int direction)
{
	if(menu_curr_level < 0) return;

	int new_top_idx = menu_top_idx;
	int prev_item_idx, new_item_idx;
	struct menu_item *item;

	prev_item_idx = new_item_idx = g_MenuStack[menu_curr_level].idx;

	for(;;)
	{
		new_item_idx += direction;
		if(new_item_idx < 0 || new_item_idx >= g_MenuStack[menu_curr_level].parent->num)
		{
			return;  // out of menu range
		}

		item = g_MenuStack[menu_curr_level].parent->items + new_item_idx;
		if(menu_get_DIS_flag(item) == 0)
		{
			break; // found!
		}
	}
	if(new_item_idx < menu_top_idx) new_top_idx = new_item_idx;
	if(new_item_idx >= (menu_top_idx + menu_maxstr)) new_top_idx = new_item_idx - menu_maxstr + 1;

	g_MenuStack[menu_curr_level].idx = new_item_idx;

	if(new_top_idx == menu_top_idx)
	{
		menu_repaint(prev_item_idx);//  repaint unselected prev & selected new
	}
	else
	{
		menu_top_idx = new_top_idx;
		menu_repaint(-1); // repaint all
	}
}
//------------------------------------------------------------------------------
int menu_walk(int direction)
{
	if(menu_curr_level < 0) return 1;

	int ret = 0;
	switch(direction)
	{
		case MENU_UP:
		case MENU_DOWN:
			menu_updown(direction);
		break;

		case MENU_GO:
			ret = menu_go();
		break;

		case MENU_BACK:
			ret = menu_pop(1);
			if(!ret)
			{
				menu_repaint(1);
			}
		break;

		default:
			ret = 1;
		break;
	}
	return ret;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
