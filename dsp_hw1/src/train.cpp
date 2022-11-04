#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "hmm.h"

using namespace std;

vector<vector<int>> load_data(string &train_file) {
    fstream f_train(train_file, ios::in);  // train_seq_01~05.txt
    string line;
    vector<vector<int>> training_data;
    while (f_train >> line && line != "") {
        vector<int> tmp;
        for (int i = 0; i < (int)line.length(); i++) {
            tmp.push_back(line[i] - 'A');
        }
        training_data.push_back(tmp);
    }
    f_train.close();
    return training_data;
}


void train(int iter, HMM *model, vector<vector<int>> &training_data) {
    const int n_seq = training_data.size(); // 10000 in train_seq_01~05.txt
	const int n_obs = training_data[0].size(); // 50 in train_seq_01~05.txt
	const int n_state = model->state_num; // 6
    cout << "=== Training Process ===\n" << endl;
    for (int it = 0; it < iter; it++) {
		cout << "\x1b[1A" << "\x1b[2K" << "Iteration " << it+1 << " / " << iter << endl;
        double all_gamma[n_obs][n_state] = {};
        double observe_gamma[n_state][n_state] = {};
        double state_gamma[n_state] = {};
        double all_epsilon[n_state][n_state] = {};

        for (int seq = 0; seq < n_seq; seq++) {
            double alpha[n_obs][n_state] = {};
            double beta[n_obs][n_state] = {};
            double gamma[n_obs][n_state] = {};
            double epsilon[n_obs-1][n_state][n_state] = {};

            // forward algorithm
            for (int i = 0; i < n_state; i++) {
				// alpha_0(i) = pi_i * b_i(o_0)
                int obs = training_data[seq][0]; // o_0
                alpha[0][i] = model->initial[i] * model->observation[obs][i];
            }
            for (int t = 0; t < n_obs-1; t++) {
				// alpha_t+1(j)=sum_i(alpha_t(i)*a_ij)*b_j(o_t+1)
                int obs = training_data[seq][t + 1]; // o_t+1
                for (int j = 0; j < n_state; j++) {
                    for (int i = 0; i < n_state; i++) {
                        alpha[t + 1][j] += alpha[t][i] * model->transition[i][j];
                    }
					alpha[t + 1][j] *= model->observation[obs][j];
                }
            }
            // backword algorithm
            for (int i = 0; i < n_state; i++) {
                beta[n_obs-1][i] = 1.0;
            }
            for (int t = n_obs-2; t >= 0; t--) {
				// beta_t(i)=sum_j(a_ij*b_j(o_t+1)*beta_t+1(j))
                int obs = training_data[seq][t + 1]; // o_t+1
                for (int i = 0; i < n_state; i++) {
                    for (int j = 0; j < n_state; j++) {
                        beta[t][i] += model->transition[i][j] * model->observation[obs][j] * beta[t + 1][j];
                    }
                }
            }
            // gamma_t(i)=alpha_t(i)*beta_t(i)/sum_j(alpha_t(j)*beta_t(j))=P(q_t=i|O,lambda)
            for (int t = 0; t < n_obs; t++) {
                double total = 0.0;
                for (int j = 0; j < n_state; j++) {
                    total += (alpha[t][j] * beta[t][j]);
                }
                int obs = training_data[seq][t]; // o_t
                for (int i = 0; i < n_state; i++) {
                    gamma[t][i] = (alpha[t][i] * beta[t][i]) / total;
					// gamma(i) = sum_t(gamma_t(i))
                    all_gamma[t][i] += gamma[t][i];
                    observe_gamma[obs][i] += gamma[t][i];
                }
            }
            // epsilon_t(i,j)=alpha_t(i)*a_ij*b_j(o_t+1)*beta_t+1(j)/sum_i,j(alpha_t(i)*a_ij*b_j(o_t+1)*beta_t+1(j))
            for (int t = 0; t < n_obs-1; t++) {
                double total = 0.0;
                int obs = training_data[seq][t + 1]; // o_t+1
                for (int i = 0; i < n_state; i++) {
                    for (int j = 0; j < n_state; j++) {
                        double val = alpha[t][i] * model->transition[i][j] * model->observation[obs][j] * beta[t + 1][j];
                        epsilon[t][i][j] = val;
                        total += val;
                    }
                }
                for (int i = 0; i < n_state; i++) {
                    for (int j = 0; j < n_state; j++) {
                        epsilon[t][i][j] /= total;
                    }
                }
            }
            // epsilon(i, j) = sum_T epsilon (t, i, j)
            for (int i = 0; i < n_state; i++) {
                for (int j = 0; j < n_state; j++) {
                    for (int t = 0; t < n_obs-1; t++) {
                        all_epsilon[i][j] += epsilon[t][i][j];
                    }
                }
            }
        }
        // update parameter
        for (int t = 0; t < n_obs-1; t++) {
            for (int i = 0; i < n_state; i++) {
                state_gamma[i] += all_gamma[t][i];
            }
        }
        // init probaility: pi_i = gamma_0(i)
        for (int i = 0; i < n_state; i++) {
            model->initial[i] = all_gamma[0][i] / (double)n_seq;
        }
        // transition probability: a[i][j]
        for (int i = 0; i < n_state; i++) {
            for (int j = 0; j < n_state; j++) {
                model->transition[i][j] = all_epsilon[i][j] / state_gamma[i];
            }
        }
        // observation probability: b[i][obs]
        for (int i = 0; i < n_state; i++) {
            for (int obs = 0; obs < n_state; obs++) {
                model->observation[obs][i] = observe_gamma[obs][i] / state_gamma[i];
            }
        }
    }
    cout << "==== Training done. ====" << endl;
}


int main(int argc, char *argv[]) {
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " <training_data> <test_data> <n_state> <n_iter>" << endl;
        exit(1);
    }
    int n_iter = atoi(argv[1]);
    string init_file(argv[2]), data_file(argv[3]), output_file(argv[4]);

    cout << "init_file: " << init_file << "\ndata_file: " << data_file << endl;
    cout << "output_file: " << output_file << "\nn_iter: " << n_iter << endl;
    // load data and model
    HMM model;
    vector<vector<int>> training_data = load_data(data_file);
    loadHMM(&model, init_file.c_str());
    // train model
    train(n_iter, &model, training_data);

    // save model
    FILE *fp = fopen(output_file.c_str(), "w");
    if(!fp) {
        cerr << "Error: cannot open file " << output_file << endl;
        exit(1);
    }
    dumpHMM(fp, &model);
    return 0;
}