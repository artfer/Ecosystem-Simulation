#include <stdio.h>
#include <stdlib.h>
#include <string.h>



typedef struct {
  int type;           // 0 -> rock ,1 -> rabbit, 2 -> fox
  int i;              // position i in the enviroment
  int j;              // position j in the enviroment
  int gen_food;       // generations passed without eating
  int state;          // 0 -> Dead, 1 -> Alive
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

    int ani_start;

    for(int i=0; i < N; i++){

        char tmp_type[10];

        scanf("%s %d %d", tmp_type, &objects[i].i, &objects[i].j);

        objects[i].type = str_to_num(tmp_type);

        matrix[objects[i].i][objects[i].j] = objects[i].type;

        if(objects[i].type == 0)
            ani_start = i;

    }

    ani_start++;

    /*
    for(int i = 0; i < R; i++){
        for(int j = 0; j < C; j++)
            printf("%c ", num_to_char(matrix[i][j]))    ;
        printf("\n");
    }

    for(int i=0; i < N; i++)
        printf("%d %d %d\n", objects[i].type, objects[i].i, objects[i].j);
    */

    int i,j;
    int moves[N];

    for(int gen=0; gen < N_GEN; gen++){
        
        for(int n = ani_start; n < N; n++){
            
            if (objects[n].state == 0)
                continue;

            i = objects[n].i;
            j = objects[n].j;

            if(matrix[i-1][j] == -1){
                moves[n] = 0;
                break;
            } else if(matrix[i][j+1] == -1) {
                moves[n] = 1;
                break;
            } else if(matrix[i+1][j] == -1){
                moves[n] = 2;
                break;
            } else if(matrix[i][j-1] == -1){
                moves[n] = 3;
                break;
            } else {
                moves[n] = -1;
                break;
            }
            
        }

    }


}