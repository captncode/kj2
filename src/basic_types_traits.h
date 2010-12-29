#pragma once

template <typename T>
struct basic_types_traits {
  static const char * mask() {
    static const char m[] = "%n";
    return m;
  }
  static const char * name() {
    static const char name[] = "unknown";
    return name;
  }
};

template <>
struct basic_types_traits<bool> {
  static const char * mask() {
    static const char m[] = "%2hi";
    return m;
  }
  static const char * name() {
    static const char name[] = "bool";
    return name;
  }
};
template <>
struct basic_types_traits<char> {
  static const char * mask() {
    static const char m[] = "%c";
    return m;
  }
  static const char * name() {
    static const char name[] = "char";
    return name;
  }
};
template <>
struct basic_types_traits<short> {
  static const char * mask() {
    static const char m[] = "%hi";
    return m;
  }
  static const char * name() {
    static const char name[] = "short";
    return name;
  }
};
template <>
struct basic_types_traits<unsigned short> {
  static const char * mask() {
    static const char m[] = "%hu";
    return m;
  }
  static const char * name() {
    static const char name[] = "unsigned short";
    return name;
  }
};
template <>
struct basic_types_traits<int> {
  static const char * mask() {
    static const char m[] = "%i";
    return m;
  }
  static const char * name() {
    static const char name[] = "int";
    return name;
  }
};
template <>
struct basic_types_traits<unsigned> {
  static const char * mask() {
    static const char m[] = "%u";
    return m;
  }
  static const char * name() {
    static const char name[] = "unsigned int";
    return name;
  }
};
template <>
struct basic_types_traits<long> {
  static const char * mask() {
    static const char m[] = "%li";
    return m;
  }
  static const char * name() {
    static const char name[] = "long";
    return name;
  }
};
template <>
struct basic_types_traits<long long> {
  static const char * mask() {
    static const char m[] = "%li";
    return m;
  }
  static const char * name() {
    static const char name[] = "long long";
    return name;
  }
};
template <>
struct basic_types_traits<float> {
  static const char * mask() {
    static const char m[] = "%f";
    return m;
  }
  static const char * name() {
    static const char name[] = "float";
    return name;
  }
};
template <>
struct basic_types_traits<double> {
  static const char * mask() {
    static const char m[] = "%lf";
    return m;
  }
  static const char * name() {
    static const char name[] = "double";
    return name;
  }
};
template <>
struct basic_types_traits<long double> {
  static const char * mask() {
    static const char m[] = "%Lf";
    return m;
  }
  static const char * name() {
    static const char name[] = "long double";
    return name;
  }
};
template <>
struct basic_types_traits<const char *> {
  static const char * mask() {
    static const char m[] = "%s";
    return m;
  }
  static const char * name() {
    static const char name[] = "const char*";
    return name;
  }
};
