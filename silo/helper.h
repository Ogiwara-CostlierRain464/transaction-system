#ifndef TRANSACTIONSYSTEM_HELPER_H
#define TRANSACTIONSYSTEM_HELPER_H

#include <vector>

void sleepMs(int ms);

void waitForReady(const std::vector<char> &readyFlags);

#endif //TRANSACTIONSYSTEM_HELPER_H
