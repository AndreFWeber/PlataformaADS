#ifndef MENUDAPLATAFORMA
#define MENUDAPLATAFORMA

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4
#define ERRO_CONEXAO_SERIAL -1 
#define COORDENADOR_NOK -2 
#define COORDENADOR_OK 1 
#define ERRO_SELECT -1 
#define TIMEOUT 0 
#define MAX_BITS_RECEPCAO 256

void trataMenu(char *name);
void inicializaItems();
void criaMenu();
void fecharMenu();

char *choices[] = {
                        "ler status",
			"opdso 2",
			"opdso 3",
                        "opdso 4",
                  };

ITEM **my_items;
int c;				
MENU *my_menu;
int n_choices, i;
ITEM *cur_item;
bool menu=false;


#endif
