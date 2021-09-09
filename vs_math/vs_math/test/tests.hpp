#pragma once

#include "IVector.h"
#include "ILogger.h"
#include "ISet.h"

#define TOLERANCE 1e-6
#define DEFAULT_NORM IVector::NORM::SECOND

#define CREATE_LOGGER ILogger* logger = ILogger::createLogger(); IVector::setLogger(logger); ISet::setLogger(logger); 
#define CREATE_VEC_ONE std::array<double, 4> data1({1, 5.5, 6, 8.5}); IVector* vec1 = IVector::createVector(data1.size(), data1.data());
#define CREATE_VEC_TWO std::array<double, 4> data2({5, 3, 9, 7}); IVector* vec2 = IVector::createVector(data2.size(), data2.data());
#define CREATE_VEC_THREE std::array<double, 3> data3({1, 10, 100}); IVector* vec3 = IVector::createVector(data3.size(), data3.data());
#define CREATE_VEC_FOUR std::array<double, 4> data4({67, 45, 10, 2}); IVector* vec4 = IVector::createVector(data4.size(), data4.data());
#define CREATE_SET_ONE ISet* set1 = ISet::createSet();
#define CREATE_SET_TWO ISet* set2 = ISet::createSet();
#define CREATE_SET_THREE ISet* set3 = ISet::createSet();
#define CREATE_ALL CREATE_LOGGER CREATE_VEC_ONE CREATE_VEC_TWO CREATE_VEC_THREE CREATE_SET_ONE CREATE_SET_TWO CREATE_SET_THREE CREATE_VEC_FOUR

#define CLEAR_VEC_ONE delete vec1;
#define CLEAR_VEC_TWO delete vec2;
#define CLEAR_VEC_THREE delete vec3;
#define CLEAR_VEC_FOUR delete vec4;
#define CLEAR_SET_ONE delete set1;
#define CLEAR_SET_TWO delete set2;
#define CLEAR_SET_THREE delete set3;
#define CLEAR_LOGGER delete logger;
#define CLEAR_ALL CLEAR_LOGGER CLEAR_VEC_ONE CLEAR_VEC_TWO CLEAR_VEC_THREE CLEAR_SET_ONE CLEAR_SET_TWO CLEAR_SET_THREE CLEAR_VEC_FOUR

namespace VecTest {
    void testCreate();

    void testCopy();
    void testMove();

    void testClone();
    void testGetData();
    void testSetData();

    void testGetCord();
    void testSetCord();
    void testScale();
    void testGetDim();

    void testInc();
    void testDec();

    void testAdd();
    void testSub();

    void testDot();
    void testEquals();

    void testFirstNorm();
    void testSecondNorm();
    void testChebyshevNorm();

    void testApplyFunc();
    void testForeach();
    
    void testAll();
};

namespace SetTest {
    void testCreate();

    void testGetDim();
    void testGetSize();
    void testGetCopy();
    void testGetCoords();

    void testInsert();
    void testRemoveByIndex();
    void testRemoveByPattern();

    void testClone();

    void testFindFirst();
    void testFindFirstAndCopy();
    void testFindFirstAndCopyCoords();

    void testMakeIntersection();
    void testMakeUnion();
    void testSub();
    void testSymSub();
    void testEquals();
    void testSubSet();

    void testAll();
};