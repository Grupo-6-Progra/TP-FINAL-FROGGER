//#define RASPI3
#ifdef RASPI3

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

#define APROX(x)                ((x>0) ? (int)((x)+0.5) : ((int)((x)-0.5)))


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
    print_rene();
    
    
    
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
        key_pressed[KEY_DOWN] = false;  
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
 
void print_rene (void)
{
    int i;
    int j;
    dcoord_t coord;
    for (i=0 ; i<ANCHO_RENE ; i++)
    {
        for (j=0 ; j<ALTO_RENE ; j++)
        {
            coord.x = i+6;
            coord.y = j+6;
            disp_write(coord, D_ON);
        }
    }
      
    
}

void print_autos (void)
{
    int i;
    int j;
    double autox;
    double autoy;
    for (i = 0 ; i< FILAS_DE_AUTOS ; i++)
    {
        for (j= 0; j< AUTOS_POR_FILA ; j++)
        {
            autox = autos[i][j].x/TAM_PIXEL;
            autoy = autos[i][j].y/TAM_PIXEL;
            if ((autox - rene.x/TAM_PIXEL <= 9 && rene.x/TAM_PIXEL - autox <= 8)&&(autoy - rene.y/TAM_PIXEL <= 8 && rene.y/TAM_PIXEL - autoy <= 8))
            {
                 print_auto1(7+autox - rene.x/TAM_PIXEL,7+autoy - rene.y/TAM_PIXEL);
                
            }
            
           
        }
    
    }
}


void print_auto1(double x, double y)
{
    APROX(x);
    APROX(y);
}

#endif