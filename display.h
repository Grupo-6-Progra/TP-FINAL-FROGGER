/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   display.h
 * Author: ivan
 *
 * Created on 1 de julio de 2021, 15:20
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

    void redraw_disp(void);
    bool timer (void);
    void print_object_display (double , double, int);
    void display_teclas(void);
    void vidas(int);
    void init_display(void);


#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_H */
