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
    int moved;          // Moved this turn, 0 -> no, 1 -> yes  
} Object;


int GEN_PROC_RABBITS; // number of generations until a rabbit can procreate
int GEN_PROC_FOXES;   // number of generations until a fox can procreate
int GEN_FOOD_FOXES;   // number of generations for a fox to die of starvation
int N_GEN;            // number of generations for the simulation
int R;                // number of rows of the matrix representing the ecosystem
int C;                // number of columns of the matrix representing the ecosystem
int N;                // number of objects in the initial ecosystem
Object *foxes;
Object *rabbits;
int* matrix;
int n_fox, n_rab;

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

/*
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
*/


void make_move(int n, int t){
    
    Object cur;
    if(t == 1)
        cur = rabbits[n];
    else
        cur = foxes[n];
    
    if(cur.state == 0){
        return;
    }

    int i = cur.i;
    int j = cur.j;
    int dest_i = -1;
    int dest_j = -1;

    //if fox, search for rabbit
    if(t == 2){
        if(loc(matrix,i-1,j,C) > 0){
            dest_i = i-1;
            dest_j = j;
        } else if(loc(matrix,i,j+1,C) > 0){
            dest_i = i;
            dest_j = j+1;
        } else if(loc(matrix,i+1,j,C) > 0){
            dest_i = i+1;
            dest_j = j;
        } else if(loc(matrix,i,j-1,C) > 0){
            dest_i = i;
            dest_j = j-1;
        }
    }
    if(dest_i == -1){
        if(loc(matrix,i-1,j,C) == EMPTY){
            dest_i = i-1;
            dest_j = j;
        }
        else if(loc(matrix,i,j+1,C) == EMPTY){
            dest_i = i;
            dest_j = j+1;
        }
        else if(loc(matrix,i+1,j,C) == EMPTY){
            dest_i = i+1;
            dest_j = j;
        }
        else if(loc(matrix,i,j-1,C) == EMPTY){
            dest_i = i;
            dest_j = j-1;
        }
        else {
            cur.moved = 0;
            return;
        }
    }

    

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


    foxes = (Object *) malloc(N * sizeof(Object));
    rabbits = (Object *) malloc(N * sizeof(Object));
    n_fox = 0;
    n_rab = 0;

    for(int n=0; n < N; n++){

        char tmp_type[10];
        int i,j;

        scanf("%s %d %d", tmp_type, &i, &j);

        int tmp = str_to_num(tmp_type);
        

        if(tmp != 0){
            if(tmp == 1){
                rabbits[n_rab].id = n_rab + 1;
                loc(matrix,i,j,C) = rabbits[n_rab].id;
                rabbits[n_rab].i = i;
                rabbits[n_rab].j = j;
                rabbits[n_rab].gen_proc = 0;
                rabbits[n_rab].state = 1;
                n_rab++;
            } else if (tmp == 2){
                foxes[n_fox].id = n_fox + 1;
                loc(matrix,i,j,C) = foxes[n_fox].id * -1;
                foxes[n_fox].i = i;
                foxes[n_fox].j = j;
                foxes[n_fox].gen_proc = 0;
                foxes[n_fox].gen_food = 0;
                foxes[n_fox].state = 1;
                n_fox++;
            }  
        } else {
            loc(matrix,i,j,C) = 0;
        }
        

    }

    
    int i;
    for(int gen = 0; gen < N_GEN; gen++){

        print_mat(gen);
        
        for(i = 0; i < n_rab; i++)
            make_move(i,1);

        for(i = 0; i < n_fox; i++)
            make_move(i,2);

    }

}