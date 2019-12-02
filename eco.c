#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY -2147483648
#define loc(i,j) matrix[ (i * C) + j ] 
#define valid(i,j) i < 0 ? 0 : i > R ? 0 : j < 0 ? 0 : j > C ? 0 : 1
#define North_f(i,j) valid(i-1,j) && loc(i-1,j) > 0
#define East_f(i,j) valid(i,j+1) && loc(i,j+1) > 0
#define South_f(i,j) valid(i+1,j) && loc(i+1,j) > 0
#define West_f(i,j) valid(i,j-1) && loc(i,j-1) > 0
#define North(i,j) valid(i-1,j) && loc(i-1,j) == EMPTY
#define East(i,j) valid(i,j+1) && loc(i,j+1) == EMPTY
#define South(i,j) valid(i+1,j) && loc(i+1,j) == EMPTY
#define West(i,j) valid(i,j-1) && loc(i,j-1) == EMPTY
#define Get_i(p,i) p == 0 ? i-1 : p == 2 ? i+1 : i
#define Get_j(p,j) p == 1 ? j+1 : p == 3 ? j-1 : j  

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
int size_fox, size_rab;
int gen;

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
            tmp = loc(i,j); 
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

void new_object(int t, int i,int j){
    if(t == 1){
        if(n_rab == size_rab){
            size_rab *= 2;
            rabbits = (Object *) realloc(rabbits,size_rab * sizeof(Object));
        }
        rabbits[n_rab].id = n_rab + 1;
        loc(i,j) = rabbits[n_rab].id;
        rabbits[n_rab].i = i;
        rabbits[n_rab].j = j;
        rabbits[n_rab].gen_proc = 0;
        rabbits[n_rab].state = 1;
        n_rab++;
    } else if (t == 2){
        if(n_fox == size_fox){
            size_fox *= 2;
            foxes = (Object *) realloc(foxes,size_fox * sizeof(Object));
        }
        foxes[n_fox].id = (n_fox + 1) * -1;
        loc(i,j) = foxes[n_fox].id;
        foxes[n_fox].i = i;
        foxes[n_fox].j = j;
        foxes[n_fox].gen_proc = 0;
        foxes[n_fox].gen_food = 0;
        foxes[n_fox].state = 1;
        n_fox++;
    }

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
        if(loc(matrix,i-1,j) > 0){
            moves[n] = 0;
            return;
        } else if(loc(matrix,i,j+1) > 0){
            moves[n] = 1;
            return;
        } else if(loc(matrix,i+1,j) > 0){
            moves[n] = 2;
            return;
        } else if(loc(matrix,i,j-1) > 0){
            moves[n] = 3;
            return;
        }
    }

    if(loc(matrix,i-1,j) == EMPTY)
        moves[n] = 0;
    else if(loc(matrix,i,j+1) == EMPTY)
        moves[n] = 1;
    else if(loc(matrix,i+1,j) == EMPTY)
        moves[n] = 2;
    else if(loc(matrix,i,j-1) == EMPTY)
        moves[n] = 3;
    else
        moves[n] = -1;
}
*/


void make_move(int n, int t, Object *cur){
    
    if(cur->state == 0){
        return;
    }

    int move[4];
    int c = 0;

    //if fox, search for rabbit
    if(cur->id < 0){
        if(North_f(cur->i,cur->j)){
             move[c] = 0; c++;
        } if(East_f(cur->i,cur->j)){
            move[c] = 1; c++;
        } if(South_f(cur->i,cur->j)){
            move[c] = 2; c++;
        } if(West_f(cur->i,cur->j)){
            move[c] = 3; c++;
        }
    }
    if(c == 0){
        if(North(cur->i,cur->j)){
             move[c] = 0; c++;
        } if(East(cur->i,cur->j)){
            move[c] = 1; c++;
        } if(South(cur->i,cur->j)){
            move[c] = 2; c++;
        } if(West(cur->i,cur->j)){
            move[c] = 3; c++;
        }
        if(c == 0){
            cur->moved = 0;
            return;
        }
    }

    if(c == 1){
        loc(cur->i,cur->j) = EMPTY;
        cur->i = Get_i(move[0],cur->i);
        cur->j = Get_j(move[0],cur->j);
        loc(cur->i, cur->j) = cur->id;
    }
    /*
    loc(matrix,i,j) = EMPTY;
    loc(matrix,dest_i,dest_j) = cur.id;
    cur.gen_proc++;
    if(t == 1 && cur.gen_proc % GEN_PROC_RABBITS == 0){
        
    }*/



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
            loc(i,j) = EMPTY;


    foxes = (Object *) malloc(N * sizeof(Object));
    rabbits = (Object *) malloc(N * sizeof(Object));
    n_fox = 0;
    n_rab = 0;
    size_rab = N;
    size_fox = N;

    for(int n=0; n < N; n++){

        char tmp_type[10];
        int i,j;

        scanf("%s %d %d", tmp_type, &i, &j);

        int tmp = str_to_num(tmp_type);
        
        if(tmp != 0)
            new_object(tmp, i,j);
        else
            loc(i,j) = 0;

    }

    size_rab = n_rab;
    size_fox = n_fox;

    printf("Rabbits:%d   Foxes:%d\n\n",size_rab,size_fox);

    
    int i;
    for(gen = 0; gen < N_GEN; gen++){

        print_mat(gen);
        
        for(i = 0; i < n_rab; i++)
            make_move(i, 1, &rabbits[i]);

        for(i = 0; i < n_fox; i++)
            make_move(i, 2, &foxes[i]);

    }

}