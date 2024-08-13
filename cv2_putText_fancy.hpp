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

    static const cv::Scalar Black = cv::Scalar::all(0);
    static const cv::Scalar White = cv::Scalar::all(255);
    static const cv::Scalar Grey = cv::Scalar::all(200);
    static const cv::Scalar Shadow = cv::Scalar::all(50);
    static const cv::Scalar Red = cv::Scalar(0, 0, 255);
    static const cv::Scalar Blue = cv::Scalar(255, 0, 0);
    static const cv::Scalar Green = cv::Scalar(0x49, 0xB1, 0x54);

    using TextAlign = cv::image_ostream::TextAlign;
    using VertAlign = cv::image_ostream::VertAlign;
    using TA = TextAlign;
    using VA = VertAlign;

    using HorzAlign = cv::image_ostream::TextAlign;
    using HA = TextAlign;

} // namespace fancy

namespace cv {

#define CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X \
  X(std::optional<Scalar>, outlineColor, std::nullopt) \
  X(int, outlineThickness, 4) \
  X(bool, shadow, false) \
  X(std::optional<Scalar>, bgColor, std::nullopt) \
  X(bool, bgFilled, true) \
  X(bool, bgBaselinePad, true)

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
#define X(type, name, default_val) std::optional<type> name = std::nullopt,
        CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_OPT_X
#undef X
        void*_=0);

    //! Copy constructor is necessary because compiler cannot perform RVO in putText() function in some cases. To deal with it we introduce copy
    //! constructor which copies image_ostream_fancy object and it's internal string buffer.
    image_ostream_fancy(const image_ostream_fancy&);
    image_ostream_fancy(const image_ostream&);

    //! Prints everything to the cv::Mat in the destructor
    ~image_ostream_fancy();

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

    //! Size results (only after drawing/destruction!)
    // Note: can't use movable references, and std::reference_wrapper too buggy
#define X(type, name) inline image_ostream_fancy& set##name##Result(type* const p){ _p##name = p; return *this; }
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_RESULT_X
#undef X

    //! Chainable setters
#define X(type, name, default_val) inline image_ostream_fancy& name(type const x){ _##name = x; return *this; }
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
    CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X
#undef X
#define X(type, name, default_val) inline image_ostream_fancy& name(std::optional<type> const x){ _##name##_opt = x; return *this; }
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_OPT_X
#undef X

protected:
    // Does not handle newlines!
    cv::Size _getLineSize(const std::string& text, int& baseline) const
    {
        return cv::getTextSize(text, _fontFace, _fontScale, _maxThickness(), &baseline);
    }
    void _nextLine();
    int _maxThickness() const
    {
        return (_outlineColor && (_outlineThickness > 0) && !_shadow) ?
            _outlineThickness + _thickness : _thickness;
    }

public:
#define X(type, name, default_val) type _##name;
    CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X
#undef X
};

static inline image_ostream_fancy operator<<(const image_ostream& lhs, const image_ostream_fancy& rhs)
{
    image_ostream_fancy fancy_settings(lhs);
    fancy_settings << rhs;
    return fancy_settings;
}

static inline image_ostream_fancy putTextOutline(
    cv::InputOutputArray img, cv::Point origin,
    cv::Scalar color = fancy::White, int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    cv::Scalar outlineColor = fancy::Black, int outlineThickness = 4,
    cv::HersheyFonts fontFace = cv::FONT_HERSHEY_SIMPLEX)
{
    return image_ostream_fancy(img, origin, outlineColor, outlineThickness, false, std::nullopt, true, true, color, thickness, fontScale, lineSpacing, fontFace);
}

static inline image_ostream_fancy putTextOutline(
    cv::Scalar color = fancy::White, int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    cv::Scalar outlineColor = fancy::Black, int outlineThickness = 4,
    cv::HersheyFonts fontFace = cv::FONT_HERSHEY_SIMPLEX)
{
    return putTextOutline(noArray(), Point(0,0), color, thickness, fontScale, lineSpacing, outlineColor, outlineThickness, fontFace);
}

static inline image_ostream_fancy putTextShadow(
    cv::InputOutputArray img, cv::Point origin,
    cv::Scalar color = fancy::White, int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    int outlineThickness = 2, cv::Scalar outlineColor = fancy::Black,
    cv::HersheyFonts fontFace = cv::FONT_HERSHEY_SIMPLEX)
{
    return image_ostream_fancy(img, origin, outlineColor, outlineThickness, true, std::nullopt, true, true, color, thickness, fontScale, lineSpacing, fontFace);
}

static inline image_ostream_fancy putTextShadow(
    cv::Scalar color = fancy::White, int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    int outlineThickness = 2, cv::Scalar outlineColor = fancy::Black,
    cv::HersheyFonts fontFace = cv::FONT_HERSHEY_SIMPLEX)
{
    return putTextShadow(noArray(), Point(0,0), color, thickness, fontScale, lineSpacing, outlineThickness, outlineColor, fontFace);
}

static inline image_ostream_fancy putTextBackground(
    cv::InputOutputArray img, cv::Point origin,
    cv::Scalar color = fancy::Black, cv::Scalar bgColor = fancy::White,
    bool filled = true, int thickness = 2, double fontScale = 1.0,
    bool baselinePad = true, double lineSpacing = 1.1,
    cv::HersheyFonts fontFace = cv::FONT_HERSHEY_SIMPLEX)
{
    return image_ostream_fancy(img, origin, std::nullopt, 0, false, bgColor, filled, baselinePad, color, thickness, fontScale, lineSpacing, fontFace);
}

static inline image_ostream_fancy putTextBackground(
    cv::Scalar color = fancy::Black, cv::Scalar bgColor = fancy::White,
    bool filled = true, int thickness = 2, double fontScale = 1.0,
    bool baselinePad = true, double lineSpacing = 1.1,
    cv::HersheyFonts fontFace = cv::FONT_HERSHEY_SIMPLEX)
{
    return putTextBackground(noArray(), Point(0,0), color, bgColor, filled, thickness, fontScale, baselinePad, lineSpacing, fontFace);
}

/* Note: if the compiler complains about ambigous overload, you can use
 * cv::putTextFancy() instead of cv::putText() to avoid conflicts.
 */

#define CV2_PUTTEXT_FANCY_HPP__putTextFancyApi \
    std::optional<cv::Scalar> outlineColor OPT_ALL_DEF, int outlineThickness = 4, \
    bool shadow = false, \
    std::optional<cv::Scalar> bgColor = std::nullopt, \
    bool bgFilled = true, bool bgBaselinePad = true, \
    cv::Scalar color = fancy::White, int thickness = 2, \
    double fontScale = 1.0, double lineSpacing = 1.1, \
    cv::HersheyFonts fontFace = cv::FONT_HERSHEY_SIMPLEX, \
    cv::LineTypes lineType = cv::LINE_8, \
    std::optional<bool> bottomLeftOrigin = std::nullopt, \
    std::optional<image_ostream::TextAlign> align = std::nullopt, \
    std::optional<bool> reverse = std::nullopt
#define OPT_ALL_DEF

/* v1.2.1: Conflicting ambiguous overload with image_ostream::putText()
 * Have not yet found a good api alternative, especially with
 * bool/int/double implicit conversions.
 */
/*
static inline image_ostream_fancy putText(
    cv::InputOutputArray img, cv::Point origin,
    CV2_PUTTEXT_FANCY_HPP__putTextFancyApi )
{
    return image_ostream_fancy(img, origin,
#define X(type, name, default_val) name,
        CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X
        CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
        CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_OPT_X
#undef X
    0);
}

static inline image_ostream_fancy putText(
    CV2_PUTTEXT_FANCY_HPP__putTextFancyApi )
{
    return image_ostream_fancy(noArray(), Point(0,0),
#define X(type, name, default_val) name,
        CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X
        CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
        CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_OPT_X
#undef X
    0);
}
*/

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
        CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_OPT_X
#undef X
    0);
}

static inline image_ostream_fancy putTextFancy(
    CV2_PUTTEXT_FANCY_HPP__putTextFancyApi )
{
    return image_ostream_fancy(noArray(), Point(0,0),
#define X(type, name, default_val) name,
        CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X
        CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
        CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_OPT_X
#undef X
    0);
}

#undef OPT_ALL_DEF
#undef CV2_PUTTEXT_FANCY_HPP__putTextFancyApi

// Put text on the top or bottom of the rectangle
static inline image_ostream_fancy putTextFancy_RelativeTo(
    InputOutputArray img, const Point& rect_tl, const Size& rect_size,
    image_ostream::VertAlign vert = image_ostream::VertAlign::Top,
    image_ostream::TextAlign horz = image_ostream::TextAlign::Left,
    bool inside = false, int pad = 6 )
{
    return image_ostream_fancy(putText_RelativeTo(img, rect_tl, rect_size, vert, horz, inside, pad));
}

// Put text on the left or right side of the rectangle
static inline image_ostream_fancy putTextFancy_RelativeTo(
    InputOutputArray img, const Point& rect_tl, const Size& rect_size,
    image_ostream::TextAlign horz /* = image_ostream::TextAlign::Right */,
    image_ostream::VertAlign vert = image_ostream::VertAlign::Top,
    bool inside = false, bool textboxBottomLeftOrigin = false, int pad_x = 6, int pad_y = 0 )
{
    return image_ostream_fancy(putText_RelativeTo(img, rect_tl, rect_size, horz, vert, inside, textboxBottomLeftOrigin, pad_x, pad_y));
}

static inline image_ostream_fancy putTextFancy_RelativeTo(
    InputOutputArray img, const cv::Rect& rect,
    image_ostream::VertAlign vert = image_ostream::VertAlign::Top,
    image_ostream::TextAlign horz = image_ostream::TextAlign::Left,
    bool inside = false, int pad = 6 )
{
    return putTextFancy_RelativeTo(img, rect.tl(), rect.size(), vert, horz, inside, pad);
}

static inline image_ostream_fancy putTextFancy_RelativeTo(
    InputOutputArray img, const cv::Rect& rect,
    image_ostream::TextAlign horz /* = image_ostream::TextAlign::Right */,
    image_ostream::VertAlign vert = image_ostream::VertAlign::Top,
    bool inside = false, bool textboxBottomLeftOrigin = false, int pad_x = 6, int pad_y = 0 )
{
    return putTextFancy_RelativeTo(img, rect.tl(), rect.size(),
        horz, vert, inside, textboxBottomLeftOrigin, pad_x, pad_y);
}

#ifdef CV2_PUTTEXT_FANCY_HPP_IMPL

image_ostream_fancy::~image_ostream_fancy()
{
    if(!_img.empty()){
        _nextLine();
    }
}

void image_ostream_fancy::_nextLine()
{
#define X(type, name, default_val) const type _##name = _##name##_opt ? _##name##_opt.value() : default_val;
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_OPT_X
#undef X
    if(_str.str().empty()){ return; }
    if(_reverse){ _reverseLines(); }
    if(_Debug.draw_origin) cv::drawMarker(_img, _origin, cv::Scalar(0, 0, 255));

    const bool oneline = _str.str().find('\n') == std::string::npos;
    const int shadow_offset = _shadow ? _outlineThickness : 0;
    const int midline_adj_k = (_bottomLeftOrigin ? 1 : -1)
            * (oneline && _align == TextAlign::Center ? 1 : 0);
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
        const cv::Size textSize = _getLineSize(line, baseline);

        const int line_width = line.empty() ? 0 : textSize.width;
        const int line_height = textSize.height + baseline;
        // Note: we shift textSize.height to make the origin the upper-left corner
        const int offset_adj = (_bottomLeftOrigin ? 0 : textSize.height);
        const int midline_adj = midline_adj_k * textSize.height / 2;
        const int offset_height = (int)std::rint(line_height * _lineSpacing) * (_reverse ? -1 : 1);
        const int alignment_shift =
            _align == TextAlign::Center ? -line_width / 2 :
            _align == TextAlign::Right  ? -line_width :
            /* _align == Left */ 0;

        if(_pLineSizes) _pLineSizes->emplace_back(line_width, offset_height);
        if(line_width > max_width) max_width = line_width;

        if(line.empty()){
            _offset += offset_height;
            continue;
        }

        // Background mask
        if(_bgColor && line_width > 0){
            const int _pad = 6;
            const int top_baseline_pad = _bgBaselinePad ?
                (!oneline ? with_space(baseline / 2) : baseline / 2) :
                with_scale(_pad);
            const int bot_line_height = _bgBaselinePad ?
                (!oneline ? with_space(line_height) : line_height) :
                textSize.height + with_scale(_pad);
            // pad with the top-baseline space; added to mirror the baseline underneath
            //_offset += topBaselinePad;
            const int rev_mag = _reverse ? -1 : 1; // This isn't a perf fit, but it's a start
            cv::rectangle(_img,
                origin(with_scale(-_pad) + alignment_shift,
                    _offset + midline_adj - top_baseline_pad * rev_mag),
                origin(with_scale(_pad) + alignment_shift + line_width,
                    _offset + midline_adj + bot_line_height * rev_mag),
                _bgColor.value(), _bgFilled ? cv::FILLED : 2, cv::LINE_AA);
        }

        // Outline text
        if(_outlineColor && _outlineThickness > 0){
            cv::putText(_img, line,
                origin(alignment_shift + shadow_offset,
                    _offset + offset_adj + midline_adj + shadow_offset),
                _fontFace, _fontScale, _outlineColor.value(), _maxThickness(),
                _lineType, false);
        }

        // Real text
        cv::putText(_img, line,
            origin(alignment_shift, _offset + offset_adj + midline_adj),
            _fontFace, _fontScale, _color, _thickness,
            _lineType, false);

        _offset += offset_height;
    } while (!_str.eof());

    _str.str("");
    _str.clear();

    if(_pTextSize)
    {
        _pTextSize->width = max_width;
        _pTextSize->height = _offset; // Negative allowed?
    }
    if(_pTextbox)
    {
        // The constructor of 2 points does the math for us
        int x1 = _origin.x;
        int x2 = _origin.x;
        if(_align == TextAlign::Center)
        {
            x1 -= max_width / 2;
            x2 += max_width / 2;
        }
        else if(_align == TextAlign::Right)
        {
            x1 -= max_width;
        }
        else
        {
            x2 += max_width;
        }
        const int midline_adj = midline_adj_k * _offset / 2;
        *_pTextbox = cv::Rect(
              cv::Point(x1, _origin.y + midline_adj),
              cv::Point(x2, _origin.y + midline_adj + _offset));
    }
    if(_pOrigin)
    {
        _pOrigin->x = _origin.x;
        _pOrigin->y = _origin.y;
    }
}

image_ostream_fancy& image_ostream_fancy::operator<<(const image_ostream_fancy& new_settings)
{
    // First, dump out the old string, with old format
    _nextLine();
    // Then, copy over the new settings
#define X(type, name, default_val) _##name = new_settings._##name;
    CV2_PUTTEXT_FANCY_HPP__IMAGE_OSTREAM_FANCY_VAR_ARGS_X
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
#undef X
#define X(type, name, default_val) if(new_settings._##name##_opt) _##name##_opt = new_settings._##name##_opt.value();
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_OPT_X
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
#define X(type, name, default_val) std::optional<type> name,
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_OPT_X
#undef X
    void*_)
    : image_ostream(img, origin
#define X(type, name, default_val) , name
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_OPT_X
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
 * The two _nextLine() functions are almost identical.
 * . Make a drawLine(line, baseline, line_width, line_height, origin_adj)
 *   protected function, that _nextLine calls, then rm _nextLine from fancy.
 * . Tho this apparently requires either CRTP or virtuals...
 * The << operator template in class prevents definition of a << operator for
 *   lhs image_ostream and rhs image_ostream_fancy.
 * . The trivial solution escapes me
 */

#endif // __CV2_PUTTEXT_FANCY_HPP__

