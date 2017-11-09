//gcc manager.c menuDaPlataforma.c -o man -lmenu -lncurses


#include <curses.h>
#include <menu.h>

#include "menuDaPlataforma.h"

void fecharMenu(){

	unpost_menu(my_menu);
	for(i = 0; i < n_choices; ++i)
		free_item(my_items[i]);
	free_menu(my_menu);
	endwin();
}

void inicializaItems(){

	if(!menu)
		menu=true;
	else
		unpost_menu(my_menu);

	/* Initialize items */
        n_choices = ARRAY_SIZE(choices);
        my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
        for(i = 0; i < n_choices; ++i)
	{
	        my_items[i] = new_item(choices[i],"");
		/* Set the user pointer */
		set_item_userptr(my_items[i], trataMenu);
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
	//mvprintw(LINES - 3, 0, "Pressione <ENTER> para selecionar uma opção");
	//mvprintw(LINES - 2, 0, "Use as teclas Up e Down para navegar (F1 para sair)");
	mvprintw(5, 0, "Pressione <ENTER> para selecionar uma opção");
	mvprintw(6, 0, "Use as teclas Up e Down para navegar (F1 para sair)");
	mvprintw(8, 0, "_________MONITOR DA PLATAFORMA DE EXPERIMENTOS_________");
	mvprintw(10, 0, "Mote conectado via serial");
	post_menu(my_menu);
	refresh();

}

int main()
{	
	if(abreComunicacaoSerial() < 0){
		printf("O mote coordenador não está acessível via serial\n");
		printf("Verifique se o mote está conectado e/ou se há permissão de acesso\n");
		return -1;
	}

	inicializaItems();
	criaMenu();

	while((c = getch()) != KEY_F(1))
	{       switch(c)
	        {	case KEY_DOWN:
				menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
			case 10: // Enter
			{	
				ITEM *cur;
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

	fecharMenu();

}


void trataMenu(char *name)
{	move(10, 0);
	clrtoeol();
	mvprintw(10, 0, "%s: ", name);
	clrtoeol();
	if(strcmp("ler status", name)==0){
			mvprintw(11, 0, "Conectando ao coordenador... ");
			int retorno = fsm_manager(1);
			if(retorno==COORDENADOR_OK)mvprintw(11, 30, "Conectado! \n");
			if(retorno==TIMEOUT) mvprintw(11, 30, "Timeout! \n");
			if(retorno==ERRO_SELECT) mvprintw(11, 30, "ERRO: Select retornou -1! \n");
	}

	else mvprintw(10, 0, "NOP");

}	
