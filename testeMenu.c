
#include <curses.h>
#include <menu.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

char *choices[] = {
                        "Choice 1",
                        "Choice 2",
                        "Choice 3",
                        "Choice 4",
			"Choice 5",
			"Choice 6",
			"Choice 7",
                        "Exit",
                  };
void func(char *name);


	ITEM **my_items;
	int c;				
	MENU *my_menu;
        int n_choices, i;
	ITEM *cur_item;
	bool menu=false;

void inicializaItems(){

	if(!menu)
		menu=true;
	else
		unpost_menu(my_menu);

	/* Initialize items */
        n_choices = ARRAY_SIZE(choices);
        my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
        for(i = 0; i < n_choices; ++i)
	{       my_items[i] = new_item(choices[i], choices[i]);
		/* Set the user pointer */
		set_item_userptr(my_items[i], func);
	}
	my_items[n_choices] = (ITEM *)NULL;

}


void inicializaItems_FOOL(){

char *choicesFOOL[] = {
                        "A",
                        "B",
                        "C",
                  };
	if(!menu)
		menu=true;
	else
		unpost_menu(my_menu);
	/* Initialize items */
        n_choices = ARRAY_SIZE(choicesFOOL);
        my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
        for(i = 0; i < n_choices; ++i)
	{       my_items[i] = new_item(choicesFOOL[i], choicesFOOL[i]);
		/* Set the user pointer */
		set_item_userptr(my_items[i], func);
	}
	my_items[n_choices] = (ITEM *)NULL;

}

void criaMenu(){

	/* Initialize curses */	
	initscr();
	start_color();
        cbreak();
        noecho();
	keypad(stdscr, TRUE);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);

	/* Create menu */
	my_menu = new_menu((ITEM **)my_items);

	/* Post the menu */
	mvprintw(LINES - 3, 0, "Press <ENTER> to see the option selected");
	mvprintw(LINES - 2, 0, "Up and Down arrow keys to naviage (F1 to Exit)");
	post_menu(my_menu);
	refresh();

}

int main()
{	

inicializaItems();
criaMenu();

	while((c = getch()) != KEY_F(1))
	{       switch(c)
	        {	case KEY_DOWN:
				menu_driver(my_menu, REQ_DOWN_ITEM);
//----------------------------------------------------------------------------------------------------------------------------------------------------------------
				/*unpost_menu(my_menu);
				ITEM **my_new_items;
				my_new_items = (ITEM **)calloc(1 + 1, sizeof(ITEM *));
				my_new_items[0] = new_item("TesteA", "issa");
				my_new_items[1] = new_item("Teste", "Blz");
				set_item_userptr(my_new_items[0], func);
				set_item_userptr(my_new_items[1], func);
	my_new_items[n_choices] = (ITEM *)NULL;
	my_menu = new_menu((ITEM **)my_new_items);
	mvprintw(LINES - 3, 0, "Press <ENTER> to see the option selected");
	mvprintw(LINES - 2, 0, "Up and Down arrow keys to naviage (F1 to Exit)");
				post_menu(my_menu);
				refresh();*/


//----------------------------------------------------------------------------------------------------------------------------------------------------------------
				break;
			case KEY_UP:


inicializaItems_FOOL();
criaMenu();
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
			case 10: /* Enter */
			{	ITEM *cur;
				void (*p)(char *);




				cur = current_item(my_menu);
				p = item_userptr(cur);
				p((char *)item_name(cur));
				pos_menu_cursor(my_menu);
				break;
			}
			break;
		}
	}	

	unpost_menu(my_menu);
	for(i = 0; i < n_choices; ++i)
		free_item(my_items[i]);
	free_menu(my_menu);
	endwin();
}

void func(char *name)
{	move(20, 0);
	clrtoeol();
	mvprintw(20, 0, "Item selected is : %s", name);
}	
