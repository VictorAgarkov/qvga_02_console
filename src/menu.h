#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

	#include <stdint.h>

	#define MENU_FLAG_SEL  0x40000000  // selected (focused) item
	#define MENU_FLAG_DIS  0x20000000  // disabled item
	#define MENU_MAX_DEPTH 8           // maximum level of menu depth

	enum
	{
		// menu item type
		MENU_NONE = 0,
		MENU_SUBMENU,
		MENU_ACTION,
		MENU_SEPARATOR,

	};


	enum
	{
		// menu walk direction
		MENU_UP = -1,
		MENU_DOWN = 1,
		MENU_GO = 2,
		MENU_BACK = 0
	};

	struct menu_item
	{
		int   type;    // MENU_NONE, MENU_SUBMENU, ...
		char *text;    // text of menu item
		void *action;  // depending of type: pointer to submenu "struct menu_def *", procedure "int  (*MENU_ACT_PROC)(int arg)", etc..
		int   act_arg; // action some argument
	};

	struct menu_def
	{
		struct menu_item *items;
		int    num;
		//char   *title;
	};

	struct menu_stack
	{
		struct menu_def *parent;
		int idx;
	};


	typedef void (*MENU_DRAW_PROC)(struct menu_item *item, uint32_t flags, int item_pos);
	typedef int  (*MENU_ACT_PROC)(int arg);


	void menu_init(struct menu_def *menu, int maxstr, MENU_DRAW_PROC proc);
	int menu_walk(int direction);




















#endif /* MENU_H_INCLUDED */
