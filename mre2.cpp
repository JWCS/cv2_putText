#include "opencv2/opencv.hpp"
#include <iostream>
#include <optional>

struct MyClass2 {

  MyClass2(cv::InputOutputArray img, cv::Point origin) : _img(img), _origin(origin) {}
  ~MyClass2() {
    std::cout << "Destroying MyClass2: img ok? " << !_img.empty() << std::endl;
    if(!_img.empty()) {
      const int _radius = _radius_opt.value_or(10); // CHANGE
      cv::circle(_img, _origin, _radius, cv::Scalar(0, 0, 255), 3);
    }
  }

  cv::InputOutputArray _img;
  const cv::Point _origin;
  std::optional<int> _radius_opt = std::nullopt; // CHANGE
};

static inline MyClass2 myClass2(cv::InputOutputArray img, cv::Point origin) {
  return MyClass2(img, origin);
}

#define PLINE() std::cout << __LINE__ << " "

int main(){
  std::cout << "GNU C++ version: " << __GNUG__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << std::endl;
  std::cout << "OpenCV version: " << CV_VERSION << std::endl;
  cv::Mat img(300, 300, CV_8UC3, cv::Scalar(255, 255, 255));
  cv::Point origin(150, 150);
  // Still working:
  PLINE(); myClass2(img, origin);
  auto ioa = cv::InputOutputArray(img);
  PLINE(); { auto fmt = myClass2(ioa, origin); fmt._radius_opt = __LINE__; } // OK, but cumbersome
  // BROKEN:
  PLINE(); { auto&& fmt = myClass2(img, origin); fmt._radius_opt = __LINE__; } // && within sub-block
  PLINE(); { auto&& fmt = myClass2(cv::InputOutputArray(img), origin); fmt._radius_opt = __LINE__; } // Uh...
  PLINE(); do { auto&& fmt = myClass2(img, origin); fmt._radius_opt = __LINE__; } while(0); // same as above
  cv::imwrite("build/mre_t2.png", img);
  return 0;
}

