#pragma once
#include "ICompact.h"
#include "ILogger.h"
#include "IMultiIndex.h"
#include "ISet.h"
#include "IVector.h"

#define TOLERANCE 1e-6
#define DEFAULT_NORM IVector::NORM::SECOND

#define SIZEOF_ARR(arr) (sizeof(arr) / sizeof((arr)[0]))

#define CREATE_LOGGER                                                                                                  \
    ILogger *logger = ILogger::createLogger();                                                                         \
    IVector::setLogger(logger);                                                                                        \
    ISet::setLogger(logger);                                                                                           \
    IMultiIndex::setLogger(logger);                                                                                    \
    ICompact::setLogger(logger);
#define CREATE_VEC_ONE                                                                                                 \
    double data1[] = {1, 5.5, 6, 8.5};                                                                                 \
    IVector *vec1 = IVector::createVector(SIZEOF_ARR(data1), data1);
#define CREATE_VEC_TWO                                                                                                 \
    double data2[] = {5, 3, 9, 7};                                                                                     \
    IVector *vec2 = IVector::createVector(SIZEOF_ARR(data2), data2);
#define CREATE_VEC_THREE                                                                                               \
    double data3[] = {1, 10, 100};                                                                                     \
    IVector *vec3 = IVector::createVector(SIZEOF_ARR(data3), data3);
#define CREATE_VEC_FOUR                                                                                                \
    double data4[] = {67, 45, 10, 2};                                                                                  \
    IVector *vec4 = IVector::createVector(SIZEOF_ARR(data4), data4);
#define CREATE_SET_ONE ISet *set1 = ISet::createSet();
#define CREATE_SET_TWO ISet *set2 = ISet::createSet();
#define CREATE_SET_THREE ISet *set3 = ISet::createSet();
#define CREATE_INDEX_ONE                                                                                               \
    size_t idata1[] = {34, 17, 42, 2};                                                                                 \
    IMultiIndex *index1 = IMultiIndex::createMultiIndex(SIZEOF_ARR(idata1), idata1);
#define CREATE_INDEX_TWO                                                                                               \
    size_t idata2[] = {12, 76, 44, 89};                                                                                \
    IMultiIndex *index2 = IMultiIndex::createMultiIndex(SIZEOF_ARR(idata2), idata2);
#define CREATE_COM_ONE                                                                                                 \
    double left_bound_data1[] = {0, 0};                                                                                \
    IVector *left_bound1 = IVector::createVector(SIZEOF_ARR(left_bound_data1), left_bound_data1);                      \
    double right_bound_data1[] = {5, 5};                                                                               \
    IVector *right_bound1 = IVector::createVector(SIZEOF_ARR(right_bound_data1), right_bound_data1);                   \
    size_t grid_data1[] = {6, 6};                                                                                      \
    IMultiIndex *grid1 = IMultiIndex::createMultiIndex(SIZEOF_ARR(grid_data1), grid_data1);                            \
    ICompact *com1 = ICompact::createCompact(left_bound1, right_bound1, grid1);
#define CREATE_COM_TWO                                                                                                 \
    double left_bound_data2[] = {3, 3};                                                                                \
    IVector *left_bound2 = IVector::createVector(SIZEOF_ARR(left_bound_data2), left_bound_data2);                      \
    double right_bound_data2[] = {8, 8};                                                                               \
    IVector *right_bound2 = IVector::createVector(SIZEOF_ARR(right_bound_data2), right_bound_data2);                   \
    size_t grid_data2[] = {6, 6};                                                                                      \
    IMultiIndex *grid2 = IMultiIndex::createMultiIndex(SIZEOF_ARR(grid_data2), grid_data2);                            \
    ICompact *com2 = ICompact::createCompact(left_bound2, right_bound2, grid2);
#define CREATE_ALL                                                                                                     \
    CREATE_LOGGER CREATE_VEC_ONE CREATE_VEC_TWO CREATE_VEC_THREE CREATE_SET_ONE CREATE_SET_TWO CREATE_SET_THREE        \
        CREATE_VEC_FOUR CREATE_INDEX_ONE CREATE_INDEX_TWO CREATE_COM_ONE CREATE_COM_TWO

#define CLEAR_LOGGER delete logger;
#define CLEAR_VEC_ONE delete vec1;
#define CLEAR_VEC_TWO delete vec2;
#define CLEAR_VEC_THREE delete vec3;
#define CLEAR_VEC_FOUR delete vec4;
#define CLEAR_SET_ONE delete set1;
#define CLEAR_SET_TWO delete set2;
#define CLEAR_SET_THREE delete set3;
#define CLEAR_INDEX_ONE delete index1;
#define CLEAR_INDEX_TWO delete index2;
#define CLEAR_COM_ONE                                                                                                  \
    delete left_bound1;                                                                                                \
    delete right_bound1;                                                                                               \
    delete grid1;                                                                                                      \
    delete com1;
#define CLEAR_COM_TWO                                                                                                  \
    delete left_bound2;                                                                                                \
    delete right_bound2;                                                                                               \
    delete grid2;                                                                                                      \
    delete com2;
#define CLEAR_ALL                                                                                                      \
    CLEAR_LOGGER CLEAR_VEC_ONE CLEAR_VEC_TWO CLEAR_VEC_THREE CLEAR_SET_ONE CLEAR_SET_TWO CLEAR_SET_THREE               \
        CLEAR_VEC_FOUR CLEAR_INDEX_ONE CLEAR_INDEX_TWO CLEAR_COM_ONE CLEAR_COM_TWO

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
}; // namespace VecTest

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
}; // namespace SetTest

namespace MultiIndexTest {
void testCreate();
void testClone();

void testGetDim();
void testGetAxisIndex();
void testGetData();

void testSetData();
void testSetAxisIndex();
void testIncAxisIndex();

void testAll();
}; // namespace MultiIndexTest

namespace CompactTest {
void testCreate();
void testClone();

void testGetDim();
void testGetGrid();
void testGetLeftBoundary();
void testGetRightBoundary();

void testGetVectorCoords();
void testGetVectorCopy();
void testIsInside();

void testIntersection();
void testSpan();

void testIterator();

void testAll();
}; // namespace CompactTest