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
    return 0;
}
/////////////////
int main(){

    pid_t repartidor2;
    int tubo[2];
    //pipe(tubo);

    FILE *inventario, *pedidos; // procedimiento Cajero
    inventario = fopen("inventario.txt", "r");
    pedidos = fopen("pedidos.txt","r");
    
    // Almacenando el archivo de texto en la estructura producto
    char temp[50];
    int cont=0;
    char aux;
    while(!feof(inventario)){
      fgets(temp,50,inventario);
      cont++;
    }
    rewind(inventario);
    prod = (producto*)malloc(cont*sizeof(producto));

    int i,j,a=0;
    for(i = 0; !feof(inventario) ; i++){
      vaciar(temp);
      char aux;
      for(j = 0; a%2!=1; j++){
        aux=fgetc(inventario);
        if(aux==',')a++;
        else if(a!=1){
          temp[j]=aux;
        }
      }
      copiar(temp,i);
      //almacenando precios      printf("%d\nxd",atoi(temp));

      for(j=0;a%2!=0;j++){

        aux=fgetc(inventario);
        if(aux==',')a++;
        else if(a!=2){
          temp[j]=aux;
        }
      }
      copiarPrecio(temp,i);


      /*fgets(temp, 6, inventario);//6 caracteres que puede llegar a leer de precio
      prod[i].precio = atoi(temp);*/
      //fin de almacenar los precios
      fgets(temp,3, inventario);
      prod[i].id = atoi(temp);
      fgets(temp,10, inventario);
      prod[i].tiempo = atoi(temp);
      printf("Nombre producto: %s Precio: %d ID: %d Tiempo de elaboracion: %d \n",prod[i].nombre,prod[i].precio, prod[i].id, prod[i].tiempo);

    }
    // Fin de almacenar el txt en la estructura
    char strLine[50];
    int buffer[100];
    //Lectura de los pedidos para guardarlos en el buffer
    cont = 0;//contador que marca la posicion en el buffer
    while (fgets(strLine, 50, pedidos)){
        //Aca se elimina el ultimo caracter "\n" de la linea
        strLine[strlen(strLine)-1]='\0';
        //Se toma el primer producto de el pedido
        char *token = strtok(strLine, ",");
        //Se pasa al buffer el id del primer producto
        buffer[cont]=obtenerId(token);
        cont++;
        if(token != NULL){
            while(token != NULL){
                // Sólo en la primera pasamos la cadena; en las siguientes pasamos NULL
                buffer[cont]= obtenerId(token); //Se pasa la id del producto leido al buffer
                cont++;//Se suma uno  a la posicion
                token = strtok(NULL, ",");//Siguiente producto
            }
            buffer[cont]=0;
            close(tubo[0]);
            write(tubo[1],buffer,(cont+1)*sizeof(int));
            wait(NULL);
            cont=0;
        }
    }
    pid_t repartidor1 = fork();  //inicializando hijo 1
    if(repartidor1<0){
        printf("Ocurrio un error con repartidor1");
        return 1;
    }else if(repartidor1==0){ //proceso hijo 1
        printf("soy el repartidor 1 jojojo\n");
        int buff[1];//Variable de lectura
        close(tubo[1]);//Se cierra el modo escritura
        int i =0;
        while (1){
            read(tubo[0],buff, sizeof(int));//Modo de lectura 1 entero
            i++;
            if(i==10 || buff[0]==0){
                break;
            }
            printf("Buffer: %d - Iteracion numero: %d\n",buff[0],i);
        }

    }

    repartidor2 = fork();  //inicializando hijo 2
    if(repartidor2<0){
        printf("Ocurrio un error con repartidor2");
        return 1;
    }else if(repartidor2==0){ //proceso hijo 2
        printf("soy el repartidor 2 jojojo\n");
    }

    return 0;

}
