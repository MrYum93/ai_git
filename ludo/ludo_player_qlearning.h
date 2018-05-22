#ifndef LUDO_PLAYER_QLEARNING_H
#define LUDO_PLAYER_QLEARNING_H
#include <QObject>
#include <iostream>
#include "positions_and_dice.h"
#include <eigen3/Eigen/Dense>
#include <fstream>
#include <random>
#include <vector>

using namespace std;

class ludo_player_qlearning : public QObject {
    Q_OBJECT
public:
    ludo_player_qlearning();
    int get_cum_rew(int player); //get cumulative reward for any player
private:
    // functions
    int make_decision();
    int make_q_decision();
    int nxt_star_pos(int pos);
    int curr_state_intepreter(float state_input[6]);
    void updateQtable(tuple<int, int, int, int> player_state_action_pos);
    void calc_possible_actions(float action_input[9], int curr_pos, int dice, int self); //token:player on spot
    void calc_curr_state(float state_input[6], int curr_pos, int self);
    vector<tuple<int, int, int>> player_state_actionInterpreter(float state_input[6], float action_input[9], int self);
    tuple<int, int, int, int> e_greedy(double eps);
    bool file_exists(const string &filename);
    bool is_star(int pos);
    bool is_globe(int pos);

    // Variables
    Eigen::MatrixXd Q_table;
    vector<int> player_pos_start_of_turn;
    vector<int> player_pos_end_of_turn;
    vector<int> star_pos;
    vector<int> globe_pos;
    vector<tuple<int,int,int>> player_state_action;
    tuple<int, int, int, int> player_state_action_prevPos;
    bool update_flag;
    bool debug = 0;
    bool a_debug = 0;
    int player_played;
    int dice_roll;
    int cum_rew_p0;
    int cum_rew_p1;
    int cum_rew_p2;
    int cum_rew_p3;
    double acc = 0;
    float numeric_min = numeric_limits<float>::min();
signals:
    void select_piece(int);
    void turn_complete(bool);
public slots:
    void start_turn(positions_and_dice relative);
    void post_game_analysis(std::vector<int> relative_pos);
};

/* -------Possible actions---------
 0 * 1 0 0 0 0 0 0 0 0 - Move in safety
 1 * 1 1 0 0 0 0 0 0 0 - Move with enemy immediately behind
 2 * 0 0 1 0 0 0 0 0 0 - Get out from home
 3 * 0 0 0 1 0 0 0 0 0 - Hit enemy
 4 * 0 1 0 1 0 0 0 0 0 - Hit enemy with enemy behind
 5 * 0 0 0 0 1 0 0 0 0 - Hit star
 6 * 0 1 0 0 1 0 0 0 0 - Hit star with enemy behind second star
 7 * 0 1 0 0 1 1 0 0 0 - Hit star with enemy behind first and second star
 8 * 0 0 0 0 0 0 1 0 0 - Hit globe, or stack with friend
 9 * 0 1 0 0 0 0 1 0 0 - Hit globe with enemy behind (great state)
10 * 0 0 0 1 0 0 1 0 0 - Hit globe with enemy on it or hit stack of enemies
11 * 0 0 0 0 0 0 0 1 0 - Hit goal
12 * 0 0 0 0 0 0 0 0 1 - Hit goal stretch
13 * 0 0 0 0 0 0 0 0 0 - No move possible
 * -------------------------------- */

/* -------possible states-------
 0 * 1 0 0 0 0 0 - Home
 1 * 0 1 0 0 0 0 - Goal
 2 * 0 0 1 0 0 0 - Goal stretch
 3 * 0 0 0 1 0 0 - Globe or with friend
 4 * 0 0 0 0 1 0 - Free space in danger
 5 * 0 0 0 0 0 1 - Free space
 * ----------------------------- */

#endif // LUDO_PLAYER_QLEARNING_H
