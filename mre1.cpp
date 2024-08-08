// Working version
#include "opencv2/opencv.hpp"
#include <iostream>
#include <optional>

struct MyClass1 {
  MyClass1(cv::InputOutputArray img, cv::Point origin) : _img(img), _origin(origin) {}
  ~MyClass1() {
    std::cout << "Destroying MyClass1: img ok? " << !_img.empty() << std::endl;
    if(!_img.empty()) {
      cv::circle(_img, _origin, _radius, cv::Scalar(0, 0, 255), 3);
    }
  }
  cv::InputOutputArray _img;
  const cv::Point _origin;
  int _radius = 10;
};

static inline MyClass1 myClass1(cv::InputOutputArray img, cv::Point origin) {
  return MyClass1(img, origin);
}

#define PLINE() std::cout << __LINE__ << " "

int main(){
  std::cout << "GNU C++ version: " << __GNUG__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << std::endl;
  std::cout << "OpenCV version: " << CV_VERSION << std::endl;
  cv::Mat img(300, 300, CV_8UC3, cv::Scalar(255, 255, 255));
  cv::Point origin(150, 150);
  PLINE(); myClass1(img, origin); // OK; but no storing as variable
  // PLINE(); { auto fmt = myClass1(img, origin); fmt._radius = __LINE__; } // SegFault: unknown operation 'empty'
  // { auto& fmt = myClass1(img, origin); fmt._radius = __LINE__; } // Not Allowed, ofc: ref to temporary
  PLINE(); { auto&& fmt = myClass1(img, origin); fmt._radius = __LINE__; } // OK, note && within sub-block
  PLINE(); { auto&& fmt = myClass1(cv::InputOutputArray(img), origin); fmt._radius = __LINE__; } // OK...
  auto ioa = cv::InputOutputArray(img);
  PLINE(); { auto fmt = myClass1(ioa, origin); fmt._radius = __LINE__; } // OK, but cumbersome
  //PLINE(); do { auto fmt = myClass1(img, origin); fmt._radius = __LINE__; } while(0); // SegFault: unknown operation 'empty'
  PLINE(); do { auto&& fmt = myClass1(img, origin); fmt._radius = __LINE__; } while(0); // OK, same as above
  cv::imwrite("build/mre_t1.png", img);
  return 0;
}

