//
// Created by jayendra on 4/27/17.
//

#ifndef PROJECT_FINAL_HELPER_1_H
#define PROJECT_FINAL_HELPER_1_H

#include "ds_libraries.h"

unsigned long andKey(unsigned long key)
{
    return (key & MAX_KEY);
}

struct node* getAddress(struct node* p)
{
    return (struct node*)((uintptr_t) p &  ~ADDRESS_MASK);
}

bool IFlagSet(struct node* p)
{
    return ((uintptr_t) p & INJECT_BIT) != 0;
}

bool DFlagSet(struct node* p)
{
    return ((uintptr_t) p & DELETE_BIT) != 0;
}

bool PFlagSet(struct node* p)
{
    return ((uintptr_t) p & PROMOTE_BIT) != 0;
}

bool isNull(struct node* p)
{
    return ((uintptr_t) p & NULL_BIT) != 0;
}

bool KeyMarked(unsigned long key)
{
    return ((key & KEY_MASK) == KEY_MASK);
}

unsigned long setReplaceFlagInKey(unsigned long key)
{
    return (key | KEY_MASK);
}

struct node* setNull(struct node* p)
{
    return (struct node*) ((uintptr_t) p | NULL_BIT);
}

struct node* setIFlag(struct node* p)
{
    return (struct node*) ((uintptr_t) p | INJECT_BIT);
}

struct node* setDFlag(struct node* p)
{
    return (struct node*) ((uintptr_t) p | DELETE_BIT);
}

struct node* setPFlag(struct node* p)
{
    return (struct node*) ((uintptr_t) p | PROMOTE_BIT);
}

#endif //PROJECT_FINAL_HELPER_1_H
