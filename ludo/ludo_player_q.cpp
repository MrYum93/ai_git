#include "ludo_player_q.h"
#include <random>

ludo_player_q::ludo_player_q():
    pos_start_of_turn(16),
    pos_end_of_turn(16),
    dice_roll(0)
{
    srand (24590836);
}

int ludo_player_q::make_decision()
{
    return calcTokenToMove();
}

int ludo_player_q::estimateNextState(int token)
{
    int state = 0;
    int pos = pos_start_of_turn[token] + dice_roll;
    //In Home
    if(pos_start_of_turn[token] == -1)
    {
        if(dice_roll == 6)
            return 0;
        else
            return 1;
    }
    //On a Globe Pos: 0 8 13 21 26 34 39 47
    if(pos == 0 || pos == 8 || pos == 13 || pos == 21 || pos == 26 || pos == 34 || pos == 39 || pos == 47)
        state = 1;

    //On a Star Pos: 5 11 18 24 31 37 44 50
    if(pos == 5 || pos == 11 || pos == 18 || pos == 24 || pos == 31 || pos == 37 || pos == 44 || pos == 50)
        if(pos_start_of_turn[token] != -1)
            state = 2;

    //On winner road
    if(pos >= 51)
        state = 4;

    //On free space
    else
        state = 6;

    if(pos != -1 && token == 0)// && pos < 51)
    {
        if(pos == pos_start_of_turn[1] || pos == pos_start_of_turn[2] || pos == pos_start_of_turn[3])
            state = 5;
    }

    if(pos != -1 && token == 1)// && pos < 51)
    {
        if(pos == pos_start_of_turn[0] || pos == pos_start_of_turn[2] || pos == pos_start_of_turn[3])
            state = 5;
    }

    if(pos != -1 && token == 2)// && pos < 51)
    {
        if(pos == pos_start_of_turn[0] || pos == pos_start_of_turn[1] || pos == pos_start_of_turn[3])
            state = 5;
    }

    if(pos != -1 && token == 3)// && pos < 51)
    {
        if(pos == pos_start_of_turn[0] || pos == pos_start_of_turn[1] || pos == pos_start_of_turn[2])
            state = 5;
    }


    if(pos == 99 || pos == 56) //Måske er pos 56 ikke nødvendig
        state = 3;

    return state;
}

int ludo_player_q::maxQ(int state)
{
    int max = -9;
    for(int i = 0; i < 11; i++)
        if(r[state][i] >= -9 && q[state][i] > max)
            max = q[state][i];
    return max;
}

int ludo_player_q::calcTokenToMove()
{
    /*std::cout << std::endl;
    std::cout << std::endl;
    std::cout << "Beginning of my turn" << std::endl;*/
    clearStateArray();
    clearActionArray();

    updateStateArray();
    updateActionArray();


    bool noMoves = 0;
    if(counter < -320000) //80000
    {
        if(counter%10000 == 0)
            std::cout << counter << std::endl;
        counter++;
        int moves[40][3];
        for(int x = 0; x < 40; x++)
        {
            moves[x][0] = -1;
            moves[x][1] = -1;
            moves[x][2] = -1;
        }
       int index = 0;
       for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 11; j++)
                if(actionArray[i][j] == 1)
                {
                    moves[index][0] = i;
                    moves[index][1] = stateArray[i];
                    moves[index][2] = j;
                    index++;
                }
        }
        if(index == 0)
        {
            //std::cout << "No moves" << std::endl;
            //noMoves = 1;
            return -1;
        }
        int random = rand() % index;
        int move[3];
        move[0] = moves[random][0]; //Token
        move[1] = moves[random][1]; //State
        move[2] = moves[random][2]; //Action
        int token = move[0];
        int state = move[1];
        int action = move[2];

        //q[state][action] = r[state][action] + rate*maxQ(estimateNextState(token));
        q[state][action] = a*(r[state][action] + rate*maxQ(estimateNextState(token))-q[state][action]);

/*
        std::cout << "---------- Action Array Matrix ----- Token " << move[0] << " ----------" << std::endl;
        for(int row = 0; row < 4; row++)
        {
            for(int col = 0; col < 11; col++)
            {
                std::cout << actionArray[row][col] << "\t";
            }
            std::cout << stateArray[row] << "\t" << pos_start_of_turn[row] << std::endl;
        }
*/
        int sum = 0;
        for(int x = 0; x < 10; x++)
            sum += actionArray[token][x];
        if(sum == 0 || noMoves == 1)
            return -1;
        return token;
    }


    int token = -1;
    float qValue = -100;
/*
    std::cout << "---------- Action Array Matrix ----------" << std::endl;
    for(int row = 0; row < 4; row++)
    {
        for(int col = 0; col < 11; col++)
        {
            std::cout << actionArray[row][col] << "\t";
        }
        std::cout << stateArray[row] << "\t" << pos_start_of_turn[row] << std::endl;
    }
*/
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 11; j++)
            if(actionArray[i][j] == 1 && q[stateArray[i]][j] > qValue)
            {
                qValue = q[stateArray[i]][j];
                token = i;
            }
    }
    //std::cout << "This is token: " << token << std::endl;
    int sum = 0;
    for(int x = 0; x < 10; x++)
        sum += actionArray[token][x];
    if(sum == 0)
        return -1;
    return token;
}

void ludo_player_q::updateStateArray()
{
    clearStateArray();
    for (int i = 0; i < 4; i++)
    {
        int pos = pos_start_of_turn[i];
        //In Home
        if(pos == -1)
            stateArray[i] = 0;

        //On a Globe Pos: 0 8 13 21 26 34 39 47
        else if(pos == 0 || pos == 8 || pos == 13 || pos == 21 || pos == 26 || pos == 34 || pos == 39 || pos == 47)
            stateArray[i] = 1;

        //On a Star Pos: 5 11 18 24 31 37 44 50
        else if(pos == 5 || pos == 11 || pos == 18 || pos == 24 || pos == 31 || pos == 37 || pos == 44 || pos == 50)
            stateArray[i] = 2;

        //On winner road
        else if(pos >= 51 && pos <= 55)
            stateArray[i] = 4;

        //On free space
        else
            stateArray[i] = 6;

    }
    if(pos_start_of_turn[0] != -1 && pos_start_of_turn[0] < 51)
    {
        int pos = pos_start_of_turn[0];
        if(pos == pos_start_of_turn[1] || pos == pos_start_of_turn[2] || pos == pos_start_of_turn[3])
            stateArray[0] = 5;
    }

    if(pos_start_of_turn[1] != -1 && pos_start_of_turn[1] < 51)
    {
        int pos = pos_start_of_turn[1];
        if(pos == pos_start_of_turn[0] || pos == pos_start_of_turn[2] || pos == pos_start_of_turn[3])
            stateArray[1] = 5;
    }

    if(pos_start_of_turn[2] != -1 && pos_start_of_turn[2] < 51)
    {
        int pos = pos_start_of_turn[2];
        if(pos == pos_start_of_turn[0] || pos == pos_start_of_turn[1] || pos == pos_start_of_turn[3])
            stateArray[2] = 5;
    }

    if(pos_start_of_turn[3] != -1 && pos_start_of_turn[3] < 51)
    {
        int pos = pos_start_of_turn[3];
        if(pos == pos_start_of_turn[0] || pos == pos_start_of_turn[1] || pos == pos_start_of_turn[2])
            stateArray[3] = 5;
    }

    for (int i = 0; i < 4; i++)
    {
        int pos = pos_start_of_turn[i];
        //In goal
        if(pos == 99 || pos == 56) //Måske er pos 56 ikke nødvendig
            stateArray[i] = 3;
    }

}

void ludo_player_q::clearStateArray()
{
    stateArray[0] = 0;
    stateArray[1] = 0;
    stateArray[2] = 0;
    stateArray[3] = 0;
}

void ludo_player_q::updateActionArray()
{
    clearActionArray();
    for(int i = 0; i < 4; i++)
    {
        int pos = pos_start_of_turn[i];
        int newPos = pos + dice_roll;

        // Out of Home
        if(pos == -1 && dice_roll == 6)
            actionArray[i][0] = 1;

        // Get into Goal
        if(newPos == 56)
            actionArray[i][1] = 1;

        // Move to Globe Pos: 0 8 13 21 26 34 39 47
        if(newPos == 8 || newPos == 13 || newPos == 21 || newPos == 26 || newPos == 34 || newPos == 39 || newPos == 47)
            actionArray[i][2] = 1;

        // Move to Star Pos: 5 11 18 24 31 37 44 50
        if(newPos == 5 || newPos == 11 || newPos == 18 || newPos == 24 || newPos == 31 || newPos == 37 || newPos == 44)
        {
            //actionArray[i][3] = 1;
            if(pos != -1)
                actionArray[i][3] = 1;//Skal fikses
        }

        // Move to goal via star Pos: 50
        if(newPos == 50)
             actionArray[i][4] = 1;

        // Get to safety(double token)
        if(i == 0 && pos != -1)
        {
            if(newPos == pos_start_of_turn[1] || newPos == pos_start_of_turn[2] || newPos == pos_start_of_turn[3])
                actionArray[i][5] = 1;
        }
        else if(i == 1 && pos != -1)
        {
            if(newPos == pos_start_of_turn[0] || newPos == pos_start_of_turn[2] || newPos == pos_start_of_turn[3])
                actionArray[i][5] = 1;
        }
        else if(i == 2 && pos != -1)
        {
            if(newPos == pos_start_of_turn[0] || newPos == pos_start_of_turn[1] || newPos == pos_start_of_turn[3])
                actionArray[i][5] = 1;
        }
        else if(i == 3 && pos != -1)
        {
            if(newPos == pos_start_of_turn[0] || newPos == pos_start_of_turn[1] || newPos == pos_start_of_turn[2])
                actionArray[i][5] = 1;
        }

        // Get to winner road
        if(pos < 51 && newPos >= 51 && newPos <= 55)
        {
            actionArray[i][6] = 1;
        }

        // Kill opponent or Commit suicide.
        int safeOpp[12] = {-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2};
        for(int x = 4; x <= 14; x++)
        {
            for(int y = x+1; y <= 15; y++)
            {
                if(pos_start_of_turn[x] >= 0 && pos_start_of_turn[x] < 51 && pos_start_of_turn[x] == pos_start_of_turn[y])
                    safeOpp[x-4] = (pos_start_of_turn[x]);
            }

        }
        bool landsOnSafeSpot = 1;
        for(int x = 0; x <= 9; x++)
            if(newPos == safeOpp[x] && pos != -1)
                landsOnSafeSpot = 0;

        for(int x = 4; x <= 15; x++)
        {
            if(newPos == pos_start_of_turn[x])
            {
                if(actionArray[i][2] == 1 || actionArray[i][3] == 1 || landsOnSafeSpot == 0)
                {
                    actionArray[i][2] = 0;
                    actionArray[i][3] = 0;
                    actionArray[i][7] = 1;
                }
                else if(pos != -1)
                    actionArray[i][8] = 1;
            }
        }

        // No special move
        if(pos != -1 && pos != 99)
            actionArray[i][9] = 1;

        // No moves
        int sum = 0;
        for(int x = 0; x < 10; x++)
            sum += actionArray[i][x];
        if(sum == 0)
            actionArray[i][10] = 0;
    }

}

void ludo_player_q::clearActionArray()
{
    for(int i = 0; i < 4; i++)
        for(int j = 0; j < 11; j++)
            actionArray[i][j] = 0;
}

void ludo_player_q::printQReadable()
{
    std::cout << "---------- Q Matrix ----------" << std::endl;
    for(int row = 0; row < 7; row++)
    {
        for(int col = 0; col < 11; col++)
        {
            std::cout << q[row][col] << "\t";
        }
        std::cout << std::endl;
    }
}

void ludo_player_q::printQ()
{
    std::cout << "---------- Q Matrix ----------" << std::endl;
    std::cout << "{";
    for(int row = 0; row < 7; row++)
    {
        std::cout << "{";
        for(int col = 0; col < 11; col++)
        {
            std::cout << q[row][col];
            if(col < 10)
                 std::cout << ",";
        }
        if(row < 6)
            std::cout << "}," << std::endl;
        else
            std::cout << "}};" << std::endl;
    }
}

void ludo_player_q::start_turn(positions_and_dice relative){
    pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;
    int decision = make_decision();
    emit select_piece(decision);
}

void ludo_player_q::post_game_analysis(std::vector<int> relative_pos){
    pos_end_of_turn = relative_pos;
    bool game_complete = true;
    for(int i = 0; i < 4; ++i){
        if(pos_end_of_turn[i] < 99){
            game_complete = false;
        }
    }
    emit turn_complete(game_complete);
}

