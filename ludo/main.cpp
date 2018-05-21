#include "dialog.h"
#include <QApplication>
#include "game.h"
#include <vector>
#include "ludo_player.h"
#include "ludo_player_random.h"
#include "ludo_player_qlearning.h"
#include "positions_and_dice.h"

Q_DECLARE_METATYPE( positions_and_dice )

int main(int argc, char *argv[]){
    bool debug = 1;
    QApplication a(argc, argv);
    qRegisterMetaType<positions_and_dice>();

    //instanciate the players here
    //ludo_player p1, p2;
    ludo_player_qlearning p1;
    if(debug) cout << "ludo player created"<< endl;
    ludo_player_random p2, p3, p4;
    if(debug) cout << "all players created" << endl;
    ludo_player_qlearning * p1Ptr = &p1;
    ludo_player_random    * p2Ptr = &p2;
    ludo_player_random    * p3Ptr = &p3;
    ludo_player_random    * p4Ptr = &p4;
    if(debug) cout << "Pointers to players created" << endl;

    game g;
    g.setGameDelay(000); //if you want to see the game, set a delay
    if(debug) cout << "Game start" << endl;


    //Add a GUI <-- remove the '/' to uncomment block
    Dialog w;
    QObject::connect(&g,SIGNAL(update_graphics(std::vector<int>)),&w,SLOT(update_graphics(std::vector<int>)));
    QObject::connect(&g,SIGNAL(set_color(int)),                   &w,SLOT(get_color(int)));
    QObject::connect(&g,SIGNAL(set_dice_result(int)),             &w,SLOT(get_dice_result(int)));
    QObject::connect(&g,SIGNAL(declare_winner(int)),              &w,SLOT(get_winner()));
    QObject::connect(&g,SIGNAL(close()),&a,SLOT(quit()));
    w.show();
    if(debug) cout << "Dialog is open" << endl;
    /*/ //Or don't add the GUI
    QObject::connect(&g,SIGNAL(close()),&a,SLOT(quit()));
    //*/
    g.add_players(p1Ptr, p2Ptr, p3Ptr, p4Ptr);
    g.init_games();
    if(debug) cout << "Game is init" << endl;
    QApplication * aPtr;
    aPtr = &a;

    for(int i = 0; i < 10000; ++i){
        g.start();
        a.exec();
        g.reset();
    }
    return 0;
}
