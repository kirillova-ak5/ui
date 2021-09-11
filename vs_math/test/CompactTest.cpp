#include "tests.hpp"
#include <cassert>
#include <iostream>

void CompactTest::testCreate() {
    CREATE_ALL

    assert(com1 != nullptr);

    CLEAR_ALL
}
void CompactTest::testClone() {
    CREATE_ALL
    ICompact *tmp = com1->clone();

    assert(tmp != nullptr);

    delete tmp;
    CLEAR_ALL
}

void CompactTest::testGetDim() {
    CREATE_ALL

    assert(com1->getDim() == left_bound1->getDim());

    CLEAR_ALL
}
void CompactTest::testGetGrid() {
    CREATE_ALL
    IMultiIndex *tmp = com1->getGrid();

    const size_t *tmp_data = tmp->getData();
    const size_t *index_data = grid1->getData();
    for (size_t idx = 0; idx < com1->getDim(); ++idx)
        assert(tmp_data[idx] == index_data[idx]);

    delete tmp;
    CLEAR_ALL
}
void CompactTest::testGetLeftBoundary() {
    CREATE_ALL
    IVector *left = nullptr;

    RC err = com1->getLeftBoundary(left);
    assert(err == RC::SUCCESS);
    assert(IVector::equals(left_bound1, left, DEFAULT_NORM, TOLERANCE));

    delete left;
    CLEAR_ALL
}
void CompactTest::testGetRightBoundary() {
    CREATE_ALL
    IVector *right = nullptr;

    RC err = com1->getRightBoundary(right);
    assert(err == RC::SUCCESS);
    assert(IVector::equals(right_bound1, right, DEFAULT_NORM, TOLERANCE));

    delete right;
    CLEAR_ALL
}
void CompactTest::testGetVectorCoords() {
    CREATE_ALL
    double *empty_data = new double[com1->getDim()];
    IVector *tmp = IVector::createVector(com1->getDim(), empty_data);
    size_t index_data[] = {3, 3};
    IMultiIndex *test_idx = IMultiIndex::createMultiIndex(SIZEOF_ARR(index_data), index_data);

    double expected_vec_data[] = {3, 3};
    RC err = com1->getVectorCoords(test_idx, tmp);
    for (size_t idx = 0; idx < SIZEOF_ARR(expected_vec_data); ++idx)
        assert(tmp->getData()[idx] == expected_vec_data[idx]);

    delete[] empty_data;
    delete tmp;
    delete test_idx;
    CLEAR_ALL
}
void CompactTest::testGetVectorCopy() {
    CREATE_ALL
    IVector *tmp;
    size_t index_data[] = {3, 3};
    IMultiIndex *test_idx = IMultiIndex::createMultiIndex(SIZEOF_ARR(index_data), index_data);

    double expected_vec_data[] = {3, 3};
    RC err = com1->getVectorCopy(test_idx, tmp);
    for (size_t idx = 0; idx < SIZEOF_ARR(expected_vec_data); ++idx)
        assert(tmp->getData()[idx] == expected_vec_data[idx]);

    delete tmp;
    delete test_idx;
    CLEAR_ALL
}

void CompactTest::testIsInside() {
    CREATE_LOGGER
    CREATE_COM_ONE
    double tmp_vec_data[] = {2.5, 4.3};
    IVector *tmp = IVector::createVector(SIZEOF_ARR(tmp_vec_data), tmp_vec_data);

    assert(com1->isInside(tmp));

    delete tmp;
    CLEAR_LOGGER
    CLEAR_COM_ONE
}

void CompactTest::testIntersection() {
    CREATE_LOGGER
    CREATE_COM_ONE
    CREATE_COM_TWO

    ICompact *inter = ICompact::createIntersection(com1, com2, grid1, TOLERANCE);
    assert(inter != nullptr);

    IVector *inter_left;
    RC err = inter->getLeftBoundary(inter_left);
    assert(err == RC::SUCCESS);
    assert(IVector::equals(inter_left, left_bound2, DEFAULT_NORM, TOLERANCE));

    IVector *inter_right;
    err = inter->getRightBoundary(inter_right);
    assert(err == RC::SUCCESS);
    assert(IVector::equals(inter_right, right_bound1, DEFAULT_NORM, TOLERANCE));

    IMultiIndex *inter_grid = inter->getGrid();
    assert(inter_grid != nullptr);
    for (size_t idx = 0; idx < inter_grid->getDim(); ++idx)
        assert(inter_grid->getData()[idx] == grid1->getData()[idx]);

    delete inter_grid;
    delete inter_left;
    delete inter_right;
    delete inter;
    CLEAR_COM_TWO
    CLEAR_COM_ONE
    CLEAR_LOGGER
}
void CompactTest::testSpan() {
    CREATE_LOGGER
    CREATE_COM_ONE
    CREATE_COM_TWO

    ICompact *span = ICompact::createCompactSpan(com1, com2, grid1);
    assert(span != nullptr);

    IVector *span_left;
    RC err = span->getLeftBoundary(span_left);
    assert(err == RC::SUCCESS);
    assert(IVector::equals(span_left, left_bound1, DEFAULT_NORM, TOLERANCE));

    IVector *span_right;
    err = span->getRightBoundary(span_right);
    assert(err == RC::SUCCESS);
    assert(IVector::equals(span_right, right_bound2, DEFAULT_NORM, TOLERANCE));

    IMultiIndex *span_grid = span->getGrid();
    assert(span_grid != nullptr);
    for (size_t idx = 0; idx < span_grid->getDim(); ++idx)
        assert(span_grid->getData()[idx] == grid1->getData()[idx]);

    delete span_grid;
    delete span_left;
    delete span_right;
    delete span;
    CLEAR_COM_TWO
    CLEAR_COM_ONE
    CLEAR_LOGGER
}

void CompactTest::testIterator() {
    CREATE_LOGGER
    CREATE_COM_ONE

    IVector *iter_vec = nullptr;
    size_t order_data[] = {0, 1};
    IMultiIndex *order = IMultiIndex::createMultiIndex(SIZEOF_ARR(order_data), order_data);
    ICompact::IIterator *iter = com1->getBegin(order);

    for (; iter->isValid(); iter->next()) {
        RC err = iter->getVectorCopy(iter_vec);
        assert(err == RC::SUCCESS);
        assert(iter_vec != nullptr);
        delete iter_vec;
        iter_vec = nullptr;
    }

    delete iter;
    delete order;
    CLEAR_COM_ONE
    CLEAR_LOGGER
}

void CompactTest::testAll() {
    std::cout << "Running all Compact tests" << std::endl;

    testCreate();
    testClone();
    testGetDim();
    testGetGrid();
    testGetLeftBoundary();
    testGetRightBoundary();
    testGetVectorCoords();
    testGetVectorCopy();
    testIsInside();
    testIntersection();
    testSpan();
    testIterator();

    std::cout << "Successfully ran all Compact tests" << std::endl;
}