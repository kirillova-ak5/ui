#include "tests.hpp"
#include <cassert>
#include <iostream>

void MultiIndexTest::testCreate() {
    CREATE_LOGGER
    CREATE_INDEX_ONE

    assert(index1 != nullptr);

    IMultiIndex *tmp = IMultiIndex::createMultiIndex(1, nullptr);
    assert(tmp == nullptr);

    delete tmp;
    CLEAR_LOGGER
    CLEAR_INDEX_ONE
}

void MultiIndexTest::testClone() {
    CREATE_LOGGER
    CREATE_INDEX_ONE

    IMultiIndex *index2 = index1->clone();

    for (size_t idx = 0; idx < SIZEOF_ARR(idata1); ++idx)
        assert(index1->getData()[idx] == index2->getData()[idx]);

    delete index2;
    CLEAR_LOGGER
    CLEAR_INDEX_ONE
}
void MultiIndexTest::testGetData() {
    CREATE_LOGGER
    CREATE_INDEX_ONE

    for (size_t idx = 0; idx < SIZEOF_ARR(idata1); ++idx)
        assert(index1->getData()[idx] == idata1[idx]);

    CLEAR_LOGGER
    CLEAR_INDEX_ONE
}
void MultiIndexTest::testSetData() {
    CREATE_LOGGER
    CREATE_INDEX_ONE
    CREATE_INDEX_TWO

    index1->setData(SIZEOF_ARR(idata2), idata2);

    for (size_t idx = 0; idx < SIZEOF_ARR(idata2); ++idx)
        assert(index1->getData()[idx] == idata2[idx]);

    CLEAR_LOGGER
    CLEAR_INDEX_ONE
    CLEAR_INDEX_TWO
}

void MultiIndexTest::testGetAxisIndex() {
    CREATE_LOGGER
    CREATE_INDEX_ONE

    size_t val;
    index1->getAxisIndex(1, val);
    assert(val == idata1[1]);

    CLEAR_LOGGER
    CLEAR_INDEX_ONE
}
void MultiIndexTest::testSetAxisIndex() {
    CREATE_LOGGER
    CREATE_INDEX_ONE

    index1->setAxisIndex(1, 45);
    assert(index1->getData()[1] == 45);

    CLEAR_LOGGER
    CLEAR_INDEX_ONE
}

void MultiIndexTest::testIncAxisIndex() {
    CREATE_LOGGER
    CREATE_INDEX_ONE

    index1->incAxisIndex(1, 5);
    assert(index1->getData()[1] == idata1[1] + 5);

    CLEAR_LOGGER
    CLEAR_INDEX_ONE
}

void MultiIndexTest::testGetDim() {
    CREATE_LOGGER
    CREATE_INDEX_ONE

    assert(index1->getDim() == SIZEOF_ARR(idata1));

    CLEAR_LOGGER
    CLEAR_INDEX_ONE
}

void MultiIndexTest::testAll() {
    std::cout << "Running all MultiIndex tests" << std::endl;

    testCreate();
    testClone();
    testGetDim();
    testGetAxisIndex();
    testGetData();
    testSetData();
    testSetAxisIndex();
    testIncAxisIndex();

    std::cout << "Successfully ran all MultiIndex tests" << std::endl;
}