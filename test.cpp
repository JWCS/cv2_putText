// test.cpp

#include "opencv2/opencv.hpp"

#define CV2_PUTTEXT_HPP_IMPL
#include "cv2_putText.hpp"
#define CV2_PUTTEXT_FANCY_HPP_IMPL
#include "cv2_putText_fancy.hpp"

static inline cv::Point operator+(const cv::Point& lhs, const cv::Size& rhs) {
  return cv::Point(lhs.x + rhs.width, lhs.y + rhs.height);
}

#define BASIC_BLURB \
  "cv::putText() Demo!" << std::endl \
  << "cv::putText(...) << \"Hello\\nWorld\";" << std::endl \
  << "You can use std::endl" << std::endl \
  << "or even\\n symbol\nto format text" << std::endl \
  << "And we support formatters:\n" \
  << "std::setprecision(5)" << std::setprecision(5) << CV_PI << std::endl \
  << "std::scientific " << std::scientific << CV_PI << std::endl \
  << "So you can use cv::putText like regular std::cout!"

// Note: cpp17 broke this syntax? must be char*, char[] gives false error
#define FileVar(NAME) ac##NAME##_File
#define TEST(NAME, filename) \
static const char* ac##NAME = #NAME; \
static const char* FileVar(NAME) = filename; \
static const std::string s##NAME##_FullFile = std::string(FileVar(NAME)) + ".png"; \
static void NAME()

TEST(Normal, "puttext_normal") {
  cv::Mat img(500, 800, CV_8UC3, fancy::kWhite);
  cv::putText(img, cv::Point(40, 40))
    << BASIC_BLURB;

  cv::imwrite(sNormal_FullFile, img);
}

TEST(NormalSetChains, "puttext_normal_setchains"){
  cv::Mat img(500, 800, CV_8UC3, fancy::kWhite);
  cv::putText(img, cv::Point(250, 40))
    .align(cv::image_ostream::TextAlign::Center)
    .lineSpacing(1.4)
    .thickness(2)
    .lineType(cv::LINE_8)
    .fontFace(cv::FONT_HERSHEY_COMPLEX)
    .color(fancy::kRed)
    << BASIC_BLURB;

  cv::imwrite(sNormalSetChains_FullFile, img);
}

TEST(NormalReuse, "puttext_normal_reuse") {
  cv::Mat img(500, 800, CV_8UC3, fancy::kWhite);
  {
    auto&& out = cv::putText(img, cv::Point(40, 40));
    out
      << BASIC_BLURB;
  }
  cv::imwrite(sNormalReuse_FullFile, img);
}

TEST(NormalPositioning, "puttext_normal_positioning") {
  cv::Mat img(500, 800, CV_8UC3, fancy::kWhite);
  const cv::Point origin1(40,40);
  cv::drawMarker(img, origin1, fancy::kRed);
  cv::putText(img, "Hg: original cv::putText", origin1, cv::FONT_HERSHEY_SIMPLEX,
      1.0, fancy::kBlack, 2, cv::LINE_AA, false);

  const cv::Point origin2(40,140);
  cv::drawMarker(img, origin2, fancy::kRed);
  cv::putText(img, "Hg: original cv::putText, bottomLeftOrigin=true", origin2, cv::FONT_HERSHEY_SIMPLEX,
      1.0, fancy::kBlack, 2, cv::LINE_AA, true);

  const cv::Point origin3(40,240);
  cv::putText(img, origin3) << "Hg: cv2_putText (+) is origin";

  const cv::Point origin4(40,340);
  cv::putText(img, origin4,
      cv::Scalar::all(0), 2, 1.0, 1.1, cv::FONT_HERSHEY_SIMPLEX, cv::LINE_AA,
      true) << "Hg: cv2_putText, bottomLeftOrigin=true";
  cv::imwrite(sNormalPositioning_FullFile, img);
}

TEST(NormalAlignment, "puttext_normal_alignment") {
  int width = 800;
  cv::Mat img(500, width, CV_8UC3, fancy::kWhite);

  cv::putText(img, cv::Point(width/2, 140)) << "Align Left\n";

  {auto fmt = cv::putText(img, cv::Point(width/2, 240));
    fmt._align_opt = cv::image_ostream::TextAlign::Center;
    fmt << "Align Center\n"; }

  {auto fmt = cv::putText(img, cv::Point(width/2, 340));
    fmt._align_opt = cv::image_ostream::TextAlign::Right;
    fmt << "Align Right\n"; }
  cv::imwrite(sNormalAlignment_FullFile, img);
}

TEST(NormalOnelineCenter, "puttext_normal_onelinecenter"){
  int width = 800;
  cv::Mat img(500, width, CV_8UC3, fancy::kWhite);

  for(int i = 1; i < 3; ++i){
      {auto fmt = cv::putText(img, cv::Point(width*i/3, 140));
        fmt._bottomLeftOrigin_opt = (bool)(i - 1);
        fmt << "Align Left"; }

      {auto fmt = cv::putText(img, cv::Point(width*i/3, 240));
        fmt._bottomLeftOrigin_opt = (bool)(i - 1);
        fmt._align_opt = cv::image_ostream::TextAlign::Center;
        fmt << "Align Center"; }

      {auto fmt = cv::putText(img, cv::Point(width*i/3, 340));
        fmt._bottomLeftOrigin_opt = (bool)(i - 1);
        fmt._align_opt = cv::image_ostream::TextAlign::Right;
        fmt << "Align Right"; }
  }

  cv::imwrite(sNormalOnelineCenter_FullFile, img);
}

TEST(Normal_RelativeTo, "puttext_normal_relativeto"){
  cv::Mat img(800, 800, CV_8UC3, fancy::kWhite);
  const cv::Rect rect(150, 150, 500, 500);
  cv::rectangle(img, rect, fancy::kGreen, 2);
  // 9 pos inside, 12 pos outside
  // Top/bottom, outside and in
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Top, fancy::TextAlign::Left, false) << "TL";
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Top, fancy::TextAlign::Center, false) << "TC";
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Top, fancy::TextAlign::Right, false) << "TR";
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Bottom, fancy::TextAlign::Left, false) << "BL";
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Bottom, fancy::TextAlign::Center, false) << "BC";
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Bottom, fancy::TextAlign::Right, false) << "BR";
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Top, fancy::TextAlign::Left, true) << "TL-in";
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Top, fancy::TextAlign::Center, true) << "TC-in";
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Top, fancy::TextAlign::Right, true) << "TR-in";
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Bottom, fancy::TextAlign::Left, true) << "BL-in";
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Bottom, fancy::TextAlign::Center, true) << "BC-in";
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Bottom, fancy::TextAlign::Right, true) << "BR-in";
  // Left/right, outside
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Left, fancy::VertAlign::Top, false) << "LT";
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Left, fancy::VertAlign::Mid, false) << "LM";
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Left, fancy::VertAlign::Bottom, false) << "LB";
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Right, fancy::VertAlign::Top, false) << "RT";
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Right, fancy::VertAlign::Mid, false) << "RM";
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Right, fancy::VertAlign::Bottom, false) << "RB";
  // Middle, inside; these are kinda dup'd, but shouldn't be able to tell vis
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Center, fancy::VertAlign::Mid, true) << "CM-in";
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Left, fancy::VertAlign::Mid, true) << "LM-in";
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Right, fancy::VertAlign::Mid, true) << "RM-in";
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Mid, fancy::TextAlign::Left, true) << "LM-in";
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Mid, fancy::TextAlign::Right, true) << "RM-in";

  cv::imwrite(sNormal_RelativeTo_FullFile, img);
}

TEST(Normal_RelativeToMultiline, "puttext_normal_relativeto_multiline"){
  const int img_size = 900;
  const int img_pad = 200;
  cv::Mat img(img_size, img_size, CV_8UC3, fancy::kWhite);
  cv::Rect bbox{};
  cv::Point origin{};
  auto box = [&](){
      cv::rectangle(img, bbox, fancy::kBlue);
      cv::drawMarker(img, origin, fancy::kRed);
  };
  const cv::Rect rect(img_pad, img_pad, img_size - 2*img_pad, img_size - 2*img_pad);
  cv::rectangle(img, rect, fancy::kGreen, 2);

  // 9 pos inside, 5*4 pos outside
  // Top/bottom, outside and in
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Top, fancy::TextAlign::Left, false).setOriginResult(&origin).setTextboxResult(&bbox) << "TL" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Top, fancy::TextAlign::Center, false).setOriginResult(&origin).setTextboxResult(&bbox) << "TC" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Top, fancy::TextAlign::Right, false).setOriginResult(&origin).setTextboxResult(&bbox) << "TR" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Bottom, fancy::TextAlign::Left, false).setOriginResult(&origin).setTextboxResult(&bbox) << "BL" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Bottom, fancy::TextAlign::Center, false).setOriginResult(&origin).setTextboxResult(&bbox) << "BC" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Bottom, fancy::TextAlign::Right, false).setOriginResult(&origin).setTextboxResult(&bbox) << "BR" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Top, fancy::TextAlign::Left, true).setOriginResult(&origin).setTextboxResult(&bbox) << "TL-in" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Top, fancy::TextAlign::Center, true).setOriginResult(&origin).setTextboxResult(&bbox) << "TC-in" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Top, fancy::TextAlign::Right, true).setOriginResult(&origin).setTextboxResult(&bbox) << "TR-in" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Bottom, fancy::TextAlign::Left, true).setOriginResult(&origin).setTextboxResult(&bbox) << "BL-in" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Bottom, fancy::TextAlign::Center, true).setOriginResult(&origin).setTextboxResult(&bbox) << "BC-in" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Bottom, fancy::TextAlign::Right, true).setOriginResult(&origin).setTextboxResult(&bbox) << "BR-in" << "\nMulti\nLine!"; box();
  // Left/right, outside
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Left, fancy::VertAlign::Top, false).setOriginResult(&origin).setTextboxResult(&bbox) << "LT" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Left, fancy::VertAlign::Mid, false).setOriginResult(&origin).setTextboxResult(&bbox) << "LM" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Left, fancy::VertAlign::Bottom, false).setOriginResult(&origin).setTextboxResult(&bbox) << "LB" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Right, fancy::VertAlign::Top, false).setOriginResult(&origin).setTextboxResult(&bbox) << "RT" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Right, fancy::VertAlign::Mid, false).setOriginResult(&origin).setTextboxResult(&bbox) << "RM" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Right, fancy::VertAlign::Bottom, false).setOriginResult(&origin).setTextboxResult(&bbox) << "RB" << "\nMulti\nLine!"; box();
  // Middle, inside << "\nMulti\nLine!"; box(); these are kinda dup'd, but shouldn't be able to tell vis
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Center, fancy::VertAlign::Mid, true).setOriginResult(&origin).setTextboxResult(&bbox) << "CM-in" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Left, fancy::VertAlign::Mid, true).setOriginResult(&origin).setTextboxResult(&bbox) << "LM-in" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Right, fancy::VertAlign::Mid, true).setOriginResult(&origin).setTextboxResult(&bbox) << "RM-in" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Mid, fancy::TextAlign::Left, true).setOriginResult(&origin).setTextboxResult(&bbox) << "LM-in" << "\nMulti\nLine!"; box();
  cv::putText_RelativeTo(img, rect, fancy::VertAlign::Mid, fancy::TextAlign::Right, true).setOriginResult(&origin).setTextboxResult(&bbox) << "RM-in" << "\nMulti\nLine!"; box();
  // When you probably want to have the textbox origin at bottom left
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Left, fancy::VertAlign::Top, false, true).setOriginResult(&origin).setTextboxResult(&bbox)    << "LT" << "\nbottomLeft\n_origin=True!"; box();
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Left, fancy::VertAlign::Bottom, false, true).setOriginResult(&origin).setTextboxResult(&bbox) << "LB" << "\nbottomLeft\n_origin=True!"; box();
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Right, fancy::VertAlign::Top, false, true).setOriginResult(&origin).setTextboxResult(&bbox)    << "RT" << "\nbottomLeft\n_origin=True!"; box();
  cv::putText_RelativeTo(img, rect, fancy::TextAlign::Right, fancy::VertAlign::Bottom, false, true).setOriginResult(&origin).setTextboxResult(&bbox) << "RB" << "\nbottomLeft\n_origin=True!"; box();

  cv::imwrite(sNormal_RelativeToMultiline_FullFile, img);
}

TEST(Normal_StackFmts, "puttext_normal_stackfmts"){
  cv::Mat img(800, 800, CV_8UC3, fancy::kWhite);
  {
  cv::putText(img, cv::Point(40, 40), fancy::kBlack, 2)
    << "Stacking fmts: " << std::endl
    << "\tDefault black, 2, scale 1, simplex" << std::endl
  << cv::putText(fancy::kRed, 4, 2, 1.0, cv::FONT_HERSHEY_COMPLEX)
    << "It's BIGGER!\n\tIt's BADDER!\n"
    << "\tIS IT red, 4, scale 2,\n\t\tcomplex?" // << std::endl
  << cv::putText(fancy::kGreen, 1, 0.5, 2.0, cv::FONT_HERSHEY_DUPLEX)
    << "Lastly, it's green, 4, scale 0.5,\n\t2x line space, duplex" << std::endl
    << BASIC_BLURB;
  }
  cv::imwrite(sNormal_StackFmts_FullFile, img);
}

TEST(Normal_Demo, "puttext_normal_demo") {
  cv::Mat img(500, 800, CV_8UC3, fancy::kWhite);
  //               color, thickness, scale, lineSpacing
  const auto&& fmt_BoldRed = cv::putText(fancy::kRed, 4, 1.3, 0.9);

  cv::putText(img, cv::Point(40, 40))
    << "cv::putText() Demo!" << std::endl
    << "cv::putText(...) << \"Hello\\nWorld\";" << std::endl
  << cv::putText(fancy::kBlue, 2, 0.8)
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
  cv::imwrite(sNormal_Demo_FullFile, img);
}

TEST(Normal_Sizes, "puttext_normal_sizes") {
  cv::Mat img(500, 800, CV_8UC3, fancy::kWhite);
  const cv::Point origin(40,40);
  std::vector<cv::Size> lineSizes{};
  cv::Size textSize{};

  (cv::putText(img, origin)
    << "ABC\n\nDEFGHI\n\nJKLMNLOPQRS\n\naaaaaaaaaaaaaa\n\nyyyyyyyyyyyyyyyyyyyy\n\n"
    << "Note that newline \\n have non-zero widths!"
  ).setTextSizeResult(&textSize).setLineSizesResult(&lineSizes);

  cv::Point lineOrigin = origin;
  int i = 0;
  for(const cv::Size& lineSize : lineSizes) {
    if(lineSize.width != 0){
      cv::rectangle(img, lineOrigin, lineOrigin + lineSize, i++ % 2 == 0 ? fancy::kRed : fancy::kBlue);
      // std::cout << "Line " << i << ": " << lineSize.height << ", " << lineSize.width << std::endl;
    }
    lineOrigin.y += lineSize.height;
  }
  cv::rectangle(img, origin, origin + textSize, fancy::kBlack);

  cv::imwrite(sNormal_Sizes_FullFile, img);
}

/*
// FIXME: this is broken, both with and without the && ref on fmt_base
TEST(Normal_Refs, "puttext_normal_refs") {
  cv::Mat img(500, 800, CV_8UC3, fancy::kGrey);
  {
    // Is this first && truly useless? Or if with a move ctor, prevents a copy?
    auto fmt_base = cv::putText(img, cv::Point(40, 40));
    auto&& fmt_res = fmt_base << BASIC_BLURB;
    fmt_base << cv::putText(cv::Scalar(0, 255, 0), 2, 2.0)
      << "\nGreen!\n(Called from reference)\n";
    fmt_res << "\nNo Overwrite! (From base)\n";
  }
  cv::imwrite(sNormal_Refs_FullFile, img);
}
*/

TEST(Fancy_Normal, "puttextfancy_normal") {
  cv::Mat img(500, 800, CV_8UC3, fancy::kWhite);
  /* TODO: is there a viable overload for putText() thats not ambiguous? */
  cv::putTextFancy(img, cv::Point(40, 40), std::nullopt, 0, false, std::nullopt, false,
      fancy::kBlack, 1, 1.0, 1.2)
    << BASIC_BLURB;
  cv::imwrite(sFancy_Normal_FullFile, img);
}

TEST(Fancy_SetChains, "puttextfancy_setchains"){
  cv::Mat img(500, 800, CV_8UC3, fancy::kWhite);
  cv::putTextShadow(img, cv::Point(250, 40))
    .align(cv::image_ostream::TextAlign::Center)
    .lineSpacing(1.4)
    .thickness(2)
    .lineType(cv::LINE_8)
    .fontFace(cv::FONT_HERSHEY_COMPLEX)
    .color(fancy::kRed)
    .bgColor(fancy::kGreen)
    .bgFilled(false)
    << BASIC_BLURB;

  cv::imwrite(sFancy_SetChains_FullFile, img);
}

TEST(Fancy_Stack, "puttextfancy_stack") {
  cv::Mat img(500, 800, CV_8UC3, fancy::kWhite);
  /* TODO: is there a viable overload for putText() thats not ambiguous? */
  cv::putTextFancy(img, cv::Point(40, 40), std::nullopt, 0, false, std::nullopt, false,
      fancy::kBlack, 1, 0.5, 1.2)
    << BASIC_BLURB
  // Note, this is non-fancy putText
  << cv::putText(fancy::kRed, 4, 2, 1.0, cv::FONT_HERSHEY_COMPLEX)
    << "\tIS IT red, 4, scale 2,\n\t\tcomplex?" // << std::endl
  /* TODO: is there a viable overload for putText() thats not ambiguous? */
  << cv::putTextFancy(std::nullopt, 0, false, std::nullopt, false, fancy::kShadow, 1, 0.5, 2.0, cv::FONT_HERSHEY_DUPLEX)
    << "Lastly, it's shadow, 1, scale 0.5,\n\t2x line space, duplex" << std::endl
    << BASIC_BLURB;
  cv::imwrite(sFancy_Stack_FullFile, img);
}

TEST(Fancy_Outline, "puttextfancy_outline") {
  cv::Mat img(2400, 1600, CV_8UC3, fancy::kGrey);
  cv::putTextOutline(img, cv::Point(40, 40),
  // Scale 1
      fancy::kWhite, 1, 1.0, 1.0, fancy::kBlack, 1)
    << "Scale 1.0, Thickness 1, Outline 1" << std::endl
  << cv::putTextOutline(fancy::kWhite, 1, 1.0, 1.0, fancy::kBlack, 2)
    << "Scale 1.0, Thickness 1, Outline 2" << std::endl
  << cv::putTextOutline(fancy::kWhite, 1, 1.0, 1.0, fancy::kBlack, 4)
    << "Scale 1.0, Thickness 1, Outline 4" << std::endl
  << cv::putTextOutline(fancy::kWhite, 2, 1.0, 1.0, fancy::kBlack, 2)
    << "Scale 1.0, Thickness 2, Outline 2" << std::endl
  << cv::putTextOutline(fancy::kWhite, 2, 1.0, 1.0, fancy::kBlack, 4)
    << "Scale 1.0, Thickness 2, Outline 4" << std::endl
  << cv::putTextOutline(fancy::kWhite, 4, 1.0, 1.0, fancy::kBlack, 4)
    << "Scale 1.0, Thickness 4, Outline 4" << std::endl
  << cv::putTextOutline(fancy::kWhite, 4, 1.0, 1.0, fancy::kBlack, 2)
    << "Scale 1.0, Thickness 4, Outline 2" << std::endl
  // Scale 2
  << cv::putTextOutline(fancy::kRed, 1, 2.0, 1.0, fancy::kBlack, 1)
    << "Scale 2.0, Thickness 1, Outline 1" << std::endl
  << cv::putTextOutline(fancy::kRed, 1, 2.0, 1.0, fancy::kBlack, 2)
    << "Scale 2.0, Thickness 1, Outline 2" << std::endl
  << cv::putTextOutline(fancy::kRed, 1, 2.0, 1.0, fancy::kBlack, 4)
    << "Scale 2.0, Thickness 1, Outline 4" << std::endl
  << cv::putTextOutline(fancy::kRed, 2, 2.0, 1.0, fancy::kBlack, 2)
    << "Scale 2.0, Thickness 2, Outline 2" << std::endl
  << cv::putTextOutline(fancy::kRed, 2, 2.0, 1.0, fancy::kBlack, 4)
    << "Scale 2.0, Thickness 2, Outline 4" << std::endl
  << cv::putTextOutline(fancy::kRed, 4, 2.0, 1.0, fancy::kBlack, 4)
    << "Scale 2.0, Thickness 4, Outline 4" << std::endl
  << cv::putTextOutline(fancy::kRed, 4, 2.0, 1.0, fancy::kBlack, 2)
    << "Scale 2.0, Thickness 4, Outline 2" << std::endl
  // Scale 0.5
  << cv::putTextOutline(fancy::kBlue, 1, 0.5, 1.0, fancy::kBlack, 1)
    << "Scale 0.5, Thickness 1, Outline 1" << std::endl
  << cv::putTextOutline(fancy::kBlue, 1, 0.5, 1.0, fancy::kBlack, 2)
    << "Scale 0.5, Thickness 1, Outline 2" << std::endl
  << cv::putTextOutline(fancy::kBlue, 1, 0.5, 1.0, fancy::kBlack, 4)
    << "Scale 0.5, Thickness 1, Outline 4" << std::endl
  << cv::putTextOutline(fancy::kBlue, 2, 0.5, 1.0, fancy::kBlack, 2)
    << "Scale 0.5, Thickness 2, Outline 2" << std::endl
  << cv::putTextOutline(fancy::kBlue, 2, 0.5, 1.0, fancy::kBlack, 4)
    << "Scale 0.5, Thickness 2, Outline 4" << std::endl
  << cv::putTextOutline(fancy::kBlue, 4, 0.5, 1.0, fancy::kBlack, 4)
    << "Scale 0.5, Thickness 4, Outline 4" << std::endl
  << cv::putTextOutline(fancy::kBlue, 4, 0.5, 1.0, fancy::kBlack, 2)
    << "Scale 0.5, Thickness 4, Outline 2" << std::endl
  ;
  cv::imwrite(sFancy_Outline_FullFile, img);
}

TEST(Fancy_Shadow, "puttextfancy_shadow") {
  cv::Mat img(500, 800, CV_8UC3, fancy::kGrey);
  cv::putTextFancy(img, cv::Point(30, 20), fancy::kShadow, 4, true)
    << "Shadowed Text:\n"
    << "putText(img, orgin, kShadow, 4, true)" << std::endl
  << cv::putText()
    << "Space out with default (reg) putText()" << std::endl
  << cv::putTextShadow()
    << "putTextShadow() for shadowed text" << std::endl
  << cv::putTextShadow(fancy::kBlue, 3, 1.3)
    << "putTextShadow(kBlue, 3, 1.3)" << std::endl
  ;
  cv::imwrite(sFancy_Shadow_FullFile, img);
}

TEST(Fancy_Background, "puttextfancy_background") {
  cv::Mat img(800, 800, CV_8UC3, fancy::kGrey);
  cv::putTextFancy(img, cv::Point(30, 20)) // TODO change to non-fancy (or another test non-fancy)... once working
  << cv::putTextBackground()
    << "Default background 'g j p q y'" << std::endl
    << "'g j p q y'        'ABCDEFGHIJ'" << std::endl
  << cv::putTextBackground(fancy::kBlue)
    << "Blue text  'g j p q y'" << std::endl
    << "'g j p q y''ABCDEFGHIJ'" << std::endl
  << cv::putTextBackground(fancy::kRed, fancy::kBlack, true, 2, 0.7)
    << "Red text, black bg, 2px border, 0.7 size, 'g j p q y'" << std::endl
    << "With no line overlap compression 'g j p q y''ABCDEFGHIJKLM'" << std::endl
  << cv::putTextBackground(fancy::kBlack, fancy::kGreen, true, 3, 2.0)
    << "Black text, green bg, 3px border, 2.0 size" << std::endl
    << "With no line spacing" << std::endl
  << cv::putTextBackground(fancy::kRed, fancy::kBlack, false, 2, 0.7)
    << "Tiny Again, With Border" << std::endl
  << cv::putTextBackground()
    << "Default again" << std::endl
  ;
  cv::imwrite(sFancy_Background_FullFile, img);
}

TEST(Fancy_Demo, "puttextfancy_demo") {
  cv::Mat img(1600, 1600, CV_8UC3, fancy::kGrey);
  const auto fancy_fmt = cv::putTextFancy(
      fancy::kBlue, 4, true, fancy::kGreen, true, // blue shadow, green bg
      fancy::kRed, 4, 2.0, 1.0, cv::FONT_HERSHEY_COMPLEX, 8, false)
    << "You can save fancy formats as\n\t\tvariables for re-use!" << std::endl;

  cv::putTextFancy(img, cv::Point(40, 40))
    << "Fancy cv::putText() Demo!" << std::endl
  << cv::putTextOutline(fancy::kRed, 4, 1.3, 0.9)
    << "This text has an outline!" << std::endl
  << cv::putTextShadow(fancy::kBlue, 2, 0.8)
    << "This text has a shadow!" << std::endl
  << cv::putTextBackground(fancy::kRed, fancy::kBlue, true, 4, 1.3, 1.0)
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
  << cv::putTextBackground(fancy::kBlue, fancy::kRed, true, 2, 0.7)
    << "To have no gaps in backgrounds," << std::endl
    << "                               " << std::endl
    << "need to fill with whitespace!  " << std::endl
    << "Note: no HERSHEY Mono font, tho" << std::endl
  ;
  cv::imwrite(sFancy_Demo_FullFile, img);
}

TEST(Fancy_Sizes, "puttextfancy_sizes") {
  cv::Mat img(800, 800, CV_8UC3, fancy::kGrey);
  const cv::Point origin(40,40);
  std::vector<cv::Size> lineSizes{};
  cv::Size textSize{};

  cv::putTextFancy(img, origin).setTextSizeResult(&textSize).setLineSizesResult(&lineSizes)
    << "ABC\n\n"
  << cv::putTextOutline()
    << "DEFGHI\n\n"
  << cv::putTextShadow()
    << "JKLMNLOPQRS\n\n"
  << cv::putTextBackground()
    << "aaaaaaaaaaaaaa\n\n"
  << cv::putTextOutline()
    << "yyyyyyyyyyyyyyyyyyyy\n\n"
  << cv::putTextBackground()
    << "Note that newline \\n have non-zero widths!"
  ;

  cv::Point lineOrigin = origin;
  int i = 0;
  for(const cv::Size& lineSize : lineSizes) {
    if(lineSize.width != 0){
      cv::rectangle(img, lineOrigin, lineOrigin + lineSize, i++ % 2 == 0 ? fancy::kRed : fancy::kBlue);
      // std::cout << "Line " << i << ": " << lineSize.height << ", " << lineSize.width << std::endl;
    }
    lineOrigin.y += lineSize.height;
  }
  cv::rectangle(img, origin, origin + textSize, fancy::kBlack);

  cv::imwrite(sFancy_Sizes_FullFile, img);
}

TEST(Fancy_RelativeTo, "puttextfancy_relativeto"){
  const int img_size = 900;
  const int img_pad = 200;
  cv::Mat img(img_size, img_size, CV_8UC3, fancy::kGrey);
  cv::Rect bbox{};
  cv::Point origin{};
  auto box = [&](){
      cv::rectangle(img, bbox, fancy::kBlue);
      cv::drawMarker(img, origin, fancy::kRed);
  };
  const cv::Rect rect(img_pad, img_pad, img_size - 2*img_pad, img_size - 2*img_pad);
  cv::rectangle(img, rect, fancy::kGreen, 2);

  cv::putTextFancy_RelativeTo(img, rect, fancy::VertAlign::Top, fancy::TextAlign::Left, false).setOriginResult(&origin).setTextboxResult(&bbox) << "TL" << "\nMulti\nLine!"; box();
  cv::putTextFancy_RelativeTo(img, rect, fancy::VertAlign::Top, fancy::TextAlign::Right, false).setOriginResult(&origin).setTextboxResult(&bbox) << cv::putTextOutline() << "TR" << "\nMulti\nLine!"; box();
  cv::putTextFancy_RelativeTo(img, rect, fancy::VertAlign::Bottom, fancy::TextAlign::Center, false).setOriginResult(&origin).setTextboxResult(&bbox) << cv::putTextShadow() << "BC" << "\nMulti\nLine!"; box();
  cv::putTextFancy_RelativeTo(img, rect, fancy::VertAlign::Top, fancy::TextAlign::Center, true).setOriginResult(&origin).setTextboxResult(&bbox) << cv::putTextBackground() << "TC-in" << "\nMulti\nLine!"; box();
  cv::putTextFancy_RelativeTo(img, rect, fancy::VertAlign::Bottom, fancy::TextAlign::Right, true).setOriginResult(&origin).setTextboxResult(&bbox) << "BR-in" << "\nMulti\nLine!"; box();
  cv::putTextFancy_RelativeTo(img, rect, fancy::TextAlign::Left, fancy::VertAlign::Mid, false).setOriginResult(&origin).setTextboxResult(&bbox) << cv::putTextOutline() << "LM" << "\nMulti\nLine!"; box();
  cv::putTextFancy_RelativeTo(img, rect, fancy::TextAlign::Right, fancy::VertAlign::Top, false).setOriginResult(&origin).setTextboxResult(&bbox) << cv::putTextShadow() << "RT" << "\nMulti\nLine!"; box();
  cv::putTextFancy_RelativeTo(img, rect, fancy::TextAlign::Center, fancy::VertAlign::Mid, true).setOriginResult(&origin).setTextboxResult(&bbox) << cv::putTextBackground() << "CM-in"; box();
  cv::putTextFancy_RelativeTo(img, rect, fancy::TextAlign::Left, fancy::VertAlign::Mid, true).setOriginResult(&origin).setTextboxResult(&bbox) << "LM-in" << "\nMulti\nLine!"; box();
  cv::putTextFancy_RelativeTo(img, rect, fancy::TextAlign::Right, fancy::VertAlign::Mid, true).setOriginResult(&origin).setTextboxResult(&bbox) << cv::putTextOutline() << "RM-in" << "\nMulti\nLine!"; box();
  cv::putTextFancy_RelativeTo(img, rect, fancy::TextAlign::Right, fancy::VertAlign::Bottom, false, true).setOriginResult(&origin).setTextboxResult(&bbox) << cv::putTextShadow() << "RB" << "\nbottomLeft\n_origin=True!"; box();
  cv::putTextFancy_RelativeTo(img, rect, fancy::TextAlign::Left, fancy::VertAlign::Top, false, true).setOriginResult(&origin).setTextboxResult(&bbox) << cv::putTextBackground() << "LT" << "\nbottomLeft\n_origin=True!"; box();

  cv::imwrite(sFancy_RelativeTo_FullFile, img);
}

/*
TEST(Fancy_IntoReg1, "puttextfancy_intoreg1"){
  cv::Mat img(500, 800, CV_8UC3, fancy::kWhite);
  cv::putText(img, cv::Point(40, 40))
    << "cv::putText() Demo!" << std::endl
    << "This is the regular putText() with fancy chaining!" << std::endl
  << cv::putTextOutline(fancy::kRed, 4, 2.0)
    << "BUT CAN IT HANDLE << putTextFancy()?" << std::endl
  << cv::putText()
    << "Yes, it can!" << std::endl
  ;
  cv::imwrite(sFancy_IntoReg1_FullFile, img);
}
*/

/*
TEST(Fancy_IntoReg2, "puttextfancy_intoreg2"){
  cv::Mat img(500, 800, CV_8UC3, fancy::kWhite);
  {
    auto fmt_reg = cv::putText(img, cv::Point(40, 40));
    const auto fmt_fancy = cv::putTextOutline(fancy::kRed, 4, 2.0);
    fmt_reg
      << "cv::putText() Demo!" << std::endl
      << "This is the regular putText() with fancy chaining!" << std::endl
    << fmt_fancy
      << "BUT CAN IT HANDLE << putTextFancy()?" << std::endl
    << cv::putText()
      << "Yes, it can!" << std::endl
    ;
  }
  cv::imwrite(sFancy_IntoReg2_FullFile, img);
}
*/

// TODO: test that assigning an already assigned fmt doesn't double apply

#define ALL_TESTS \
  X(Normal) \
  X(NormalSetChains) \
  X(NormalReuse) \
  X(NormalPositioning) \
  X(NormalAlignment) \
  X(NormalOnelineCenter) \
  X(Normal_RelativeTo) \
  X(Normal_RelativeToMultiline) \
  X(Normal_StackFmts) \
  X(Normal_Demo) \
  X(Normal_Sizes) \
  X(Fancy_Normal) \
  X(Fancy_SetChains) \
  X(Fancy_Stack) \
  X(Fancy_Outline) \
  X(Fancy_Shadow) \
  X(Fancy_Background) \
  X(Fancy_Demo) \
  X(Fancy_Sizes) \
  X(Fancy_RelativeTo)
  //X(Normal_Refs) // This has img/InputOutputArray mem issues
  //X(Fancy_IntoReg1) // These have typing/template issues
  //X(Fancy_IntoReg2)

#define STR_EQ(a, b) (strcmp(a, b) == 0)

int main(int argc, char** argv) {
  //cv::setBreakOnError(true);
  cv::image_ostream::_Debug.draw_origin = true;
  try {
    if(argc > 1) {
      for(int i = 1; i < argc; i++) {
        if(false){
#define X(NAME) }else if(STR_EQ(argv[i], ac##NAME) || STR_EQ(argv[i], FileVar(NAME))) { NAME();
          ALL_TESTS
#undef X
        }
      }
    }else{
#define X(NAME) NAME();
      ALL_TESTS
#undef X
    }
  } catch(const cv::Exception& e) {
    std::cerr << "CV:Exception: " << e.what() << std::endl;
  }
  return 0;
}

// test.cpp

