#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cassert>
#include <cmath>
#include <chrono>
using namespace std;
using namespace std::chrono;



class Player {
    //Class representing a football player
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


//GLOBAL VARIABLES
int N1, N2, N3, T, J,max_points;
vector<Player> players;
string output_file;
time_point<high_resolution_clock> start_time;
time_point<high_resolution_clock> end_time;


class Tactic {
    //Class representing a football tactic
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


void print_sol (const vector<Player>& sol, int points, int price, double time){

    string gk;
    vector<string> def;
    vector<string> mid;
    vector<string> fw;
    
    for (const  Player& player : sol) {
        if (player.position == "por") gk = player.name;
        else if (player.position == "def") def.push_back(player.name);
        else if (player.position == "mig") mid.push_back(player.name);
        else if (player.position == "dav") fw.push_back(player.name);
    }
   
    Tactic strategy(time,gk, def, mid, fw, points, price);
    strategy.print_Tactic();                
}

bool satisfies_price(Player player, int partial_price){
    //Check if adding a player to the solution satisfies the budget constraint.
    return partial_price + player.price <= T ;
}


bool check_points(int n1, int n2, int n3, bool gk, int partial_points, int idx){
    //Precomputes whether there will be an improvement in points by adding the players who have not yet been used, sorted by points.

    int size = players.size();
    int sum_points = 0;
   
    while((idx < size) and (n1>0 or n2>0 or n3>0 or not gk)){
        if (players[idx].position == "def" and n1 > 0){
            sum_points += players[idx].points;
            n1 -= 1;            
            if (sum_points + partial_points > max_points) return true;
        }
        else if (players[idx].position == "mig" and n2 > 0){
            sum_points += players[idx].points;
            n2 -= 1;
            
            if (sum_points + partial_points > max_points) return true;
        }
        else if (players[idx].position == "dav" and n3 > 0){
            sum_points += players[idx].points;
            n3 -= 1;
            
            if (sum_points + partial_points > max_points) return true;
        }
        else if (players[idx].position == "por" and not gk){
            sum_points += players[idx].points;
            gk = true;
            
            if (sum_points + partial_points > max_points) return true;
        }
        idx += 1;
    }
    return sum_points + partial_points > max_points;
}


void find_sol(int n1, int n2, int n3, int partial_points, int partial_price, int idx, vector<Player>& sol, bool goalkeeper){

    //Recursively finds the optimal solution by exploring possible player combinations, using exhaustive search.

    int size = players.size(); 
    
    
    if ((n1==0) and (n2== 0) and (n3== 0) and (goalkeeper) and (partial_points > max_points)){
        max_points = partial_points;
        
        end_time = high_resolution_clock::now();
        duration<double> elapsed_seconds= end_time - start_time;
        double seconds = round(elapsed_seconds.count()*10.0)/10.0;
        print_sol(sol, max_points, partial_price, seconds);
        
    }
    
    else if (idx< size){

        find_sol(n1,n2,n3, partial_points, partial_price, idx+1, sol, goalkeeper);
        Player p = players[idx];

        if (p.position == "por" and goalkeeper == false and satisfies_price(p, partial_price) and check_points(n1, n2, n3, goalkeeper, partial_points, idx)){
            
            sol.push_back(p);
            find_sol(n1,n2,n3, partial_points + p.points, partial_price + p.price, idx+1, sol, true);
            sol.pop_back();
        }

        else if (p.position == "def" and n1 > 0 and satisfies_price(p, partial_price) and check_points(n1, n2, n3, goalkeeper, partial_points, idx)){
    
            sol.push_back(p);
            find_sol(n1-1,n2,n3, partial_points + p.points, partial_price + p.price, idx+1, sol, goalkeeper) ;
            sol.pop_back();
    
        }       
        
        else if (p.position == "mig" and n2 > 0 and satisfies_price(p, partial_price) and check_points(n1, n2, n3, goalkeeper, partial_points, idx)){
            sol.push_back(p);
            find_sol(n1,n2-1,n3, partial_points + p.points, partial_price + p.price, idx+1, sol, goalkeeper) ;
            sol.pop_back();
    
        }

        else if (p.position == "dav" and n3 > 0 and satisfies_price(p, partial_price) and check_points(n1, n2, n3, goalkeeper, partial_points, idx)){
            sol.push_back(p);                        
            find_sol(n1,n2,n3-1, partial_points+ p.points, partial_price+ p.price, idx+1, sol, goalkeeper);
            sol.pop_back();
        }
    }   
}


bool order_by_points(const Player& p1, const Player& p2){
    return p1.points > p2.points;
}


void read_data(ifstream& data, const string& input_path){
    //Reads the data base from a file and converts it to propper data to be used.

    ifstream query(input_path);
    if (!query.is_open()) {
        cout << "Error opening input file." << endl;
        exit(1);
    }
    query >> N1 >> N2 >> N3 >> T >> J;

    int id = 0; //create an index to distinguish all the players
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

        if (price <= J and not (price > 0 and points == 0)) players.push_back(Player(id, name, position, price, club, points));
        //check that we don't exceed the player's price and that we don't choose players that don't provide any points but take us budget
        ++id;
    }
    data.close();
}



int main(int argc, char** argv) {

    if (argc != 4) {
        cout << "Syntax: " << argv[0] << "data_base.txt input_file.txt output_file.txt" << endl;
        exit(1);
    }

    ifstream data_base(argv[1]);
    string input_path = argv[2];
    output_file = argv[3];

    read_data(data_base, input_path); 
    sort(players.begin(), players.end(), order_by_points); //sort players by its points

    max_points = 0;
    vector<Player> sol(11);    

    start_time = high_resolution_clock::now();

    find_sol(N1, N2, N3, 0, 0, 0, sol, false);
 
}
 