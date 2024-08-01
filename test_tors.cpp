// test_tors.cpp

#include <cstddef>
#include <cassert>
#include <string>
#include <sstream>
#include <iostream>

// BEGIN IMAGE_OSTREAM DECL
class image_ostream
{
public:
    typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
    typedef CoutType& (*ManipType)(CoutType&);
    // typedef std::optional<std::reference_wrapper<CoutType>> CoutOptType; // Not working c++17
    typedef CoutType* const CoutOptType;

    image_ostream(
        CoutOptType& cout,
        const std::string& stx,
        const std::string& etx );

    image_ostream(const image_ostream& rhs);

    ~image_ostream();

    image_ostream& operator<<(const image_ostream& new_settings);

    template <typename T>
    image_ostream& operator<<(const T& x){
        _str << x;
        return *this;
    }

    inline image_ostream& operator<<(ManipType manip){
        manip(_str);
        return *this;
    }
protected:
    void emit();

protected:
    CoutOptType _cout;
    std::string _stx;
    std::string _etx;

    std::stringstream _str;
};

static inline image_ostream formatter(
    image_ostream::CoutOptType& cout, const std::string& stx = "", const std::string& etx = ""
){
    return image_ostream(cout, stx, etx);
}
static inline image_ostream formatter(
    const std::string& stx, const std::string& etx
){
    return image_ostream(nullptr, stx, etx);
}

// BEGIN IMAGE_OSTREAM IMPL

image_ostream::~image_ostream(){
    if(_cout){
        emit();
    }
}

void image_ostream::emit(){
    assert(_cout);
    std::string line;
    do {
        std::getline(_str, line);
        if(line.empty()) continue;
        *_cout << _stx << line << _etx << std::endl;
    } while (!_str.eof());
    _str.clear();
}

image_ostream& image_ostream::operator<<(const image_ostream& new_settings){
    assert(!new_settings._cout);
    emit();
    _stx = new_settings._stx;
    _etx = new_settings._etx;
    _str << new_settings._str.str();
    return *this;
}

image_ostream::image_ostream(
    image_ostream::CoutOptType& cout,
    const std::string& stx,
    const std::string& etx
) : _cout(cout), _stx(stx), _etx(etx) {}

image_ostream::image_ostream(const image_ostream& rhs)
    : _cout(rhs._cout), _stx(rhs._stx), _etx(rhs._etx), _str(rhs._str.str()) {}

// END IMAGE_OSTREAM IMPL

typedef void (*test_t)();
static const test_t tests_reg[] = {
// 01
[](){
    image_ostream out(&std::cout, "(", ")");
    out << "Hello World!";
},
// 02
[](){
    auto out = formatter(&std::cout, "(", ")");
    out
      << "Hello World!" << std::endl
    << formatter("[", "]")
      << "Goodbye World!" << std::endl
    ;
},
// 03
[](){
    auto out = formatter(&std::cout);
    const auto parenths = formatter("(", ")");
    const auto brackets = formatter("[", "]");
    out
      << "Hello World!" << std::endl
    << parenths
      << "Troubled World!" << std::endl
    << brackets
      << "Goodbye World!" << std::endl
    ;
},
// 04
[](){
    auto fmt_base = formatter(&std::cout);
    fmt_base << formatter("(", ")")
      << "auto immed overwrit" << std::endl;
},
// 05
[](){
    auto&& fmt_base = formatter(&std::cout);
    fmt_base << formatter("(", ")")
      << "auto&& immed overwrit" << std::endl;
},
// 06
[](){
    auto fmt_base = formatter(&std::cout, "\t", ";");
    auto&& fmt_res = fmt_base << "To base, then save ref";
    fmt_base << formatter("(", ")")
      << "After ref saved at base, to base" << std::endl;
    fmt_res << "After ref saved at base, to ref" << std::endl;
},
// 07
[](){
    auto&& fmt_base = formatter(&std::cout, "\t", ";");
    auto&& fmt_res = fmt_base << "To base, then save ref";
    fmt_base << formatter("(", ")")
      << "After ref saved at base, to base" << std::endl;
    fmt_res << "After ref saved at base, to ref" << std::endl;
},
// 08
[](){
    std::stringstream ss;
    {
        auto&& fmt_base = formatter(&ss, "\t", ";");
        auto&& fmt_res = fmt_base << "To base, then save ref";
        fmt_base << formatter("(", ")")
          << "After ref saved at base, to base" << std::endl;
        fmt_res << "After ref saved at base, to ref" << std::endl;
    }
    std::cout << ss.str();
},

}; // tests_reg

// BEGIN image_ostream_fancy DECL
class image_ostream_fancy
{
public:
    typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
    typedef CoutType& (*ManipType)(CoutType&);
    // typedef std::optional<std::reference_wrapper<CoutType>> CoutOptType; // Not working c++17
    typedef CoutType* const CoutOptType;

    image_ostream_fancy(
        CoutOptType& cout,
        const std::string& stx,
        const std::string& etx );

    image_ostream_fancy(const image_ostream_fancy& rhs);

    ~image_ostream_fancy();

    image_ostream_fancy& operator<<(const image_ostream_fancy& new_settings);

    template <typename T>
    image_ostream_fancy& operator<<(const T& x){
        _str << x;
        return *this;
    }

    inline image_ostream_fancy& operator<<(ManipType manip){
        manip(_str);
        return *this;
    }
protected:
    void emit();

protected:
    CoutOptType _cout;
    std::string _stx;
    std::string _etx;

    std::stringstream _str;
};

static inline image_ostream_fancy formatter(
    image_ostream_fancy::CoutOptType& cout, const std::string& stx = "", const std::string& etx = ""
){
    return image_ostream_fancy(cout, stx, etx);
}
static inline image_ostream_fancy formatter(
    const std::string& stx, const std::string& etx
){
    return image_ostream_fancy(nullptr, stx, etx);
}

// BEGIN image_ostream_fancy IMPL

image_ostream_fancy::~image_ostream_fancy(){
    if(_cout){
        emit();
    }
}

void image_ostream_fancy::emit(){
    assert(_cout);
    std::string line;
    do {
        std::getline(_str, line);
        if(line.empty()) continue;
        *_cout << _stx << line << _etx << std::endl;
    } while (!_str.eof());
    _str.clear();
}

image_ostream_fancy& image_ostream_fancy::operator<<(const image_ostream_fancy& new_settings){
    assert(!new_settings._cout);
    emit();
    _stx = new_settings._stx;
    _etx = new_settings._etx;
    _str << new_settings._str.str();
    return *this;
}

image_ostream_fancy::image_ostream_fancy(
    image_ostream_fancy::CoutOptType& cout,
    const std::string& stx,
    const std::string& etx
) : _cout(cout), _stx(stx), _etx(etx) {}

image_ostream_fancy::image_ostream_fancy(const image_ostream_fancy& rhs)
    : _cout(rhs._cout), _stx(rhs._stx), _etx(rhs._etx), _str(rhs._str.str()) {}

// END IMAGE_OSTREAM_FANCY IMPL

static const test_t tests_fancy[] = {

}; // tests_fancy

int main(){
    std::cout << "Testing image_ostream" << std::endl;
    int i = 0;
    for(const auto& t : tests_reg){
        std::cout << "Running Reg " << ++i << "():" << std::endl;
        t();
        std::cout << std::endl;
    }
    i = 0;
    for(const auto& t : tests_fancy){
        std::cout << "Running Fancy " << ++i << "():" << std::endl;
        t();
        std::cout << std::endl;
    }
    return 0;
}

// test_tors.cpp

