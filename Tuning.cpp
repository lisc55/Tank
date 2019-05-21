#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

int main() {
    FILE *RES = fopen("res", "w");
    vector<double> C_LIST = {0.2, 0.4, 0.6, 0.8, 1.0};
    map<double, int> final_res;
    for (auto i : C_LIST) {
        for (auto j : C_LIST) {
            if (fabs(i - j) > 1e-5) {
                string cmd = string("./Play ") + to_string(i) + string(" ") + to_string(j);
                int T = 5;
                for (int _ = 1; _ <= 5; _++) {
                    printf("%.3lf %.3lf round %d ", i, j, _);
                    int res = system(cmd.c_str());
                    if (res == 1) {
                        final_res[i] += 2;
                        printf("Result = Blue\n", i, j);
                    } else if (res == 2) {
                        final_res[j] += 2;
                        printf("Result = Red\n", i, j);
                    } else if (res == 0) {
                        final_res[i]++, final_res[j]++;
                        printf("Result = Draw\n", i, j);
                    }
                }
            }
        }
    }
    for (auto i : final_res) {
        fprintf(RES, "%.3lf %d\n", i.first, i.second);
        printf("%.3lf %d\n", i.first, i.second);
    }
    puts("FINISHED");
    return 0;
}
