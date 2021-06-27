/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "juego.h"

bool key_pressed[TECLAS_MAX];
RANA rene = {.x = RANA_ANCHO/2, .y = RANA_ALTO /2, .dx = VELOCIDAD_RANA_ANCHO, .dy = VELOCIDAD_RANA_ALTO};


void frogger (void)
{
    static unsigned int timer_up = 0, timer_down = 0, timer_right = 0, timer_left = 0;
    
    if ((key_pressed[KEY_UP] == true || timer_up != 0 ) && timer_down == 0 && timer_right == 0 && timer_left == 0)
    {
        if(timer_up == 0)
        {
            timer_up = (int) TIEMPO_SALTO_ALTO + TARDA_SALTO;
        }
        
        if(timer_up > TARDA_SALTO && rene.y >= RANA_ALTO/2 + rene.dy)
        {
             rene.y -= rene.dy;
        }
        
        timer_up--;
        
    }
    
    if ( (key_pressed[KEY_DOWN] == true || timer_down != 0 ) && timer_up == 0 && timer_right == 0 && timer_left == 0)
    {
        if(timer_down == 0)
        {
            timer_down = (int) TIEMPO_SALTO_ALTO + TARDA_SALTO;
        }
        
        if (timer_down > TARDA_SALTO && rene.y <= MUNDO_ALTO - RANA_ALTO/2)
        {
            rene.y += rene.dy;
        }
        
        timer_down--;
    }
    
    if ((key_pressed[KEY_RIGHT] == true || timer_right != 0 ) && timer_down == 0 && timer_up== 0 && timer_left == 0)
    {
        if(timer_right == 0)
        {
            timer_right = (int) TIEMPO_SALTO_ANCHO + TARDA_SALTO;
        }
        
        if (timer_right > TARDA_SALTO && rene.x <= MUNDO_ANCHO - RANA_ANCHO/2)
        {
            rene.x += rene.dx;
        }
        
        timer_right--;
    }
    
    if ((key_pressed[KEY_LEFT] == true || timer_left != 0 ) && timer_down == 0 && timer_right == 0 && timer_up == 0)
    {
        if(timer_left == 0)
        {
            timer_left = (int) TIEMPO_SALTO_ANCHO + TARDA_SALTO;
        }
        
        if(timer_left > TARDA_SALTO && rene.x >= RANA_ANCHO/2 + rene.dx)
        {
            rene.x -= rene.dx;
        }
        
        timer_left--;
    }
    
}