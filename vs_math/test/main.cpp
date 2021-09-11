#include "tests.hpp"

int main() {
    VecTest::testAll();
    SetTest::testAll();
    MultiIndexTest::testAll();
    CompactTest::testAll();
    return 0;
}