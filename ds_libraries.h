//
// Created by jayendra on 4/17/17.
//

#ifndef PROJECT_FINAL_DS_LIBRARIES_H
#define PROJECT_FINAL_DS_LIBRARIES_H

#include<pthread.h>
#include<atomic>
#include<random>
#include<time.h>
#include<cstdint>
#include<stdbool.h>
#include<assert.h>
#include <mutex>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<stdint.h>
#include <unistd.h>


#define LC 0
#define RC 1

#define SENTI_R 0x0
#define SENTI_S 0x1
#define SENTI_T 0x7FFFFFFE
#define KEY_MASK 0x80000000
#define MAX_KEY 0x7FFFFFFF
#define ADDRESS_MASK 15

#define NULL_BIT 8
#define INJECT_BIT 4
#define DELETE_BIT 2
#define PROMOTE_BIT 1

typedef enum {INJECTION, DISCOVERY, CLEANUP} Mode;
typedef enum {SIMPLE, COMPLEX} Type;
typedef enum {DELETE_FLAG, PROMOTE_FLAG} Flag;


struct node
{
    std::atomic<size_t> markedKey;
    std::atomic<node*> child[2];
    std::atomic<bool> readyToReplace;
};

struct edge
{
    struct node* parent;
    struct node* child;
    bool which;
};

struct seekRecord
{
    struct edge lastEdge;
    struct edge pLastEdge;
    struct edge injectionEdge;
};

struct anchorRecord
{
    struct node* node;
    size_t key;
};

struct stateRecord
{
    size_t depth;
    struct edge targetEdge;
    struct edge pTargetEdge;
    size_t targetKey;
    size_t currentKey;
    Mode mode;
    Type type;
    struct seekRecord successorRecord;
};

/*  thread specific records  */
struct args
{
    size_t val;
    struct node* newNode;
    bool isNewNodeAvailable;
    struct seekRecord targetRecord;
    struct seekRecord pSeekRecord;
    struct stateRecord myState;
    struct anchorRecord anchorRecord;
    struct anchorRecord pAnchorRecord;
};

struct args** argsNL;
struct args** argsL;

struct node* R;
struct node* S;
struct node* T;


#endif //PROJECT_FINAL_DS_LIBRARIES_H
