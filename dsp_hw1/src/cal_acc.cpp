#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
    if(argc != 3) {
        cout << "Usage: " << argv[0] << " <result_file> <gt_file>" << endl;
        return 0;
    }
    string result(argv[1]), gt(argv[2]);
    fstream result_f, gt_f;
    result_f.open(result, ios::in), gt_f.open(gt, ios::in);
    if(!result_f.is_open() || !gt_f.is_open()) {
        cout << "Can not open file!" << endl;
        exit(1);
    }
    int correct = 0, total = 0;
    string result_line, gt_line;
    double _;
    while(result_f >> result_line >> _ && gt_f >> gt_line) {
        if(result_line == gt_line) {
            correct++;
        }
        total++;
    }
    cout << "Accuracy: " << (double)correct / total << endl;
    return 0;
}