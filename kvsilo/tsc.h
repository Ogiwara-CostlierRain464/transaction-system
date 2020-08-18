#ifndef TRANSACTIONSYSTEM_TSC_H
#define TRANSACTIONSYSTEM_TSC_H

#include <cstdint>

/**
 * Time Stamp Counter
 *
 * The Time Stamp Counter (TSC) is a 64-bit register
 * present on all x86 processors since the Pentium.
 * It counts the number of cycles since reset.
 * The instruction RDTSC returns the TSC in EDX:EAX.
 *
 * @see https://en.wikipedia.org/wiki/Time_Stamp_Counter
 */


/**
 * Read Time-Stamp Counter and processor ID
 */
static uint64_t rdtscp(){
  uint64_t rax;
  uint64_t rdx;
  uint32_t aux;
  asm volatile("rdtscp"
  : "=a" (rax), "=d" (rdx), "=c"(aux)
  ::);

  return (rdx << 32) | rax;
}

#endif //TRANSACTIONSYSTEM_TSC_H
