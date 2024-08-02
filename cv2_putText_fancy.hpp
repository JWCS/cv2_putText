/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                          License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Copyright (C) 2013, OpenCV Foundation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#ifndef __CV2_PUTTEXT_FANCY_HPP__
#define __CV2_PUTTEXT_FANCY_HPP__

// cv::putText() with std::cout-like formatting
// See the inline definitions for api.
// Note: there's 2 definitions for every "helper" inline putText method:
// one for starting the stream, for a given image/origin, and another
// for changing the formatting on the fly, to the same target image/orgin.
// If you don't need "fancy features", you can use the included basic
// cv2_putText.hpp putText() function(s), which has same signature.
//
// API DIFFERENCE:
// The original cv::putText(cv::Point origin) is the origin of the LOWER-left
// corner of the text. Ex, the corner of 'L', the lower-left curve of 'q'.
// For precise positioning, this is too tricky. This version uses the UPPER-left
// corner: the upper-left corner of 'R'/'F'. This is therefore invariant to font size.
// You can revert this behavior by setting bottomLeftOrigin=true (although the original
// use case was to mirror the text about origin... which is not supported here).
//
// Fancy features (over basic): outline/shadow, background mask
//
// This is an improvement over the adjacent cv2_putText.hpp, by Eugene Khvedchenya,
// adding "fancy" logic from YoniChechik's python version:
// https://stackoverflow.com/a/73471951/
// https://creativecommons.org/licenses/by-sa/4.0/
//
// This is a single-file header. To use it, before including it in ONE
// cpp file, define CV2_PUTTEXT_FANCY_HPP_IMPL. This will include the implementation.

/* Example:
*/

#include <opencv2/core.hpp>
#include <optional>

#if defined(CV2_PUTTEXT_FANCY_HPP_IMPL) && !defined(CV2_PUTTEXT_HPP_IMPL)
#define CV2_PUTTEXT_HPP_IMPL
#endif
#include "cv2_putText.hpp"

namespace fancy {

    static const cv::Scalar kBlack = cv::Scalar::all(0);
    static const cv::Scalar kWhite = cv::Scalar::all(255);
    static const cv::Scalar kGrey = cv::Scalar::all(200);
    static const cv::Scalar kShadow = cv::Scalar::all(50);
    static const cv::Scalar kRed = cv::Scalar(0, 0, 255);
    static const cv::Scalar kBlue = cv::Scalar(255, 0, 0);
    static const cv::Scalar kGreen = cv::Scalar(0x49, 0xB1, 0x54);

} // namespace fancy

namespace cv {

#define CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X \
  X(std::optional<Scalar>, outlineColor, std::nullopt) \
  X(int, outlineThickness, 4) \
  X(bool, shadow, false) \
  X(std::optional<Scalar>, bgColor, std::nullopt)

//! Creates and return image_ostream_fancy object to render text on the image like the std::cout does.
//! An image_ostream_fancy class supports operator<< for both primitive and opencv types.
class CV_EXPORTS image_ostream_fancy : public cv::image_ostream
{
public:
    image_ostream_fancy(
        InputOutputArray img, Point origin,
#define X(type, name, default_val) type name = default_val,
        CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X
        CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
#undef X
        void*_=0);

    //! Copy constructor is necessary because compiler cannot perform RVO in putText() function in some cases. To deal with it we introduce copy
    //! constructor which copies image_ostream_fancy object and it's internal string buffer.
    image_ostream_fancy(const image_ostream_fancy&);
    image_ostream_fancy(const image_ostream&);

    //! Prints everything to the cv::Mat in the destructor
    ~image_ostream_fancy();

    //! Size results (only after drawing/destruction!)
    // Note: can't use movable references, and std::reference_wrapper too buggy
    image_ostream_fancy& setTextSizeResult (cv::Size* const pSize){ _pTextSize = pSize; return *this; }
    image_ostream_fancy& setLineSizesResult(std::vector<cv::Size>* const pSizes){ _pLineSizes = pSizes; return *this; }

    //! Self operator<< to chain multiple settings
    image_ostream_fancy& operator<<(const image_ostream_fancy& new_settings);
    image_ostream_fancy& operator<<(const image_ostream& new_settings);

    //! Defalt operator<< to take everything
    template <typename T>
    image_ostream_fancy& operator<<(const T& x)
    {
        _str << x;
        return *this;
    }

    //! Define an operator<< to take in std::endl and other manipulators
    inline image_ostream_fancy& operator<<(ManipType manip)
    {
        manip(_str);
        return *this;
    }

protected:
    void nextLine();

protected:
#define X(type, name, default_val) type _##name;
    CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X
#undef X
};

/*
static inline image_ostream_fancy operator<<(const image_ostream& lhs, const image_ostream_fancy& rhs)
{
    image_ostream_fancy fancy_settings(lhs);
    fancy_settings << rhs;
    return fancy_settings;
}
*/

static inline image_ostream_fancy putTextOutline(
    cv::InputOutputArray img, cv::Point origin,
    cv::Scalar color = fancy::kWhite, int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    cv::Scalar outlineColor = fancy::kBlack, int outlineThickness = 4,
    int fontFace = cv::FONT_HERSHEY_SIMPLEX)
{
    return image_ostream_fancy(img, origin, outlineColor, outlineThickness, false, std::nullopt, color, thickness, fontScale, lineSpacing, fontFace);
}

static inline image_ostream_fancy putTextOutline(
    cv::Scalar color = fancy::kWhite, int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    cv::Scalar outlineColor = fancy::kBlack, int outlineThickness = 4,
    int fontFace = cv::FONT_HERSHEY_SIMPLEX)
{
    return putTextOutline(Mat(), Point(0,0), color, thickness, fontScale, lineSpacing, outlineColor, outlineThickness, fontFace);
}

static inline image_ostream_fancy putTextShadow(
    cv::InputOutputArray img, cv::Point origin,
    cv::Scalar color = fancy::kWhite, int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    cv::Scalar outlineColor = fancy::kShadow, int outlineThickness = 4,
    int fontFace = cv::FONT_HERSHEY_SIMPLEX)
{
    return image_ostream_fancy(img, origin, outlineColor, outlineThickness, true, std::nullopt, color, thickness, fontScale, lineSpacing, fontFace);
}

static inline image_ostream_fancy putTextShadow(
    cv::Scalar color = fancy::kWhite, int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    cv::Scalar outlineColor = fancy::kShadow, int outlineThickness = 4,
    int fontFace = cv::FONT_HERSHEY_SIMPLEX)
{
    return putTextShadow(Mat(), Point(0,0), color, thickness, fontScale, lineSpacing, outlineColor, outlineThickness, fontFace);
}

static inline image_ostream_fancy putTextBackground(
    cv::InputOutputArray img, cv::Point origin,
    cv::Scalar color = fancy::kBlack, cv::Scalar bgColor = fancy::kWhite,
    int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    int fontFace = cv::FONT_HERSHEY_SIMPLEX)
{
    return image_ostream_fancy(img, origin, std::nullopt, 0, false, bgColor, color, thickness, fontScale, lineSpacing, fontFace);
}

static inline image_ostream_fancy putTextBackground(
    cv::Scalar color = fancy::kBlack, cv::Scalar bgColor = fancy::kWhite,
    int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    int fontFace = cv::FONT_HERSHEY_SIMPLEX)
{
    return putTextBackground(Mat(), Point(0,0), color, bgColor, thickness, fontScale, lineSpacing, fontFace);
}

/* Note: if the compiler complains about ambigous overload, you can use
 * cv::putTextFancy() instead of cv::putText() to avoid conflicts.
 */

#define CV2_PUTTEXT_FANCY_HPP__putTextFancyApi \
    std::optional<cv::Scalar> outlineColor OPT_ALL_DEF, int outlineThickness = 4, \
    bool shadow = false, \
    std::optional<cv::Scalar> bgColor = std::nullopt, \
    cv::Scalar color = fancy::kWhite, int thickness = 2, \
    double fontScale = 1.0, double lineSpacing = 1.1, \
    int fontFace = FONT_HERSHEY_SIMPLEX, \
    int lineType = 8, bool bottomLeftOrigin = false
#define OPT_ALL_DEF

static inline image_ostream_fancy putText(
    cv::InputOutputArray img, cv::Point origin,
    CV2_PUTTEXT_FANCY_HPP__putTextFancyApi )
{
    return image_ostream_fancy(img, origin,
#define X(type, name, default_val) name,
        CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X
        CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
#undef X
    0);
}

static inline image_ostream_fancy putText(
    CV2_PUTTEXT_FANCY_HPP__putTextFancyApi )
{
    return image_ostream_fancy(Mat(), Point(0,0),
#define X(type, name, default_val) name,
        CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X
        CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
#undef X
    0);
}

#undef OPT_ALL_DEF
#define OPT_ALL_DEF = std::nullopt

static inline image_ostream_fancy putTextFancy(
    cv::InputOutputArray img, cv::Point origin,
    CV2_PUTTEXT_FANCY_HPP__putTextFancyApi )
{
    return image_ostream_fancy(img, origin,
#define X(type, name, default_val) name,
        CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X
        CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
#undef X
    0);
}

static inline image_ostream_fancy putTextFancy(
    CV2_PUTTEXT_FANCY_HPP__putTextFancyApi )
{
    return image_ostream_fancy(Mat(), Point(0,0),
#define X(type, name, default_val) name,
        CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X
        CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
#undef X
    0);
}

#undef OPT_ALL_DEF
#undef CV2_PUTTEXT_FANCY_HPP__putTextFancyApi

#ifdef CV2_PUTTEXT_FANCY_HPP_IMPL

image_ostream_fancy::~image_ostream_fancy()
{
    if(!_img.empty()){
        nextLine();
    }
}

void image_ostream_fancy::nextLine()
{
    if(_str.str().empty()){ return; }

    const int max_thickness = (_outlineColor && (_outlineThickness > 0))
      ? _outlineThickness + _thickness : _thickness;
    const int shadow_offset = _shadow ? _outlineThickness : 0;
    const auto with_space = [c = _lineSpacing](int x) -> int { return (int)std::rint(c * x); };
    const auto with_scale = [c = _fontScale](int x) -> int { return (int)std::rint(c * x); };

    std::string line;
    int max_width = 0;
    do
    {
        std::getline(_str, line);
        replaceAll(line, "\t", "  ");

        // baseline is the distance from the line letters are written on
        // to the bottom of characters that go below the line, like 'g' or 'y'
        // height without baseline will cover 'ABC' but not 'g'
        int baseline;
        const cv::Size textSize = cv::getTextSize(line, _fontFace, _fontScale, max_thickness, &baseline);

        const int line_width = line.empty() ? 0 : textSize.width;
        const int line_height = textSize.height + baseline;
        // Note: we shift textSize.height to make the origin the upper-left corner
        const int origin_correction = _bottomLeftOrigin ? 0 : textSize.height;
        const int offset_height = with_space(line_height);

        if(_pLineSizes) _pLineSizes->emplace_back(line_width, offset_height);
        if(line_width > max_width) max_width = line_width;

        if(line.empty()){
            _offset += offset_height;
            continue;
        }

        // Background mask
        if(_bgColor && line_width > 0){
            const int topBaselinePad = with_space(baseline / 2);
            // pad with the top-baseline space; added to mirror the baseline underneath
            _offset += topBaselinePad;
            cv::rectangle(_img,
                _origin + cv::Point(with_scale(-6),
                    _offset - topBaselinePad),
                _origin + cv::Point(with_scale(6) + line_width,
                    _offset + with_space(line_height)),
                _bgColor.value(), cv::FILLED);
        }

        // Outline text
        if(_outlineColor && _outlineThickness > 0){
            cv::putText(_img, line,
                _origin + cv::Point(shadow_offset, _offset + origin_correction + shadow_offset),
                _fontFace, _fontScale, _outlineColor.value(), max_thickness,
                _lineType, false);
        }

        // Real text
        cv::putText(_img, line,
            _origin + cv::Point(0, _offset + origin_correction),
            _fontFace, _fontScale, _color, _thickness,
            _lineType, false);

        _offset += offset_height;
    } while (!_str.eof());
    _str.str("");
    _str.clear();
    if(_pTextSize){
      _pTextSize->width = std::max(max_width, _pTextSize->width);
      _pTextSize->height = _offset;
    }
}

image_ostream_fancy& image_ostream_fancy::operator<<(const image_ostream_fancy& new_settings)
{
    // First, dump out the old string, with old format
    nextLine();
    // Then, copy over the new settings
#define X(type, name, default_val) _##name = new_settings._##name;
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
    CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X
#undef X
    // And any string
    _str << new_settings._str.str();
    return *this;
}

image_ostream_fancy& image_ostream_fancy::operator<<(const image_ostream& new_settings)
{
    *this << image_ostream_fancy(new_settings);
    return *this;
}

image_ostream_fancy::image_ostream_fancy(
    InputOutputArray img, Point origin,
#define X(type, name, default_val) type name,
        CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X
        CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
#undef X
    void*_)
    : image_ostream(img, origin
#define X(type, name, default_val) , name
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
#undef X
    )
#define X(type, name, default_val) , _##name(name)
    CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X
#undef X
{ (void)_;
}

image_ostream_fancy::image_ostream_fancy(const image_ostream_fancy& rhs)
    : image_ostream(rhs)
#define X(type, name, default_val) , _##name(rhs._##name)
    CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X
#undef X
{
}

image_ostream_fancy::image_ostream_fancy(const image_ostream& rhs)
    : image_ostream(rhs)
#define X(type, name, default_val) , _##name(default_val)
    CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X
#undef X
{
}

#endif // CV2_PUTTEXT_FANCY_HPP_IMPL

} // namespace cv

/* TODO:
 * The two nextLine() functions are almost identical.
 * . Make a drawLine(line, baseline, line_width, line_height, origin_correction)
 *   protected function, that nextLine calls, then rm nextLine from fancy.
 * . Redefine in fancy that _thickness is the max_thickness; have outLineThickness
 *   still as a parameter, but before fancy destructor, swap thickness to max_thickness,
 *   and carefully mention in drawLine that the "real" thickness of the text is
 *   outlineThickness. Or something cleaner. If not, don't "fix".
 * The << operator template in class prevents definition of a << operator for
 *   lhs image_ostream and rhs image_ostream_fancy.
 * . The trivial solution escapes me
 */

#endif // __CV2_PUTTEXT_FANCY_HPP__

