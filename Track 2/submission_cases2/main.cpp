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
int N = 48; // number of coefs
vvd data, M(600, vd(6*N)), T(600, vd(6)); //coefs and periods
vector<vector<int>> cases(600, vector<int>(6)); // The models

bool linear(int n);
int armonics(int n);
int type(int n);

int num_coef(int arm, int num_model){
    if (num_model == 1) return 8+arm*4;
    if (num_model == 2) return 2+arm*4;
    if (num_model == 3) return max(10+arm*2, arm*4)+6;
    if (num_model == 4) return 8+arm*2;
    return 2+arm*2;
}

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

        for (int j = 0; j < 6; ++j){
            fin2 >> T[i][j];

            if (j == 5) getline(fin3, info);
            else getline(fin3, info, ',');
            cases[i][j] = stoi(info);

            int arm, num_model, num_coefs;
            if (not linear(cases[i][j])) {
                arm = armonics(cases[i][j]);
                num_model = type(cases[i][j]);
                num_coefs = num_coef(arm, num_model);
            } else {
                num_coefs = 7;
            }
    
            for (int k = 0; k < num_coefs; ++k){
                if (k > 0) {
                    fin >> c;
            
                }
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

bool linear(int n) {
    return n == 6 or n == 47;
}

int armonics(int n) {
    if ((n >= 7 and n <= 11) or (n >= 48 and n <= 52) or 
        (n >= 88 and n <= 92) or (n >= 128 and n <= 132)) return 3;
    if ((n >= 12 and n <= 16) or (n >= 53 and n <= 57) or 
        (n >= 93 and n <= 97) or (n >= 133 and n <= 137)) return 4;
    if ((n >= 1 and n <= 5) or (n >= 17 and n <= 21) or (n >= 58 and n <= 62) or 
        (n >= 98 and n <= 102) or (n >= 138 and n <= 142)) return 5;
    if ((n >= 22 and n <= 26) or (n >= 63 and n <= 67) or 
        (n >= 103 and n <= 107) or (n >= 143 and n <= 147)) return 6;
    if ((n >= 27 and n <= 31) or (n >= 68 and n <= 72) or 
        (n >= 108 and n <= 112) or (n >= 148 and n <= 152)) return 7;
    if ((n >= 32 and n <= 36) or (n >= 73 and n <= 77) or 
        (n >= 113 and n <= 117) or (n >= 153 and n <= 157)) return 8;
    if ((n >= 37 and n <= 41) or (n >= 78 and n <= 82) or 
        (n >= 118 and n <= 122) or (n >= 158 and n <= 162)) return 9;
    return 10;
}

int type (int n) {
    if (n <= 5) return n;
    if (n < 48) {
        int last_d = (n % 10);
        if (last_d >= 2 and last_d <= 6) return last_d - 1;
        return (last_d + 4) % 10;
    } 
    int last_d = (n % 10);
    if (last_d >= 3 and last_d <= 7) return last_d - 2;
    return (last_d + 3) % 10;
}

double evaluate(double t, double x, int i, int index, int k, int num_model){
    double ans = 0;
    if (not linear(num_model)) ans += M[index][k] + M[index][k+1] * x; //intercept + epoch 
    else {
        ans += M[index][k];
        for (int j = 1; j < 7; ++j)  // linear model
            ans += M[index][k+j] * data[i][j+2]; 
        return ans;
    }

    int arm = armonics(num_model);
    num_model = type(num_model);
    for (int j = 2; j < 2+arm*2; j += 2) { // sin and cos
        ans += M[index][k+j] * sin(pi*x*j/t) + M[index][k+j+1] * cos(pi*x*j/t);
    }

    if (num_model == 1 or num_model == 3 or num_model == 4 or num_model == 6)
        for (int j = 2+arm*2; j < 8+arm*2; ++j)  // linear model
            ans += M[index][k+j] * data[i][j-(-1+arm*2)];

    if (num_model == 1) 
        for (int j = 8+arm*2; j < 8+arm*4; j += 2)  // epoch*sin and epoch * cos
            ans += M[index][k+j] * x * sin(pi*x*(j-(6+arm*2))/t) + M[index][k+j+1] * x * cos(pi*x*(j-(6+arm*2))/t);

    if (num_model == 2) 
        for (int j = 2+arm*2; j < 2+arm*4; j += 2)  // epoch*sin and epoch * cos
            ans += M[index][k+j] * x * sin(pi*x*(j-(arm*2))/t) + M[index][k+j+1] * x * cos(pi*x*(j-(arm*2))/t);

    if (num_model == 3) {
        for (int j = 8+arm*2; j < 10+arm*2; j += 2)  // epoch*sin and epoch * cos
            ans += M[index][k+j] * x * sin(pi*x*(j-(6+arm*2))/t) + M[index][k+j+1] * x * cos(pi*x*(j-(6+arm*2))/t);
        for (int j = 10+arm*2; j < arm*4; j += 2)  // epoch*sin and epoch * cos
            ans += M[index][k+j] * x * sin(pi*x*(j-(-2+arm*2))/t) + M[index][k+j+1] * x * cos(pi*x*(j-(-2+arm*2))/t);
        for (int j = max(10+arm*2, arm*4); j < max(10+arm*2, arm*4)+6; ++j) // epoch * pos and epoch * vel
            ans += M[index][k+j] * x * data[i][j-(max(10+arm*2, arm*4)-3)];     
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