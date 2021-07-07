#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "juego.h"
#include "colisiones.h"
#include "display.h"

#include "/home/pi/libs/joydisp/disdrv.h"
#include "/home/pi/libs/joydisp/joydrv.h"
#include "/home/pi/libs/joydisp/termlib.h"

#define THRESHOLD 40	//Límite a partir del cual se mueve el LED encendido


void init_display()
{
    joy_init();								//inicializa el joystick
    disp_init();							//inicializa el display
    disp_clear();
}

void redraw_disp (void) 
{
    int i;
    int j;
    int k;
    int l;
    dcoord_t coord;
    for (i=0 ; i<DISP_CANT_X_DOTS ; i++)
    {
        for (j=0 ; j<DISP_CANT_Y_DOTS ; j++)
        {
            coord.x = i;
            coord.y = j;
            disp_write(coord, D_OFF);
        }
    }
  
    print_object_display(rene.x, rene.y, D_ON);
    for (k=0 ; k<FILAS_DE_AUTOS ; k++)
    {
        for (l=0 ; l<AUTOS_POR_FILA ; l++)
        {
            if ((autos[k][l].x/CASILLA_ANCHO < DISP_CANT_X_DOTS)&&(autos[k][l].x/CASILLA_ANCHO > 0))
            {
                print_object_display(autos[k][l].x, autos[k][l].y,D_ON);               
            }

        }
    }
       
    vidas(3);
	
	
		
/*		disp_update();	//Actualiza el display con el contenido del buffer
		joy_update();	//Mide las coordenadas del joystick
		coord = joy_get_coord();	//Guarda las coordenadas medidas
		
		
		
		disp_write(pos,D_OFF);	//apaga la posición vieja en el buffer
		disp_write(npos,D_ON);	//enciende la posición nueva en el buffer
		pos = npos;				//actualiza la posición actual
		
        }
*/
    disp_update();
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
        printf("subo");
    }
    else
    {
        key_pressed[KEY_UP] = false;
    }
    
    if(coord.y < -THRESHOLD)
    {
        key_pressed[KEY_DOWN] = true;
        printf("bajo");
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

bool timer(void)
{
    static bool flag = true;
    static clock_t t;       
    if (flag == true)
    {
        t = clock();
    }

    if (clock() -t < CLOCKS_PER_SEC/20/REFRESCO)
    {
        flag = false;
        return false;
    }
    else
    {
        flag = true;
        return true; 
        
    }
}
 
 