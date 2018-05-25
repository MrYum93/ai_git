#include "ludo_player_qlearning.h"
#include <random>

ludo_player_qlearning::ludo_player_qlearning():
    player_pos_start_of_turn(16),
    player_pos_end_of_turn(16),
    dice_roll(0),
    star_pos(7),
    globe_pos(7)
{
    star_pos[0] = 5;
    star_pos[1] = 11;
    star_pos[2] = 18;
    star_pos[3] = 24;
    star_pos[4] = 31;
    star_pos[5] = 37;
    star_pos[6] = 44;
    globe_pos[0] = 8;
    globe_pos[1] = 13;
    globe_pos[2] = 21;
    globe_pos[3] = 26;
    globe_pos[4] = 34;
    globe_pos[5] = 39;
    globe_pos[6] = 47;
    if(debug) cout << "Q_table before init: " << Q_table << endl;
    if(debug) cout << "Q_learning construction" << endl;
    cum_rew_p0 = 0; //reset all the cum rewards
    cum_rew_p1 = 0;
    cum_rew_p2 = 0;
    cum_rew_p3 = 0;
    int nrows = 6; //all possible states
    int ncols = 14;//all possible actions

    if(file_exists("trainDataQ.txt")) // Continue from prev training set
    {
        if(debug) cout << "tranDataQ.txt reset\n";
        Q_table = Eigen::MatrixXd(nrows, ncols);
        ifstream f_in ("trainDataQ.txt");
        if(f_in.is_open())
        {
            for(int row = 0; row < nrows; row++)
                for(int col = 0; col < ncols; col++)
                {
                    float item = 0.0;
                    f_in >> item;
                    Q_table(row,col) = item;
                }
            f_in.close();
        }
    }
    else
    {
        Q_table = Eigen::MatrixXd::Zero(nrows, ncols);
    }
    if(file_exists("ludoLog.txt"))
        remove("ludoLog.txt");
    update_flag = 0;
    bool train  = 0;
    if(train)
    {
        alpha = 0.7; // learning rate
        gamma = 0.01; // discount factor
        epsilon = 0.9; // 1 = random, 0 = greedy
    }
    else
    {
        alpha = 0.3; // learning rate
        gamma = 0.1; // discount factor
        epsilon = 0.01; // 1 = random, 0 = greedy
    }
    //good results: first train with a: 0.7, g: 0.01, e: 1
    //Then train run greedy with a: 0.3, g: 0.1, e: 0.01
}

bool ludo_player_qlearning::file_exists(const string &filename)
{
    return access(filename.c_str(), 0) == 0;
}

bool ludo_player_qlearning::is_star(int pos)
{
    for(int i = 0; i < 7; i++)
        if(pos == star_pos[i])
            return true;
        else
            return false;
}

bool ludo_player_qlearning::is_globe(int pos)
{
    for(int i = 0; i < 7; i++)
        if(pos == globe_pos[i])
            return true;
        else
            return false;
}

int ludo_player_qlearning::nxt_star_pos(int pos)
{
    for(int i = 0; i < 8; i++) //one extra i to handle overflow
        if(i < 7)
        {
            if(pos == star_pos[i])
                return star_pos[i+1];
        }
        else
            return pos;

}

int ludo_player_qlearning::curr_state_intepreter(float state_input[6])
{
    for(int i = 0; i < sizeof(state_input); i++)
    {
        if(state_input[i] == 1)
            return i;
    }
}

void ludo_player_qlearning::updateQtable(tuple<int, int, int, int> player_state_action_pos)
{
    int player_played_i = get<0>(player_state_action_pos);
    int prev_state = get<1>(player_state_action_pos);
    int performed_action = get<2>(player_state_action_pos);
    int prev_pos = get<3>(player_state_action_pos);
    int curr_pos = player_pos_start_of_turn[player_played_i];
    if(debug) cout << "Prev pos: " << prev_pos << endl;
    if(debug) cout << "Dice_roll: " << dice_roll << endl;
    if(debug) cout << "Curr pos: " << curr_pos << endl;

    // define the states used for other func
    float curr_state[7];
    calc_curr_state(curr_state, curr_pos, player_played_i); //calc all the possible states
    int curr = curr_state_intepreter(curr_state); //store the current state

    double reward_gain = 1;//curr_pos*0.1; //init reward based on curr player pos
    double reward = 0;
    /*//
    if(prev_pos == curr_pos && prev_state == curr)
    {
        if(debug) cout << "Piece has not moved" << endl;
        reward -= -0.3;
    }
    if(curr_pos == 99 && prev_pos != curr_pos)
    {
        if(debug) cout << "Goal reached\n";
        reward += 10; //99
    }
    if(curr_pos == 0 && prev_pos == -1 && performed_action == 2)
    {
        if(debug) cout << "You moved out from home... Yay!\n";
        reward += 0.3;
    }
    /*/ //
    if(performed_action == 0)
    {
        if(debug) cout << "Moves in safety\n";
        reward += 5*reward_gain;
    }
    if(performed_action == 1)
    {
        if(debug) cout << "Move with enemy behind nxt pos\n";
        reward += 1*reward_gain;
    }
    if(performed_action == 2)
    {
        if(debug) cout << "get out of home\n";
        reward += 6*reward_gain;
    }
    if(performed_action == 3)
    {
        if(debug) cout << "hit enemy\n";
        reward += 15*reward_gain;
    }
    if(performed_action == 4)
    {
        if(debug) cout << "hit enemy but with danger\n";
        reward += 10*reward_gain;
    }
    if(performed_action == 5)
    {
        if(debug) cout << "hit star\n";
        reward += 10*reward_gain;
    }
    if(performed_action == 6)
    {
        if(debug) cout << "hit star with enemy behind second star\n";
        reward += 7*reward_gain;
    }
    if(performed_action == 7)
    {
        if(debug) cout << "hit star with enemy behind both star\n";
        reward += 5*reward_gain;
    }
    if(performed_action == 8)
    {
        if(debug) cout << "hit globe or friend\n";
        reward += 5*reward_gain;
    }
    if(performed_action == 9)
    {
        if(debug) cout << "hit globe with enemy behind\n";
        reward += 10*reward_gain;
    }
    if(performed_action == 10)
    {
        if(debug) cout << "kill yoself\n";
        reward += -40*reward_gain;
    }
    if(performed_action == 11)
    {
        if(debug) cout << "hit goal\n";
        reward += 20*reward_gain;
    }
    if(performed_action == 12)
    {
        if(debug) cout << "hit goal stretch\n";
        reward += 20*reward_gain;
    }
    if(performed_action == 13)
    {
        if(debug) cout << "do nothing\n";
        reward += -5*reward_gain;
    }
    //*/

    acc += reward; //accululative reward
    if(debug) cout << "Accum: " << acc << endl;

    for(int i = 0; i < 4; i++)
        if(reward_debug) cout << "reward for #" << i << " " << player_pos_start_of_turn[i]*10 << endl;

    if(debug) cout << "Player: " << player_played << " at: " << curr_pos << " with possible reward: " << reward << endl;

    double max = -10000000000000;
    for(int i = 0; i < 13; i++)
    {
        double test = Q_table(curr, i);
        if(debug) cout << "test: \n" << test << endl;
        if(test > max)
        {
            max = i;
        }
    }
    if(debug) cout << "Prev state: " << prev_state << endl;
    if(debug) cout << "performed action: " << performed_action << endl;
    if(debug) cout << "prev value\n" << Q_table << endl;
    Q_table(prev_state, performed_action) += alpha * (reward + gamma * max - Q_table(prev_state, performed_action));
    if(debug) cout << "update value\n" << Q_table << endl;

}

void ludo_player_qlearning::calc_possible_actions(float action_input[9], int curr_pos, int dice, int self)
{
    if(a_debug) cout << endl << "New check for " << self << endl;
    int nxt_pos = curr_pos;
    if(curr_pos != -1 || curr_pos >= 99) //calc the next pos of piece
    {
        nxt_pos = curr_pos + dice;
        if(nxt_pos == is_star(nxt_pos))
        {
            nxt_pos = nxt_star_pos(nxt_pos);
        }
    }

    //Move out from home                                                - 0 0 1 0 0 0 0 0 0
    if(curr_pos == -1 && dice == 6)
    {
        action_input[2] = 1;
        if(a_debug) cout << "Move out from start\n";
    }

    if(curr_pos != -1) //all the actions availble when not in home
    {
        //Hit star                                                      - 0 0 0 0 1 0 0 0 0
        if(is_star(nxt_pos))
        {
            action_input[4] = 1;
            if(a_debug) cout << "Hits star\n";
        }
        //Hit globe                                                     - 0 0 0 0 0 0 1 0 0
        if(is_globe(nxt_pos))
        {
            action_input[6] = 1;
            if(a_debug) cout << "Hit globe\n";
        }
        //or friend                                                     - 0 0 0 0 0 0 1 0 0
        for(int i = 0; i < 4; i++)
            if(nxt_pos == player_pos_start_of_turn[i] && self != i)
            {
                action_input[6] = 1;
                if(a_debug) cout << "hit friend\n";
            }
                //Hit globe with enemy behind (great state)             - 0 1 0 0 0 0 1 0 0
        //Hit globe with enemy on it or hit stack of enemies            - 0 0 0 1 0 0 1 0 0
        for(int i = 1; i < 4; i++)
            for(int j = 0; j < 3; j++)
                for(int k = j + 1; k < 4; k++)
                    if(player_pos_start_of_turn[i*4+j] == player_pos_start_of_turn[i*4+k])
                        if(nxt_pos == player_pos_start_of_turn[i*4+j])
                        {
                            action_input[3] = 1;
                            action_input[6] = 1;
                            if(a_debug) cout << "hit globe with enemy\n";
                        }
        //Hit goal                                                      - 0 0 0 0 0 0 0 1 0
        if(nxt_pos == 56)
        {
            action_input[7] = 1;
            if(a_debug) cout << "hit goal\n";
        }
        //Hit goal stretch                                              - 0 0 0 0 0 0 0 0 1
        if(nxt_pos > 50 && nxt_pos < 56)
        {
            action_input[8] = 1;
            if(a_debug) cout << "Hit goal stretch\n";
        }
        //Go through all enemy
        for(int i = 4; i < 16; i++)
        {
            //Check the 6 spaces behind curr piece
            for(int j = 1; j < 6; j++)
            {
                //Check if any enemy is 6 or less behind                - 0 1 0 0 0 0 0 0 0
                if(player_pos_start_of_turn[i] == nxt_pos - j)
                {
                    action_input[1] = 1;
                    if(a_debug) cout << "enemy is behind\n";
                }
                //Check if enemy is behind second star                  - 0 1 0 0 1 0 0 0 0
                if(action_input[4] == 1)
                    if(player_pos_start_of_turn[i] == nxt_star_pos(nxt_pos) - j)
                    {
                        action_input[1] = 1;
                        if(a_debug) cout << "enemy behind second star\n";
                        //Check if enemy is behind second or first star - 0 1 0 0 1 1 0 0 0
                        if(player_pos_start_of_turn[i] == nxt_pos - j)
                        {
                            action_input[5] = 1;
                            if(a_debug) cout << "enemy is behind first star\n";
                        }
                    }
            }
            //Hit enemy                                                 - 0 0 0 1 0 0 0 0 0
            if(nxt_pos == player_pos_start_of_turn[i])
            {
                action_input[3] = 1;
                if(a_debug) cout << "Hit enemy home\n";
                //Hit enemy with enemy behind                           - 0 1 0 1 0 0 0 0 0
                //Is taken care of above
            }

        }
        if(     action_input[0] == 0 && action_input[2] == 0 && action_input[3] == 0 &&
                action_input[4] == 0 && action_input[5] == 0 && action_input[6] == 0 &&
                action_input[7] == 0 && action_input[8] == 0)
        {
            if(a_debug) cout << "No special moves";
            if(curr_pos != 99)
            {
                //Move in danger                                        - 1 1 0 0 0 0 0 0 0
                if(action_input[1] == 1)
                {
                    if(a_debug) cout << " and not in goal, but move with danger" << endl;
                    action_input[0] = 1;
                }
                //Move                                                  - 1 0 0 0 0 0 0 0 0
                else
                {
                    if(a_debug) cout << " and not in goal, so just move in safety" << endl;
                    action_input[0] = 1;
                    action_input[1] = 0;
                 }
            }
            else
                if(a_debug) cout << " but is already in goal!" << endl;
        }
        //No move possible                                              - 0 0 0 0 0 0 0 0 0
    }
    else
        if(a_debug) cout << "Is in home\n";
}

//For all moves have a factor for how long you are in the game !!NEEDS TO BE SOMEWHERE ELSE!!

void ludo_player_qlearning::calc_curr_state(float state_input[6], int curr_pos, int self)
{
    // Home                   - 1 0 0 0 0 0
    if(curr_pos == -1)
    {
        state_input[0] = 1;
        return;
    }
    // Goal                   - 0 1 0 0 0 0
    if(curr_pos == 99 || curr_pos == 56)
    {
        state_input[1] = 1;
        return;
    }
    // Goal stretch           - 0 0 1 0 0 0
    if(curr_pos > 50 && curr_pos < 56)
    {
        state_input[2] = 1;
        return;
    }
    // Globe                  - 0 0 0 1 0 0
    if(is_globe(curr_pos))
    {
        state_input[3] = 1;
        return;
    }
    //or friend               - 0 0 0 1 0 0
    for(int i = 0; i < 4; i++)
        if(curr_pos == player_pos_start_of_turn[i] && self != i)
        {
            state_input[3] = 1;
            return;
        }
    // Free space in danger   - 0 0 0 0 1 0
    if(     state_input[0] != 1 && state_input[1] != 1 &&
            state_input[2] != 1 && state_input[3] != 1   )
    {
        //Go through all enemy
        for(int i = 4; i < 16; i++)
        {
            //Check the 6 spaces behind curr piece
            for(int j = 1; j < 6; j++)
            {
                if(player_pos_start_of_turn[i] == curr_pos - j)
                {
                    state_input[4] = 1;
                    return;
                }
            }
        }
    }
    // Free space             - 0 0 0 0 0 1
    if(state_input[0] != 1 && state_input[1] != 1 && state_input[2] != 1 &&
            state_input[3] != 1 && state_input[4] != 1)
    {
        state_input[5] = 1;
        return;
    }
}

vector<tuple<int, int, int> > ludo_player_qlearning::player_state_actionInterpreter(float state_input[6], float action_input[9], int self)
{
    vector<tuple<int, int, int>> output; //What is returned in the end, all the ppossible state/action pairs
    for(int i = 0; i < 6; i++)
    {
        if(state_input[i] == 1) //Check state we are in
        {
            vector<int> actions; //Create vector where all the possible actions to take
            /* push the viable action back for other func to deal with */
            if(action_input[0] == 1 && action_input[1] == 0 )
                actions.push_back(0);
            else if(action_input[0] == 1 && action_input[1] == 1)
                actions.push_back(1);
            else if(action_input[2] == 1)
                actions.push_back(2);
            else if(action_input[3] == 1 && action_input[4] == 0 && action_input[6] == 0)
                actions.push_back(3);
            else if(action_input[1] == 1 && action_input[2] == 0 && action_input[3] == 1 && action_input[4] == 0 && action_input[5] == 0 && action_input[6] == 0)
                actions.push_back(4);
            else if(action_input[1] == 0 && action_input[4] == 1 && action_input[5] == 0)
                actions.push_back(5);
            else if(action_input[1] == 1 && action_input[4] == 1 && action_input[5] == 0)
                actions.push_back(6);
            else if(action_input[1] == 1 && action_input[4] == 1 && action_input[5] == 1)
                actions.push_back(7);
            else if(action_input[1] == 0 && action_input[3] == 0 && action_input[6] == 1)
                actions.push_back(8);
            else if(action_input[1] == 1 && action_input[3] == 0 && action_input[6] == 1)
                actions.push_back(9);
            else if(action_input[1] == 0 && action_input[3] == 1 && action_input[6] == 1)
                actions.push_back(10);
            else if(action_input[7] == 1)
                actions.push_back(11);
            else if(action_input[8] == 1)
                actions.push_back(12);
            else
                actions.push_back(13);

            // done with all the state_actions
            for(int j = 0; j < actions.size(); j++)
            {
                int action = actions[j];
                output.push_back(make_tuple(self, i, action));
            }
        }
    }
    return output;
}

tuple<int, int, int, int> ludo_player_qlearning::e_greedy(double eps)
{
    double limit = eps*100;
    random_device rd; //used as a seed for mt19937
    mt19937 mt(rd()); //much like the standard rand() func but better as it has a longer sequence
    uniform_int_distribution<int> dist(1, 100);

    if(debug) cout << "Dist mt: " << dist(mt) << " vs " << limit << endl;

    if(dist(mt) < limit)
    {
        int first_p = get<0>(player_state_action[0]);
        int last_p = get<0>(player_state_action[player_state_action.size()-1]);

        //pick a random player in the following code
        random_device player_rand;
        mt19937 player_mt(player_rand());
        uniform_int_distribution<int> dist_p(first_p, last_p); //play with this

        int player = dist_p(player_mt); //choose a random player here!
        player_played = player;
        int prev_pos = player_pos_start_of_turn[player_played];
    //figure out what this does!!
        auto it = find_if(player_state_action.begin(), player_state_action.end(), [](const tuple<int, int, int>& e) {return get<0>(e)  == 0;});
        int pos = distance(player_state_action.begin(), it); //find distance between first element and itterator
        if(debug) cout << "Position in random e_greedy: " << pos << endl;
        int state = get<1>(player_state_action[pos]);
        int action = get<2>(player_state_action[pos]);

        for(unsigned int i = 0; i < player_state_action.size(); i++)
        {
            int player = get<0>(player_state_action[i]);
            int state = get<1>(player_state_action[i]);
            int action = get<2>(player_state_action[i]);

            double test = Q_table(state, action);
            if(debug) cout << test << " ";
        }
        if(debug) cout << endl;
        if(debug) cout << "Random player: " << player << " state: " << state << " action: " << action << endl;

        return make_tuple(player, state, action, prev_pos);
    }
    else
    {
        double max = -1000000000000000000000;
        int pos;
        float cout_gain = 0;
        for(int i = 0; i < player_state_action.size(); i++)
        {
//            if(player_pos_start_of_turn[i] >= 99)
//                continue;
            float curr_pos_gain = (player_pos_start_of_turn[i] + 2);
            curr_pos_gain = (curr_pos_gain / 58)+1; //creates a gain between 1 and 2 so it prioritizes further ahead pieces

            int player = get<0>(player_state_action[i]);
            int state = get<1>(player_state_action[i]);
            int action = get<2>(player_state_action[i]);

            if(debug) cout << "Player: " << player << " in state " << state << " can do " << action << " with diceroll: " << dice_roll << endl; //The correct is chosen
            double test = Q_table(state, action);
            if(debug) cout << "testing for qtable value: " << test << endl;
//            cout << "Gain: " << curr_pos_gain << endl;
//            cout << "Player: " << player << " before gain: " << test << endl;
            test = test * curr_pos_gain;
//            cout << "Player: " << player << " after gain : " << test << endl << endl;
            if(test > (max * curr_pos_gain))
            {
//                cout << "test: " << test << " max: " << (max ) << endl;
                max = test;
                pos = i;
                player_played = player;
                cout_gain = curr_pos_gain;
            }
        }
//        cout << "And player " << player_played << " is choosen with " << cout_gain << " gain \n\n";//The correct is chosen
        int prev_pos = player_pos_start_of_turn[player_played];
        int state = get<1>(player_state_action[pos]);
        int action = get<2>(player_state_action[pos]);

        return make_tuple(player_played, state, action, prev_pos);
    }
}

int ludo_player_qlearning::make_q_decision()
{

    if(debug) cout << "***** New decision *****" << endl << endl;

    if(update_flag != 1)
    {
        if(debug) cout << "No update...\n";
    }
    else
    {
        if(debug) cout << "Update!!!" << endl;
//        if(debug) cout << "Previous player played: " << get<0>(player_state_action_prevPos) << endl;
//        if(debug) cout << "It was in state: " << get<1>(player_state_action_prevPos) << " and did: " << get<2>(player_state_action_prevPos) << endl;
        updateQtable(player_state_action_prevPos);
        player_state_action.clear();
        ofstream log("ludoLog.txt", ios_base::app);
        ofstream current_table("trainDataQ.txt");
        current_table << Q_table << endl;
        log << Q_table << endl << endl;
//        if(debug) cout << "The table is saved" << endl;
        update_flag = 0;
    }
    player_state_action.clear();
    for(int i = 0; i < 4; i++)
    {
        if(player_pos_start_of_turn[i] >= -1 && player_pos_start_of_turn[i] < 99)
        {
            if(debug) cout << "Player: " << i << " at pos: " << player_pos_start_of_turn[i] << " is about to move " << dice_roll << endl;

            float action_input[9] = {0,0,0,0,0,0,0,0,0};
            float state_input[6] = {0,0,0,0,0,0};

            calc_possible_actions(action_input, player_pos_start_of_turn[i], dice_roll, i);
            calc_curr_state(state_input, player_pos_start_of_turn[i], i);

            if(make_q_decision_debug) cout << "Possible actions: ";
            for(int i = 0; i < (sizeof(action_input)/sizeof(float)); i++)
                if(make_q_decision_debug) cout << action_input[i] << " ";
            if(make_q_decision_debug) cout << endl;
            if(make_q_decision_debug) cout << "Curr state: ";
            for(int i = 0; i < (sizeof(state_input)/sizeof(float)); i++)
                if(make_q_decision_debug) cout << state_input[i] << " ";
            if(make_q_decision_debug) cout << endl;

            vector<tuple<int, int, int>> player_i = player_state_actionInterpreter(state_input, action_input, i);
            player_state_action.insert(player_state_action.end(), player_i.begin(), player_i.end());
        }
        else
            if(debug) cout << "Player: " << i << " is already in goal \n";
    }
//    if(game_obj.get_update_q_table())
//        update_flag = 1;
    if(player_state_action.size() > 4)
    {
        cout << "More actions than allowed, with: " << player_state_action.size() << "\n";
        exit(0);
    }

    player_state_action_prevPos = e_greedy(epsilon); //0 = greedy, 1 = random

    if(reward_debug) cout << "Player played: " << player_played << endl;
    if(debug) cout << "egreedy is done" << endl;
    return player_played; //gets from e_greedy algorithm
}

int ludo_player_qlearning::make_decision()
{
    if(dice_roll == 6){
        for(int i = 0; i < 4; ++i){
            if(player_pos_start_of_turn[i]<0){
                return i;
            }
        }
        for(int i = 0; i < 4; ++i){
            if(player_pos_start_of_turn[i]>=0 && player_pos_start_of_turn[i] != 99){
                return i;
            }
        }
    } else {
        for(int i = 0; i < 4; ++i){
            if(player_pos_start_of_turn[i]>=0 && player_pos_start_of_turn[i] != 99){
                return i;
            }
        }
        for(int i = 0; i < 4; ++i){ //maybe they are all locked in
            if(player_pos_start_of_turn[i]<0){
                return i;
            }
        }
    }
    return -1;
}

void ludo_player_qlearning::start_turn(positions_and_dice relative){
    player_pos_start_of_turn = relative.pos;
    dice_roll = relative.dice;
    int decision = make_q_decision();
    if(debug) cout << "Decisions made!" << endl;
    emit select_piece(decision);
}

void ludo_player_qlearning::post_game_analysis(std::vector<int> relative_pos){
    player_pos_end_of_turn = relative_pos;
    bool game_complete = true;
    for(int i = 0; i < 4; ++i){
        if(player_pos_end_of_turn[i] < 99){
            game_complete = false;
        }
    }
    updateCnt++;
    if(updateCnt >= 400)
    {
        updateCnt = 0;
        update_flag = 1;
    }
    emit turn_complete(game_complete);
}
