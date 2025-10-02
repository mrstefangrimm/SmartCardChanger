#ifndef Singleton_h
#define Singleton_h

template <class T>
struct Singleton {
  using Type = T;

  static T* create() {
    return &instance;
  }

  static T& instance;
};

#endif
