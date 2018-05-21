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

    if(file_exists("trainDataQ.txt")) // if already trained once reset
    {

        Q_table = Eigen::MatrixXd::Zero(nrows, ncols);
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
        }
    }
    else
    {
        Q_table = Eigen::MatrixXd(nrows, ncols);

        ofstream logFile;
        logFile.open("logLudo.txt");
        logFile << Q_table;
        logFile.close();
        if(debug) cout << "qtable created\n";

    }
    if(debug) cout << "Q_table init done" << endl;
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
    double alfa = 0.5; // learning rate
    double gamma = 0.1;
    int player_played = get<0>(player_state_action_pos);
    int prev_state = get<1>(player_state_action_pos);
    int performed_action = get<2>(player_state_action_pos);
    int prev_pos = get<3>(player_state_action_pos);
    int curr_pos = player_pos_start_of_turn[player_played];
    cout << "Prev pos: " << prev_pos << endl;
    cout << "Dice_roll: " << dice_roll << endl;
    cout << "Curr pos: " << curr_pos << endl;

    // define the states used for other func
    float curr_state[7];
    calc_curr_state(curr_state, curr_pos, player_played); //calc all the possible states
    int curr = curr_state_intepreter(curr_state); //store the current state

    double reward = curr_pos; //init reward based on curr player pos
    if(prev_pos == curr_pos && prev_state == curr)
    {
        cout << "Piece has not moved" << endl;
        reward -= -0.3;
    }

    if(curr_pos == 99 && prev_pos != curr_pos)
    {
        cout << "Goal reached\n";
        reward += 99;
    }

    if(curr_pos == 0 && prev_pos == -1 && performed_action == 2)
    {
        cout << "You moved out from home... Yay!\n";
        reward += 0.3;
    }

    acc += reward; //accululative reward

    for(int i = 0; i < 4; i++)
        cout << "reward for #" << i << " " << player_pos_start_of_turn[i]*10 << endl;

    cout << "Player: " << player_played << " at: " << curr_pos << " with possible reward: " << reward << endl;

    double max = -100000000;
    for(int i = 0; i < 13; i++)
    {
        double test = Q_table(curr, i);
        if(test > max)
            max = i;
    }

    cout << "Prev state: " << prev_state << endl;
    cout << "performed action: " << performed_action << endl;
//    cout << "prev value: " << Q_table(prev_state, performed_action) << endl;
    Q_table(prev_state, performed_action) += alfa * (reward + gamma * max - Q_table(prev_state, performed_action));
//    cout << "prev value: " << Q_table(prev_state, performed_action) << endl;

}

void ludo_player_qlearning::calc_possible_actions(float action_input[9], int curr_pos, int dice, int self)
{
//    cout << "now we calc possible actions!" << endl;
    int nxt_pos;
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
        action_input[2] = 1;

    if(curr_pos != -1) //all the actions availble when not in home
    {
        //Hit star                                                      - 0 0 0 0 1 0 0 0 0
        if(is_star(nxt_pos))
            action_input[4] = 1;
        //Hit globe                                                     - 0 0 0 0 0 0 1 0 0
        if(is_globe(nxt_pos))
            action_input[6] = 1;
        //or friend                                                     - 0 0 0 0 0 0 1 0 0
        for(int i = 0; i < 4; i++)
            if(nxt_pos == player_pos_start_of_turn[i] && self != i)
                action_input[6] = 1;
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
                        }
        //Hit goal                                                      - 0 0 0 0 0 0 0 1 0
        if(nxt_pos == 56)
            action_input[7] = 1;
        //Hit goal stretch                                              - 0 0 0 0 0 0 0 0 1
        if(nxt_pos > 50 && nxt_pos < 56)
            action_input[8] = 1;
        //Go through all enemy
        for(int i = 4; i < 16; i++)
        {
            //Check the 6 spaces behind curr piece
            for(int j = 1; j < 6; j++)
            {
                //Check if any enemy is 6 or less behind                - 0 1 0 0 0 0 0 0 0
                if(player_pos_start_of_turn[i] == nxt_pos - j)
                    action_input[1] = 1;
                //Check if enemy is behind second star                  - 0 1 0 0 1 0 0 0 0
                if(action_input[4] == 1)
                    if(player_pos_start_of_turn[i] == nxt_star_pos(nxt_pos) - j)
                    {
                        action_input[1] = 1;
                        //Check if enemy is behind second or first star - 0 1 0 0 1 1 0 0 0
                        if(player_pos_start_of_turn[i] == nxt_pos - j)
                            action_input[5] = 1;
                    }
            }
            //Hit enemy                                                 - 0 0 0 1 0 0 0 0 0
            if(nxt_pos == player_pos_start_of_turn[i])
            {
                action_input[3] = 1;
                //Hit enemy with enemy behind                           - 0 1 0 1 0 0 0 0 0
                //Is taken care of above
            }

        }
        if(     action_input[0] == 0 && action_input[2] == 0 && action_input[3] == 0 &&
                action_input[4] == 0 && action_input[5] == 0 && action_input[6] == 0 &&
                action_input[7] == 0 && action_input[8] == 0)
        {
            cout << "No special moves";
            if(curr_pos != 99)
            {
                //Move in danger                                        - 1 1 0 0 0 0 0 0 0
                if(action_input[1] == 1)
                {
                    cout << " and not in goal, but move with danger" << endl;
                    action_input[0] = 1;
                }
                //Move                                                  - 1 0 0 0 0 0 0 0 0
                else
                {
                    cout << " and not in goal, so just move in safety" << endl;
                    action_input[0] = 1;
                    action_input[1] = 0;
                 }
            }
            else
                cout << " but is already in goal!" << endl;
        }
        //No move possible                                              - 0 0 0 0 0 0 0 0 0
    }
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
        if(state_input[i] == 1) //Player is in home
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

    if(dist(mt) < limit)
    {
        int first_p = get<0>(player_state_action[0]);
        int last_p = get<0>(player_state_action[player_state_action.size()-1]);

        //pick a random player in the following code
        random_device player_rand;
        mt19937 player_mt(player_rand());
        uniform_int_distribution<int> dist_p(first_p, last_p); //play with this

        int player = dist_p(player_mt); //choose the player here!
        player_played = player;
        int prev_pos = player_pos_start_of_turn[player_played];
    //figure out what this does!!
        auto it = find_if(player_state_action.begin(), player_state_action.end(), [](const tuple<int, int, int>& e) {return get<0>(e)  == 0;});
        int pos = distance(player_state_action.begin(), it); //find distance between first element and itterator
        cout << "Position: " << pos << endl;
        int state = get<1>(player_state_action[pos]);
        int action = get<2>(player_state_action[pos]);

        cout << "TEST:" << endl;
        for(int i = 0; i < player_state_action.size(); i++)
        {
            int player = get<0>(player_state_action[i]);
            int state = get<1>(player_state_action[i]);
            int action = get<2>(player_state_action[i]);

            double test = Q_table(state, action);
            cout << test << " ";
        }
        cout << endl;
        cout << "Player: " << player << " state: " << state << " action: " << action << endl;

        return make_tuple(player, state, action, prev_pos);
    }
    else
    {
        double max = -1000000000;
        int pos;
        for(int i = 0; i < player_state_action.size(); i++)
        {
            int player = get<0>(player_state_action[i]);
            int state = get<1>(player_state_action[i]);
            int action = get<2>(player_state_action[i]);

            double test = Q_table(state, action);
            cout << "test: " << test;
            if(test > max)
            {
                max = test;
                pos = i;
                player_played = player;
            }
            cout << endl;
        }
        int prev_pos = player_pos_start_of_turn[player_played];
        int state = get<1>(player_state_action[pos]);
        int action = get<2>(player_state_action[pos]);

        return make_tuple(player_played, state, action, prev_pos);
    }
}

int ludo_player_qlearning::make_q_decision()
{

    cout << "***** New decision *****" << endl << endl;

    if(update_flag != 1)
        cout << "No update...\n";
    else
    {
        cout << "Update!!!" << endl;
//        cout << "Previous player played: " << get<0>(player_state_action_prevPos) << endl;
//        cout << "It was in state: " << get<1>(player_state_action_prevPos) << " and did: " << get<2>(player_state_action_prevPos) << endl;
        updateQtable(player_state_action_prevPos);
        player_state_action.clear();
        ofstream log("ludoLog.txt", ios_base::app);
        ofstream current_table("trainDataQ.txt");
        current_table << Q_table << endl;
        log << Q_table << endl << endl;
//        cout << "The table is saved" << endl;
        update_flag = 0;
    }
    player_state_action.clear();
    for(int i = 0; i < 4; i++)
    {
        if(player_pos_start_of_turn[i] >= -1 && player_pos_start_of_turn[i] < 99)
        {
            cout << "Player: " << i << " at pos: " << player_pos_start_of_turn[i] << " is about to move " << dice_roll << endl;

            float action_input[9] = {0,0,0,0,0,0,0,0,0};
            float state_input[6] = {0,0,0,0,0,0};

            calc_possible_actions(action_input, player_pos_start_of_turn[i], dice_roll, i);
            calc_curr_state(state_input, player_pos_start_of_turn[i], i);

            cout << "Possible actions: ";
            for(int i = 0; i < (sizeof(action_input)/sizeof(float)); i++)
                cout << action_input[i] << " ";
            cout << endl;
            cout << "Curr state: ";
            for(int i = 0; i < (sizeof(state_input)/sizeof(float)); i++)
                cout << state_input[i] << " ";
            cout << endl;

            vector<tuple<int, int, int>> player_i = player_state_actionInterpreter(state_input, action_input, i);
            player_state_action.insert(player_state_action.end(), player_i.begin(), player_i.end());
        }
        else
            cout << "Player: " << i << " is already in goal \n";
    }
    update_flag = 1;
    if(player_state_action.size() > 4)
    {
        cout << "More actions than allowed, with: " << player_state_action.size() << "\n";
        exit(0);
    }

    player_state_action_prevPos = e_greedy(0); //0 = greedy, 1 = random

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
    emit turn_complete(game_complete);
}
