#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#define FLECHAD 1
#define FLECHAA 2
char arr[]={"PLQ"};     //inicializo un arreglo de strings para los estados
char *p=arr;            //se posiciona un puntero para ver los estados y as� decidir a d�nde va
void MenuLevels(void);
void Level (void);
void Menu_start (void);
void Play();

void Level (void)
{
	MenuLevels();
}

void Menu_start(void)
{     	 
 		int c=0;			//la variable a usar como si fuera el stick

        while(*p=='P')
		
		{
        	
		printf ("PLAY\n");		//ac� iria una funcion que enciende  los leds de la palabra PLAY
		c=(getchar()-'0');		//simulo provisoriamente que ingreso algo , que puede ser mover la flecha hacia abajo o al costado
		if (c==FLECHAD) 
		{
		p++;  //cambia de estado
		Level();  
		}
		if (c==FLECHAA) 
		{
		Play(); //llama la funci�n que inicia el juego? Entrar�a al juego directamente
		
		}        
        }
			//salio del estado de PLAY
}

void MenuLevels(void)
{
	printf("Armar el men� de niveles");	//Necesitar�a saber si va existir el men� de niveles
}

void Play (void)

{
	printf ("Va al juego");			//creo que no ser�a una funci�n.
}

