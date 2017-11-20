/*
gcc xmlParse.c -I/usr/include/libxml2 -L/usr/lib -lxml2 -lz -lpthread -lm
./a.out test.xml storyinfo author
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
 
void die(char *msg)
{
  printf("%s", msg);
  return;
}

xmlNodePtr getMote(xmlDocPtr doc, xmlNodePtr cur_, int moteID){
    xmlChar *key;
    xmlNodePtr cur = cur_->xmlChildrenNode; //cur está no nivel dos <mote> no xml

    //Vou primeiro encontrar um <mote>
    while (cur != NULL)
    {
	//Vou primeiro encontrar um <mote>
        if ((!xmlStrcmp(cur->name, "mote")))
        {

	   xmlNodePtr curV = cur->xmlChildrenNode;//cur está no nivel das variaveis do xml
	   while (curV != NULL)
    	   {
			//Agora vai procurar a variável passada como subchild
		        if ((!xmlStrcmp(curV->name, "id")))
        		{
			    key = xmlNodeListGetString(doc, curV->xmlChildrenNode, 1);
			    if(atoi(key)==moteID){
					//Encontrou ptr para o moteID
					return cur;
			    }
			    xmlFree(key);
			   break;
			}
       			 curV = curV->next;
	   }
        }
        cur = cur->next;
    }

    //Nao encontrou um <mote>
    return cur;
}

int
parseNode (xmlDocPtr doc, xmlNodePtr cur, char *subchild, char *retorno)
{
 
    xmlChar *key;
    cur = cur->xmlChildrenNode;
    while (cur != NULL)
    {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)subchild)))
        {
            key = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
	    memcpy (retorno, key, sizeof key);
            xmlFree(key);
	    return 0;
        }
        cur = cur->next;
    }
    return -1;
}
 
static void
parseDoc(char *docname, char *child, char *subchild, int id, char *retorno)
{
 
    xmlDocPtr doc;
    xmlNodePtr cur;
 
    doc = xmlParseFile(docname);
 
    if (doc == NULL )
        die("Document parsing failed. \n");
 
    cur = xmlDocGetRootElement(doc); //Gets the root element of the XML Doc
 
    if (cur == NULL)
    {
        xmlFreeDoc(doc);
        die("Document is Empty!!!\n");
    }

    cur = cur->xmlChildrenNode;
    while (cur != NULL)
    {
        if ((!xmlStrcmp(cur->name, (const xmlChar *)child)))
        {
   	    cur = getMote(doc, cur, id);
	    if(cur)parseNode (doc, cur, subchild, retorno);
	    break;
        }
        cur = cur->next;
    }
 
    xmlFreeDoc(doc);
    return;
}

void
leParametro(int id, char *subchild, char *retorno){
    parseDoc ("test.xml","Motes_Config",subchild, id, retorno);

}
 
/*
int
main()
{
    char ret[256]={0};
    leParametro(2, "modo", ret);
    printf("retorno da leitura é %s\n", ret);
    return (1);
}*/
