#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

typedef struct{
    char *nombre;
    int precio;
    int id;
    int tiempo;
} producto;

producto *prod;

int total=0; //Puntero que almacena el precio total del dia


//funciones de utilidad
void vaciar(char temp[]){
  int i;
  for(i = 0; i<50;i++){
    temp[i] = '\0';
  }
}

void copiar(char* temp, int i){
  int N = strlen(temp)+1;
  prod[i].nombre = (char*)malloc(N*sizeof(char));
  strcpy(prod[i].nombre,temp);
} 
void copiarPrecio(char temp[], int i){
  //prod[i].precio = (int)malloc(sizeof(int));
  prod[i].precio=atoi(temp);
}
//Obtiene una id a partir del nombre del producto
int obtenerId(char* str){
    int i =0;
    while (i<100){
        if(strcmp(str,prod[i].nombre)==0){
            return prod[i].id;
        }
        i++;
    }
}
/////////////////
int main(){

    pid_t repartidor1,repartidor2;
    int tubo[2];
    pipe(tubo);

    FILE *inventario, *pedidos; // procedimiento Cajero
    inventario = fopen("inventario.txt", "r");
    pedidos = fopen("pedidos.txt","r");


    // Almacenando el archivo de texto en la estructura producto
    char temp[50];
    int cont=0;
    //Se cuentan las lineas del archivo
    while(!feof(inventario)){
      fgets(temp,50,inventario);
      cont++;
    }
    //Se vuelve al princio del archivo
    rewind(inventario);
    //Se reserva memoria para el arreglo de productos
    prod = (producto*)malloc(cont*sizeof(producto));

    int i,j,a=0; // variable "i" marca la posicion de el producto en el arreglo, j la posicion del caracter para armar el string y "a" el contador que si es "1" es nombre y "2" si es el precio
    //Ciclo for hasta que se termine de leer el archivo
    for(i = 0; !feof(inventario) ; i++){
      vaciar(temp);
      char aux;
      //Ciclo for hasta que se encuentra el delimitador ","
      for(j = 0; a%2!=1; j++){
        aux=fgetc(inventario);
        if(aux==',')a++;
        else if(a!=1){
          temp[j]=aux;
        }
      }
      //Se almacena el nombre de la structura en la posicion "i"
      copiar(temp,i);

      //Ciclo for hasta que se encuentra el delimitador ","
      for(j=0;a%2!=0;j++){
        aux=fgetc(inventario);
        if(aux==',')a++;
        else if(a!=2){
          temp[j]=aux;
        }
      }
      //Se almacena el precio de la structura en la posicion "i"
      copiarPrecio(temp,i);

      //fin de almacenar los precios
      fgets(temp,3, inventario);
      //Se almacena la id de la structura en la posicion "i"
      prod[i].id = atoi(temp);
      fgets(temp,10, inventario);
      //Se almacena el tiempo de la structura en la posicion "i"
      prod[i].tiempo = atoi(temp);
      printf("Nombre producto: %s Precio: %d ID: %d Tiempo de elaboracion: %d \n",prod[i].nombre,prod[i].precio, prod[i].id, prod[i].tiempo);
    }
    // Fin de almacenar el txt en la estructura
    char strLine[50];
    //Lectura de los pedidos para guardarlos en el buffer
    cont = 0;//contador que marca la posicion en el buffer
    char *lectura = fgets(strLine, 50, pedidos);//Variable que confirma si se estan leyendo pedidos
    while (lectura){
        int buffer[100];
        //Aca se elimina el ultimo caracter "\n" de la linea
        strLine[strlen(strLine)-1]='\0';
        //Se toma el primer producto de el pedido
        char *token = strtok(strLine, ",");
        if(token != NULL){
            while(token != NULL){
                // Sólo en la primera pasamos la cadena; en las siguientes pasamos NULL
                buffer[cont]= obtenerId(token); //Se pasa la id del producto leido al buffer
                cont++;//Se suma uno  a la posicion
                token = strtok(NULL, ",");//Siguiente producto
            }
            buffer[cont]=0;
            write(tubo[1],buffer,sizeof(int)*100);
            wait(NULL);//Se espera a que el proceso hijo termine
            cont=0;
        }
        lectura = fgets(strLine, 50, pedidos);
    }
    repartidor1 = fork();  //inicializando hijo 1
    if(repartidor1<0){
        printf("Fallo en la creacion del hijo");
    }else if(repartidor1==0){ //proceso hijo 1
        printf("soy el repartidor 1 jojojo\n");
        int buff[1];//Variable de lectura
        close(tubo[1]);//Se cierra el modo escritura
        int espera = 0; //Variable que definira el tiempo de preparacion total del pedido
        int i =0;
        while (1){
            read(tubo[0],buff,sizeof(int));//Modo lectura de 2 enteros
            i++;
            if(i==10 || buff[0]==0){
                break;
            }
            printf("Buffer: %d - Iteracion numero: %d - Repartidor 1\n",buff[0],i);
            espera+=prod[buff[0]-1].tiempo;
            total+=prod[buff[0]-1].precio;
        }
        usleep(espera); //Tiempo de espera en micro segundos
        close(tubo[0]);//Se cierra el modo lectura del tubo
    }

    repartidor2 = fork();  //inicializando hijo 2
    if(repartidor2<0){
        printf("Fallo la creacion del hijo");
    }else if(repartidor2==0){ //proceso hijo 2
        printf("soy el repartidor 2 jojojo\n");
        int buff[1];//Variable de lectura
        close(tubo[1]);//Se cierra el modo escritura
        int espera = 0; //Variable que definira el tiempo de preparacion total del pedido
        int i =0;
        while (1){
            read(tubo[0],buff,sizeof(int));//Modo lectura de 2 enteros
            i++;
            if(i==10 || buff[0]==0){
                break;
            }
            printf("Buffer: %d - Iteracion numero: %d - Repartidor 2\n",buff[0],i);
            espera+=prod[buff[0]-1].tiempo;
            total+=prod[buff[0]-1].precio;
        }
        usleep(espera);
        close(tubo[0]);
    }
    printf("El total del día es : $%d\n",total);
    return 0;

}
