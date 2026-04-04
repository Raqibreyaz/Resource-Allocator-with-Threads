#include <cstdlib>
#include <exception>
#include <iostream>

void fn() {}

class A {
public:
  A() { std::clog << "A constructor called" << std::endl; }
  ~A() { std::clog << "A destructor called!" << std::endl; }
};

class B {
public:
  B() { std::clog << "B constructor called" << std::endl; }
  ~B() { std::clog << "B destructor called!" << ::std::endl; }
};

int main() {

  std::clog << "Hello World!" << std::endl;

  // SIGINT: ctrl+c, SIGPIPE

  A a;
  B b;

  std::atexit([] { std::clog << "Additional Resources freed!"; });
  std::atexit([] { std::clog << "Second Additional Resources freed!"; });

  try {
    int *a = nullptr;
    *a = 10;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  std::clog << "Bye World!" << std::endl;

  thread_local int fs = 32;

  return EXIT_SUCCESS;
}