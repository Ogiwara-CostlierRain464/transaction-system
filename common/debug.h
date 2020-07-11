#ifndef TRANSACTIONSYSTEM_DEBUG_H
#define TRANSACTIONSYSTEM_DEBUG_H

#define ERR         \
do {                \
  perror("ERROR");  \
  exit(1);          \
} while(0)

#endif //TRANSACTIONSYSTEM_DEBUG_H
