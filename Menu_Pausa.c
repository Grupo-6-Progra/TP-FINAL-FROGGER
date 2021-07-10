

void Menu_pause(void)                           //función del menú en pausa, tiene 3 opciones regidas por variables globales
{    
    while (estado_juego==PAUSA)                //se ejecutará mientras el juego esté en pausa
    {
       printf("Reanudar juego\n");             //MOSTRAR EN PANTALLA LO INTERNO DEL PRINTF
       
       
            if(key_pressed[KEY_DOWN]== true)                       //ve si toco hacia abajo el stick //key_pressed[KEY_DOWN]== true  // se elige ir hacia abajo para cambiar de opción
            {					    		     
	                           
                    printf("Elegir nivel\n");             //MOSTRAR EN PANTALLA LO INTERNO DEL PRINTF

                        if (key_pressed[KEY_DOWN]== true)            //sige hasta la opción de Quit
                        {
                            printf("Quit\n");       //MOSTRAR EN PANTALLA LO INTERNO DEL PRINTF

                                if(key_pressed[KEY_DOWN]== true)    //si se ingresó la tecla hacia abajo para moverse abajo en las opciones
                                {
                                printf("NO HACE NADA"); //acá no debería hacer nada y solo volver a mostrar la opción de REANUDAR, ya que se le acabaron las opciones
                                }

                                else if (key_pressed[KEY_ENTER]== true) //si se tocó la tecla de enter entonces es porque seleccionó
                                {
                                    estado_juego=MENU;     //se vuelve al menú principal ya que cambió el estado de la variable global
                    
                                }

                        }  
                        else if (key_pressed[KEY_ENTER]== true)     // ve si se seleccionó algo
                        {
                        Menu_Level();       //va a la función de elegir niveles (externa)
                        }                   
	        }				
	       else if(key_pressed[KEY_ENTER]== true)   //si fue tocado enter entonces vuelve al juego
           {
                estado_juego =JUEGO;	//vuelve al estado de juego
	           
            }
 


    }

}
    