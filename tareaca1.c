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
    char aux;
    while(!feof(inventario)){
      fgets(temp,50,inventario);
      cont++;
    }
    rewind(inventario);
    prod = (producto*)malloc(cont*sizeof(producto));

    int i,j;
    for(i = 0; !feof(inventario) ; i++){
      vaciar(temp);
      aux = '0';
      for(j = 0; aux != ','; j++){
        aux=fgetc(inventario);
        if(aux!=','){
          temp[j]=aux;
        }
      }
      copiar(temp,i);

      fgets(temp, 6, inventario);//6 caracteres que puede llegar a leer de precio
      prod[i].precio = atoi(temp);

      fgets(temp, 4, inventario);
      prod[i].id = atoi(temp);

      fgets(temp,6, inventario);
      prod[i].tiempo = atoi(temp);
      printf("Nombre producto: %s Precio: %d ID: %d Tiempo de elaboracion: %d \n",prod[i].nombre,prod[i].precio, prod[i].id, prod[i].tiempo);
    }// Fin de almacenar el txt en la estructura
    char *contents = NULL;
    size_t len = 0;
    char *ped = (char) malloc(120);
    int c=0;
    while (getline(&contents, &len, pedidos) != -1){
        ped[c]=contents;
        c++;
    }

    repartidor1 = fork();  //inicializando hijo 1
    if(repartidor1==0){ //proceso hijo 1
        printf("soy el repartidor 1 jojojo\n");
    }
    else if (repartidor1<0){
        printf("Ocurrio un error con repartidor1");
        return 1;
    }

    repartidor2 = fork();  //inicializando hijo 2
    if(repartidor2==0){ //proceso hijo 2
        printf("soy el repartidor 2 jojojo\n");
    }
    else if (repartidor2<0){
        printf("Ocurrio un error con repartidor2");
        return 1;
    }
    return 0;

}
