#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct {
  int type;           // 0 -> rock ,1 -> rabbit, 2 -> fox
  int i;              // position i in the enviroment
  int j;              // position j in the enviroment
  int gen_food;       // generations passed without eating
} Object;


int str_to_num(char str[]){
    if(!strcmp(str,"ROCK"))
        return 0;
    else if(!strcmp(str,"RABBIT"))
        return 1;
    else
        return 2;    
}

int num_to_char(int x){
    if(x == -1)
        return ' ';
    else if(x == 0)
        return '*';
    else if (x == 1)
        return 'R';
    else 
        return 'F';    
}

int main(){

    int GEN_PROC_RABBITS; // number of generations until a rabbit can procreate
    int GEN_PROC_FOXES;   // number of generations until a fox can procreate
    int GEN_FOOD_FOXES;   // number of generations for a fox to die of starvation
    int N_GEN;            // number of generations for the simulation
    int R;                // number of rows of the matrix representing the ecosystem
    int C;                // number of columns of the matrix representing the ecosystem
    int N;                // number of objects in the initial ecosystem

    scanf("%d",&GEN_PROC_RABBITS);
    scanf("%d",&GEN_PROC_FOXES);
    scanf("%d",&GEN_FOOD_FOXES);
    scanf("%d",&N_GEN);
    scanf("%d",&R);
    scanf("%d",&C);
    scanf("%d",&N);

    int matrix[R][C];
    for(int i = 0; i < R; i++)
        for(int j = 0; j < C; j++)
            matrix[i][j] = -1;


    Object *objects = (Object *) malloc(N * sizeof(Object));

    for(int i=0; i < N; i++){

        char tmp_type[10];

        scanf("%s %d %d", tmp_type, &objects[i].i, &objects[i].j);

        objects[i].type = str_to_num(tmp_type);

        printf("%d %d %d\n", objects[i].type, objects[i].i, objects[i].j);

        matrix[objects[i].i][objects[i].j] = objects[i].type;

    }

    for(int i = 0; i < R; i++){
        for(int j = 0; j < C; j++)
            printf("%c ", num_to_char(matrix[i][j]))    ;
        printf("\n");
    }



} tester