/* Proceso de escritura/lectura del archivo Highscore */


#include <stdio.h>
#define MENU 1
int main ()
{
  FILE * HighScore;				//se crea el file
  char caracter; 		//creo el caracter para leerlos
  int puntaje1=0;
  int puntaje;				//lo uso como variable del puntaje que ya tenemos
  char n=0;
  int estado_juego;
  HighScore = fopen ("HighScore.txt","r+");	//abro el arcivo como escritura y lectura 
  
 		if (HighScore == NULL)			// caso err�neo y que no deber�a pasar
        {		
            estado_juego=MENU; //caso err�neo que no deber�a pasar y por eso va al men�
        }
        else			//caso para leer y analizar el score con el ya obtenido
        
        /* Aca es donde lee el puntaje ya escrito previamente */
		
		{
            printf("\nEl puntaje m�ximo es: \n\n");
            while((caracter = fgetc(HighScore)) != EOF)
	    	{
			printf("%c",caracter);		//deber�a imprimir cada caracter
		
	    	}
        }
        
        /* Idea de una secuencia que cuente cada digito y arme el n�mero */ //la idea es guardarlo y compararlo con el puntaje que se obtuvo, si �ste es mayor pues luego hago la misma secuencia pero guardando, sino no y queda guardado el anterior puntaje de Highscore
        
        /*
        
		while((caracter = fgetc(HighScore)) != EOF)	//recorre caracter por caracter
	    	{
	    		n++;					//cuenta digitos
				
				if(n>1){		//caso con m�s de un d�gito
							
					puntaje1*=10;
						
					}
				
				
			puntaje1+=caracter;
		
	    	}
		
		printf("%d",puntaje1);//eber�a imprimir
		        fclose(pFile);
	
        
       */ 
        

  return 0;
}
