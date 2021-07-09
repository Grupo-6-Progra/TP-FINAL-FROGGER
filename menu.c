#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include"menu.h"

char arr[]={"PLQ"};     //inicializo un arreglo de strings para los estados
char *p=arr;            //se posiciona un puntero para ver los estados y as� decidir a d�nde va
char t=0;


enum SELECTOR
{
    PLAY, LEVEL, QUIT
};

void menu_start(void)
{  
    static int selector = PLAY;
    if (key_pressed[KEY_RIGHT] == true) 
    {
        if(selector == QUIT)
        {
            selector = PLAY;
        }
        else
        {
            selector++;
        }
    }
    else if (key_pressed[KEY_LEFT] == true)
    {
        if(selector == PLAY)
        {
            selector = QUIT;
        }
        else
        {
            selector--;
        }
    }
    else if(key_pressed[KEY_ENTER])
    {
        switch(selector)
        {
            case PLAY:
                play();
                break;
                
        }
    }

    
    /*while(*p=='P')		//la condicion es que el puntero apunte al estado correspondiente	
    {	
        if (key_pressed[KEY_RIGHT] == true) 
        {
            p++;  //cambia de estado
            Level(); //llamo a la selecci�n de los niveles 
        }
        if (key_pressed[KEY_DOWN] == true) 
        {
            Play(); //llama la funci�n que inicia el juego? Entrar�a al juego directamente
        }	        
    }*/
}

void level (void)
{
    if(key_pressed[KEY_RIGHT])
    {
        p++;
        quit();
    }
    else if (key_pressed[KEY_DOWN])
    {
        menu_levels();
    }
    else
    {
        level();
    }
}

void menu_levels(void)
{
	printf("Seleccione su nivel\n");	//Necesitar�a saber si va existir el men� de niveles
	
}

void play (void)

{
	printf ("Va al juego\n");			//creo que no ser�a una funci�n.
	p++;
}
void quit(void)

{
	printf("Entramos en el modo QUIT\n");		//mensaje avisando que va a terminar el juego (imrpimiendo QUIT)
	if (key_pressed[KEY_RIGHT]) 							//ve si ese valor es el stick a la derecha
        {
            p=arr;						//si lo es, vuelve a posicionar el puntero a estado inicial para que vuelva al estado PLAY 
        }
        if (key_pressed[KEY_DOWN]) 
        {
            play(); //modifica la variable  del juego,alternando su valor y lo termina
        }
}
