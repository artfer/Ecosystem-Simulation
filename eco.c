#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY -2147483648
#define loc(mat,i,j,C) mat[ (i * C) + j ] 

typedef struct {
    int id;             // id of object (rocks are all 0)
    int i;              // position i in the enviroment
    int j;              // position j in the enviroment
    int gen_food;       // generations passed without eating
    int gen_proc;       // generations passed without procreating
    int state;          // 0 -> Dead, 1 -> Alive
} Object;

int* matrix;
int* moves;
int GEN_PROC_RABBITS; // number of generations until a rabbit can procreate
int GEN_PROC_FOXES;   // number of generations until a fox can procreate
int GEN_FOOD_FOXES;   // number of generations for a fox to die of starvation
int N_GEN;            // number of generations for the simulation
int R;                // number of rows of the matrix representing the ecosystem
int C;                // number of columns of the matrix representing the ecosystem
int N;                // number of objects in the initial ecosystem
Object *objects;

int str_to_num(char str[]){
    if(!strcmp(str,"ROCK"))
        return 0;
    else if(!strcmp(str,"RABBIT"))
        return 1;
    else
        return 2;    
}

char num_to_char(int x){
    if(x == EMPTY)
        return ' ';
    else if(x == 0)
        return '*';
    else if (x > 1)
        return 'R';
    else 
        return 'F';    
}

char* num_to_str(int x){
    if(x == 0)
        return "ROCK";
    else if (x == 1)
        return "RABBIT";
    else 
        return "FOX";    
}

void print_mat(int g){
    printf("Generation %d\n",g);
    int tmp;
    for (int c = 0; c < C+2; c++)
        printf("-");
    printf("\n");
    for(int i = 0; i < R; i++){
        printf("|");
        for(int j = 0; j < C; j++){
            tmp = loc(matrix,i,j,C); 
            if(tmp == EMPTY)
                printf(" ");
            else if(tmp < 0)
                printf("F");
            else if(tmp == 0)
                printf("*");
            else
                printf("R");
        }
        printf("|\n");
    }
    for (int c = 0; c < C+2; c++)
        printf("-");
    printf("\n\n");
    
}


void prepare_move(int n){
    if(objects[n].state == 0){
        moves[n] = -1;
        return;
    }

    int i = objects[n].i;
    int j = objects[n].j;

    //if fox, search for rabbit
    if(objects[n].id < 0){
        if(loc(matrix,i-1,j,C) > 0){
            moves[n] = 0;
            return;
        } else if(loc(matrix,i,j+1,C) > 0){
            moves[n] = 1;
            return;
        } else if(loc(matrix,i+1,j,C) > 0){
            moves[n] = 2;
            return;
        } else if(loc(matrix,i,j-1,C) > 0){
            moves[n] = 3;
            return;
        }
    }

    if(loc(matrix,i-1,j,C) == EMPTY)
        moves[n] = 0;
    else if(loc(matrix,i,j+1,C) == EMPTY)
        moves[n] = 1;
    else if(loc(matrix,i+1,j,C) == EMPTY)
        moves[n] = 2;
    else if(loc(matrix,i,j-1,C) == EMPTY)
        moves[n] = 3;
    else
        moves[n] = -1;
}



int main(){

    scanf("%d",&GEN_PROC_RABBITS);
    scanf("%d",&GEN_PROC_FOXES);
    scanf("%d",&GEN_FOOD_FOXES);
    scanf("%d",&N_GEN);
    scanf("%d",&R);
    scanf("%d",&C);
    scanf("%d",&N);

    matrix = malloc( R * C * sizeof(int));
    for(int i = 0; i < R; i++)
        for(int j = 0; j < C; j++)
            loc(matrix,i,j,C) = EMPTY;


    objects = (Object *) malloc(N * sizeof(Object));

    for(int i=0; i < N; i++){

        char tmp_type[10];

        scanf("%s %d %d", tmp_type, &objects[i].i, &objects[i].j);

        

        int tmp = str_to_num(tmp_type);

        objects[i].id = tmp == 0 ? 0 : tmp == 1 ? i : i*(-1); 

        loc(matrix,objects[i].i,objects[i].j,C) = objects[i].id;

    }

    
    print_mat(0);
    
    /*
    for(int i=0; i < N; i++)
        printf("%d %d %d\n", objects[i].type, objects[i].i, objects[i].j);
    */
    /*

    moves = (int *)malloc(N * sizeof(int));

    for(int gen=0; gen < N_GEN; gen++){
        
        for(int n = 0; n < N; n++){
            
            if (objects[n].type == 0 || objects[n].state == 0)
                continue;

            prepare_move(n);
        }

        for(int n = 0; n < N; n++){
            
            if (objects[n].type == 0 || objects[n].state == 0 || moves[n] == -1)
                continue;

        }

    }

    */

}