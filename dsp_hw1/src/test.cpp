#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "hmm.h"

using namespace std;

vector<vector<int>> load_data(string &test_file) {
    fstream f_test;
    f_test.open(test_file, ios::in);  // test_seq_01~05.txt
    string line;
	vector<vector<int>> testing_data;
    while (f_test >> line && line != "") {
        vector<int> tmp;
        for (int i = 0; i < (int)line.length(); i++) {
            tmp.push_back(line[i] - 'A');
        }
        testing_data.push_back(tmp);
    }
    f_test.close();
	return testing_data;
}


vector<int> ans_arr;
vector<double> likelihood;


void test(HMM models[], vector<vector<int>> &testing_data){
	const int n_seq = testing_data.size(); // 2500 in test_seq_01~05.txt
	const int n_obs = testing_data[0].size(); // 50 in test_seq_01~05.txt
	const int n_state = models[0].state_num; // 6
	for(int l = 0; l < n_seq; l++){
		int ans = 0;
		double max_p = 0.0;
		for (int m = 0; m < 5; m++){	// model num
			// viterbi algo.
			double delta[n_obs][n_state] = {};
			// init
			int obs = testing_data[l][0];
			for (int i = 0; i < n_state; i++){
				delta[0][i] = models[m].initial[i] * models[m].observation[obs][i];
			}
			// dynamic programming
			for (int t = 1; t < n_obs; t++){
				obs = testing_data[l][t];
				for (int j = 0; j < n_state; j++){
					double max_val = 0.0;
					for (int i = 0; i < n_state; i++){
						double val = delta[t-1][i] * models[m].transition[i][j];
						if (val > max_val){
							max_val = val;
						}
					}
					delta[t][j] = max_val * models[m].observation[obs][j];
				}
			}
			double p_star = 0.0;
			for (int i = 0; i < n_state; i++){
				if (delta[n_obs-1][i] > p_star){
					p_star = delta[n_obs-1][i];
				}
			}
			if (p_star > max_p){
				max_p = p_star;
				ans = m;
			}
		}
		ans_arr[l] = ans+1;
		likelihood[l] = max_p;
	}
}

int main(int argc, char *argv[]){
	if (argc != 4){
		fprintf(stderr, "Usage: ./test_hmm [model_list file] [testing_file] [result file]\n");
		exit(1);
	}
    string model_list_file(argv[1]), testing_file(argv[2]), result_file(argv[3]);

	HMM hmms[5];
    vector<vector<int>> testing_data = load_data(testing_file);
	load_models(model_list_file.c_str(), hmms, 5);

	// predict process
	const int n_seq = testing_data.size(); // 2500 in test_seq_01~05.txt
	ans_arr.resize(n_seq), likelihood.resize(n_seq);
	test(hmms, testing_data);
	
	// dump to output file
	fstream result_f;
	result_f.open(result_file, ios::out);
	if ((!result_f.is_open())){
		fprintf(stderr, "Can not open file [%s] !\n", result_file);
	}
	for (int i = 0; i < n_seq; i++){
		result_f << "model_0" << ans_arr[i] << ".txt " << likelihood[i] << endl;
	}
	return 0;
}