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

#ifndef __CV2_PUTTEXT_HPP__
#define __CV2_PUTTEXT_HPP__

// cv::putText() with std::cout-like formatting
// See the inline definitions for api.
//
// The following contribution was written by Eugene Khvedchenya.
// See the rejected PR https://github.com/opencv/opencv/pull/313 for details.
// He also had a blog post:
// https://computer-vision-talks.com/2013-01-21-are-you-still-using-cvputtext-to-render-debug-text-on-the-image/
//
// The devs back in 2013 rejected it because they didn't want to maintain another class.
// However, it is still compatible and usable with OpenCV 4x.
// This should just be considered a patch; it adds to the cv namespace, but is not official.
// For reference, the vanilla putText header/impl are found in:
// opencv/modules/imgproc/include/opencv2/imgproc.hpp
// opencv/modules/imgproc/src/drawing.cpp
//
// This is a single-file header. To use it, before including it in ONE
// cpp file, define CV2_PUTTEXT_HPP_IMPL. This will include the implementation.
//
// API DIFFERENCE:
// The original cv::putText(cv::Point origin) is the origin of the LOWER-left
// corner of the text. Ex, the corner of 'L', the lower-left curve of 'q'.
// For precise positioning, this is too tricky. This version uses the UPPER-left
// corner: the upper-left corner of 'R'/'F'. This is therefore invariant to font size.
// You can revert this behavior by setting bottomLeftOrigin=true (although the original
// use case was to mirror the text about origin... which is not supported here).

/* Example:
cv::putText(image2, cv::Point(100,100), Scalar(i, i, 255))
  << "cv::putText() Demo!" << std::endl
  << "cv::putText(...) << \"Hello\\nWorld\";" << std::endl
  << "You can use std::endl" << std::endl
  << "or even\\n symbol\nto format text" << std::endl
<< cv::putText(cv::Scalar(0, 255, 0), 1, 0.5, 2.0, cv::FONT_HERSHEY_DUPLEX)
  << "and you can change the format on the fly! (omit image/origin)" << std::endl
  << "And we support formatters:\n"
  << "std::setprecision(5)" << std::setprecision(5) << CV_PI << std::endl
  << "std::scientific " << std::scientific << CV_PI << std::endl
  << "So you can use cv::putText like regular std::cout!";
*/

#include <opencv2/core.hpp>
#include <cstddef>
#include <vector>

namespace cv {

#define CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X \
  X(Scalar, color, cv::Scalar::all(0)) \
  X(int, thickness, 2) \
  X(double, fontScale, 1.0) \
  X(double, lineSpacing, 1.1) \
  X(int, fontFace, cv::FONT_HERSHEY_SIMPLEX) \
  X(int, lineType, 8) \
  X(bool, bottomLeftOrigin, false) \
  X(image_ostream::TextAlign, align, image_ostream::TextAlign::Left) \
  X(bool, reverse, false) \
  X(int, pad, 6)

//! Creates and return image_ostream object to render text on the image like the std::cout does.
//! An image_ostream class supports operator<< for both primitive and opencv types.
struct CV_EXPORTS image_ostream
{

    enum class TextAlign : unsigned { Left, Right, Center };
    enum class VertAlign : unsigned { Top, Bottom, Mid };

    image_ostream(
        InputOutputArray img, Point origin,
#define X(type, name, default_val) type name = default_val,
        CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
#undef X
        void*_=0);

    //! Copy constructor is necessary because compiler cannot perform RVO in putText() function in some cases. To deal with it we introduce copy
    //! constructor which copies image_ostream object and it's internal string buffer.
    image_ostream(const image_ostream&);

    //! Prints everything to the cv::Mat in the destructor
    ~image_ostream();

    //! Size results (only after drawing/destruction!)
    // Note: can't use movable references, and std::reference_wrapper too buggy
    image_ostream& setTextSizeResult (cv::Size* const pSize){ _pTextSize = pSize; return *this; }
    image_ostream& setLineSizesResult(std::vector<cv::Size>* const pSizes){ _pLineSizes = pSizes; return *this; }
    image_ostream& setTextboxResult(cv::Rect* const pRect){ _pRect = pRect; return *this; }
    image_ostream& setOriginResult(cv::Point* const pPoint){ _pPoint = pPoint; return *this; }

    //! Self operator<< to chain multiple settings
    image_ostream& operator<<(const image_ostream& new_settings);

    //! Defalt operator<< to take everything
    template <typename T>
    image_ostream& operator<<(const T& x)
    {
        _str << x;
        return *this;
    }

    // this is the type of std::cout
    typedef std::basic_ostream<char, std::char_traits<char> > CoutType;

    // this is the function signature of std::endl
    typedef CoutType& (*ManipType)(CoutType&);

    //! Define an operator<< to take in std::endl and other manipulators
    inline image_ostream& operator<<(ManipType manip)
    {
        manip(_str);
        return *this;
    }

protected:
    // Does not handle newlines!
    cv::Size getLineSize(const std::string& text, int& baseline) const
    {
        return cv::getTextSize(text, _fontFace, _fontScale, _thickness, &baseline);
    }
    cv::Point origin(int x, int y) const { return _origin + cv::Point(x, y); }
    void nextLine();
    void reverse();
    static void replaceAll(std::string& str, const std::string& from, const std::string& to);

protected:
    InputOutputArray _img;
public:
    const Point _origin;
#define X(type, name, default_val) type _##name;
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
#undef X
    std::vector<cv::Size>* _pLineSizes;
    cv::Size*              _pTextSize;
    cv::Rect*              _pRect;
    cv::Point*             _pPoint;
protected:
    int               _offset;
    std::stringstream _str;
};

//! Creates and return image_ostream object to render text on the image like the std::cout does.
//! An image_ostream class supports operator<< for both primitive and opencv types.
static inline image_ostream putText(
    InputOutputArray img, Point origin,
    Scalar color = cv::Scalar::all(0), int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    int fontFace = cv::FONT_HERSHEY_SIMPLEX,
    int lineType=8, bool bottomLeftOrigin=false,
    image_ostream::TextAlign align = image_ostream::TextAlign::Left,
    bool reverse = false, int pad = 6 )
{
    return image_ostream(img, origin,
#define X(type, name, default_val) name,
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
#undef X
    0);
}

static inline image_ostream putText(
    Scalar color = cv::Scalar::all(0), int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    int fontFace = cv::FONT_HERSHEY_SIMPLEX,
    int lineType=8, bool bottomLeftOrigin=false,
    image_ostream::TextAlign align = image_ostream::TextAlign::Left,
    bool reverse = false, int pad = 6 )
{
    return image_ostream(noArray(), Point(0,0),
#define X(type, name, default_val) name,
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
#undef X
    0);
}

// Put text on the top or bottom of the rectangle
image_ostream putText_RelativeTo(
    InputOutputArray img, const Point& rect_tl, const Size& rect_size,
    image_ostream::VertAlign vert = image_ostream::VertAlign::Top,
    image_ostream::TextAlign horz = image_ostream::TextAlign::Left,
    bool inside = false, int pad = 6 );

// Put text on the left or right side of the rectangle
image_ostream putText_RelativeTo(
    InputOutputArray img, const Point& rect_tl, const Size& rect_size,
    image_ostream::TextAlign horz = image_ostream::TextAlign::Right,
    image_ostream::VertAlign vert = image_ostream::VertAlign::Top,
    bool inside = false, bool textboxBottomLeftOrigin = false, int pad = 6 );

static inline image_ostream putText_RelativeTo(
    InputOutputArray img, const cv::Rect& rect,
    image_ostream::VertAlign vert = image_ostream::VertAlign::Top,
    image_ostream::TextAlign horz = image_ostream::TextAlign::Left,
    bool inside = false, int pad = 6 )
{
    return putText_RelativeTo(img, rect.tl(), rect.size(), vert, horz, inside, pad);
}

static inline image_ostream putText_RelativeTo(
    InputOutputArray img, const cv::Rect& rect,
    image_ostream::TextAlign horz = image_ostream::TextAlign::Right,
    image_ostream::VertAlign vert = image_ostream::VertAlign::Top,
    bool inside = false, bool textboxBottomLeftOrigin = false, int pad = 6 )
{
    return putText_RelativeTo(img, rect.tl(), rect.size(),
        horz, vert, inside, textboxBottomLeftOrigin, pad);
}

#ifdef CV2_PUTTEXT_HPP_IMPL

image_ostream::~image_ostream()
{
    if(!_img.empty()){
        nextLine();
    }
}

void image_ostream::nextLine()
{
    if(_str.str().empty()){ return; }
    if(_reverse){ reverse(); }

    std::string line;
    int max_width = 0;
    do
    {
        std::getline(_str, line);
        replaceAll(line, "\t", "  ");

        // baseline is the distance from the line letters are written on
        // to the bottom of characters that go below the line, like 'g' or 'y'
        // height without baseline will cover 'ABC' but not 'g'
        int baseLine;
        const cv::Size textSize = getLineSize(line, baseLine);

        const int line_width = line.empty() ? 0 : textSize.width;
        const int line_height = textSize.height + baseLine;
        // Note: we shift textSize.height to make the origin the upper-left corner
        const int offset_correction = _bottomLeftOrigin ? 0 : textSize.height;
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

        cv::putText(_img, line,
            origin(alignment_shift, _offset + offset_correction),
            _fontFace, _fontScale, _color,_thickness, _lineType, false);

        _offset += offset_height;
    } while (!_str.eof());
    _str.str("");
    _str.clear();
    if(_pTextSize)
    {
        _pTextSize->width = max_width;
        _pTextSize->height = _offset; // Negative allowed?
    }
    if(_pRect)
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
        *_pRect = cv::Rect(
              cv::Point(x1, _origin.y), cv::Point(x2, _origin.y + _offset));
    }
    if(_pPoint)
    {
        _pPoint->x = _origin.x;
        _pPoint->y = _origin.y;
    }
}

image_ostream& image_ostream::operator<<(const image_ostream& new_settings)
{
    // First, dump out the old string, with old format
    nextLine();
    // Then, copy over the new settings
#define X(type, name, default_val) _##name = new_settings._##name;
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
#undef X
    // And any string
    _str << new_settings._str.str();
    return *this;
}

image_ostream::image_ostream(
    InputOutputArray img, Point origin,
#define X(type, name, default_val) type name,
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
#undef X
    void*_)
    : _img(img)
    , _origin(origin)
#define X(type, name, default_val) , _##name(name)
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
#undef X
    , _pLineSizes(nullptr)
    , _pTextSize(nullptr)
    , _pRect(nullptr)
    , _pPoint(nullptr)
    , _offset(0)
{ (void)_;
}

image_ostream::image_ostream(const image_ostream& rhs)
    : _img(rhs._img)
    , _origin(rhs._origin)
#define X(type, name, default_val) , _##name(rhs._##name)
    CV2_PUTTEXT_HPP__IMAGE_OSTREAM_VAR_ARGS_X
#undef X
    , _pLineSizes(rhs._pLineSizes)
    , _pTextSize(rhs._pTextSize)
    , _pRect(rhs._pRect)
    , _pPoint(rhs._pPoint)
    , _offset(rhs._offset)
    , _str(rhs._str.str())
{
}

// Put text on the top or bottom of the rectangle
image_ostream putText_RelativeTo(
    InputOutputArray img, const Point& rect_tl, const Size& rect_size,
    image_ostream::VertAlign vert, image_ostream::TextAlign horz,
    bool inside, int pad )
{
    // Inside meaningless for Mid-Center
    using TA = image_ostream::TextAlign;
    using VA = image_ostream::VertAlign;
    // The only case where text not within left/right sides
    // This fn: everything is inside of left/right sides
    if(vert == VA::Mid && horz != TA::Center && !inside)
        return putText_RelativeTo(img, rect_tl, rect_size, horz, vert, inside, pad);

    const int x = rect_tl.x + (
        horz == TA::Center ? rect_size.width / 2 :
        horz == TA::Right ? rect_size.width - pad : // with_scale?
        /* Left */ pad ); // with_scale?
    const bool blOrigin = ((vert == VA::Top) && !inside) || ((vert == VA::Bottom) && inside);
    const int y = rect_tl.y + (
        vert == VA::Top ? (inside ? pad : -pad) :
        vert == VA::Bottom ? rect_size.height + (inside ? -pad : pad) :
        /* Mid */ rect_size.height / 2 );
    auto fmt = image_ostream(img, cv::Point(x, y));
    fmt._align = horz;
    fmt._bottomLeftOrigin = blOrigin;
    // move the offset up, instead of down, such that whole text block has origin at bottomLeft
    fmt._reverse = blOrigin;
    return fmt;
}

// Put text on the left or right side of the rectangle
image_ostream putText_RelativeTo(
    InputOutputArray img, const Point& rect_tl, const Size& rect_size,
    image_ostream::TextAlign horz, image_ostream::VertAlign vert,
    bool inside, bool textboxBottomLeftOrigin, int pad )
{
    using TA = image_ostream::TextAlign;
    using VA = image_ostream::VertAlign;
    // Only handle the outside cases here
    if(horz == TA::Center || inside)
        return putText_RelativeTo(img, rect_tl, rect_size, vert, horz, inside, pad);

    const bool blOrigin = textboxBottomLeftOrigin;
    // Note: this x has opposite padding directions as other function, bc outside left/right
    const int x = rect_tl.x + (
        horz == TA::Right ? rect_size.width + pad :
        /* Left */ -pad );
    const int y = rect_tl.y + (
        vert == VA::Top ? (blOrigin ? -pad : pad) :
        vert == VA::Bottom ? rect_size.height + (blOrigin ? -pad : pad) :
        /* Mid */ rect_size.height / 2 );
    auto fmt = image_ostream(img, cv::Point(x, y));
    if(horz == TA::Left)
        fmt._align = TA::Right; // flip
    fmt._bottomLeftOrigin = blOrigin;
    // move the offset up, instead of down, such that whole text block has origin at bottomLeft
    fmt._reverse = blOrigin;
    return fmt;
}

void image_ostream::reverse()
{
    std::string line;
    std::vector<std::string> lines;
    while(!std::getline(_str, line).eof())
    {
        lines.push_back(std::move(line));
    }
    lines.push_back(std::move(line));
    _str.str("");
    _str.clear();
    if(lines.empty()){ return; }
    auto rit = lines.crbegin();
    _str << *rit;
    while(++rit != lines.crend())
    {
        _str << '\n' << *rit;
    }
}

void image_ostream::replaceAll(std::string& str, const std::string& from, const std::string& to) {
    // https://stackoverflow.com/a/3418285/
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

#endif // CV2_PUTTEXT_HPP_IMPL

} // cv

#endif // __CV2_PUTTEXT_HPP__

