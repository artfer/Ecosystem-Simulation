#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h> 

#define NTHREADS 2
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
omp_lock_t rab_lock;
omp_lock_t fox_lock;
omp_lock_t *mat_lock;

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

    if (t == 1){
        if (n_rab == size_rab){
            
            int n = 0;
            for(n = 0; n < n_rab; n++)
                if (rabbits[n].state == 0){
                    omp_set_lock(&mat_lock[j]);
                    loc(i,j) = rabbits[n].id;
                    omp_unset_lock(&mat_lock[j]);
                    rabbits[n].i = i;
                    rabbits[n].j = j;
                    rabbits[n].gen_proc = 0;
                    rabbits[n].state = 1;
                    rabbits[n].move = -2;
                    return;
            }
            size_rab *= 2;
            rabbits = (Object *)realloc(rabbits, size_rab * sizeof(Object));
            //printf("gen %d   after size_rab %d\n\n",gen,size_rab);
        }
        rabbits[n_rab].id = n_rab + 1;
        //omp_set_lock(&mat_lock[j]);
        loc(i,j) = rabbits[n_rab].id;
        //omp_unset_lock(&mat_lock[j]);
        rabbits[n_rab].i = i;
        rabbits[n_rab].j = j;
        rabbits[n_rab].gen_proc = 0;
        rabbits[n_rab].state = 1;
        n_rab++;
    }
    else if (t == 2){
        if (n_fox == size_fox){

            int n = 0;
            for(n = 0; n < n_fox; n++)
                if (foxes[n].state == 0){
                    loc(i,j) = foxes[n].id;
                    foxes[n].i = i;
                    foxes[n].j = j;
                    foxes[n].gen_proc = 0;
                    foxes[n].gen_food = 0;
                    foxes[n].state = 1;
                    foxes[n].move = -2;
                    return;
                }


            size_fox *= 2;
            foxes = (Object *)realloc(foxes, size_fox * sizeof(Object));
            //printf("gen %d   after size_fox %d\n\n",gen,size_fox);
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
        /*if (c == 0){
            cur->move = -1;
            return;
        }*/
    }
    cur->move = (c == 0) ? -1 : (c == 1) ? moves[0] : moves[(gen + cur->i + cur->j) % c]; 

    //printf("i:%d  j:%d  c:%d  move:%d\n",cur->i,cur->j,c,cur->move);
}



void move_rabbit(int i){
    //printf("tid %d\n",omp_get_thread_num());

    if(rabbits[i].move == -1){
        rabbits[i].gen_proc++;
        return;
    }

    //printf("i:%d   j:%d   state:%d\n",cur->i,cur->j,cur->state);
    loc(rabbits[i].i,rabbits[i].j) = EMPTY;

    //printf("1\n");
    if(rabbits[i].gen_proc == GEN_PROC_RABBITS){
        rabbits[i].gen_proc = 0;
        omp_set_lock(&rab_lock);
        new_object(1,rabbits[i].i,rabbits[i].j);
        omp_unset_lock(&rab_lock);
        
    } else 
        rabbits[i].gen_proc++;

    //printf("2\n");
    int tmp = loc(Get_i(rabbits[i].move,rabbits[i].i), 
                Get_j(rabbits[i].move,rabbits[i].j));

    // another rabbit moved here
    if(tmp > 0){
        if(rabbits[i].gen_proc <= rabbits[(tmp-1)].gen_proc){
            rabbits[i].state = 0;
            //return;
        }
        else if (rabbits[i].gen_proc > rabbits[(tmp-1)].gen_proc){
            rabbits[(tmp-1)].state = 0;
        }
    }


    //printf("3\n");
    // update location
    rabbits[i].i = Get_i(rabbits[i].move, rabbits[i].i);
    rabbits[i].j = Get_j(rabbits[i].move, rabbits[i].j);

    omp_set_lock(&mat_lock[rabbits[i].j]);
    loc(rabbits[i].i,rabbits[i].j) = rabbits[i].id;
    omp_unset_lock(&mat_lock[rabbits[i].j]);
}


void move_fox(int i){
    //printf("id %d\n",foxes[i].id);

    if(foxes[i].move == -1){
        foxes[i].gen_food++;
        foxes[i].gen_proc++;
        return;
    }

    loc(foxes[i].i,foxes[i].j) = EMPTY;

    int tmp = loc(Get_i(foxes[i].move,foxes[i].i), 
                Get_j(foxes[i].move,foxes[i].j));

    // a rabbit is here
    if(tmp > 0){
        foxes[i].gen_food = 0;
        rabbits[(tmp-1)].state = 0;
    } else {
        foxes[i].gen_food++;
        if(foxes[i].gen_food == GEN_FOOD_FOXES){
            foxes[i].state = 0;
            return;
        }
    }

    if(foxes[i].gen_proc == GEN_PROC_FOXES){
        new_object(2,foxes[i].i,foxes[i].j);
        foxes[i].gen_proc = 0;
    } else
        foxes[i].gen_proc++;


    // another fox is here
    if(tmp != EMPTY && tmp < 0){
        if(foxes[i].gen_proc < foxes[((tmp * -1) -1) ].gen_proc){
            foxes[i].state = 0;
            return;
        }
        else if (foxes[i].gen_proc > foxes[((tmp * -1) -1) ].gen_proc){
            foxes[((tmp * -1) -1)].state = 0;
        }
        else if (foxes[i].gen_food < foxes[((tmp * -1) -1) ].gen_food){
            foxes[((tmp * -1) -1)].state = 0;
        }
        else if (foxes[i].gen_food < foxes[((tmp * -1) -1) ].gen_food){
            foxes[i].state = 0;
            return;
        }
        else{
            foxes[i].state = 0;
            return;
        }
    }

    // update location
    foxes[i].i = Get_i(foxes[i].move, foxes[i].i);
    foxes[i].j = Get_j(foxes[i].move, foxes[i].j);

    loc(foxes[i].i,foxes[i].j) = foxes[i].id;
}


int main(){

    scanf("%d", &GEN_PROC_RABBITS);
    scanf("%d", &GEN_PROC_FOXES);
    scanf("%d", &GEN_FOOD_FOXES);
    scanf("%d", &N_GEN);
    scanf("%d", &R);
    scanf("%d", &C);
    scanf("%d", &N);

    //printf("R:%d   C:%d\n", R, C);

    matrix = (int *)malloc(R * C * sizeof(int));

    for (int i = 0; i < R; i++)
        for (int j = 0; j < C; j++)
            loc(i, j) = EMPTY;


    size_rab = N;
    size_fox = N;
    rabbits = (Object *)malloc(size_rab * sizeof(Object));
    foxes = (Object *)malloc(size_fox * sizeof(Object));
    n_fox = 0;
    n_rab = 0;
    

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
    
    mat_lock = (omp_lock_t *)malloc(C * sizeof(omp_lock_t));
    for(i = 0; i < C; i++){
        omp_init_lock(&mat_lock[i]);
    }

    omp_init_lock(&rab_lock);
    omp_init_lock(&fox_lock);
    

    //double t = omp_get_wtime();
    //double t1;
    clock_t t,t1;
    t = clock(); 

    for(gen = 0; gen <= N_GEN; gen++){

        //print_mat(gen);
        //printf("gen %d\n",gen);
        cur_rab = n_rab;
        cur_fox = n_fox;

        for(i = 0; i < cur_rab; i++)
            if(rabbits[i].state)
                prepare_move(&rabbits[i]);

        //t1 = omp_get_wtime();
        t1 = clock();

        #pragma omp parallel for num_threads(2) 
        for(int x = 0; x < cur_rab; x++)
            if(rabbits[x].state || rabbits[x].move == -2)
                move_rabbit(x);        

        //t1 = omp_get_wtime() - t1;
        //printf("time %f\n", t1);
        t1 = clock() - t1;
        double ti= ((double)t1)/CLOCKS_PER_SEC;
        printf("time %f\n",ti);


        
        for(i = 0; i < cur_fox; i++)
            if(foxes[i].state)
                prepare_move(&foxes[i]);


        for(i = 0; i < cur_fox; i++)
            if(foxes[i].state || foxes[i].move == -2)
                move_fox(i);

        update_matrix();
        
    }

    //t = omp_get_wtime() - t;
    //printf("time %f\n", t);
    t = clock() - t;
    double time = ((double)t)/CLOCKS_PER_SEC;
    printf("time %f\n",time);


    free(rabbits);
    free(foxes);
    free(matrix);
}