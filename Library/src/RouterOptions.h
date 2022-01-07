#ifndef ROUTEROPTIONS_H_INCLUDED
#define ROUTEROPTIONS_H_INCLUDED

#include <vector>
#include <string>

struct RouterOptions {
    std::vector<std::string> databasePath{};

    uint32_t closestRoutableNodeRadius = 1000; // meters
};

#endif // ROUTEROPTIONS_H_INCLUDED
