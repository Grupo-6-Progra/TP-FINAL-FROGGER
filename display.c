//#define RASPI_DISPLAY
#ifdef RASPI_DISPLAY 

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

#define APROX(x)                ((x>0) ? ((int)((x)+0.5)) : ((int)((x)-0.5)))

static void redraw_rana_d (void);
static void redraw_autos_d(void);
static void print_auto1 (int,int, int);
static void print_auto2 (int,int, int);
static void redraw_troncos_d(void);
static void print_tronco1(int, int);
static void print_tronco2(int, int);
static void print_tronco3(int, int);
static void redraw_tortugas_d(void);
static void print_tortuga1(int,int,int);
static void print_tortuga2(int,int,int);
static void print_tortuga3(int,int,int);
//static void redraw_tiempo_d(void);
//static void redraw_llegada_d(void);
//static void redraw_fondo_d(void);
static void vidas(int);
static void delete_disp(void);

void init_display()
{
    joy_init();								//inicializa el joystick
    disp_init();							//inicializa el display
    disp_clear();
}

void redraw_disp (void) 
{
        if(estado_juego != MENU)
    {
        delete_disp();
        //IMPRESION DEL FONDO
        //redraw_fondo_d();

        //IMPRESION DE LOS AUTOS
        redraw_autos_d();

        //impresión de troncos
        redraw_troncos_d();

        //IMPRESIÓN DE TORTUGAS
        redraw_tortugas_d();
        
        //IMPRESION DE LAS CASILLAS DE LLEGADA
       // redraw_llegada_d();

        //IMPRESIÓN DE LA RANA
        redraw_rana_d();
        vidas(3);     
    }
    disp_update();

}


static void vidas(int numero)
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
        key_pressed[KEY_ENTER] = false;
    }
    else
    {
        key_pressed[KEY_ENTER] = true;
    }
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
 
static void redraw_rana_d (void)
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

static void redraw_autos_d (void)
{
    int i;
    int j;
    int coordx;
    int coordy;
    for (i = 0 ; i< FILAS_DE_AUTOS ; i++)
    {
        for (j = 0; j< AUTOS_POR_FILA ; j++)
        { 
             coordx = APROX((autos[i][j].x - rene.x)/TAM_PIXEL)+7;//coordenadas del centro del auto
             coordy = APROX((autos[i][j].y - rene.y)/TAM_PIXEL)+7;
             
             if (i<4)
             {
                print_auto1(coordx, coordy, autos[i][j].direccion); //recibe diferencia de coordenadas auto-rana   
             }
             else 
             {
                 print_auto2(coordx, coordy, autos[i][j].direccion); //recibe diferencia de coordenadas auto-rana 
             }
        }
    
    }

}


static void print_auto1(int coordx, int coordy, int direc)//posicion relativa entre la rana y el enemigo
{
    dcoord_t coord;
    
    int i;
    int j;   
        for (i=0 ; i<3 ; i++)
        {
            for (j=0 ; j<3 ; j++)
            {
                if ((coordx - 1 + j < DISP_CANT_X_DOTS && coordx -1 +j >= 0)&&(coordy - 1 + i < DISP_CANT_Y_DOTS -1 && coordy -1 +i >= 0))
                {
                    if (direc == DERECHA)
                    {
                        if((j!=2 || i!=0) && (j!=2 || i!=2))
                        {
                            coord.x = coordx + j -1;
                            coord.y = coordy + i -1;
                
                         disp_write(coord, D_ON);                        
                        }                       
                    }
                    
                   else if (direc == IZQUIERDA)
                    {
                        if((j!=0 || i!=0) && (j!=0 || i!=2))
                        {
                            coord.x = coordx + j -1;
                            coord.y = coordy + i -1;
                
                         disp_write(coord, D_ON);                        
                        }      
                    }

                }
            }
        }
    

    }
    
static void print_auto2(int coordx, int coordy, int direc)
{
    dcoord_t coord;
    
    int i;
    int j;   
        for (i=0 ; i<3 ; i++)
        {
            for (j=0 ; j<8 ; j++)
            {
                if ((coordx - 3 + j < DISP_CANT_X_DOTS && coordx -3 +j >= 0)&&(coordy - 1 + i < DISP_CANT_Y_DOTS -1 && coordy -1 +i >= 0))
                {
                    if (direc == DERECHA)
                    {
                        if((j!=7 || i!=0) && (j!=7 || i!=2))
                        {
                            coord.x = coordx + j -3;
                            coord.y = coordy + i -1;
                
                         disp_write(coord, D_ON);                        
                        }                       
                    }
                    
                   else if (direc == IZQUIERDA)
                    {
                        if((j!=0 || i!=0) && (j!=0 || i!=2))
                        {
                            coord.x = coordx + j -3;
                            coord.y = coordy + i -1;
                
                         disp_write(coord, D_ON);                        
                        }      
                    }

                }
            }
        }
    
}

static void delete_disp(void)
{
    int i;
    int j;

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
}

static void redraw_troncos_d(void)
{
    unsigned int j,k;
    int coordx;
    int coordy;
    //impresión de troncos
    for(j=0; j < FILAS_DE_TRONCOS; j++)
    {
        for(k=0; k < TRONCOS_POR_FILA; k++)
        {
            coordx = APROX((troncos[j][k].x - rene.x)/TAM_PIXEL)+7;//coordenadas del centro del auto
            coordy = APROX((troncos[j][k].y - rene.y)/TAM_PIXEL)+7;
            
            if(j == 0)
            {
                print_tronco1(coordx, coordy);
            }
            if(j == 1)
            {
                print_tronco2(coordx, coordy);
            }
            if(j == 2)
            {
                print_tronco3(coordx,coordy);
            }
        }
    }
}
    
static void print_tronco1(int coordx, int coordy)
{
    dcoord_t coord;
    
    int i;
    int j;   
        for (i=0 ; i<5 ; i++)
        {
            for (j=0 ; j<15 ; j++)
            {
                if ((coordx - 7 + j < DISP_CANT_X_DOTS && coordx -7 +j >= 0)&&(coordy - 2 + i < DISP_CANT_Y_DOTS -1 && coordy -2 +i >= 0))
                {
                    if (j==0 || j==14 || i==0 || i==4)
                    {
                        coord.x = coordx + j -7;
                        coord.y = coordy + i -2;

                        disp_write(coord, D_ON);                         
                    }

                }
            }
        }
}

static void print_tronco2(int coordx, int coordy)
{
    dcoord_t coord;
    
    int i;
    int j;   
        for (i=0 ; i<5 ; i++)
        {
            for (j=0 ; j<25 ; j++)
            {
                if ((coordx - 12 + j < DISP_CANT_X_DOTS && coordx -12 +j >= 0)&&(coordy - 2 + i < DISP_CANT_Y_DOTS -1 && coordy -2 +i >= 0))
                {
                    if (j==0 || j==24 || i==0 || i==4)
                    {
                        coord.x = coordx + j -12;
                        coord.y = coordy + i -2;

                        disp_write(coord, D_ON);                         
                    }

                }
            }
        }
}



static void print_tronco3(int coordx, int coordy)
{
    dcoord_t coord;
    
    int i;
    int j;   
        for (i=0 ; i<5 ; i++)
        {
            for (j=0 ; j<20 ; j++)
            {
                if ((coordx - 9 + j < DISP_CANT_X_DOTS && coordx -9 +j >= 0)&&(coordy - 2 + i < DISP_CANT_Y_DOTS -1 && coordy -2 +i >= 0))
                {
                    if (j==0 || j==19 || i==0 || i==4)
                    {
                        coord.x = coordx + j -9;
                        coord.y = coordy + i -2;

                        disp_write(coord, D_ON);                         
                    }

                }
            }
        }
}
static void redraw_tortugas_d(void)
{
    unsigned int j,k;
    int coordx;
    int coordy;
    //impresión de troncos
    for(j=0; j < FILAS_DE_TORTUGAS; j++)
    {
        for(k=0; k < TORTUGAS_POR_FILA; k++)
        {
            coordx = APROX((tortugas[j][k].x - rene.x)/TAM_PIXEL)+7;//coordenadas del centro del auto
            coordy = APROX((tortugas[j][k].y - rene.y)/TAM_PIXEL)+7;
            
            if(tortugas[j][k].hundirse == false)
            {
                if(j == 0)
                {
                    if (tortugas[j][k].frames == 0 || tortugas[j][k].frames == 1)
                    {
                        print_tortuga1(coordx-5, coordy ,tortugas[j][k].direccion);
                        print_tortuga1(coordx, coordy ,tortugas[j][k].direccion);
                        print_tortuga1(coordx+5, coordy ,tortugas[j][k].direccion);
                    }
                    else if (tortugas[j][k].frames == 2)
                    {
                        print_tortuga2(coordx-5, coordy ,tortugas[j][k].direccion);
                        print_tortuga2(coordx, coordy ,tortugas[j][k].direccion);
                        print_tortuga2(coordx+5, coordy ,tortugas[j][k].direccion);                        
                    }
                    
                    else if (tortugas[j][k].frames == 3)
                    {
                        print_tortuga3(coordx-5, coordy ,tortugas[j][k].direccion);
                        print_tortuga3(coordx, coordy ,tortugas[j][k].direccion);
                        print_tortuga3(coordx+5, coordy ,tortugas[j][k].direccion);                        
                    }

                }

                if(j == 1)
                {                   
                    if (tortugas[j][k].frames == 0 || tortugas[j][k].frames == 1)
                    {
                        print_tortuga1(coordx-2, coordy ,tortugas[j][k].direccion);
                        print_tortuga1(coordx+3, coordy ,tortugas[j][k].direccion);
                    }
                    else if (tortugas[j][k].frames == 2)
                    {
                        print_tortuga2(coordx-2, coordy ,tortugas[j][k].direccion);
                        print_tortuga2(coordx+3, coordy ,tortugas[j][k].direccion);                        
                    }
                    
                    else if (tortugas[j][k].frames == 3)
                    {
                        print_tortuga3(coordx-2, coordy ,tortugas[j][k].direccion);
                        print_tortuga3(coordx+3, coordy ,tortugas[j][k].direccion);                    
                    }
                    
                }
            }
         
        }
    }
}

static void print_tortuga1(int coordx, int coordy, int direc)
{
    dcoord_t coord;
    int i;
    int j;
    
    int mat_izq[5][5]={{1,0,0,0,1},   
                   {0,1,1,1,0},
                   {1,0,0,1,0},
                   {0,1,1,1,0},
                   {1,0,0,0,1}};  
    
    int mat_der[5][5]={{1,0,0,0,1},   
                   {0,1,1,1,0},
                   {0,1,0,0,1},
                   {0,1,1,1,0},
                   {1,0,0,0,1}};  
  
  
 /*  
.   .
 ...
.  .
 ...      
.   .      
 
 */
    for (i=0 ; i<5 ; i++)
    {
        for(j=0 ; j<5 ; j++)
        {
            if ((coordx - 2 + j < DISP_CANT_X_DOTS && coordx -2 +j >= 0)&&(coordy - 2 + i < DISP_CANT_Y_DOTS -1 && coordy -2 +i >= 0))
            {
                if (direc == IZQUIERDA)
                {
                    if (mat_izq[i][j]==1)
                    {
                        coord.x = coordx -2 + j;

                        coord.y = coordy -2 + i;

                        disp_write(coord, D_ON);
                    }
                }
                else if (direc == DERECHA)
                {
                    if (mat_der[i][j]==1)
                    {
                        coord.x = coordx -2 + j;

                        coord.y = coordy -2 + i;

                        disp_write(coord, D_ON);
                    }
                }
                
            }
                    
        }
    }
}

static void print_tortuga2(int coordx, int coordy, int direc)
{
     dcoord_t coord;
    int i;
    int j;
    
    int mat[5][5]={{0,0,0,0,0},   
                   {0,1,1,1,0},
                   {0,1,0,1,0},
                   {0,1,1,1,0},
                   {0,0,0,0,0}};  
  
  
 /*  
.   .
 ...
.  .
 ...      
.   .      
 
 */
    for (i=0 ; i<5 ; i++)
    {
        for(j=0 ; j<5 ; j++)
        {
            if ((coordx - 2 + j < DISP_CANT_X_DOTS && coordx -2 +j >= 0)&&(coordy - 2 + i < DISP_CANT_Y_DOTS -1 && coordy -2 +i >= 0))
            {

                    if (mat[i][j]==1)
                    {
                        coord.x = coordx -2 + j;

                        coord.y = coordy -2 + i;

                        disp_write(coord, D_ON);
                    }

               
            }
                
        }
                    
        
    }
}


static void print_tortuga3(int coordx, int coordy, int direc)
{
    dcoord_t coord;
    int i;
    int j;
    
    int mat[5][5]={{0,0,0,0,0},   
                   {0,0,0,0,0},
                   {0,0,1,0,0},
                   {0,0,0,0,0},
                   {0,0,0,0,0}};  
  
  
 /*  
.   .
 ...
.  .
 ...      
.   .      
 
 */
    for (i=0 ; i<5 ; i++)
    {
        for(j=0 ; j<5 ; j++)
        {
            if ((coordx - 2 + j < DISP_CANT_X_DOTS && coordx -2 +j >= 0)&&(coordy - 2 + i < DISP_CANT_Y_DOTS -1 && coordy -2 +i >= 0))
            {

                    if (mat[i][j]==1)
                    {
                        coord.x = coordx -2 + j;

                        coord.y = coordy -2 + i;

                        disp_write(coord, D_ON);
                    }

               
            }
                
        }
                    
        
    }
}
#endif




