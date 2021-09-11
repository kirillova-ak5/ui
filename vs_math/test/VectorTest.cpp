#include "tests.hpp"
#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>

void VecTest::testCreate() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    assert(vec1 != nullptr);

    IVector *tmp = IVector::createVector(1, nullptr);
    assert(tmp == nullptr);

    delete tmp;
    CLEAR_LOGGER
    CLEAR_VEC_ONE
}

void VecTest::testCopy() {
    CREATE_LOGGER
    CREATE_VEC_ONE
    CREATE_VEC_TWO

    if (IVector::copyInstance(vec2, vec1) != RC::SUCCESS)
        return;

    for (size_t idx = 0; idx < SIZEOF_ARR(data1); ++idx)
        assert(vec1->getData()[idx] == vec2->getData()[idx]);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    CLEAR_VEC_TWO
}
void VecTest::testMove() {
    CREATE_LOGGER
    CREATE_VEC_ONE
    CREATE_VEC_TWO

    IVector::moveInstance(vec2, vec1);

    assert(vec1 == nullptr);
    for (size_t idx = 0; idx < SIZEOF_ARR(data1); ++idx)
        assert(data1[idx] == vec2->getData()[idx]);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    CLEAR_VEC_TWO
}

void VecTest::testClone() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    std::unique_ptr<IVector> vec2 = std::unique_ptr<IVector>(vec1->clone());

    for (size_t idx = 0; idx < SIZEOF_ARR(data1); ++idx)
        assert(vec1->getData()[idx] == vec2->getData()[idx]);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
}
void VecTest::testGetData() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    for (size_t idx = 0; idx < SIZEOF_ARR(data1); ++idx)
        assert(vec1->getData()[idx] == data1[idx]);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
}
void VecTest::testSetData() {
    CREATE_LOGGER
    CREATE_VEC_ONE
    CREATE_VEC_TWO

    vec1->setData(SIZEOF_ARR(data2), data2);

    for (size_t idx = 0; idx < SIZEOF_ARR(data2); ++idx)
        assert(vec1->getData()[idx] == data2[idx]);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    CLEAR_VEC_TWO
}

void VecTest::testGetCord() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    double val;
    vec1->getCord(2, val);
    assert(val == 6);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
}
void VecTest::testSetCord() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    vec1->setCord(1, 45);
    assert(vec1->getData()[1] == 45);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
}
void VecTest::testScale() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    vec1->scale(2);
    for (size_t idx = 0; idx < SIZEOF_ARR(data1); ++idx)
        assert(vec1->getData()[idx] == data1[idx] * 2);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
}
void VecTest::testGetDim() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    assert(vec1->getDim() == 4);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
}

void VecTest::testInc() {
    CREATE_LOGGER
    CREATE_VEC_ONE
    CREATE_VEC_TWO

    vec1->inc(vec2);
    for (size_t idx = 0; idx < SIZEOF_ARR(data1); ++idx)
        assert(vec1->getData()[idx] == data1[idx] + data2[idx]);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    CLEAR_VEC_TWO
}
void VecTest::testDec() {
    CREATE_LOGGER
    CREATE_VEC_ONE
    CREATE_VEC_TWO

    vec1->dec(vec2);
    for (size_t idx = 0; idx < SIZEOF_ARR(data1); ++idx)
        assert(vec1->getData()[idx] == data1[idx] - data2[idx]);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    CLEAR_VEC_TWO
}

void VecTest::testAdd() {
    CREATE_LOGGER
    CREATE_VEC_ONE
    CREATE_VEC_TWO

    IVector *vec3 = IVector::add(vec1, vec2);
    for (size_t idx = 0; idx < SIZEOF_ARR(data1); ++idx)
        assert(vec3->getData()[idx] == data1[idx] + data2[idx]);

    delete vec3;
    CLEAR_LOGGER
    CLEAR_VEC_ONE
    CLEAR_VEC_TWO
}
void VecTest::testSub() {
    CREATE_LOGGER
    CREATE_VEC_ONE
    CREATE_VEC_TWO

    IVector *vec3 = IVector::sub(vec1, vec2);
    for (size_t idx = 0; idx < SIZEOF_ARR(data1); ++idx)
        assert(vec3->getData()[idx] == data1[idx] - data2[idx]);

    delete vec3;
    CLEAR_LOGGER
    CLEAR_VEC_ONE
    CLEAR_VEC_TWO
}

void VecTest::testDot() {
    CREATE_LOGGER
    CREATE_VEC_ONE
    CREATE_VEC_TWO

    assert(IVector::dot(vec1, vec2) == 135);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
    CLEAR_VEC_TWO
}
void VecTest::testEquals() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    IVector *vec2 = vec1->clone();
    assert(IVector::equals(vec1, vec2, IVector::NORM::FIRST, TOLERANCE));

    delete vec2;
    CLEAR_LOGGER
    CLEAR_VEC_ONE
}

void VecTest::testFirstNorm() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    assert(vec1->norm(IVector::NORM::FIRST) == 21);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
}
void VecTest::testSecondNorm() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    assert(fabs(vec1->norm(IVector::NORM::SECOND) - 11.81101181) < TOLERANCE);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
}
void VecTest::testChebyshevNorm() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    assert(vec1->norm(IVector::NORM::CHEBYSHEV) == 8.5);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
}

void VecTest::testApplyFunc() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    vec1->applyFunction([](double val) { return val + 1; });
    for (size_t idx = 0; idx < SIZEOF_ARR(data1); ++idx)
        assert(vec1->getData()[idx] == data1[idx] + 1);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
}
void VecTest::testForeach() {
    CREATE_LOGGER
    CREATE_VEC_ONE

    vec1->foreach ([](double val) { return; });
    for (size_t idx = 0; idx < SIZEOF_ARR(data1); ++idx)
        assert(vec1->getData()[idx] == data1[idx]);

    CLEAR_LOGGER
    CLEAR_VEC_ONE
}

void VecTest::testAll() {
    std::cout << "Running all Vector tests" << std::endl;

    testCreate();
    testCopy();
    testMove();
    testClone();
    testGetData();
    testSetData();
    testGetCord();
    testSetCord();
    testScale();
    testGetDim();
    testInc();
    testDec();
    testAdd();
    testSub();
    testDot();
    testEquals();
    testFirstNorm();
    testSecondNorm();
    testChebyshevNorm();
    testApplyFunc();
    testForeach();

    std::cout << "Successfully ran all Vector tests" << std::endl;
}