#include <string>
#include <iostream>
#include "pb2json.h"

using namespace std;

int main(int argc, const char *argv[]) {
    const char* path = "/data/home/wenhuijia/project/src/RecSys/";
    const char* file = "NewApp/Proto/Profile.proto";
    int ret = init(path, file);
    if (ret != 0) {
        return -1;
    }
    char* data;
    int len;
    dynamic_encode("omg.new_app.GSessionItem", &data, &len);

    char* output;
    int output_len;
    dynamic_decode("omg.new_app.GSessionItem", data, len, &output, &output_len);
    cout << "output: " << output << endl;

    delete data;
    delete output;
    uninit();
}
