#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
typedef vector <double> vd;
typedef vector <vd> vvd;

vvd data, M(600, vd(42));

void init(){
	fstream fin; 
	fin.open("betas.txt", ios::in); 

	for (int i = 0; i < 600; ++i){
		for (int j = 0; j < 6; ++j){
			for (int k = 0; k < 7; ++k){
				if (k > 0){
					char c;
					fin >> c;
				}

				double x;
				fin >> x;
				M[i][j*7 + k] = x;
			}
		}
	}

	fin.close();
}

void read(){
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

  			if (i == 1) continue;

			query.push_back(stod(info)); 
  		} 

        data.push_back(query);
    }

    fin.close();
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
    		
    		vd coefs;
    		for (int k = (j-2) * 7; k < (j-1) * 7; ++k) coefs.push_back(M[index][k]);
    		
    		double result = coefs.front();
    		for (int k = 1; k < 7; ++k) result += coefs[k] * data[i][k+1];

    		fout << result;
    		if (j == 7) fout << '\n';
    		else fout << ',';
    	}
    }

    fout.close();
}

int main(){
	init();
	read();
	solve();
}