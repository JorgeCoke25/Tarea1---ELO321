#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
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

void copiar(char temp[], int i){
  int N = strlen(temp)+1;
  prod[i].nombre = (char*)malloc(N*sizeof(char));
  strcpy(prod[i].nombre,temp);
} 
void copiarPrecio(char temp[], int i){
  //prod[i].precio = (int)malloc(sizeof(int));
  prod[i].precio=atoi(temp);
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
      aux = '0';
      for(j = 0; a%2!=1; j++){
        aux=fgetc(inventario);
        if(aux==',')a++;
        if(a!=1){
          temp[j]=aux;
        }
      }
      copiar(temp,i);
      //almacenando precios
      
      for(j=0;a%2!=0;j++){
        aux=fgetc(inventario);
        if(aux==',')a++;
        if(a!=2){
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
    
    pid_t repartidor1 = fork();  //inicializando hijo 1
    if(repartidor1<0){
        printf("Ocurrio un error con repartidor1");
        return 1;
    }else if(repartidor1==0){ //proceso hijo 1
        printf("soy el repartidor 1 jojojo\n");
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
