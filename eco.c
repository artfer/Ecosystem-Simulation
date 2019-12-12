#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EMPTY -2147483648
#define loc(i, j) (matrix[(i * C) + j])
#define valid(i, j) (i < 0 ? 0 : i >= R ? 0 : j < 0 ? 0 : j >= C ? 0 : 1)
#define North_f(i, j) (valid((i - 1), j) && (loc((i - 1), j) > 0))
#define East_f(i, j) (valid(i, (j + 1)) && (loc(i, (j + 1)) > 0))
#define South_f(i, j) (valid((i + 1), j) && (loc((i + 1), j) > 0))
#define West_f(i, j) (valid(i, (j - 1)) && (loc(i, (j - 1)) > 0))
#define North(i, j) (valid((i - 1), j) && (loc((i - 1), j) == EMPTY))
#define East(i, j) (valid(i, (j + 1)) && (loc(i, (j + 1)) == EMPTY))
#define South(i, j) (valid((i + 1), j) && (loc((i + 1), j) == EMPTY))
#define West(i, j) (valid(i, (j - 1)) && (loc(i, (j - 1)) == EMPTY))
#define Get_i(p, i) ((p == 0) ? (i - 1) : (p == 2) ? (i + 1) : i)
#define Get_j(p, j) ((p == 1) ? (j + 1) : (p == 3) ? (j - 1) : j)

typedef struct{
    int id;       // id of object
    int i;        // position i in the enviroment
    int j;        // position j in the enviroment
    int gen_food; // generations passed without eating
    int gen_proc; // generations passed without procreating
    int state;    // 0 -> Dead, 1 -> Alive
    int move;
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
int *matrix;
int n_fox, n_rab;
int size_fox, size_rab;
int gen;

int str_to_num(char str[]){
    if (!strcmp(str, "ROCK"))
        return 0;
    else if (!strcmp(str, "RABBIT"))
        return 1;
    else
        return 2;
}

char num_to_char(int x){
    if (x == EMPTY)
        return ' ';
    else if (x == 0)
        return '*';
    else if (x > 1)
        return 'R';
    else
        return 'F';
}

char *num_to_str(int x){
    if (x == EMPTY)
        return "";
    else if (x < 0)
        return "FOX";
    else if (x == 0)
        return "ROCK";
    else
        return "RABBIT";
}

void print_mat(int g){
    printf("Generation %d\n", g);
    int tmp;
    for (int c = 0; c < C + 2; c++)
        printf("-");
    printf("\n");
    for (int i = 0; i < R; i++)
    {
        printf("|");
        for (int j = 0; j < C; j++)
        {
            tmp = loc(i, j);
            if (tmp == EMPTY)
                printf(" ");
            else if (tmp < 0)
                printf("F");
            else if (tmp == 0)
                printf("*");
            else if (tmp > 0)
                printf("R");
        }
        printf("|\n");
    }
    for (int c = 0; c < C + 2; c++)
        printf("-");
    printf("\n\n");
}

void print_real_mat(int g){
    printf("Generation %d\n", g);
    for (int c = 0; c < C + 2; c++)
        printf("-");
    printf("\n");
    for (int i = 0; i < R; i++)
    {
        printf("|");
        for (int j = 0; j < C; j++)
        {
            printf("%d ", loc(i, j));
        }
        printf("|\n");
    }
    for (int c = 0; c < C + 2; c++)
        printf("-");
    printf("\n\n");
}

void other_print(){
    int i = 0;
    for (i = 0; i < R * C; i++)
    {
        if (i % (C) == 0 && i != 0)
            printf("\n");
        printf("%d ", matrix[i]);
    }
    printf("\n");
}

void print_around(Object *cur){
    printf("i:%d j:%d\n", cur->i, cur->j);
    if (valid(cur->i - 1, cur->j))
        printf("0 -> %d\n", loc((cur->i - 1), cur->j));
    if (valid(cur->i, cur->j + 1))
        printf("1 -> %d\n", loc(cur->i, (cur->j + 1)));
    if (valid(cur->i + 1, cur->j))
        printf("2 -> %d\n", loc((cur->i + 1), cur->j));
    if (valid(cur->i, cur->j - 1))
        printf("3 -> %d\n", loc(cur->i, (cur->j - 1)));
    printf("\n");
}

void new_object(int t, int i, int j){
    if (t == 1)
    {   
        if (n_rab == size_rab){
            size_rab *= 2;
            rabbits = (Object *)realloc(rabbits, size_rab * sizeof(Object));
        }
        rabbits[n_rab].id = n_rab + 1;
        loc(i,j) = rabbits[n_rab].id;
        rabbits[n_rab].i = i;
        rabbits[n_rab].j = j;
        rabbits[n_rab].gen_proc = 0;
        rabbits[n_rab].state = 1;
        n_rab++;
    }
    else if (t == 2)
    {
        if (n_fox == size_fox){
            size_fox *= 2;
            foxes = (Object *)realloc(foxes, size_fox * sizeof(Object));
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


void update_matrix(){
    int n;

    for(n=0; n < (R*C); n++)
        matrix[n] = matrix[n] == 0 ? 0 : EMPTY;

    for(n=0; n < n_rab; n++)
        if(rabbits[n].state==1)
            loc(rabbits[n].i,rabbits[n].j) = rabbits[n].id;
        

    for(n=0; n < n_fox; n++)
        if(foxes[n].state==1)
            loc(foxes[n].i,foxes[n].j) = foxes[n].id;
}


void prepare_move(Object *cur){
    
    int moves[4];
    int c = 0;

    // if fox, search for rabbit
    if (cur->id < 0){
        if (North_f(cur->i, cur->j)){
            moves[c] = 0; 
            c++;
        } 
        if (East_f(cur->i, cur->j)){
            moves[c] = 1; 
            c++;
        } 
        if (South_f(cur->i, cur->j)){
            moves[c] = 2; 
            c++;
        } 
        if (West_f(cur->i, cur->j)){
            moves[c] = 3; 
            c++;
        }
    }
    if (c == 0){
        if (North(cur->i, cur->j)){
            moves[c] = 0; 
            c++;
        } 
        if (East(cur->i, cur->j)){
            moves[c] = 1; 
            c++;
        }
        if (South(cur->i, cur->j)){
            moves[c] = 2; 
            c++;
        }
        if (West(cur->i, cur->j)){
            moves[c] = 3; 
            c++;
        }
        if (c == 0){
            cur->move = -1;
            return;
        }
    }

    cur->move = (c == 1) ? moves[0] : moves[(gen + cur->i + cur->j) % c]; 

    //printf("i:%d  j:%d  c:%d  move:%d\n",cur->i,cur->j,c,cur->move);
}

void move_rabbit(Object *cur){

    //printf("i:%d   j:%d   state:%d\n",cur->i,cur->j,cur->state);
    loc(cur->i,cur->j) = EMPTY;

    if(cur->gen_proc == GEN_PROC_RABBITS){
        new_object(1,cur->i,cur->j);
        cur->gen_proc = 0;
    } else 
        cur->gen_proc++;

    int tmp = loc(Get_i(cur->move,cur->i), Get_j(cur->move,cur->j));

    // another rabbit moved here
    if(tmp > 0){
        cur->state = 0;
        return;
    }

    // update location
    cur->i = Get_i(cur->move, cur->i);
    cur->j = Get_j(cur->move, cur->j);

    loc(cur->i,cur->j) = cur->id;
}


void move_fox(Object *cur){

    loc(cur->i,cur->j) = EMPTY;

    int tmp = loc(Get_i(cur->move,cur->i), Get_j(cur->move,cur->j));

    // a rabbit is here
    if(tmp > 0){
        cur->gen_food = 0;
        rabbits[(tmp-1)].state = 0;
    } else {
        cur->gen_food++;
        if(cur->gen_food == GEN_FOOD_FOXES){
            cur->state = 0;
            return;
        }
    }

    // another fox is here
    if(tmp != EMPTY && tmp < 0){
        if(cur->gen_food >= foxes[((tmp * -1) -1) ].gen_food){
            cur->state = 0;
            return;
        }
        else
            foxes[((tmp * -1) -1)].state = 0;
    }

    if(cur->gen_proc == GEN_PROC_FOXES){
        new_object(2,cur->i,cur->j);
        cur->gen_proc = 0;
    } else
        cur->gen_proc++;


    // update location
    cur->i = Get_i(cur->move, cur->i);
    cur->j = Get_j(cur->move, cur->j);

    loc(cur->i,cur->j) = cur->id;
}


int main(){

    scanf("%d", &GEN_PROC_RABBITS);
    scanf("%d", &GEN_PROC_FOXES);
    scanf("%d", &GEN_FOOD_FOXES);
    scanf("%d", &N_GEN);
    scanf("%d", &R);
    scanf("%d", &C);
    scanf("%d", &N);

    printf("R:%d   C:%d\n", R, C);

    matrix = (int *)malloc(R * C * sizeof(int));

    for (int i = 0; i < R; i++)
        for (int j = 0; j < C; j++)
            loc(i, j) = EMPTY;

    foxes = (Object *)malloc(N * sizeof(Object));
    rabbits = (Object *)malloc(N * sizeof(Object));
    n_fox = 0;
    n_rab = 0;
    size_rab = N;
    size_fox = N;

    int i, j;
    for (int n = 0; n < N; n++)
    {

        char tmp_type[10];

        scanf("%s %d %d", tmp_type, &i, &j);

        int tmp = str_to_num(tmp_type);

        if (tmp != 0)
            new_object(tmp, i, j);
        else
            loc(i, j) = 0;
    }

    //update_matrix();

    int cur_rab, cur_fox;
    
    for(gen = 0; gen <= N_GEN; gen++){

        print_mat(gen);

        cur_rab = n_rab;
        cur_fox = n_fox;


        for(i = 0; i < cur_rab; i++)
            if(rabbits[i].state)
                prepare_move(&rabbits[i]);

        for(i = 0; i < cur_rab; i++)
            if(rabbits[i].state || rabbits[i].move == -1)
                move_rabbit(&rabbits[i]);        


        for(i = 0; i < cur_fox; i++)
            if(foxes[i].state)
                prepare_move(&foxes[i]);

        for(i = 0; i < cur_fox; i++)
            if(foxes[i].state || foxes[i].move == -1)
                move_fox(&foxes[i]);

        update_matrix();
        
    }

    /*
    free(rabbits);
    free(foxes);
    free(matrix);
    */
}