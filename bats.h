//
// Created by ferragut on 28/09/2019.
//
#include "queue.h"

#ifndef EXP1_STRUCTS_H

typedef enum {
    NORTH = 0,
    EAST,
    SOUTH,
    WEST
} possibles_direction;

typedef struct BATStruct{
    possibles_direction direction;
    int car_number;
} BAT ;


BAT* new_BAT(int direction, int car_number){
    BAT* new_car_BAT = malloc(sizeof(BAT));
    new_car_BAT->direction = direction;
    new_car_BAT->car_number = car_number;
    return new_car_BAT;
}
#define EXP1_STRUCTS_H

#endif //EXP1_STRUCTS_H
