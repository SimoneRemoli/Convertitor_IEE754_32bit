#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

/*
Implementare un programma che converta nel formato IEE-754 un numero con la virgola con:
- 1 bit per il segno;
- 8 bit per l'esponente;
- 23 bit per la mantissa;
*/


int* worker(char*,int*);
int* tokenize(char*);
void view(int*);
void to_bin(int,char*);
int scrittura_notazione(char*,char*);
int K_SUM_E(int,int,int);
int indice = 0;
int* mantissa(int,char*,int,int,double*);
void scrivi_la_rappresentazione_definitivamente(int,char*,int*,int,char*,char*,int);
char numero_decimale[1024];
void converti_in_esadecimale(char*,char*,char**,char*);
int da_binario_a_decimale(char*);



int main(int argc, char **argv)
{
	int* numero;
	char* vettore_finale = (char*)malloc(sizeof(char)*1024);
	int E = 0,K_bias=0,bit_exp=8,e=0,val_mantissa=23;
	char numero_binario[1024]={0}, numero_notazione[1024]={0},bias_sommato[1024]={0},mantissa_bin[1024]={0},vettore_esadecimale[1024]={0};
	printf("Inserisci il numero (con la virgola) da rappresentare in formato IEE-754 (I.E: 7,124) : ");
	if(fgets(numero_decimale,1024,stdin))
		numero_decimale[strcspn(numero_decimale,"\n")] = 0;
	numero = tokenize(numero_decimale);
	view(numero);
	to_bin(*(numero+0),numero_binario); //converte il numero *(numero + 0) in bnario e il risultato lo scrive in numero_binario
	//di quanto sposto la virgola sulla rappresentazione intera?
	E = scrittura_notazione(numero_binario,numero_notazione);
	printf("E = %d \n", E);
	double *vettore_mantissa = (double*)malloc(sizeof(double)*val_mantissa-E);
	int *vettore_mantissa_netto;
	K_bias = K_SUM_E(E,K_bias,bit_exp);
	printf("Bias K = %d \n", K_bias);
	e = E + K_bias;
	printf("Valore di e = %d \n", e);
	to_bin(e,bias_sommato);
	puts("REGOLA: e = E + K");
	printf("Valore di %d + %d = %s \n", E,K_bias,bias_sommato);
	puts("Valori della mantissa:");
	vettore_mantissa_netto = mantissa(*(numero + 1),mantissa_bin,val_mantissa,E,vettore_mantissa);
	for(int i=0;i<val_mantissa-E;i++) printf("[%d] = %d \n", i+1, *(vettore_mantissa_netto +i));
	indice = 0;
	scrivi_la_rappresentazione_definitivamente(*(numero+0),vettore_finale,vettore_mantissa_netto,val_mantissa,bias_sommato,numero_notazione,val_mantissa-E);
	char** quartetto = (char**)malloc(sizeof(char*)*8);
	char* lettere = (char*)malloc(sizeof(char*)*6);
	converti_in_esadecimale(vettore_finale,vettore_esadecimale,quartetto,lettere);
	return 0;

}
void converti_in_esadecimale(char* vettore_finale, char*vettore_esadecimale, char**quartetto, char* lettere)
{
	char *appoggio = (char*)malloc(sizeof(char)*1024);
	char *temp = (char*)malloc(sizeof(char)*4);
	char *swap = (char*)malloc(sizeof(char)*4);
	int indexer = 7;

	int j = 0;
	for(int i=0;i<strlen(vettore_finale);i++)
	{
		if(*(vettore_finale + i)!=' ')
		{
			*(appoggio + j) = *(vettore_finale + i);
			j = j + 1;
		}
	}

	printf("Vettore da studiare = %s \n", appoggio);

	//per convertire il numero in hexadecimale passiamo prima per la rappresentazione decimale a quartetti
	
	
	for(int i=strlen(appoggio)-1;i>=0;i--)
	{
		indice = indice + 1;
		*(temp + (indice-1)) = *(appoggio + i);
		if(indice == 4)
		{
			//si è formata la stringa temp
			quartetto[indexer] = malloc(sizeof(char)*1024);

			for(int j=strlen(temp)-1,ii=0;j>=0;j--,ii++)
				*(swap + ii) = *(temp + j);

			memcpy(quartetto[indexer],swap,1024);

			indice = 0;
			memset(temp,0,4);
			indexer = indexer - 1;
		}
	}

	char car = 'A';
	int carch = (int)car;
	for(int i=0;i<6;i++) *(lettere + i) = (char)carch++; //ABCDEF
	int numero_decimale = 0;
	char buffered[1024];
	strcat(vettore_esadecimale,"0x");


	for(int i=2;i<10;i++)
	{
		printf("Valore binario del [%d] - quartetto = %s ",i-1,*(quartetto + (i-2)));
		numero_decimale = da_binario_a_decimale(*(quartetto + (i-2)));
		if(numero_decimale < 10)
		{
			sprintf(buffered,"%d",numero_decimale);
			strcat(vettore_esadecimale,buffered);
		}
		else
		{
			for(int value=10;value<16;value++)
			{
				if(numero_decimale==value)
				{
					*(vettore_esadecimale + i) = *(lettere + (value-10));
				}
			}
		}
		numero_decimale = 0;

	}
	printf("Valore esadecimale = %s \n", vettore_esadecimale);
	return;
}

int da_binario_a_decimale(char *num)
{
	char *app = (char*)malloc(sizeof(char)*1024);
	int valore_decimale = 0,intero = 1,pos=0;
	for(int i=strlen(num)-1,j=0;i>=0;i--,j++) //rigiro il numero per far si che il bit zeresimo sia il primo
	{
		*(app + j) = *(num + i);
	}
	for(int i=0;i<strlen(app);i++)
	{
		if(app[i]=='1')
		{
			pos = i;

			while(pos--)
			{
				intero *= 2;
			}
			valore_decimale += intero;
		}
		
		intero = 1;
	}
	printf("- Il suo valore decimale = %d \n", valore_decimale);
	return valore_decimale;
}

void scrivi_la_rappresentazione_definitivamente(int parte_reale,char* vettore_finale,int* vettore_mantissa_netto, int val_mantissa,char* bias_sommato,char* numero_notazione,int mantdim)
{
	char c;
	int base=0;
	if(parte_reale>0)
		*(vettore_finale + 0) = '0';
	else
		*(vettore_finale + 0) = '1'; //in [0] scrivo o 0 o 1
	*(vettore_finale + 1) = ' '; //in [1] metto lo spazio
	for(int i=2,j=0;i<strlen(bias_sommato)+2;i++,j++)
		*(vettore_finale + i) = *(bias_sommato + j);
	*(vettore_finale + strlen(bias_sommato)+2) = ' ';
	for(int k=0;k<strlen(numero_notazione);k++)
	{
		if(*(numero_notazione + k)=='.')
			base = k+1;
	}
	for(int i=strlen(bias_sommato)+3,j=base;j<strlen(numero_notazione);i++,j++)
	{
		*(vettore_finale + i) = *(numero_notazione + j);
	}
	for(int i=strlen(bias_sommato)+3+strlen(numero_notazione)-base,j=0;j<mantdim;i++,j++)
	{
		c = *(vettore_mantissa_netto+j)+'0'; //convert num int in char
		*(vettore_finale + i) = c;
	}


	printf("RAPPRESENTAZIONE FINALE IN IEE-754 a 32 bit: %s \n", vettore_finale);

}
int* mantissa(int mantissa, char* mantissa_bin,int val_mantissa,int E,double*vettore_mantissa)
{
	//devo convertire la mantissa intera in stringa perchè devo conoscere quanto è grande
	char buffered[30];
	sprintf(buffered,"%d",mantissa); //converto int a stringa in una botta
	printf("Valore di buffered = %s \n", buffered);//mantissa in stringa
	int lunghezza_mantissa = strlen(buffered);//se è grande 2, ossia 22, 22/100, ossia 22/(10*10);
	int divisore = 1;
	for(int i=0;i<lunghezza_mantissa;i++)
		divisore *=10;
	double mantex = (double)mantissa/divisore;
	printf("Valore di mantissa normalizzata = %f \n", mantex);
	puts(" ");
	int numero_di_mul = val_mantissa - E;
	int app;
	double num;
	int *vettore_mantissa_secco = (int*)malloc(sizeof(int)*numero_di_mul);
	for(int i=0;i<numero_di_mul;i++)
	{
		if(i==0)
		{
			*(vettore_mantissa+i) = mantex*2;
			printf("%f x 2 = %f \n", mantex, *(vettore_mantissa+i));
			*(vettore_mantissa_secco + i) = (int)*(vettore_mantissa+i);
		}
		else
		{
			app = (int)*(vettore_mantissa+(i-1));
			if(app>0)
			{
				num = *(vettore_mantissa+(i-1))-1;
				*(vettore_mantissa + i) = num*2;
				printf("%f x 2 = %f \n",num,*(vettore_mantissa+i));
				*(vettore_mantissa_secco + i) = (int)*(vettore_mantissa+i);

			}
			else
			{
				*(vettore_mantissa+i) = *(vettore_mantissa+(i-1))*2;
				printf("%f x 2 = %f \n",*(vettore_mantissa+(i-1)), *(vettore_mantissa+i));
				*(vettore_mantissa_secco + i) = (int)*(vettore_mantissa+i);

			}

		}
	}
	return vettore_mantissa_secco;
}
int K_SUM_E(int E, int k, int bit_exp)
{
	int loop = bit_exp-1;
	int counter=1;
	for(int i=0;i<loop;i++)
	{
		counter *= 2;
	}
	k = counter-1;
	return k;

}
int scrittura_notazione(char* numero_binario, char* numero_notazione)
{
	int E = 0,j=0;
	int posizione_uno = 0;
	printf("Valore del numero intero convertito in binario= %s \n", numero_binario);
	for(int i=0;i<strlen(numero_binario);i++)
	{
		if((*(numero_binario + i))=='1')
		{
			posizione_uno = i;
			posizione_uno++;
			i = strlen(numero_binario)-1; //forzo l'uscita dal ciclo for
		}
		E = strlen(numero_binario) - (posizione_uno);
	}
	for(int i=0,j=0;i<strlen(numero_binario)+1;i++,j++)
	{
		if(i==posizione_uno)
		{
			*(numero_notazione + i) = '.';
			i++;
		}
		*(numero_notazione + i) = *(numero_binario + j);
	}
	printf("Valore del numero binario espresso sotto forma di notazione  = %s \n",numero_notazione);
	return E;
}
void to_bin(int num, char* binary)
{
	char* appoggio = (char*)malloc(sizeof(char)*1024);

	while(num!=0)
	{
		if(num%2==0)
			strcat(appoggio,"0");
		if(num%2!=0)
			strcat(appoggio,"1");
		num = num / 2;
	}
	memset(binary, 0, 1024);
	for(int i=strlen(appoggio)-1,j=0;i>=0;i--,j++)
		*(binary + j) = *(appoggio + i);

}
void view(int* ptr)
{
	for(int i=0;i<2;i++)
	{
		if(i==0)
		{
			printf("Valore intero digitato = %d \n", *(ptr + i));
		}
		else
			printf("Valore decimale digitato = %d \n", *(ptr + i));
	}
}
int* worker(char* n,int* vettore_primavirgola_e_dopo_virgola)
{
	 //0 =parte intera, 1 = parte decimale
	*(vettore_primavirgola_e_dopo_virgola + indice) = (int)strtol(n,NULL,10);
	indice = indice + 1;
	return vettore_primavirgola_e_dopo_virgola;

}
int* tokenize(char *numero_decimale)
{
	int* vettore_primavirgola_e_dopo_virgola = (int*)malloc(sizeof(int)*2);
	char* token = strtok(numero_decimale, ",");
	int* valori_numerici;
	while(token != NULL)
	{
		valori_numerici=worker(token,vettore_primavirgola_e_dopo_virgola);
		token = strtok(NULL,",");
	}
	return valori_numerici;
}