#include <iostream>
#include <vector> 
#include <string>
#include <time.h>
#include <omp.h>

using namespace std; 

#define EMPTY -2147483648
#define loc(i, j) (matrix[(i * C) + j])
#define lloc(i,j) ((i * C) + j)
#define valid(i, j) ((i < 0) ? 0 : (i >= R) ? 0 : (j < 0) ? 0 : (j >= C) ? 0 : 1)
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
#define NTHREADS 2


typedef struct{
    int id;       // id of object
    int i;        // position i in the enviroment
    int j;        // position j in the enviroment
    int gen_food; // generations passed without eating
    int gen_proc; // generations passed without procreating
    int state;    // 0 -> Dead, 1 -> Alive
    int move;
} Object;


int GEN_PROC_RABBITS;    // number of generations until a rabbit can procreate
int GEN_PROC_FOXES;      // number of generations until a fox can procreate
int GEN_FOOD_FOXES;      // number of generations for a fox to die of starvation
int N_GEN;               // number of generations for the simulation
int R;                   // number of rows of the matrix representing the ecosystem
int C;                   // number of columns of the matrix representing the ecosystem
int N;                   // number of objects in the initial ecosystem
vector<Object*> foxes;   // vector for rabbits
vector<Object*> rabbits; // vector for rabbits
vector<int> dead_r;      // positions of dead rabbits
vector<int> dead_f;      // positions of dead foxes
int r_ids, f_ids;        // ids for objects
int *matrix;             // matrix to store values
int gen;                 // number of current genaration
omp_lock_t r_lock;
omp_lock_t dr_lock;
omp_lock_t *mat_lock;




int str_to_num(string str){
    if (str.compare("ROCK") == 0)
        return 0;
    else if (str.compare("RABBIT") == 0)
        return 1;
    else
        return 2;
}


// print a string matrix
void print_mat(int g){
    cout << "Generation " << g << "\n";
    
    for (int c = 0; c < C + 2; c++)
        cout << "-";
    cout << "\n";

    int tmp;
    for (int i = 0; i < R; i++)
    {
        cout << "|";
        for (int j = 0; j < C; j++){

            tmp = loc(i, j);
            if (tmp == EMPTY)
                cout << " ";
            else if (tmp < 0)
                cout << "F";
            else if (tmp == 0)
                cout << "*";
            else if (tmp > 0)
                cout << "R";
        }
        cout << "|\n"; 
    }

    for (int c = 0; c < C + 2; c++)
        cout << "-";
    cout << "\n\n";
}

// print the number matrix
void print_real_mat(int g){
    
    cout << "Generation " << g << "\n";

    for (int c = 0; c < C + 2; c++)
        cout << "-";
    cout << "\n";

    for (int i = 0; i < R; i++){
        cout << "|";
        for (int j = 0; j < C; j++)
            cout << loc(i,j) << " ";
        cout << "|\n";
    }

    for (int c = 0; c < C + 2; c++)
        cout << "-";
    cout << "\n\n";
}


// print the final output
void print_final(){
    cout << GEN_PROC_RABBITS << " ";
    cout << GEN_PROC_FOXES << " ";
    cout << GEN_FOOD_FOXES << " ";
    cout << "0 ";
    cout << R << " ";
    cout << C << " ";

    N = 0;

    for(int i = 0; i < R; i++)
        for(int j = 0; j < C; j++)
            if(loc(i,j)!=EMPTY)
                N++;

    cout << N << "\n";

    for(int i = 0; i < R; i++)
        for(int j = 0; j < C; j++){
            if(loc(i,j) == 0)
                printf("ROCK %d %d\n",i,j);
            else if(loc(i,j) > 0)
                printf("RABBIT %d %d\n",i,j);
            else if(loc(i,j) != EMPTY)
                printf("FOX %d %d\n",i,j);
        }
}


void new_object(int t, int i, int j, int from_move){

    Object *tmp;
    
    if (t == 1){ // its a rabbit

        if(dead_r.empty()){ // use new position
            loc(i,j) = r_ids;
            tmp = new Object();
            tmp->id = r_ids;
            r_ids++;
            rabbits.push_back(tmp);

        } else { // re-use
            int idx = dead_r.back();
            dead_r.pop_back();
            
            tmp = rabbits.at(idx);
            loc(i,j) = tmp->id;
        }
    }
    else if (t == 2){ // its a fox

        if(dead_f.empty()){ // use new position
            loc(i,j) = (f_ids*(-1));
            tmp = new Object();
            tmp->id = (f_ids*(-1));
            f_ids++;
            foxes.push_back(tmp);
        } else { // re-use 
            int idx = dead_f.back();
            dead_f.pop_back();
            tmp = foxes.at(idx);
            loc(i,j) = tmp->id;
        }
    }

    tmp->i = i;
    tmp->j = j;
    tmp->gen_proc = 0;
    tmp->gen_food = 0;
    tmp->move = (from_move == 0) ? -1 : -2; // to distinguish cant move from new borns
    tmp->state = 1;
   
}

void prepare_move(Object *cur){
    
    int moves[4]; // 4 possible moves
    int c = 0; // counter for number of available moves

    // if fox, search for rabbit first
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
}


void move_rabbit(Object *cur){

    // cant move
    if(cur->move == -1){
        cur->gen_proc++;
        //return;
    } else {

        loc(cur->i,cur->j) = EMPTY;

        // check if its time to procreate
        if(cur->gen_proc == GEN_PROC_RABBITS){
            new_object(1, cur->i, cur->j, 1);
            cur->gen_proc = 0;
        } else 
            cur->gen_proc++;

        // whats in the next position?
        int tmp = loc(Get_i(cur->move,cur->i), 
                    Get_j(cur->move,cur->j));

        int dead = 0;

        // another rabbit moved there
        if(tmp > 0){

            // check who has higher proc_age and keep it
            if(cur->gen_proc <= rabbits.at((tmp-1))->gen_proc){
                cur->state = 0;
                dead_r.push_back((cur->id-1));
                //return;
                dead = 1;
            }
            else {
                rabbits.at((tmp-1))->state = 0;
                dead_r.push_back((tmp-1));
            }
        }

        if(!dead){
            // update location
            cur->i = Get_i(cur->move, cur->i);
            cur->j = Get_j(cur->move, cur->j);
            loc(cur->i,cur->j) = cur->id;
        }
    }
}


void move_fox(Object *cur){

    // cant move
    if(cur->move == -1){
        cur->gen_food++;
        cur->gen_proc++;
        return;
    }

    loc(cur->i,cur->j) = EMPTY;

    // whats in the next position
    int tmp = loc(Get_i(cur->move,cur->i), 
                Get_j(cur->move,cur->j));

    // if its a rabbit, eat it otherwise check if too hungry
    if(tmp > 0){
        cur->gen_food = 0;
        rabbits.at((tmp-1))->state = 0;
        dead_r.push_back(tmp-1);
    } else {
        cur->gen_food++;
        if(cur->gen_food == GEN_FOOD_FOXES){
            dead_f.push_back((cur->id * -1) -1);
            cur->state = 0;
            return;
        }
    }

    // check if its time to procreate
    if(cur->gen_proc == GEN_PROC_FOXES){
        new_object(2, cur->i, cur->j, 1);
        cur->gen_proc = 0;
    } else
        cur->gen_proc++;


    // another fox is there, keep the one according to conflict res 
    if(tmp != EMPTY && tmp < 0){
        
        if(cur->gen_proc < foxes.at(((tmp * -1) -1))->gen_proc){
            cur->state = 0;
            dead_f.push_back((cur->id * -1) -1);
            return;
        }
        else if (cur->gen_proc > foxes.at(((tmp * -1) -1))->gen_proc){
            foxes.at(((tmp * -1) -1))->state = 0;
            dead_f.push_back((tmp * -1) -1);
        }
        else if (cur->gen_food < foxes.at(((tmp * -1) -1))->gen_food){
            foxes.at(((tmp * -1) -1))->state = 0;
            dead_f.push_back((tmp * -1) -1);
        }
        else if (cur->gen_food < foxes.at(((tmp * -1) -1))->gen_food){
            cur->state = 0;
            dead_f.push_back((cur->id * -1) -1);
            return;
        }
        else{
            cur->state = 0;
            dead_f.push_back((cur->id * -1) -1);
            return;
        }
    }

    // update location
    cur->i = Get_i(cur->move,cur->i);
    cur->j = Get_j(cur->move,cur->j);
    loc(cur->i,cur->j) = cur->id;
}


int main(){
    
    cin >> GEN_PROC_RABBITS;
    cin >> GEN_PROC_FOXES;
    cin >> GEN_FOOD_FOXES;
    cin >> N_GEN;
    cin >> R;
    cin >> C;
    cin >> N;
    
    matrix = (int *)malloc(R * C * sizeof(int));

    //fill matrix with empty space
    for (int i = 0; i < R; i++)
        for (int j = 0; j < C; j++)
            loc(i, j) = EMPTY;

    //ids for the objects
    r_ids = 1;
    f_ids = 1;

    //read objects
    int i,j;
    for (int n = 0; n < N; n++){
        char tmp_type[10];

        scanf("%s %d %d", tmp_type, &i, &j);

        int tmp = str_to_num(tmp_type);

        if (tmp != 0)
            new_object(tmp, i, j, 0);
        else
            loc(i, j) = 0;

    }
    
    //current number of objects
    int cur_rab, cur_fox;

    double t = omp_get_wtime();

    for(gen = 0; gen < N_GEN; gen++){

        //print_mat(gen);
        //cout << gen << "\n";
        cur_rab = rabbits.size();
        cur_fox = foxes.size();

        #pragma omp parallel for num_threads(NTHREADS)
        for(i = 0; i < cur_rab; i++)
            if(rabbits.at(i)->state)
                prepare_move(rabbits.at(i));
        
        for(i = 0; i < cur_rab; i++)
            if(rabbits.at(i)->state && rabbits.at(i)->move > -2)
                move_rabbit(rabbits.at(i));  

        #pragma omp parallel for num_threads(NTHREADS)
        for(i = 0; i < cur_fox; i++)
            if(foxes.at(i)->state)
                prepare_move(foxes.at(i));
        
        for(i = 0; i < cur_fox; i++)
            if(foxes.at(i)->state && foxes.at(i)->move > -2)
                move_fox(foxes.at(i));  
        
    }

    t = omp_get_wtime() - t ;
    //std::printf("time %.3f\n",t);
    //print_mat(gen);
    print_final();


    return 0;
}