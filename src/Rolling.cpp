#include "Rolling.h"
#include "RollingAlgorithms.h"
#include <numeric>
#include <algorithm>

std::vector<double> Rolling::apply(std::function<double (const std::vector<double>&)> f) {
  auto ra = RollingApply(f);
  return this->run_algorithm(ra);
}

std::vector<double> Rolling::sum() {
  auto ra =  RollingSumOrMean();
  return this->run_algorithm(ra);
}

std::vector<double> Rolling::mean() {
  auto ra = RollingSumOrMean(RollingSumOrMean::Type::MEAN);
  return this->run_algorithm(ra);
}

std::vector<double> Rolling::priority(std::function<bool (double, double) > f){
  auto ra = RollingPriority(f);
  return this->run_algorithm(ra);
}

std::vector<double> Rolling::min(){
  auto ra = RollingPriority([](double d1, double d2){ return d1<d2;});
  return this->run_algorithm(ra);
}

std::vector<double> Rolling::max(){
  auto ra = RollingPriority([](double d1, double d2){ return d1>d2;});
  return this->run_algorithm(ra);
}

std::vector<double> Rolling::run_algorithm(IRbAlgorithm& alg) {
  size_t n = this->_v->size();
  size_t winsz = this->_window.length();
  alg.init(n, this->_window);

  std::vector<double> v = *(this->_v);
  std::vector<double> ret = std::vector<double>(n);

  int offs =0;
  switch(this->_window.type()){
  case RollWindow::Type::LEFT:
    offs = 0;
    break;
  case RollWindow::Type::CENTER:
    offs = -(int)winsz/2;
    break;
  case RollWindow::Type::RIGHT:
    offs = 1-(int)winsz;
    break;
  }
  auto step_type = IRbAlgorithm::StepType::STEP;
  using std::max;
  using std::min;
  auto clamp = [n](int x){return min(max(0,x),(int)n);};
  for(size_t i=0; i<n; i++){
    if(i==0){
      step_type = IRbAlgorithm::StepType::START;
    }
    else if(i==n-1){
      step_type = IRbAlgorithm::StepType::END;
    }
    else{
      step_type = IRbAlgorithm::StepType::STEP;
    }

    size_t b = clamp((int)i + offs);
    size_t e = clamp((int)i + (int)winsz + offs);
    // TODO: change to non-copying allocator
    ret[i]=alg.step(std::vector<double>(v.begin()+b,v.begin()+e), step_type);
    // move rolling window
    if(e<n){
      e++;
    }
  }


  return ret;
}

