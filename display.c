/*#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "juego.h"
#include "colisiones.h"

#include "/home/ivan/Repositorios/TP-FINAL-FROGGER/librerias/libs/joydisp/disdrv.h"
#include "/home/ivan/Repositorios/TP-FINAL-FROGGER/librerias/libs/joydisp/joydrv.h"
#include "/home/ivan/Repositorios/TP-FINAL-FROGGER/librerias/libs/joydisp/termlib.h"

#define THRESHOLD 40	//Límite a partir del cual se mueve el LED encendido
void vidas(int n);

void init_display()
{
    joy_init();								//inicializa el joystick
    disp_init();							//inicializa el display
    disp_clear();
}

int main2(int argc, char** argv) {
    
{
								//limpia todo el display
	dcoord_t pos = {DISP_MAX_X>>1 , DISP_MAX_Y-2};	//pos es la posición actual, empieza en el centro y abajo de todo(14x13)
	dcoord_t npos = pos;							//npos es la próxima posición
	
	jcoord_t coord = {0,0};	
        time_t timer;
        time(&timer);
        
        while(difftime(time(NULL),timer) < 2)
        {
            printf("%f", difftime(time(NULL),timer));
        }
        //coordenadas medidas del joystick
	
    vidas(3);
	do
	{
		printf(CYAN_TEXT "Joystick: (%4d,%4d)" , coord.x, coord.y);	//Imprime las coordenadas del joystick
		printf(WHITE_TEXT " | ");
		printf(RED_TEXT "Display: (%2d,%2d)\n" , npos.x, npos.y);	//Imprime la posición del LED encendio
		
		disp_update();	//Actualiza el display con el contenido del buffer
		joy_update();	//Mide las coordenadas del joystick
		coord = joy_get_coord();	//Guarda las coordenadas medidas
		
		//Establece la próxima posición según las coordenadas medidas
		if(coord.x > THRESHOLD && npos.x < DISP_MAX_X-1)//si movi el joystick lo suficiente para la derecha y no esta en el borde der
		{
			npos.x++;//derecha
		}
		if(coord.x < -THRESHOLD && npos.x > DISP_MIN) //si movi el joystick lo suficiente para la izq y no esta en el borde izq
		{
			npos.x--;//izquierda
		}
		if(coord.y > THRESHOLD && npos.y > DISP_MIN)//si movi el joystick lo suficiente para abajo y no esta abajo de todo
		{
			npos.y--;//baja
		}
		if(coord.y < -THRESHOLD && npos.y < DISP_MAX_Y-2)//si movi el joystick lo suficiente para arriba y no esta arriba de todo
		{
			npos.y++;//sube
		}
		
		disp_write(pos,D_OFF);	//apaga la posición vieja en el buffer
		disp_write(npos,D_ON);	//enciende la posición nueva en el buffer
		pos = npos;				//actualiza la posición actual
		
		int i;
		int j;
		
		for (i=0 ; i<FILAS_DE_AUTOS ; i++)
		{
			for (j=0 ; j<AUTOS_POR_FILA ; j++)
			{
				dcoord_t posE = {DISP_MIN+ i, (DISP_MIN+2)*j};
				disp_write(posE,D_ON);
						dcoord_t nposE = posE;
		while (nposE.x>DISP_MIN && posE.x > DISP_MIN)
		{
		posE.x++;
		disp_write(posE, D_ON);
		nposE.x--;
		disp_write(nposE, D_OFF);
		}	
			}

		}

		
		
	} while( joy_get_switch() == J_NOPRESS );	//termina si se presiona el switch
}

}


void vidas(int numero)
{
	dcoord_t tresVidas = {13,15};
	dcoord_t dosVidas = {14,15};
    dcoord_t unaVida = {15,15};
    int i;
    int j;
    
    switch (numero)
    {
    	case 0: 
    		for (i=0; i<=DISP_MAX_X ; i++)
    		{
    			for (j=0; j<=DISP_MAX_Y;j++)
    			{
    				dcoord_t pintar = {i,j};
    				disp_write(pintar,D_ON);	
    			}   		
    		}

		//	sleep(5);
    	//	disp_clear();
    		
    		break;
    	
    	
    	case 1: 
    	
    		disp_write(unaVida, D_ON);
    		disp_write(dosVidas, D_OFF);
    		disp_write(tresVidas, D_OFF);
    		break;
    	
    	
    	case 2: 
    	
    		disp_write(unaVida, D_ON);
    		disp_write(dosVidas, D_ON);
    		disp_write(tresVidas, D_OFF);
    		break;
    	
    	case 3: 
    	
    		disp_write(unaVida, D_ON);
    		disp_write(dosVidas, D_ON);
    		disp_write(tresVidas, D_ON);
    		break;
    	
    	    	    
    }

}

void display_teclas (void)
{
    jcoord_t coord;
    
    joy_update();
    
    coord = joy_get_coord();
    
    if(coord.x > THRESHOLD)
    {
        key_pressed[KEY_RIGHT] = true;
    }
    else
    {
        key_pressed[KEY_RIGHT] = false;
    }
    
    if(coord.x < -THRESHOLD)
    {
        key_pressed[KEY_LEFT] = true;
    }
    else
    {
        key_pressed[KEY_LEFT] = false;
    }
    
    if(coord.y > THRESHOLD)
    {
        key_pressed[KEY_UP] = true;
    }
    else
    {
        key_pressed[KEY_UP] = false;
    }
    
    if(coord.y < -THRESHOLD)
    {
        key_pressed[KEY_DOWN] = true;
    }
    else
    {
        key_pressed[KEY_DOWN] = true;  
    }
    
    if(joy_get_switch() == J_NOPRESS)
    {
        key_pressed[KEY_ENTER] = true;
    }
    else
    {
        key_pressed[KEY_ENTER] = false;
    }
}

void print_object_display (double x, double y, int estado)
{
    dcoord_t coordenadas;
    coordenadas.x =(int) (x / CASILLA_ANCHO);
    coordenadas.y =(int) (y / CASILLA_ALTO);
    
    disp_write(coordenadas,estado);	//enciende o apaga una posición en el buffer
}

void timer(double h)
{
    clock_t t;
    t = clock();
    while (clock() - t < CLOCKS_PER_SEC * h)
    {
        //sacar esta función y hacer funcionar todo el tema de los fps
    }
}
 * 
 */