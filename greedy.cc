#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <cmath>

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



void print_sol (const vector<Player>& sol, int points, int price, double seconds){
    
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
    Tactic strategy(seconds,gk, def, mid, fw, points, price);
    strategy.print_Tactic();                
}

void find_sol(int n1, int n2, int n3, int points, int price, vector<Player>& sol){
    
    //Finds the optimal solution by exploring possible player combinations, using a greedy algorithm.
    
    int total = 0, i = 0;
    int size = players.size();
    bool gk = false;

    while(total < 11 and i < size){
        if (n1 > 0 and players[i].position == "def" and players[i].price + price <= T){
            sol[total] = players[i];
            points += players[i].points;
            price += players[i].price;
            ++total;
            --n1;
        }
        if (n2 > 0 and players[i].position == "mig" and players[i].price + price <= T){
            sol[total] = players[i];
            points += players[i].points;
            price += players[i].price;
            ++total;
            --n2;
        }
        if (n3 > 0 and players[i].position == "dav" and players[i].price + price <= T){
            sol[total] = players[i];
            points += players[i].points;
            price += players[i].price;
            ++total;
            --n3;
        }
        if (gk == false and players[i].position == "por" and players[i].price + price <= T){
            sol[total] = players[i];
            points += players[i].points;
            price += players[i].price;
            ++total;
            gk = true;
        }
        ++i;
    }

    if (total == 11){
        end_time = high_resolution_clock::now();
        duration<double> seconds = end_time - start_time;
        double seconds_decimal = round(seconds.count() * 10.0)/10.0;
        print_sol(sol, points, price, seconds_decimal);
    }
}

bool order_by_ratio(const Player& p1, const Player& p2){
    //order the players by ratio, giving more importance to the points to maximize the result
    double ratio1 = (static_cast<double>(p1.points)* static_cast<double>(p1.points)) / p1.price;
    double ratio2 = (static_cast<double>(p2.points)*static_cast<double>(p2.points))/ p2.price;
    return ratio1 > ratio2;
}


void read_data(ifstream& data, const string& input_path){

    ifstream query(input_path);
    if (!query.is_open()) {
        cout << "Error opening input file." << endl;
        exit(1);
    }
    query >> N1 >> N2 >> N3 >> T >> J;

    int id = 0; //we create an index to distinguidh all the players
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

        if (price <= J) players.push_back(Player(id, name, position, price, club, points));
        ++id;
        
    }
    data.close();

}

int main(int argc, char** argv){
   
    if (argc != 4) {
        cout << "Syntax: " << argv[0] << "data_base.txt query.txt output_file.txt" << endl;
        exit(1);
    }

    ifstream data_base(argv[1]);
    string input_path = argv[2];
    output_file = argv[3];

    read_data(data_base, input_path);
    
    sort(players.begin(), players.end(), order_by_ratio);

    vector<Player> sol(11);
    
    start_time = high_resolution_clock::now();

    find_sol(N1, N2, N3, 0, 0, sol);

    
   
}

