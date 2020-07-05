#ifndef TRANSACTIONSYSTEM_ZIP_FIAN_H
#define TRANSACTIONSYSTEM_ZIP_FIAN_H

#include "xoroshiro128_plus.h"
#include <cassert>

static double zeta_(size_t nr, double theta){
  double ans = 0.0;
  for(size_t i = 0; i < nr; i++){
    ans += std::pow(1.0 / (double)(i+1), theta);
  }
  return ans;
}

/**
 * Zipfian distribution is introduced by
 * Jim Gray et al.
 *
 * from Abstract:
 * Evaluating database system performance often requires generating synthetic databases
 * – ones having certain statistical properties but filled with dummy information.
 * When evaluating different database designs, it is often necessary to generate
 * several databases and evaluate each design. As database sizes grow to terabytes,
 * generation often takes longer than evaluation.
 * This paper presents several database generation techniques.
 *
 * @see https://jimgray.azurewebsites.net/papers/SyntheticDataGen.pdf
 */
struct FastZipf{
  Xoroshiro128Plus *rand;
  const size_t nr;
  const double alpha, zeta, eta;
  const double threshold;

  FastZipf(Xoroshiro128Plus *rnd, double theta, size_t nr)
    : rand(rnd)
    , nr(nr)
    , alpha(1.0 / (1.0 - theta))
    , zeta(zeta_(nr, theta))
    , eta(
      (1.0 - std::pow(2.0 / (double)nr, 1.0 - theta)) /
      (1.0 - zeta_(2, theta) / zeta))
    , threshold(1.0 + std::pow(0.5, theta))
  {
    assert(0.0 <= theta and theta < 1.0);
  }

  uint64_t randNext(){ return rand->next(); }

  inline size_t operator()(){
    double u = rand->next() / (double) UINT64_MAX;
    double uz = u * zeta;
    if(uz < 1.0) return 0;
    if(uz < threshold) return 1;
    return (size_t)
      ((double) nr * std::pow(eta * u - eta + 1.0, alpha));
  }

};

#endif //TRANSACTIONSYSTEM_ZIP_FIAN_H
