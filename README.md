# cv2\_putText(\_fancy).hpp: Modern C++ wrappers for OpenCV's cv::putText

## Features
`cv2_putText.hpp`:
* Modern C++ `std::cout <<`-like syntax for OpenCV's `cv::putText` function
* Handles multiple lines of text ('\n', '\r\n', std::endl) and tabs ('\t')
* `cv::putText` calls can be chained together, to pivot the format in the middle of the `<<` chain
* More ergonomic parameter defaults and ordering
* Formats can be saved as variables, for re-use: `auto format_1 = cv::putText(color_1, thickness_1, ...);`
* Intuitive origin: defaults to UPPER left, not baseline lower left
* Allows right-justified text/origin
* Minimal changes/API, faithful to original OpenCV proposal

`cv2_putText_fancy.hpp`:
* Does everything `cv2_putText.hpp` does, and...
* Outlines
* Shadows
* Backgrounds
    * and any combination thereof
* Relative positioning and alignment to other objects

## Usage
###### The gh md cpp syntax highlighting is not handling strings correctly; sorry
```cpp
#define CV2_PUTTEXT_HPP_IMPL
#include "cv2_putText.hpp"

const cv::Scalar kRed(0, 0, 255);
const cv::Scalar kBlue(255, 0, 0);

cv::Mat img(500, 800, CV_8UC3, cv::Scalar::all(255));
//               color, thickness, scale, lineSpacing
auto fmt_BoldRed = cv::putText(kRed, 4, 1.3, 0.9);

cv::putText(img, cv::Point(40, 40))
  << "cv::putText() Demo!" << std::endl
  << "cv::putText(...) << \"Hello\\nWorld\";" << std::endl
<< cv::putText(kBlue, 2, 0.8)
  << "You can even change the format mid-chain!" << std::endl
  << "You can use std::endl" << std::endl
  << "or even\\n symbol\nto format text" << std::endl
<< fmt_BoldRed
  << "You can save formats as variables for re-use!" << std::endl
  << "And we support formatters:\n"
  << "std::setprecision(5)" << std::setprecision(5) << CV_PI << std::endl
  << "std::scientific " << std::scientific << CV_PI << std::endl
  << "So you can use cv::putText like regular std::cout!"
;

std::vector<cv::Size> lineSizes{};
cv::Size textSize{};
cv::putText(img, cv::Point(40,40))
    .setTextSizeResult(&textSize).setLineSizesResult(&lineSizes)
  << "You can get the sizes of each line\nAnd the total size of the text!\n"
  << "Note: only when it actually gets drawn, then are they filled!"
;
```
```cpp
/* But wait, there's more! */
/* cv2_putText.hpp minimizes extra features/namespace pollution;
 * here is the kitchen sink version, with all the features */
#define CV2_PUTTEXT_FANCY_HPP_IMPL
#include "cv2_putText_fancy.hpp"

const auto fancy_fmt = cv::putTextFancy(
    fancy::kBlue, 4, true, fancy::kGreen, false, // blue shadow, green bg
    fancy::kRed, 4, 2.0, 1.0, cv::FONT_HERSHEY_COMPLEX, cv::LINE_AA, false)
  << "You can save fancy formats as\n\t\tvariables for re-use!" << std::endl;

cv::putTextFancy(img, cv::Point(40, 40))
  << "Fancy cv::putText() Demo!" << std::endl
<< cv::putTextOutline(fancy::kRed, 4, 1.3, 0.9)
  << "This text has an outline!" << std::endl
<< cv::putTextShadow(fancy::kBlue, 2, 0.8)
  << "This text has a shadow!" << std::endl
<< cv::putTextBackground(fancy::kRed, fancy::kBlue, true, 4, 1.3)
  << "This text has a background!" << std::endl

<< fancy_fmt
  << "You can combine them all!" << std::endl
<< cv::putText()
  << "But wait, there's more!\n(also, intermix regular cv2_putText)" << std::endl
<< fancy_fmt
  << "^^^ Even the text stored with them!"

<< cv::putTextOutline()
  << "\tChanging formats has implicit newline;\n\t\tonly add more if you want the space!"
<< cv::putTextBackground()
  << "\tAnd the newline spacing is the\n\t\tsame size as the format/text!"
<< cv::putTextBackground(fancy::kBlue, fancy::kRed, 2, 0.7)
  << "To have no gaps in backgrounds," << std::endl
  << "                               " << std::endl
  << "need to fill with whitespace!  " << std::endl
  << "Note: no HERSHEY Mono font, tho" << std::endl
  << "\tthese lines WILL NOT be even!" << std::endl
;

// Note: when assigning a (chain of) format expressions (with an img) to a variable,
// make the variable `auto&&`. That's probably what you really want. Otherwise... :_(
// If you become some kind of power user, mixing lots of formatter variables and
// re-using them and handling scope weirdly, there might be an edge case; file a good MWE bug.

// Note: currently the left-most formatter is not const, and gets modified further formatters down the call chain.
// If you'd like to keep the original format, make a copy of it, feed it into another,
// or at least mark it `const`, to warn you if you're modifying it.
// This is recommended usage:
const auto my_fmt = cv::putText(kRed, 4, 1.3, 0.9);
cv::putText(img, origin) << my_fmt << "Hello, World!" << std::endl;
// ^^^^^^^^ This temporary formatter is the one being modified, and writing on destruction
```
Note, this is implemented as a single file header, so in one and only one .cpp file, the IMPL flags must be defined. See the Installation section for more information.

With reasonable defaults, the following putText functions are defined in the middle of the .hpp files:
### `cv2_putText.hpp`:
```cpp
cv::putText(
    cv::InputOutputArray /* cv::Mat */ img, cv::Point origin,
    cv::Scalar color = /* black */, int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    int fontFace = cv::FONT_HERSHEY_SIMPLEX,
    int lineType = cv::LINE_AA, bool bottomLeftOrigin=false);

/* Inline version */
cv::putText(
    cv::Scalar color = /* black */, int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    int fontFace = cv::FONT_HERSHEY_SIMPLEX,
    int lineType = cv::LINE_AA, bool bottomLeftOrigin=false);

/* The returned object has the relevent methods: */
this& setTextSizeResult (            cv::Size * pSize  );
this& setLineSizesResult(std::vector<cv::Size>* pSizes );
this& setTextBoxResult  (            cv::Rect * pRect  );
this& setOriginResult   (            cv::Point* pOrigin);
```
There is also a "relative" version, that will set the origin to the side, top/bottom, or inside of a space.
There's 2 varients, one with cv::Rect and one with cv::Point top-left and cv::Size.
They don't support any text formatting, but you can chain into them with the regular `cv::putText` calls.
```cpp
// Put text on the top or bottom of the rectangle
cv::putText_RelativeTo(
    InputOutputArray /* cv::Mat */ img,
    /* const cv::Rect& rect, */ /* OR */ /* const Point& rect_tl, const Size& rect_size */
    VertAlign vert = Top, TextAlign horz = Left,
    bool inside = false, int pad = 6 );

// Put text on the left or right side of the rectangle; swapped horz/vert!
cv::putText_RelativeTo(
    InputOutputArray /* cv::Mat */ img,
    /* const cv::Rect& rect, */ /* OR */ /* const Point& rect_tl, const Size& rect_size */
    TextAlign horz = Right, VertAlign vert = Top,
    bool inside = false, bool textboxBottomLeftOrigin = false, int pad = 6 );
// Note: textboxBottomLeftOrigin makes all the text go up, instead of drop-down

/* Enumeration types */
/* Note: there are fancy:: aliases in cv2_putText_fancy.hpp */
cv::image_ostream::TextAlign { Left, Right, Center } // enum class : unsigned
cv::image_ostream::VertAlign { Top, Bottom, Mid } // enum class : unsigned
```
### `cv2_putText_fancy.hpp`:
```cpp
/* Inherits from cv2_putText.hpp, aforementioned methods still relevant */
/* Note: inline versions are omitted for brevity; just drop the img and origin */

/* Predefined colors used within the fancy putText functions */
cv::Scalar fancy::kBlack;
cv::Scalar fancy::kWhite;
cv::Scalar fancy::kShadow;
cv::Scalar fancy::kRed;
cv::Scalar fancy::kGreen;
cv::Scalar fancy::kBlue;

cv::putTextOutline(
    cv::InputOutputArray /* cv::Mat */ img, cv::Point origin,
    cv::Scalar color = fancy::kWhite, int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    cv::Scalar outlineColor = fancy::kBlack, int outlineThickness = 4,
    int fontFace = cv::FONT_HERSHEY_SIMPLEX);

cv::putTextShadow(
    cv::InputOutputArray /* cv::Mat */ img, cv::Point origin,
    cv::Scalar color = fancy::kWhite, int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    int outlineThickness = 4, cv::Scalar outlineColor = fancy::kShadow,
    int fontFace = FONT_HERSHEY_SIMPLEX);

cv::putTextBackground(
    cv::InputOutputArray /* cv::Mat */ img, cv::Point origin,
    cv::Scalar color = fancy::kBlack, cv::Scalar bgColor = fancy::kWhite,
    bool filled = true, int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    int fontFace = cv::FONT_HERSHEY_SIMPLEX)

/* This is the generic version, which can be used to combine the above */
cv::putTextFancy(
    cv::InputOutputArray /* cv::Mat */ img, cv::Point origin,
    std::optional<cv::Scalar> outlineColor, int outlineThickness = 4,
    bool shadow = false,
    std::optional<cv::Scalar> bgColor = std::nullopt, bool bgFilled = true,
    cv::Scalar color = fancy::kWhite, int thickness = 2,
    double fontScale = 1.0, double lineSpacing = 1.1,
    int fontFace = cv::FONT_HERSHEY_SIMPLEX, int lineType = cv::LINE_AA,
    bool bottomLeftOrigin = nullopt(false),
    TextAlign align = nullopt(Left), bool reverse = nullopt(false));

cv::putText = cv::putTextFancy; // Alias for the generic version, ambiguous calls without more arguments

/* Relative versions are exact same API as regular version */
// Put text on the top or bottom of the rectangle
cv::putTextFancy_RelativeTo(
    InputOutputArray /* cv::Mat */ img,
    /* const cv::Rect& rect, */ /* OR */ /* const Point& rect_tl, const Size& rect_size */
    VertAlign vert = Top, TextAlign horz = Left,
    bool inside = false, int pad = 6 );

// Put text on the left or right side of the rectangle; swapped horz/vert!
cv::putTextFancy_RelativeTo(
    InputOutputArray /* cv::Mat */ img,
    /* const cv::Rect& rect, */ /* OR */ /* const Point& rect_tl, const Size& rect_size */
    TextAlign horz /* Right */, VertAlign vert = Top,
    bool inside = false, bool textboxBottomLeftOrigin = false, int pad = 6 );
// Note: textboxBottomLeftOrigin makes all the text go up, instead of drop-down

/* Enumeration types, in fancy:: */
/* Note: there are fancy:: aliases in cv2_putText_fancy.hpp */
fancy::TextAlign { Left, Right, Center } // enum class : unsigned
fancy::VertAlign { Top, Bottom, Mid } // enum class : unsigned
fancy::TA = TextAlign; // alias
fancy::VA = VertAlign; // alias
```
### Help! I don't see anything!
To make the `<<` cout-style and formatter chaining work, the **first** `cv::putText` call _must_:
* be the one with the image and origin (omitting the image == no text)
* be destroyed
The low-level, original cv::putText calls only happen on destruction. This happens naturally when either
* the first `cv::putText` is not stored as a variable, and the `<<` chain ends
* the first `cv::putText` is stored as a variable, and the variable goes out of scope
The second one is most likely; you probably had some code like this, where you use the image before the formatter is destroyed, so the text is only drawn after the image is used:
```cpp
void myDrawFn(){
    cv::Mat img(500, 800, CV_8UC3, cv::Scalar::all(255));
    auto fmt = cv::putText(img, cv::Point(40, 40)) << "Super cool text!\n";
    fmt << "More text!\n";
    // Uh-oh, fmt is still in scope, so the text is not drawn yet!
    cv::imshow("Image", img);
} // Uh-oh, the text is only drawn after the image is shown!
```
In this trivial example, it could be solved by wrapping the fmt in a block `{ ... }`, calling `cv::imshow` after the function or block, or by not storing the `cv::putText` call as a variable.
Note: you also may need to have the type be an r-value reference: `auto&& fmt = ...;`.

### Help! There's weird '?' characters in my text!
That's the underlying, original `cv::putText` function, which sometimes doesn't handle some things. Try transforming the text first, if there's an encoding it likes better.

### Help! None of the fancy settings are working!
If, perhaps, the very first `cv::putText` was actually the "regular" version, then any later, downstream "fancy" `cv::putText` calls will also only be treated as the "regular" version.

## Style Tips
The provided cv::putText\* functions have default arguments ordered in what seems to be the most useful order. But, if unfamiliar, here are some tips on the parameterization.

* For a given scale, each character takes the same space, regardless of the thickness (or outline). If the thickness/outline is too thick, the characters will start mushing together. The lower the scale, the lower the mushing threshold for thickness/outline.
* Scale 0.5 is difficult to use, as it's very small. Avoid using outlines/shadow; if you must, put it on a background. Prefer the non-fancy version. Keep the thickness low, 1 or 2, and if you must outline, 1 or 2 respectively.
* Scale 1.0 is the default. Thicknesses range from 1-4 without mushing. With the default outline of 4, the thickness ranges then from outline-dominated to text-dominated.
* Scale 2.0 is sharper and clearer, but also larger. The same recommendations from scale 1.0 apply, but with a higher threshold for mushing.
* If you re-use the same heavily parameterized putText calls, consider saving it as a variable, specifically the putText without image/origin arguments. See the example below, which uses these calls to pivot the format in the middle of the `<<` chain.
* The text can be slightly compressed (overlapped) or spread out by modifying the `lineSpacing = 1.0` parameter.

## Installation
So long as the `#include <opencv2/core.hpp>` is available to the rest of your project, there are no further dependencies, for headers or for test.cpp. The `fancy` header depends on the adjacent `cv2_putText.hpp` header. `std::optional` is used in the `fancy` header, which thus requires C++17. If you're stuck in the dark ages, you can either modify it to use pointers, or just use the non-fancy version.

On a side note, the easiest way to install this to your project, with the hopes of documentation and future bugfixes, would be to simply make it a `git subrepo`, see [https://github.com/ingydotnet/git-subrepo](https://github.com/ingydotnet/git-subrepo).

<details>
<summary>git-subrepo Installation Instructions</summary>

git subrepo doesn't need to be installed on any computer, except the one using it. The final changes to the repo are a commit, the files of the subrepo, and a .gitrepo file in that folder, containing metadata for future updates.
```bash
mkdir -p ~/.local/share
git clone https://github.com/ingydotnet/git-subrepo ~/.local/share/git-subrepo
echo 'source ~/.local/share/git-subrepo/.rc' >> ~/.bashrc
```
Lastly, in the **root** directory of your git project, with a **clean** state, run:
``` bash
git subrepo clone <repository> [<subdir>] -f
```
</details>

### Compiling the Implementation
The `cv2_putText.hpp` and `cv2_putText_fancy.hpp` files are single-file libraries, with the implementation included within them behind `#ifdef` guards. The implementations must be defined in one, and only one, file. The two recommendations are to either take your core visualization .cpp file, and add the `#define`'s there, or to create a new .cpp file, with just the `#include`'s and the `#define`'s. This has the up and downside of being another file to manage. In both cases:
```cpp
#define CV2_PUTTEXT_HPP_IMPL
#include "cv2_putText.hpp"
#define CV2_PUTTEXT_FANCY_HPP_IMPL
#include "cv2_putText_fancy.hpp"
```

## License
The core of this (cv2\_putText.hpp) came from a rejected opencv PR, and as it was committed under the OpenCV license, it is also under the OpenCV license. See the license header in both header files for more information.

The original contribution was written by Eugene Khvedchenya.
See the rejected PR https://github.com/opencv/opencv/pull/313 for details.
He also had a blog post:
https://computer-vision-talks.com/2013-01-21-are-you-still-using-cvputtext-to-render-debug-text-on-the-image/

Some upkeep was performed to migrate from OpenCV 2 to OpenCV 4, and to make it more user-friendly.

Additionally, cv2\_putText\_fancy.hpp is an extension of cv2\_putText.hpp, and is also under the OpenCV license. It did use ideas from
[YoniChechik's python version](https://stackoverflow.com/a/73471951/), which is under the [CC BY-SA 4.0 license](https://creativecommons.org/licenses/by-sa/4.0/), though the code is not directly copied.

Any further modifications from the original code are under the OpenCV license.

## Known Issues (TODO)
* Having a `cv2_putText` call here currently doesn't allow any further `cv2_putText_fancy` settings, due to operator overload issues.
* The code has grown over time, has lots of redundancy, and could use a refactor. Simple solutions, however, have failed to compile or not-segfault, so any external review would be appreciated.

