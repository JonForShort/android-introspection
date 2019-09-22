#ifndef ANDROID_INTROSPECTION_UTILS_H_
#define ANDROID_INTROSPECTION_UTILS_H_

#include <fstream>

using namespace std;

namespace ai::utils {

    auto fileExists(const char *path) -> bool {
        ifstream infile(path);
        return infile.good();
    }
}

#endif /* ANDROID_INTROSPECTION_UTILS_H_ */
