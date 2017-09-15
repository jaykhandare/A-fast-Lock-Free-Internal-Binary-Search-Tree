//
// Created by jayendra on 4/17/17.
//
//  I have made some changes in the code after I've finished the report.
//  Before this edit,operateOnTree functions would directly start working with given arguments.
//  In this edit(April29), I have added one more function which will fill up the tree before anything else starts.



//  Remember to change report

#include <iostream>
#include <random>
#include <thread>
#include <cstring>
#include <mutex>

#include "bst_functions.h"

using namespace std;

int NUM_OF_THREADS;
int NUM_OF_OPS;
int insertPercent;
int searchPercent;
int deletePercent;
int inserts,searches,deletes;

void *operateOnTree_NL(void* args){

    struct args* tData = (struct args*) args;
    random_device rd_NL;
    default_random_engine entropy_engine(rd_NL());
    uniform_int_distribution<uint16_t> ui_dist16(0, numeric_limits<uint16_t>::max());
    uniform_int_distribution<uint32_t> ui_dist32(0, numeric_limits<uint32_t>::max());

    inserts = insertPercent*NUM_OF_OPS*NUM_OF_THREADS/100;
    searches = searchPercent*NUM_OF_OPS*NUM_OF_THREADS/100;
    deletes = deletePercent*NUM_OF_OPS*NUM_OF_THREADS/100;

    while(true){

        if(inserts>0){
            insert(tData,ui_dist32(entropy_engine));
            inserts--;
        }
        else if(searches>0){
            search(tData,ui_dist32(entropy_engine));
            searches--;
        }
        else if(deletes>0){
            remove(tData,ui_dist32(entropy_engine));
            deletes--;
        }
        else{
            break;
        }
    }

//    cout<<"INSERTS : "<<insertPercent<<endl;
//    cout<<"SEARCHES : "<<searchPercent<<endl;
//    cout<<"DELETES : "<<deletePercent<<endl;
}
void *operateOnTree_L(void* args){

    mutex mtx;
    struct args* tData = (struct args*) args;
    random_device rd_L;
    default_random_engine entropy_engine(rd_L());
    uniform_int_distribution<uint16_t> ui_dist16(0, numeric_limits<uint16_t>::max());
    uniform_int_distribution<uint32_t> ui_dist32(0, numeric_limits<uint32_t>::max());

    inserts = insertPercent*NUM_OF_OPS*NUM_OF_THREADS/100;
    searches = searchPercent*NUM_OF_OPS*NUM_OF_THREADS/100;
    deletes = deletePercent*NUM_OF_OPS*NUM_OF_THREADS/100;

    while(true){

        if(inserts>0){
            mtx.lock();
            insert(tData,ui_dist32(entropy_engine));
            inserts--;
            mtx.unlock();
        }
        else if(searches>0){
            mtx.lock();
            search(tData,ui_dist32(entropy_engine));
            searches--;
            mtx.unlock();
        }
        else if(deletes>0){
            mtx.lock();
            remove(tData,ui_dist32(entropy_engine));
            deletes--;
            mtx.unlock();
        }
        else{
            break;
        }
    }
//    cout<<"INSERTS : "<<insertPercent<<endl;
//    cout<<"SEARCHES : "<<searchPercent<<endl;
//    cout<<"DELETES : "<<deletePercent<<endl;
}

void *operateOnTree(void* args){

    struct args* tData = (struct args*) args;
    random_device rd_NL;
    default_random_engine entropy_engine(rd_NL());
    uniform_int_distribution<uint16_t> ui_dist16(0, numeric_limits<uint16_t>::max());
    uniform_int_distribution<uint32_t> ui_dist32(0, numeric_limits<uint32_t>::max());

    inserts = NUM_OF_OPS*NUM_OF_THREADS;

    while(true){

        if(inserts>0){
            insert(tData,ui_dist32(entropy_engine));
            inserts--;
        }
        else{
            break;
        }
    }

//    cout<<"INSERTS : "<<insertPercent<<endl;
}



int main(int argc, char** argv) {
    if (argc == 2 and strncmp("-h", argv[1], 2) == 0) {
        cout << "Usage: " << argv[0] << " [NUM_OF_THREADS][NUM_OF_OPS_PER_THREAD][INSERT_PERCENT][SEARCH_PERCENT][DELETE_PERCENT]"<<endl;
        exit(0);
    }

    NUM_OF_THREADS = argc >= 2 ? atoi(argv[1]) : 64;
    NUM_OF_OPS = argc >= 3 ? atoi(argv[2]) : 100;
    insertPercent = argc >= 4 ? atoi(argv[3]) : 0;
    searchPercent = argc >= 5 ? atoi(argv[4]) : 50;
    deletePercent = argc >= 6 ? atoi(argv[5]) : 50;

    clock_t start, end;
    double cpu_time_used;

    if(searchPercent+insertPercent+deletePercent != 100){
        cout<<"Invalid Percentages"<<endl;
        return 1;
    }

    argsNL = (struct args**) malloc(NUM_OF_THREADS * sizeof(struct tArgs*));
    argsL = (struct args**) malloc(NUM_OF_THREADS * sizeof(struct tArgs*));

    // Filling up args
    for(size_t i=0;i<NUM_OF_THREADS;i++)
    {
        argsNL[i] = (struct args*) malloc(sizeof(struct args));
        argsNL[i]->val = i;
        argsL[i] = (struct args*) malloc(sizeof(struct args));
        argsL[i]->val = argsNL[i]->val;   // so that they have the same key value
    }


    createSENTINodes();
    // Reserve space for our vector of thread objects for lock free implementation
    vector<thread> thread_NL;
    thread_NL.reserve(NUM_OF_THREADS);
/*    thread_NL.emplace_back(operateOnTree, (void*) argsNL[0]);
    usleep(10000);
*/
    // Spawn our threads for lock free implementation
    start = clock();
    for (size_t thread_id = 1; thread_id < NUM_OF_THREADS;thread_id++) {
        thread_NL.emplace_back(operateOnTree_NL, (void*) argsNL[thread_id]);
    }

    // Join our threads for lock free implementation
    for (thread& t : thread_NL) {
        t.join();
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    cout<<"#threads : \t"<<NUM_OF_THREADS<<endl;
    cout<<"#ops : \t\t"<<NUM_OF_OPS<<endl;
    cout<<"Insert % : \t"<<insertPercent<<endl;
    cout<<"Search % : \t"<<searchPercent<<endl;
    cout<<"Delete % : \t"<<deletePercent<<endl;
    cout<<"Time_NL : \t"<<cpu_time_used<<endl;


    createSENTINodes();
    // Reserve space for our vector of thread objects for lock based implementation
    vector<thread> thread_L;
    thread_L.reserve(NUM_OF_THREADS);
/*    thread_L.emplace_back(operateOnTree, (void*) argsNL[0]);
    usleep(10000);
*/
    // Spawn our threads for lock based implementation
    start = clock();
    for (int thread_id = 1; thread_id < NUM_OF_THREADS;thread_id++) {
        thread_L.emplace_back(operateOnTree_L, (void*) argsL[thread_id]);
    }

    // Join our threads for lock based implementation
    for (thread& t : thread_L) {
        t.join();
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    cout<<"Time_L : \t"<<cpu_time_used<<endl;



/* verifying tree validity

    if(isBSTValid(T->child[LC],0,MAX_KEY)){
        cout<<"Yeah !!! it works alright "<<endl;
    }

*/
    return 0;
}