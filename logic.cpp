#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using std::cout, std::endl, std::ifstream, std::string;


char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {
    ifstream ifs(fileName);

   
    if(!ifs.is_open() || !ifs.good()){
        return nullptr;
    }

    else {
        int counter = 0;
        ifs >> maxRow >> maxCol;
        ifs >> player.row >> player.col;

        int totalchar = maxRow * maxCol;
        
        string line;
        

        while (getline(ifs, line) )
        {
            
            int lineSize = line.size();
            for (int c = 0; c < lineSize; c++)
            {
                if (line.at(c)!=' ')
                {
                    counter++;
                }
            }
            
        }
        if (counter!=totalchar)
            return nullptr;
        
        ifs.close();
    }

    

    ifs.open(fileName);

    
    ifs >> maxRow;
    if(ifs.fail()){
        return nullptr;
    }
    ifs >> maxCol;
    if(ifs.fail()){
        return nullptr;
    }
    
    if(maxRow <= 0 || maxRow <= 0 || maxCol>=(INT32_MAX-maxRow) || maxRow>=(INT32_MAX-maxCol) ){
        return nullptr;
    }

    ifs >> player.row;
    if(ifs.fail()){
        return nullptr;
    }
    ifs >> player.col;
    if(ifs.fail()){
        return nullptr;
    }

    if(player.row >= maxRow || player.col >= maxCol || player.row < 0 || player.col < 0){
        return nullptr;
    }

    int doorOrexit= 0;
    char** map = createMap(maxRow,maxCol);
    for(int i = 0; i < maxRow; i++){
        for(int j = 0; j < maxCol; j++){
            ifs >> map[i][j];
            

            if(ifs.fail()){
                deleteMap(map,maxRow);
                return nullptr;
            }

            char tile = map[i][j];
            if (!(tile==TILE_OPEN || tile==TILE_PLAYER || tile==TILE_TREASURE || tile==TILE_AMULET || tile==TILE_MONSTER || tile==TILE_PILLAR || tile==TILE_DOOR || tile==TILE_EXIT)){
                deleteMap(map,maxRow);
                return nullptr;
            }


            if(map[i][j] == TILE_DOOR || map[i][j] == TILE_EXIT){
                doorOrexit +=1;
            }
        }
    }
    
    if(doorOrexit == 0){
        deleteMap(map,maxRow);
        return nullptr;
    }



    map[player.row][player.col] = TILE_PLAYER;
    ifs.close();
    return map;
}
    



void getDirection(char input, int& nextRow, int& nextCol) {
   
    //up down left right
    //wasd

    if (input == MOVE_UP) {
        nextRow--;
    }
    if (input == MOVE_DOWN) {
        nextRow++;
    }
    if (input == MOVE_LEFT) {
        nextCol--;
    }
    if (input == MOVE_RIGHT) {
        nextCol++;
    }
}


char** createMap(int maxRow, int maxCol) {
    if(maxRow <= 0|| maxCol <= 0){
        return nullptr;
    }
    
    char** map = new char*[maxRow] {};         
    for(int row = 0; row < maxRow; row++){      
        map[row] = new char[maxCol] {};
    }
    for(int i = 0; i < maxRow ; i++){
        for(int j = 0; j < maxCol; j++){
            map[i][j] = TILE_OPEN;
        }
    }
    return map;
    
}


void deleteMap(char**& map, int& maxRow) {
    if (map!=nullptr) {
        for (int i=0; i<maxRow; i++) {
            delete[] map[i];
        }   
        delete[] map;
    }

    maxRow = 0;
    map = nullptr;
}


char** resizeMap(char** map, int& maxRow, int& maxCol) {
    int iMaxRow = maxRow;
    int iMaxCol = maxCol;

    if(maxRow <= 0 || maxCol <= 0 || map == nullptr){
        return nullptr;
    }

    maxRow *=2;
    maxCol *=2;
   
    char** newMap = createMap(maxRow,maxCol);


    for(int i =0; i < iMaxRow; i++){
        for(int j = 0; j < iMaxCol; j++){
            newMap[i][j] = map[i][j];
        }
    }


    for(int i = 0 ; i < iMaxRow; i++){
        for(int j = iMaxCol; j < maxCol; j++){
            newMap[i][j] = map[i][j - iMaxCol];

            if(newMap[i][j] == TILE_PLAYER){
                newMap[i][j] = TILE_OPEN;
            }
        }
    }


    for(int i = iMaxRow; i < maxRow; i++){
        for(int j = 0; j < iMaxCol; j++){
            newMap[i][j] = map[i - iMaxRow][j];

            if(newMap[i][j] == TILE_PLAYER){
                newMap[i][j] = TILE_OPEN;
            }
        }
    }

    for(int i =iMaxRow; i < maxRow; i++){
        for(int j = iMaxCol; j < maxCol; j++){
            newMap[i][j] = map[i - iMaxRow][j - iMaxCol];

            if(newMap[i][j] == TILE_PLAYER){
                newMap[i][j] = TILE_OPEN;
            }
        }
    }

    deleteMap(map,iMaxRow);
    return newMap;
}


int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {

    int status = STATUS_STAY;

    if(nextRow == player.row && nextCol == player.col) {

        status = STATUS_STAY;
    }

    //out of bounds, next is monster, next is pillar
    if(nextRow>=maxRow || nextCol>=maxCol || nextRow<0 || nextCol<0 || (map[nextRow][nextCol] == TILE_MONSTER) || (map[nextRow][nextCol]==TILE_PILLAR) ) {
        nextRow=player.row;
        nextCol=player.col;
        status = STATUS_STAY;
    }

    if (map[nextRow][nextCol] == TILE_OPEN)
    {
        map[player.row][player.col] = TILE_OPEN ;                     
        player.row = nextRow;
        player.col = nextCol;
        map[player.row][player.col] = TILE_PLAYER; 
        status = STATUS_MOVE;
    }

    if (map[nextRow][nextCol] == TILE_TREASURE) {
        player.treasure+=1;

        map[player.row][player.col] = TILE_OPEN ; 
        player.row = nextRow;
        player.col = nextCol;
        map[player.row][player.col] = TILE_PLAYER;

        status = STATUS_TREASURE;
        
    }

    else if (map[nextRow][nextCol] == TILE_AMULET) {
        map[player.row][player.col] = TILE_OPEN ; 
        player.row = nextRow;
        player.col = nextCol;
        map[player.row][player.col] = TILE_PLAYER;

        status = STATUS_AMULET;
    }

    else if (map[nextRow][nextCol] == TILE_DOOR) {
        map[player.row][player.col] = TILE_OPEN ; 
        player.row = nextRow;
        player.col = nextCol;
        map[player.row][player.col] = TILE_PLAYER;

        status = STATUS_LEAVE;
    }

    else if (map[nextRow][nextCol] == TILE_EXIT) {
        if (player.treasure>0) {

            map[player.row][player.col] = TILE_OPEN ; 
            player.row = nextRow;
            player.col = nextCol;
            map[player.row][player.col] = TILE_PLAYER;

            status = STATUS_ESCAPE;
        }
        else {
            nextRow=player.row;
            nextCol=player.col;

            status=STATUS_STAY;
        }
    } 
    return status;

}


bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {

//check up
    for (int i = player.row-1; i>=0; i--) {
        if (map[i][player.col] == TILE_PILLAR)
            break; 
        if (map[i][player.col] == TILE_MONSTER) {
            map[i][player.col] =TILE_OPEN;
            map[i+1][player.col] = TILE_MONSTER;
        }
    }


//check down
    for (int i = player.row+1; i<maxRow; i++) {
        if (map[i][player.col] == TILE_PILLAR)
            break; 
        if (map[i][player.col] == TILE_MONSTER) {
            map[i][player.col] = TILE_OPEN;
            map[i-1][player.col] = TILE_MONSTER;
        }
    }


// check right
    for (int i = player.col+1; i<maxCol; i++) {
        if (map[player.row][i] == TILE_PILLAR)
            break; 
        if (map[player.row][i] == TILE_MONSTER) {
            map[player.row][i] = TILE_OPEN;
            map[player.row][i-1] = TILE_MONSTER;
        }
    }


//check left
    for (int i = player.col-1; i>=0; i--) {
        if (map[player.row][i] == TILE_PILLAR)
            break; 
        if (map[player.row][i] == TILE_MONSTER) {
            map[player.row][i] = TILE_OPEN;
            map[player.row][i+1] = TILE_MONSTER;
        }
    }


    if (map[player.row][player.col] == TILE_MONSTER) {
        return true;
    }

    return false;
}
