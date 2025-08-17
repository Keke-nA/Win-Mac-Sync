#include <atomic>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

size_t ipToint(std::string str) {
    size_t ans{};
    int x = 0;
    for (int i = 0; i <= str.length(); i++) {
        if (i == str.length() || str[i] == '.') {
            ans = (ans << 8) + x;
            x = 0;
        } else {
            x = x * 10 + str[i] - '0';
        }
    }
    return ans;
}

std::string intToip(size_t n) {
  std::vector<std::string> vec(4);
  size_t x = 255;
  for (int i = 0; i < 4; i++) {
    size_t temp{};
    temp = n & x;
    vec[3 - i] = std::to_string(temp);
    n = n >> 8;
  }
  std::string ans{};
  for (auto& v : vec) {
    ans += v + '.';
  }
  ans.pop_back();
  return ans;
}

template <typename Derived>
class Base {
public:
  void interface() {
    static_cast<Derived*>(this)->inter();
  }
private:
  Base() = default;
  friend Derived;
  int data = 42;
};

class Derived : public Base<Derived> {
public:
  void inter() {
    std::cout << data << std::endl;
    std::cout << "hello world!" << std::endl;
  }
};

int main() {
  // std::string str{"10.0.3.193"};
  // std::cout << ipToint(str) << std::endl;
  // std::cout << intToip(167773121) << std::endl;
  Derived d;
  d.interface();
  return 0;
}