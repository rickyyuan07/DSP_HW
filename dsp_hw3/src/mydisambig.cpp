#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "Ngram.h"

const int MAXWORD = 3000;
const int MAXLEN = 100;
const double INF = 1e9;

using namespace std;

double getBigramProb(const char *w1, const char *w2, Ngram &lm, Vocab &voc) {
    VocabIndex wid1 = voc.getIndex(w1);
    VocabIndex wid2 = voc.getIndex(w2);

    if (wid1 == Vocab_None) // replace OOV with <unk>
        wid1 = voc.getIndex(Vocab_Unknown);
    if (wid2 == Vocab_None) // replace OOV with <unk>
        wid2 = voc.getIndex(Vocab_Unknown);

    VocabIndex context[] = {wid1, Vocab_None};
    return lm.wordProb(wid2, context);
}

int main(int argc, char *argv[]) {
    if (argc < 9) {
        cerr << "Usage: ./mydisambig -text testdata/$$i.txt -map ./ZhuYin-Big5.map -lm $LM -order $order" << endl;
    }
    int ngram_order = 0;
    string text_fname, map_fname, lm_fname;
    for (int i = 0; i < argc-1; i++) {
        if ("-text" == string(argv[i]))
            text_fname = argv[i + 1];
        if ("-map" == string(argv[i]))
            map_fname = argv[i + 1];
        if ("-lm" == string(argv[i]))
            lm_fname = argv[i + 1];
        if ("-order" == string(argv[i]))
            ngram_order = atoi(argv[i + 1]);
    }
    Vocab voc;
    Ngram lm(voc, ngram_order);
    string tmp_line;
    
    // load lm
    File lmFile(lm_fname.c_str(), "r");
    lm.read(lmFile);
    lmFile.close();

    // load mapping from ZhuYin to Big5
    map<string, vector<string>> mapping;
    ifstream mapfile(map_fname);
    while (getline(mapfile, tmp_line)) {
        string zhuyin(tmp_line.begin(), tmp_line.begin()+2), word;
        int slen = tmp_line.size();
        for (int i = 3; i < slen; i += 3) {
            word.assign(tmp_line.begin() + i, tmp_line.begin() + i + 2); // get each character
            mapping[zhuyin].push_back(word);
        }
    }
    mapfile.close();

    // load input text file
    vector<string> texts;
    ifstream textfile(text_fname);
    while (getline(textfile, tmp_line)) {
        texts.push_back(tmp_line);
    }
    textfile.close();

    // run viterbi algorithm throughout the sequences and output the result
    for (string tmp_line : texts) {
        tmp_line.erase(remove(tmp_line.begin(), tmp_line.end(), ' '), tmp_line.end()); // erase space
        int sent_len = tmp_line.size() / 2; // big5 is 2 bytes
        double delta[MAXLEN][MAXWORD] = {}; // log prob.
        int path[MAXLEN][MAXWORD] = {}; // backtracking idx table
        string zhuyin(tmp_line.begin(), tmp_line.begin() + 2);

        // initialize
        int N = mapping[zhuyin].size();
        for (int i = 0; i < N; i++) {
            delta[0][i] = getBigramProb(Vocab_SentStart, mapping[zhuyin][i].c_str(), lm, voc);
        }
        string prev_zhuyin = zhuyin;
        int preN = N;
        // dynamic programming
        for (int t = 1; t < sent_len; t++) {
            zhuyin.assign(tmp_line.begin() + 2*t, tmp_line.begin() + 2*t + 2);
            // O(n^2) time complexity find the best path
            int M = mapping[zhuyin].size();
            for (int j = 0; j < M; j++) {
                double best_prob = -INF;
                int best_idx = 0;
                for (int i = 0; i < preN; i++) {
                    double cur_prob = getBigramProb(mapping[prev_zhuyin][i].c_str(), mapping[zhuyin][j].c_str(), lm, voc) + delta[t-1][i];
                    if (cur_prob > best_prob) {
                        best_prob = cur_prob;
                        best_idx = i;
                    }
                }
                delta[t][j] = best_prob, path[t][j] = best_idx;
            }
            preN = M, prev_zhuyin = zhuyin;
        }
        // EOS idx
        double best_prob = -INF;
        int best_idx = 0;
        for (int i = 0; i < preN; i++) {
            double cur_prob = getBigramProb(mapping[prev_zhuyin][i].c_str(), Vocab_SentEnd, lm, voc) + delta[sent_len-1][i];
            if (cur_prob > best_prob) {
                best_prob = cur_prob;
                best_idx = i;
            }
        }
        // backtracking
        vector<string> sentence;
        for (int t = sent_len - 1; t >= 0; t--) {
            zhuyin.assign(tmp_line.begin() + 2 * t, tmp_line.begin() + 2 * t + 2);
            sentence.push_back(mapping[zhuyin][best_idx]);
            if (t > 0) {
                best_idx = path[t][best_idx];
            }
        }

        // output sentence
        reverse(sentence.begin(), sentence.end());
        cout << "<s> ";
        for (string tok : sentence) {
            cout << tok << ' ';
        }
        cout << "</s>" << endl;
    }
}