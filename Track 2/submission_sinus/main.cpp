#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <cmath>
#include <time.h>

using namespace std;
typedef vector <double> vd;
typedef vector <vd> vvd;

const double pi = acos(-1);
double initial_time;
struct tm time1970 = {0};
vvd data, M(600, vd(6*11)); //coefs
vd T(600); //periods

void init(){
    fstream fin; 
    fin.open("sin_betas.txt", ios::in); 

    fin >> initial_time;
    initial_time -= 86400;

    for (int i = 0; i < 600; ++i){
        fin >> T[i];
        for (int j = 0; j < 6; ++j){
            for (int k = 0; k < 11; ++k){
                if (k > 0){
                    char c;
                    fin >> c;
                }
                
                fin >> M[i][j*11 + k];
            }
        }
    }
    fin.close();

    time1970.tm_hour = 0;   time1970.tm_min = 0; time1970.tm_sec = 0;
    time1970.tm_year = 1970; time1970.tm_mon = 0; time1970.tm_mday = 1;
}

double time(string info){  
    struct tm timenew = {0};
    
    stringstream ss(info);

    vector <string> V(6);

    getline(ss, V[0], '-');
    getline(ss, V[1], '-');
    getline(ss, V[2], 'T');
    getline(ss, V[3], ':');
    getline(ss, V[4], ':');
    getline(ss, V[5]);

    timenew.tm_hour = stod(V[3]);   timenew.tm_min = stod(V[4]); timenew.tm_sec = stod(V[5]);
    timenew.tm_year = stod(V[0]); timenew.tm_mon = stod(V[1])-1; timenew.tm_mday = stod(V[2]);

    return difftime(timegm(&timenew),timegm(&time1970)) - initial_time;
}

void read_train(){
    fstream fin; 
  
    fin.open("train.csv", ios::in); 
    vd query; 
    string s, info;

    for (int i = 0; i < 15; ++i){
        if (i == 14) getline(fin, info);
        else getline(fin, info, ',');
    }

    int cnt = 0;
    while (cnt++ < 2 and getline(fin, info, ',')){ 
        query = {stod(info)};

        for (int i = 0; i < 14; ++i){
            if (i == 13) getline(fin, info);
            else getline(fin, info, ',');

            if (i >= 2) continue;

            if (i == 1) query.push_back(stod(info)); 
            else query.push_back(time(info));
        } 

        data.push_back(query);
    }

    fin.close();
} 

void read_test(){
    fstream fin; 
  
    fin.open("test.csv", ios::in); 
    vd query; 
    string s, info;

    for (int i = 0; i < 9; ++i){
        if (i == 8) getline(fin, info);
        else getline(fin, info, ',');
    }

    while (getline(fin, info, ',')){ 
        query = {stod(info)};
        for (int i = 0; i < 8; ++i){
            if (i == 7) getline(fin, info);
            else getline(fin, info, ',');

            if (i >= 2) continue;

            if (i == 0) query.push_back(stod(info)); 
            else query.push_back(time(info));
        } 

        data.push_back(query);
    }

    fin.close();
} 

double evaluate(double x, int i, int k){
    double ans = M[i][k];
    for (int j = 2; j < 11; j += 2) ans += M[i][k+j-1] * sin(pi*x*j/T[i]) + M[i][k+j] * cos(pi*x*j/T[i]);
    
    return ans;
}

void solve() {
    fstream fout; 
    fout.setf(ios::fixed);
    fout.precision(15);

    fout.open("submission.csv", ios::out); 

    fout << "id,x,y,z,Vx,Vy,Vz\n";
    
    int index = 0;

    for (int i = 0; i < data.size(); ++i){
        for (int j = 0; j < 8; ++j){
            if (j == 0){
                fout << int(0.5 + data[i][j]) << ',';
                continue;
            }

            if (j == 1){
                index = int(0.5 + data[i][j]);
                continue;
            }
            
            fout << evaluate(data[i][2], index, (j-2)*11);
            if (j == 7) fout << '\n';
            else fout << ',';
        }
    }

    fout.close();
}

int main(){
    init();
    //read_train();
    read_test();
    solve();
}