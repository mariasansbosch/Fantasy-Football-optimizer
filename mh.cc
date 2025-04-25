#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <chrono>
#include <ctime>
#include <stdlib.h>

using namespace std;
using namespace std::chrono;


class Player {
public:
  int    id;
  string name;
  string position;
  int    price;
  string club;
  int    points;

  Player(){}
  Player(int ident, const string& n, const string& pos, int pr, const string& c, int p):
    id(ident), name(n), position(pos), price(pr), club(c), points(p){}
};

class Solution { 
    /*Class representing a possible solution. 
    Contains the total price spend, the points obtained and the team created using a vector of the class Player*/
public:
  vector<Player> sol;
  int    price;
  int    points;

  Solution(){}
  Solution(const vector<Player>& s, int pr, int p):
    sol(s), price(pr), points(p){}
};


//variables globals
int N1, N2, N3, T, J;
vector<Player> players, def, dav, gkep, mid;
string output_file;
time_point<high_resolution_clock> start_time;
time_point<high_resolution_clock> end_time;
bool goalkeeper= false;
Solution max_sol({}, 0, 0);

class Tactic {
    /*
    Class that contains the result of the team search. Consist of vectors containing players classificated by their position,
    the points obtained and the price spend and the time to find the solution. 
    */
public:
    double time;
    string gk;
    vector<string> def;
    vector<string> mid;
    vector<string> fw;
    int points;
    int price;
    
    Tactic(){}
    Tactic(double t, const string& goalkeeper, const vector<string>& defense, 
            const vector<string>& midfield, const vector<string>& forward,
            int pts, int prc): time(t), gk(goalkeeper), def(defense), mid(midfield), fw(forward), points(pts), price(prc) {}

        
    void print_Tactic() {
        //Write the solution in an output text file
        ofstream fich(output_file);
        if (!fich) {
            cout << "Error a l'obrir l'arxiu\n";
            exit(EXIT_FAILURE);
        }
        fich << time << endl;
        fich << "POR: " << gk << endl;

        fich << "DEF: ";
        for (uint i = 0; i < def.size(); ++i) {
            fich << def[i];
            if (i < def.size() - 1) {
                fich << ";";
            }
        }
        fich << endl;

        fich << "MIG: ";
        for (uint i = 0; i < mid.size(); ++i) {
            fich << mid[i];
            if (i < mid.size() - 1) {
                fich << ";";
            }
        }
        fich << endl;

        fich << "DAV: ";
        for (uint i = 0; i < fw.size(); ++i) {
            fich << fw[i];
            if (i < fw.size() - 1) {
                fich << ";";
            }
        }
        fich << endl;

        fich << "Punts: " << points << endl;
        fich << "Preu: " << price << endl;
    fich.clear();
    fich.close(); 
    }
};


void print_tactic (const vector<Player>& sol, int points, int price){
    
    string gk;
    vector<string> def;
    vector<string> mid;
    vector<string> fw;

    end_time = high_resolution_clock::now();
    duration<double> elapsed_seconds = end_time - start_time;
    double seconds= round(elapsed_seconds.count()*10.0)/10.0;
    
    for (const  Player& player : sol) {
        if (player.position == "por") gk = player.name;
        else if (player.position == "def") def.push_back(player.name);
        else if (player.position == "mig") mid.push_back(player.name);
        else if (player.position == "dav") fw.push_back(player.name);
    }
    Tactic strategy(seconds, gk, def, mid, fw, points, price);

    strategy.print_Tactic();                
}

bool satisfies_price(Player player, int total_price){
    return total_price + player.price <= T ;
}


Solution greedy_players(vector<Player>& players, int idx, vector<bool>& used){
    //Finds a first solution. The solutions is created in 2 pahses. The first one using the greedy algorithm and the second one randomly. 
    
    int i = 0, points = 0, price = 0;
    int average_price = 0.70; //in the first phase we will choose 8 players out ot 11, therefore we will reduce the price in order not to spend all the money in the greedy phase.  
    int n1 = N1, n2 = N2, n3 = N3;
    int size = players.size(), size_gk = gkep.size(), size_def = def.size(), size_dav = dav.size(), size_mid = mid.size();
    vector<Player> sol;
   
    
    //We use a chose all the players except one of each neither the goalkeeper
    while(i < size and (n1 > 1 or n2 > 1 or n3> 1) ){

        if (n1 > 1 and players[i].position == "def" and players[i].price + price <= T * average_price){
            Player& p = players[i];
            sol.push_back(p);
            points += p.points;
            price += p.price;
            used[p.id] = true;
            --n1;
        }

        else if (n2 > 1 and players[i].position == "mig" and players[i].price + price <= T * average_price){
            Player& p = players[i];
            sol.push_back(p);
            points += p.points;
            price += p.price;
            used[p.id] = true;
            --n2;
        }

        else if (n3 > 1 and players[i].position == "dav" and players[i].price + price <= T * average_price){
            Player& p = players[i];
            sol.push_back(p);
            points += p.points;
            price += p.price;
            used[p.id] = true;
            --n3;
        }
        ++i;
    }

    //The rest of the players are chosen randomly. However we need to check that satisfies de constrains of price and that the player isn't in the solution already;

    srand(time(NULL));
    bool found_def = false;

    while(not found_def){
        int random_int = rand()%size_def;

        if (not used[def[random_int].id] and satisfies_price(def[random_int], price)){
            sol.push_back(def[random_int]);
            points += def[random_int].points;
            price += def[random_int].price;
            
            used[def[random_int].id] = true;
            found_def = true;
        }
    }

    bool found_mid = false;
    while(not found_mid){
        int random_int = rand()%size_mid;   

        if (not used[mid[random_int].id] and satisfies_price(mid[random_int], price)){
            sol.push_back(mid[random_int]);
            points += mid[random_int].points;
            price += mid[random_int].price;
            
            used[mid[random_int].id] = true;
            found_mid = true;
        }
    }

    bool found_dav = false;
    while(not found_dav){
        int random_int = rand()%size_dav;

        if (not used[dav[random_int].id] and satisfies_price(dav[random_int], price)){
            sol.push_back(dav[random_int]);
            points += dav[random_int].points;
            price += dav[random_int].price;

            used[dav[random_int].id] = true;
            found_dav= true;
        }
    }

    bool found_gk = false;
    while(not found_gk){
        int random_int = rand()%size_gk;

        if (not used[gkep[random_int].id] and satisfies_price(gkep[random_int], price)){
            sol.push_back(gkep[random_int]);
            points += gkep[random_int].points;
            price += gkep[random_int].price;

            used[gkep[random_int].id] = true;
            found_gk= true;
        }
    }

    Solution first_sol = Solution(sol, price, points);
    return first_sol;
}


Solution find_neigh(Solution first_sol, vector<bool>& used){
    /* We find all the neighborhoods of the solution sol. Our neighborhood is the same solution except for one player that is changed. */

    //We create a vector with random numbers from 0 to 10
    srand(time(NULL));
    vector<int> random_vector;

    while(random_vector.size() < 11){
        int random_num = rand()%11;
        if (find(random_vector.begin(), random_vector.end(), random_num) == random_vector.end()) random_vector.push_back(random_num);
    }

    /*Each random number represents a player that will go out of the solution.   We look for a substitue that accomplishes all the constrains. 
    If a better solutions is found, we return our team and then we start looking its neighborhoods*/

    uint i = 0;
    while (i < random_vector.size()){
        
        int random = random_vector[i];
        Player p_out = first_sol.sol[random];
        uint idx = 0;

        while (idx < players.size()){
            Player& p = players[idx];   

            if (p.position == p_out.position and satisfies_price(p, first_sol.price - p_out.price) and not used[p.id] and (p.points > p_out.points)){
                first_sol.sol[random] = p;
                first_sol.price = first_sol.price - p_out.price + p.price; // earse the price of the player we take out and add the price of the new player
                first_sol.points = first_sol.points - p_out.points + p.points; // earse the points of the player we take out and add the points of the new player

                used[p_out.id] = false;
                used[p.id] = true;
                
                return first_sol; //we have foundbetter solution  

            }
            ++idx;
        }
        ++i;
    }

    //any neighborhood is better than the first solution
    return first_sol;    
}

bool order_by_points(const Player& p1, const Player& p2){
    //order the players in ascending order
    return p1.points > p2.points;
}

void read_data(ifstream& data, const string& input_path){
    //Reads the input data, and stores the information in a proper data type
    
    ifstream query(input_path);
    if (!query.is_open()) {
        cout << "Error opening input file." << endl;
        exit(1);
    }
    query >> N1 >> N2 >> N3 >> T >> J;

    int id = 0; //we create an index to distinguish all the players
    while (not data.eof()) {
        string name, position, club;
        int points, price;
        getline(data, name, ';');    if (name == "") break;
        getline(data, position, ';');
        data >> price;
        char aux; data >> aux;
        getline(data, club, ';');
        data >> points;
        string aux2;
        getline(data, aux2);

        if (price <= J  and not (price > 0 and points == 0)) {
            players.push_back(Player(id, name, position, price, club, points));
            if (position == "def") def.push_back(Player(id, name, position, price, club, points));
            else if (position == "dav") dav.push_back(Player(id, name, position, price, club, points));
            else if (position == "mig") mid.push_back(Player(id, name, position, price, club, points));
            else gkep.push_back(Player(id, name, position, price, club, points));
            ++id;
        }
    }
    data.close();

}

int main(int argc, char** argv) {

    start_time = high_resolution_clock::now();

    if (argc != 4) {
        cout << "Syntax: " << argv[0] << "data_base.txt input_file.txt output_file.txt" << endl;
        exit(1);
    }

    ifstream data_base(argv[1]);
    string input_path = argv[2];
    output_file = argv[3];

    read_data(data_base, input_path);
    sort(players.begin(), players.end(), order_by_points);
     
    int size = players.size() ;   

    while(true){

        vector<bool> used(size);
        Solution sol_greedy = greedy_players(players, 0, used); //apply greedy algorithm

        Solution sol_local_search = find_neigh(sol_greedy, used); //apply local search

        while (sol_local_search.points > sol_greedy.points){
            sol_greedy = sol_local_search;

            if (sol_greedy.points > max_sol.points){
                max_sol = sol_greedy; //ensure that the solution found is better thant the best general solution obtained by the moment
                print_tactic(max_sol.sol, max_sol.points, max_sol.price);
            }
            sol_local_search = find_neigh(sol_greedy, used); //replay the search;
        }

        if (sol_greedy.points > max_sol.points) {
            max_sol = sol_local_search;
            print_tactic(max_sol.sol, max_sol.points, max_sol.price); 
        }
    }
}
 

 
