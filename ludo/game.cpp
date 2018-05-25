#include "game.h"
#define DEBUG 0
bool debug = 0;

game::game():
    game_complete(false),
    turn_complete(true),
    game_delay(1000),
    relative(),
    dice_result(1),
    rd(),
    gen(rd()),
    color(3),
    player_positions({-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1})
{
    if(debug) cout << "Win cnt init\n";
    winCnt1 = winCnt2 = winCnt3 = winCnt4 = 0;
    firstInit = true;
}

void game::init_games()
{
    if(debug) cout << "Start with init!" << endl;
    if(!firstInit)
    {
        if(debug) cout << "first init" << endl;
        QObject::disconnect(this,SIGNAL(player1_start(positions_and_dice)),&(*pp1), SLOT(start_turn(positions_and_dice)));
        QObject::disconnect(&(*pp1),SIGNAL(select_piece(int)),               this, SLOT(movePiece(int)));
        QObject::disconnect(this, SIGNAL(player1_end(std::vector<int>)),    &(*pp1),SLOT(post_game_analysis(std::vector<int>)));
        QObject::disconnect(&(*pp1),SIGNAL(turn_complete(bool)),             this,SLOT(turnComplete(bool)));

        QObject::disconnect(this, SIGNAL(player2_start(positions_and_dice)),&(*pp2),SLOT(start_turn(positions_and_dice)));
        QObject::disconnect(&(*pp2),SIGNAL(select_piece(int)),                 this, SLOT(movePiece(int)));
        QObject::disconnect(this, SIGNAL(player2_end(std::vector<int>)),    &(*pp2),SLOT(post_game_analysis(std::vector<int>)));
        QObject::disconnect(&(*pp2),SIGNAL(turn_complete(bool)),               this, SLOT(turnComplete(bool)));

        QObject::disconnect(this, SIGNAL(player3_start(positions_and_dice)),&(*pp3),SLOT(start_turn(positions_and_dice)));
        QObject::disconnect(&(*pp3),SIGNAL(select_piece(int)),                 this, SLOT(movePiece(int)));
        QObject::disconnect(this, SIGNAL(player3_end(std::vector<int>)),    &(*pp3),SLOT(post_game_analysis(std::vector<int>)));
        QObject::disconnect(&(*pp3),SIGNAL(turn_complete(bool)),               this, SLOT(turnComplete(bool)));

        QObject::disconnect(this, SIGNAL(player4_start(positions_and_dice)),&(*pp4),SLOT(start_turn(positions_and_dice)));
        QObject::disconnect(&(*pp4),SIGNAL(select_piece(int)),                 this, SLOT(movePiece(int)));
        QObject::disconnect(this, SIGNAL(player4_end(std::vector<int>)),    &(*pp4),SLOT(post_game_analysis(std::vector<int>)));
        QObject::disconnect(&(*pp4),SIGNAL(turn_complete(bool)),              this, SLOT(turnComplete(bool)));
        if(debug) cout << "disconnected" << endl;
    }
    if(debug) cout << "Begin to connect" << endl;
    QObject::connect(this, SIGNAL(player1_start(positions_and_dice)),&(*pp1),SLOT(start_turn(positions_and_dice)));
    QObject::connect(&(*pp1),SIGNAL(select_piece(int)),                this, SLOT(movePiece(int)));
    QObject::connect(this, SIGNAL(player1_end(std::vector<int>)),    &(*pp1),SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&(*pp1),SIGNAL(turn_complete(bool)),              this, SLOT(turnComplete(bool)));

    QObject::connect(this, SIGNAL(player2_start(positions_and_dice)),&(*pp2),SLOT(start_turn(positions_and_dice)));
    QObject::connect(&(*pp2),SIGNAL(select_piece(int)),                this, SLOT(movePiece(int)));
    QObject::connect(this, SIGNAL(player2_end(std::vector<int>)),    &(*pp2),SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&(*pp2),SIGNAL(turn_complete(bool)),              this, SLOT(turnComplete(bool)));

    QObject::connect(this, SIGNAL(player3_start(positions_and_dice)),&(*pp3),SLOT(start_turn(positions_and_dice)));
    QObject::connect(&(*pp3),SIGNAL(select_piece(int)),                this, SLOT(movePiece(int)));
    QObject::connect(this, SIGNAL(player3_end(std::vector<int>)),    &(*pp3),SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&(*pp3),SIGNAL(turn_complete(bool)),              this, SLOT(turnComplete(bool)));

    QObject::connect(this, SIGNAL(player4_start(positions_and_dice)),&(*pp4),SLOT(start_turn(positions_and_dice)));
    QObject::connect(&(*pp4),SIGNAL(select_piece(int)),                this, SLOT(movePiece(int)));
    QObject::connect(this, SIGNAL(player4_end(std::vector<int>)),    &(*pp4),SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&(*pp4),SIGNAL(turn_complete(bool)),              this, SLOT(turnComplete(bool)));
    if(debug) cout << "connection established\n";

    if(debug) cout << "Init end" << endl;
    if(firstInit == true)
        firstInit = false;
    gameTrainFlag = false;
    gameFlag = true;
}

void game::init_train_games()
{
    if(!firstInit)
    {
        QObject::disconnect(this,SIGNAL(player1_start(positions_and_dice)),&(*pp1), SLOT(start_turn(positions_and_dice)));
        QObject::disconnect(&(*pp1),SIGNAL(select_piece(int)),               this, SLOT(movePiece(int)));
        QObject::disconnect(this, SIGNAL(player1_end(std::vector<int>)),    &(*pp1),SLOT(post_game_analysis(std::vector<int>)));
        QObject::disconnect(&(*pp1),SIGNAL(turn_complete(bool)),                this,SLOT(turnComplete(bool)));

        QObject::disconnect(this, SIGNAL(player2_start(positions_and_dice)),&(*pp2),SLOT(start_turn(positions_and_dice)));
        QObject::disconnect(&(*pp2),SIGNAL(select_piece(int)),                 this, SLOT(movePiece(int)));
        QObject::disconnect(this, SIGNAL(player2_end(std::vector<int>)),    &(*pp2),SLOT(post_game_analysis(std::vector<int>)));
        QObject::disconnect(&(*pp2),SIGNAL(turn_complete(bool)),               this, SLOT(turnComplete(bool)));

        QObject::disconnect(this, SIGNAL(player3_start(positions_and_dice)),&(*pp3),SLOT(start_turn(positions_and_dice)));
        QObject::disconnect(&(*pp3),SIGNAL(select_piece(int)),                 this, SLOT(movePiece(int)));
        QObject::disconnect(this, SIGNAL(player3_end(std::vector<int>)),    &(*pp3),SLOT(post_game_analysis(std::vector<int>)));
        QObject::disconnect(&(*pp3),SIGNAL(turn_complete(bool)),               this, SLOT(turnComplete(bool)));

        QObject::disconnect(this, SIGNAL(player4_start(positions_and_dice)),&(*pp4),SLOT(start_turn(positions_and_dice)));
        QObject::disconnect(&(*pp4),SIGNAL(select_piece(int)),                 this, SLOT(movePiece(int)));
        QObject::disconnect(this, SIGNAL(player4_end(std::vector<int>)),    &(*pp4),SLOT(post_game_analysis(std::vector<int>)));
        QObject::disconnect(&(*pp4),SIGNAL(turn_complete(bool)),              this, SLOT(turnComplete(bool)));
        //std::cout << "Disconnected \n";
    }
    QObject::connect(this, SIGNAL(player1_start(positions_and_dice)),&(*pp1),SLOT(start_turn(positions_and_dice)));
    QObject::connect(&(*pp1),SIGNAL(select_piece(int)),                this, SLOT(movePiece(int)));
    QObject::connect(this, SIGNAL(player1_end(std::vector<int>)),    &(*pp1),SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&(*pp1),SIGNAL(turn_complete(bool)),              this, SLOT(turnComplete(bool)));

    QObject::connect(this, SIGNAL(player2_start(positions_and_dice)),&(*pp2),SLOT(start_turn(positions_and_dice)));
    QObject::connect(&(*pp2),SIGNAL(select_piece(int)),                this, SLOT(movePiece(int)));
    QObject::connect(this, SIGNAL(player2_end(std::vector<int>)),    &(*pp2),SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&(*pp2),SIGNAL(turn_complete(bool)),              this, SLOT(turnComplete(bool)));

    QObject::connect(this, SIGNAL(player3_start(positions_and_dice)),&(*pp3),SLOT(start_turn(positions_and_dice)));
    QObject::connect(&(*pp3),SIGNAL(select_piece(int)),                this, SLOT(movePiece(int)));
    QObject::connect(this, SIGNAL(player3_end(std::vector<int>)),    &(*pp3),SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&(*pp3),SIGNAL(turn_complete(bool)),              this, SLOT(turnComplete(bool)));

    QObject::connect(this, SIGNAL(player4_start(positions_and_dice)),&(*pp4),SLOT(start_turn(positions_and_dice)));
    QObject::connect(&(*pp4),SIGNAL(select_piece(int)),                this, SLOT(movePiece(int)));
    QObject::connect(this, SIGNAL(player4_end(std::vector<int>)),    &(*pp4),SLOT(post_game_analysis(std::vector<int>)));
    QObject::connect(&(*pp4),SIGNAL(turn_complete(bool)),              this, SLOT(turnComplete(bool)));

    if(firstInit == 0)
        firstInit = 1;
    gameTrainFlag = true;
    gameFlag = false;
}

int game::isOccupied(int index){ //returns number of people of another color
    int number_of_people = 0;

    if(index != 99){
        for(size_t i = 0; i < player_positions.size(); ++i){
            if(i < static_cast<size_t>(color)*4 || i >= static_cast<size_t>(color)*4 + 4){        //Disregard own players
                if(player_positions[i] == index){
                    ++number_of_people;
                }
            }
        }
    }
    return number_of_people;
}

void game::reset(){
    game_complete = false;
    turn_complete = true;
    for(auto &i : player_positions){ //without & we're changing the copy made in auto rather than the player_position
        i = -1;
    }
    color = 3;
}


int game::rel_to_fixed(int relative_piece_index){
    return relative_piece_index + color * 4;
}

int game::isStar(int index){
    if(index == 5  ||
       index == 18 ||
       index == 31 ||
       index == 44){
        return 6;
    } else if(index == 11 ||
              index == 24 ||
              index == 37 ||
              index == 50){
        return 7;
    }
    return 0;
}

bool game::isGlobe(int index){
    if(index < 52){     //check only the indexes on the board, not in the home streak
        if(index % 13 == 0 || (index - 8) % 13 == 0 || isOccupied(index) > 1){  //if more people of the same team stand on the same spot it counts as globe
            return true;
        }
    }
    return false;
}

void game::send_them_home(int index){
    for(size_t i = 0; i < player_positions.size(); ++i){
        if(i < static_cast<size_t>(color)*4 || i >= static_cast<size_t>(color)*4 + 4){        //this way we don't skip one player position
            if(player_positions[i] == index){
                player_positions[i] = -1;
            }
        }
    }
}

void game::move_start(int fixed_piece){
    if(dice_result == 6 && player_positions[fixed_piece] < 0){
        player_positions[fixed_piece] = color*13; //move me to start
        send_them_home(color*13); //send pieces home if they are on our start
    }
}

void game::runUserDef(QApplication *gameObject)
{
    anApplication = gameObject;
}

int game::next_turn(unsigned int delay = 0){
    if(game_complete){
        return 0;
    }
    switch(color){
        case 0:
        case 1:
        case 2:
            ++color;
            break;
        case 3:
        default:
            color = 0;
            break;
    }
    // global_color = color;
    rollDice();
    relative.dice = getDiceRoll();
    relative.pos = relativePosition();
    emit set_color(color);
    emit set_dice_result(dice_result);

    msleep(delay);
    switch(color){
        case 0:
            emit player1_start(relative);
            break;
        case 1:
            emit player2_start(relative);
            break;
        case 2:
            emit player3_start(relative);
            break;
        case 3:
            emit player4_start(relative);
        default:
            break;
    }

    return 0;
}

void game::movePiece(int relative_piece){
    int fixed_piece = rel_to_fixed(relative_piece);     //index of the piece in player_positions
    int modifier = color * 13;
    int relative_pos = player_positions[fixed_piece];
    int target_pos = 0;
    if(player_positions[fixed_piece] == -1){        //if the selected piece is in the safe house, try to move it to start
        move_start(fixed_piece);
    } else {
        //convert to relative position
        if(relative_pos == 99){
            //std::cout << "I tought this would be it ";
        } else if(relative_pos == 51){ //if people land on 51, they shouldn't be sent to goal stretch
            switch(color){
            case 0 : relative_pos = 51; break;
            case 1 : relative_pos = 38; break;
            case 2 : relative_pos = 25; break;
            case 3 : relative_pos = 12; break;
            }
        } else if( relative_pos > 50) {
            relative_pos = relative_pos - color * 5 - 1;
        } else if(relative_pos < modifier) {
            relative_pos = relative_pos + 52 - modifier;
        } else if( relative_pos > 50) {
            relative_pos = relative_pos - color * 5 - 1;
        } else {//if(relative >= modifier)
            relative_pos = relative_pos - modifier;
        }
        if(DEBUG) std::cout << "color: " << color << " pos: " << relative_pos << " + " << dice_result << " = " << relative_pos + dice_result;
        //add dice roll
        relative_pos += dice_result;    //this is relative position of the selected token + the dice number

        int jump = isStar(relative_pos); //return 0 | 6 | 7
        if(jump){
            if(jump + relative_pos == 57){
                relative_pos = 56;
            } else {
                relative_pos += jump;
            }
        }
        //special case checks
        if(relative_pos > 56 && relative_pos < 72){ // go back
            target_pos = 56-(relative_pos-56) + color * 5 + 1; //If the player moves over the goal, it should move backwards
        }else if(relative_pos == 56 || relative_pos >= 99){
            target_pos = 99;
        }else if(relative_pos > 50){ // goal stretch
            target_pos = relative_pos + color * 5 + 1;
        } else {
            int new_pos = relative_pos + modifier;
            if(new_pos < 52){
                target_pos = new_pos;
            } else { //wrap around
                target_pos = new_pos - 52;  //this is the global position wrap around at the green entry point
            }
        }
        //check for game stuff

        if(isOccupied(target_pos)){
            if(isGlobe(target_pos)){
                target_pos = -1; //send me home
            } else {
                send_them_home(target_pos);
            }
        }
        if(DEBUG) std::cout << " => " << target_pos << std::endl;
        player_positions[fixed_piece] = target_pos;
    }
    std::vector<int> new_relative = relativePosition();
    switch(color){
        case 0:
            emit player1_end(new_relative);
            break;
        case 1:
            emit player2_end(new_relative);
            break;
        case 2:
            emit player3_end(new_relative);
            break;
        case 3:
            emit player4_end(new_relative);
        default:
            break;
    }
    emit update_graphics(player_positions);
}

std::vector<int> game::relativePosition(){
    std::vector<int> relative_positions;
    int modifier = color * 13;

    //from start id to end
    for(int i = color*4; i < static_cast<int>(player_positions.size()); ++i){
        relative_positions.push_back(player_positions[i]);
    }
    //from 0 to start id
    for(int i = 0; i < color*4; ++i){
        relative_positions.push_back(player_positions[i]);
    }


    for(size_t i = 0; i < relative_positions.size(); ++i){
        if(relative_positions[i] == 99 || relative_positions[i] == -1){
            relative_positions[i] = (relative_positions[i]);
        } else if(relative_positions[i] < modifier) {
            relative_positions[i] = (relative_positions[i]+52-modifier);
        } else if(relative_positions[i] > 50) {
            relative_positions[i] = (relative_positions[i]-color*5-1);
        } else if(relative_positions[i] >= modifier) {
            relative_positions[i] = (relative_positions[i]-modifier);
        }
    }
    return std::move(relative_positions);
}

void game::add_players(ludo_player_qlearning *p1, ludo_player_random *p2, ludo_player_random *p3, ludo_player_random *p4)
{
    pp1 = p1;
    pp2 = p2;
    pp3 = p3;
    pp4 = p4;
}

//void game::add_players(ludo_player_qlearning *p1, ludo_player *p2, ludo_player_q *p3, ludo_player_random *p4)
//{
//    pp1 = p1;
//    pp2 = p2;
//    pp3 = p3;
//    pp4 = p4;
//}

void game::turnComplete(bool win){
    game_complete = win;
    turn_complete = true;
    if(game_complete){
        if(debug) cout << "player: " << color << " won" << std::endl;
        switch(color){
        case 0:
            winCnt1++;
            break;
        case 1:
            winCnt2++;
            break;
        case 2:
            winCnt3++;
            break;
        case 3:
            winCnt4++;
            break;
        }
        totalWinCnt++;
        outWinCnt++;
        if (outWinCnt == timesToPlay && gameFlag == true)
        {
            std::cout << "\nPlayer 1 has won: " << winCnt1 << " times with a winrate of: " << (float)winCnt1/(float)outWinCnt << endl;
            std::cout << "Player 2 has won: " << winCnt2 << " times with a winrate of: " << (float)winCnt2/(float)outWinCnt << endl;
            std::cout << "Player 3 has won: " << winCnt3 << " times with a winrate of: " << (float)winCnt3/(float)outWinCnt << endl;
            std::cout << "Player 4 has won: " << winCnt4 << " times with a winrate of: " << (float)winCnt4/(float)outWinCnt << endl;
            std::cout << "Total games: " << totalWinCnt << "\n";

            double percent_win1 = (float)winCnt1/(float)outWinCnt * 100;
            double percent_win2 = (float)winCnt2/(float)outWinCnt * 100;
            double percent_win3 = (float)winCnt3/(float)outWinCnt * 100;
            double percent_win4 = (float)winCnt4/(float)outWinCnt * 100;

            tmpMeanVec1.push_back(percent_win1);
            tmpMeanVec2.push_back(percent_win2);
            tmpMeanVec3.push_back(percent_win3);
            tmpMeanVec4.push_back(percent_win4);
            statCnt++;

            resetCnt();
        }
        if(statCnt == 5)
        {
            plotTimeLine++;

            double sum1 = std::accumulate(tmpMeanVec1.begin(), tmpMeanVec1.end(), 0.0);
            double mean1 = sum1 / tmpMeanVec1.size();
            double sum2 = std::accumulate(tmpMeanVec2.begin(), tmpMeanVec2.end(), 0.0);
            double mean2 = sum2 / tmpMeanVec2.size();
            double sum3 = std::accumulate(tmpMeanVec3.begin(), tmpMeanVec3.end(), 0.0);
            double mean3 = sum3 / tmpMeanVec3.size();
            double sum4 = std::accumulate(tmpMeanVec4.begin(), tmpMeanVec4.end(), 0.0);
            double mean4 = sum4 / tmpMeanVec4.size();

            double sq_sum1 = std::inner_product(tmpMeanVec1.begin(), tmpMeanVec1.end(), tmpMeanVec1.begin(), 0.0);
            double stdev1 = std::sqrt(sq_sum1 / tmpMeanVec1.size() - mean1 * mean1);
            double sq_sum2 = std::inner_product(tmpMeanVec2.begin(), tmpMeanVec2.end(), tmpMeanVec2.begin(), 0.0);
            double stdev2 = std::sqrt(sq_sum2 / tmpMeanVec2.size() - mean2 * mean2);
            double sq_sum3 = std::inner_product(tmpMeanVec3.begin(), tmpMeanVec3.end(), tmpMeanVec3.begin(), 0.0);
            double stdev3 = std::sqrt(sq_sum3 / tmpMeanVec3.size() - mean3 * mean3);
            double sq_sum4 = std::inner_product(tmpMeanVec4.begin(), tmpMeanVec4.end(), tmpMeanVec4.begin(), 0.0);
            double stdev4 = std::sqrt(sq_sum4 / tmpMeanVec4.size() - mean4 * mean4);

            tmpMeanVec1.clear();
            tmpMeanVec2.clear();
            tmpMeanVec3.clear();
            tmpMeanVec4.clear();

            meanVector1.push_back(mean1);
            stdVector1.push_back(stdev1);
            meanVector2.push_back(mean2);
            stdVector2.push_back(stdev2);
            meanVector3.push_back(mean3);
            stdVector3.push_back(stdev3);
            meanVector4.push_back(mean4);
            stdVector4.push_back(stdev4);

            statCnt = 0;
        }
        if(totalWinCnt == timesToPlay*5)
        {
            ofstream stat("stat.txt");
//            ofstream stat1("stat1.txt");
//            ofstream stat2("stat2.txt");
//            ofstream stat3("stat3.txt");
//            ofstream stat4("stat4.txt");
            for(int i = 0; i <  meanVector1.size(); i++)
            {
//                cout << "Mean vecotr i: " << meanVector1[i]<< " ";
//                stat1 << i+1 << " " << meanVector1[i] << " " << stdVector1[i] << endl;
//                stat2 << i+1 << " " << meanVector2[i] << " " << stdVector2[i] << endl;
//                stat3 << i+1 << " " << meanVector3[i] << " " << stdVector3[i] << endl;
//                stat4 << i+1 << " " << meanVector4[i] << " " << stdVector4[i] << endl;
                stat << i+1 << " " << meanVector1[i] << " " << stdVector1[i] << " ";
                stat << meanVector2[i] << " " << stdVector2[i] << " ";
                stat << meanVector3[i] << " " << stdVector3[i] << " ";
                stat << meanVector4[i] << " " << stdVector4[i] << endl;

            }
//            cout << endl;
//            for(int i = 0; i < stdVector1.size(); i++)
//            {
//                cout << "std vecotr i: " << stdVector1[i]<< " ";
//                stat << stdVector1[i] << " ";
//            }
//            cout << endl;
        }
//        if(totalWinCnt == timesToPlay*10)
//        {
//            write_vector_to_file(meanVector1, "stat.txt");
//        }
        emit declare_winner(color);
    }
}

void game::resetCnt()
{
    winCnt1 = winCnt2 = winCnt3 = winCnt4 = 0;
    outWinCnt = 0;
}

void game::run()
{
    for(int i = 0; i < timesToPlay*10; i++)
    {
        start();
        while(!game_complete)
        {
            if(turn_complete)
            {
                turn_complete = false;
                msleep(game_delay/4);
                next_turn(game_delay - game_delay/4);
            }
        }
        reset();
    }
    emit close();
    QThread::exit();
    if(debug) cout << "Exiting" << endl;
}

void game::write_vector_to_file(const std::vector<double>& myVector,std::string filename)
{
    std::ofstream ofs(filename,std::ios::out | std::ofstream::binary);
    std::ostream_iterator<char> osi{ofs};
    std::copy(myVector.begin(),myVector.end(),osi);
}

//void game::run() {
//    std::cout << "Entered run \n" ;
//    if(DEBUG) std::cout << "color:     relative pos => fixed\n";
//    for (int ii = 0; ii < 100000000; ii++){
//        start();
//        // std::cout<< "Started game\n" ;
//        while(!game_complete){
//            //std::cout << "Running...\n";
//            if(turn_complete){
//                turn_complete = false;
//                msleep(game_delay/4);
//                next_turn(game_delay - game_delay/4);
//            }
//        }
//        reset();

//        //std::cout << ii << "\n";
//    }
//    emit close();
//    QThread::exit();
//}
