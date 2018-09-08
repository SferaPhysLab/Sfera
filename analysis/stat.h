#ifndef stat_h
#define stat_h
//stat.h
#include <stdlib.h>
#include <iostream>
#include <vector>

class Statistics {
public:
  Statistics();
  Statistics(const double& mean, const double& stdDev, const double& meanError);
  Statistics(const std::vector<double> data);
  Statistics(const int& size, const double& sum, const double& sum2);

  double getMean();
  double getMeanError();
  double getStdDev();
 
  private:
  double mean_;
  double stdDev_;
  double meanError_;
};
#endif
