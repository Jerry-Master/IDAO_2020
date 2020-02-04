#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <cmath>
#include <time.h>
#include <cassert>

using namespace std;
typedef vector <double> vd;
typedef vector <vd> vvd;

const double pi = acos(-1);
double initial_time;
struct tm time1970 = {0};
int N = 28; // number of coefs
vvd data, M(600, vd(6*N)), T(600, vd(6)); //coefs and periods
vector<vector<int>> cases(600, vector<int>(6)); // The models
vector<int> num_coef = {28, 22, 26, 18, 12};

void init(){
    fstream fin, fin2, fin3; 
    fin.open("betas.txt", ios::in); 
    fin2.open("periods.txt", ios::in);
    fin3.open("cases.txt", ios::in);

    fin >> initial_time;
    initial_time -= 86400;

    char c;
    int sat_id;
    string info;
    for (int i = 0; i < 600; ++i){
        getline(fin3, info, ',');
        sat_id = stoi(info);
        assert(sat_id == i);
        for (int j = 0; j < 6; ++j){
            fin2 >> T[i][j];
            if (j == 5) getline(fin3, info);
            else getline(fin3, info, ',');
            cases[i][j] = stoi(info);

            for (int k = 0; k < num_coef[cases[i][j]-1]; ++k){
                if (k > 0) fin >> c;
                fin >> M[i][j*N + k];
            }
        }
    }
    fin.close();
    fin2.close();
    fin3.close();
/*
    for (int i = 0; i < 6; ++i) cout << T[550][i] << " ";
    cout << endl; 
    for (int i = 0; i < N; ++i) cout << M[550][i] << " ";
    cout << endl;
*/
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

    while (getline(fin, info, ',')){ 
        query = {stod(info)};

        for (int i = 0; i < 14; ++i){
            if (i == 13) getline(fin, info);
            else getline(fin, info, ',');

            if (i >= 2 and i <= 7) continue;

            if (i == 0) query.push_back(time(info));
            else query.push_back(stod(info)); 
        } 
        swap(query[1], query[2]);
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

            if (i == 1) query.push_back(time(info));
            else query.push_back(stod(info)); 
        } 

        data.push_back(query);
    }

    fin.close();
} 

double evaluate(double t, double x, int i, int index, int k, int num_model){
    double ans = M[index][k] + M[index][k+1] * x; //intercept + epoch 
    
    for (int j = 2; j < 12; j += 2) { // sin and cos
        ans += M[index][k+j] * sin(pi*x*j/t) + M[index][k+j+1] * cos(pi*x*j/t);
    }

    if (num_model == 1 or num_model == 3 or num_model == 4)
        for (int j = 12; j < 18; ++j)  // linear model
            ans += M[index][k+j] * data[i][j-9];

    if (num_model == 1) 
        for (int j = 18; j < 28; j += 2)  // epoch*sin and epoch * cos
            ans += M[index][k+j] * x * sin(pi*x*(j-16)/t) + M[index][k+j+1] * x * cos(pi*x*(j-16)/t);

    if (num_model == 2) 
        for (int j = 12; j < 22; j += 2)  // epoch*sin and epoch * cos
            ans += M[index][k+j] * x * sin(pi*x*(j-10)/t) + M[index][k+j+1] * x * cos(pi*x*(j-10)/t);

    if (num_model == 3) {
        for (int j = 18; j < 20; j += 2)  // epoch*sin and epoch * cos
            ans += M[index][k+j] * x * sin(pi*x*(j-16)/t) + M[index][k+j+1] * x * cos(pi*x*(j-16)/t);
        for (int j = 20; j < 26; ++j) // epoch * pos and epoch * vel
            ans += M[index][k+j] * x * data[i][j-17];     
    }

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
            
            fout << evaluate(T[index][j-2], data[i][2], i, index, (j-2)*N, cases[index][j-2]);
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