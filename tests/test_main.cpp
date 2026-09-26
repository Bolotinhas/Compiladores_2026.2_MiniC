#include <exception>
#include <iostream>

#include "mini_test.hpp"

int main() {
    int failed = 0;
    for (const auto& c : mt::registry()) {
        try {
            c.fn();
            std::cout << "[ OK ] " << c.name << "\n";
        } catch (const mt::Failure& f) {
            ++failed;
            std::cout << "[FAIL] " << c.name << "\n       " << f.message << "\n";
        } catch (const std::exception& e) {
            ++failed;
            std::cout << "[FAIL] " << c.name << "\n       excecao inesperada: " << e.what() << "\n";
        }
    }
    std::cout << "\n" << (mt::registry().size() - failed) << "/" << mt::registry().size()
              << " testes passaram\n";
    return failed == 0 ? 0 : 1;
}
