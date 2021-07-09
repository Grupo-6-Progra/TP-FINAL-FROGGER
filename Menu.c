#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#define FLECHAD 1
#define FLECHAA 2
#define ASCII 48
char arr[]={"PLQ"};     //inicializo un arreglo de strings para los estados
char *p=arr;            //se posiciona un puntero para ver los estados y as� decidir a d�nde va
char c=0;			//la variable a usar como si fuera el stick
char t=0;
void MenuLevels(void);
void Level (void);
void Menu_start (void);
void Play(void);
void Quit(void);

int main (void)
{
Menu_start();			//función de menu de inicio
	return 0;
}


void Menu_start(void)
{     	 
        while(*p=='P')					//la condicion es que el puntero apunte al estado correspondiente
		
		{
			printf ("Entramos en el modo PLAY\n");		//aca iria una funcion que enciende  los leds de la palabra PLAY
			c=((getch())-'0');				//input
			if (c==FLECHAD) 
			{
				p++;  //cambia de estado
				Level(); //llamo a la selecci�n de los niveles 
			}
			if (c==FLECHAA) 
			{
				Play(); //llama la funci�n que inicia el juego? Entrar�a al juego directamente
		
			}
		        
        }
			//salio del estado de PLAY
}

void Level (void)
{
	printf("Entramos en el modo LEVELS\n");
	c=((getch())-ASCII);				//input 2
	switch(c) {							//analiza lo que ingreso con el stick
	case FLECHAD: {	p++;				//cambia al estado de Quit en el arreglo
					Quit();				//si fue hacia la derecha entonces va al modo QUIT
	}
	break;				
	case FLECHAA: MenuLevels();			//si fue hacia abajo entró en el modo de selección de niveles
	break;
	default: Level();					//si toca cualquier otra cosa vuelve al estado Level manteniendose allí
}
}

void MenuLevels(void)
{
	printf("Seleccione su nivel\n");	//Necesitar�a saber si va existir el men� de niveles
	scanf("%c",&t);
}

void Play (void)

{
	printf ("Va al juego\n");			//creo que no ser�a una funci�n.
	p++;
}
void Quit(void)

{
	printf("Entramos en el modo QUIT\n");		//mensaje avisando que va a terminar el juego (imrpimiendo QUIT)
	c=((getch())-ASCII);						//se queda a la espera de un valor
	if (c==FLECHAD) 							//ve si ese valor es el stick a la derecha
			{
				p=arr;						//si lo es, vuelve a posicionar el puntero a estado inicial para que vuelva al estado PLAY 
			}
			if (c==FLECHAA) 
			{
				Play(); //modifica la variable  del juego,alternando su valor y lo termina
			}
	
}
