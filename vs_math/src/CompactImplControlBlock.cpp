#include "CompactImplControlBlock.h"
#include "CompactImpl.h"

CompactImplControlBlock::CompactImplControlBlock(CompactImpl *com) : compact(com) {}

RC CompactImplControlBlock::get(IMultiIndex *const &currentIndex, IMultiIndex const *const &bypassOrder) const {
    if (currentIndex == nullptr || bypassOrder == nullptr) {
        ICompact::getLogger()->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    if (currentIndex->getDim() != bypassOrder->getDim()) {
        ICompact::getLogger()->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    IMultiIndex *compact_grid = compact->getGrid();
    if (compact_grid == nullptr) {
        ICompact::getLogger()->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return RC::ALLOCATION_ERROR;
    }
    const size_t *compact_grid_data = compact_grid->getData();
    const size_t *order_data = bypassOrder->getData();
    const size_t *index_data = currentIndex->getData();

    for (size_t idx = 0; idx < bypassOrder->getDim(); ++idx) {
        size_t odidx = order_data[idx];
        if (index_data[odidx] == compact_grid_data[odidx]) {
            currentIndex->setAxisIndex(odidx, 1);
        } else if (index_data[odidx] < compact_grid_data[odidx]) {
            currentIndex->incAxisIndex(odidx, 1);
            delete compact_grid;
            return RC::SUCCESS;
        }
    }
    ICompact::getLogger()->severe(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
    delete compact_grid;
    return RC::INDEX_OUT_OF_BOUND;
}

RC CompactImplControlBlock::get(IMultiIndex const *const &currentIndex, IVector *const &val) const {
    if (currentIndex == nullptr || val == nullptr) {
        ICompact::getLogger()->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    IMultiIndex *compact_grid = compact->getGrid();
    if (compact_grid == nullptr) {
        ICompact::getLogger()->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return RC::ALLOCATION_ERROR;
    }
    if (currentIndex->getDim() != compact_grid->getDim()) {
        ICompact::getLogger()->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        delete compact_grid;
        return RC::MISMATCHING_DIMENSIONS;
    }
    for (size_t idx = 0; idx < compact->getDim(); ++idx)
        if (currentIndex->getData()[idx] > compact_grid->getData()[idx]) {
            ICompact::getLogger()->severe(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
            delete compact_grid;
            return RC::INDEX_OUT_OF_BOUND;
        }
    delete compact_grid;

    IVector *vec;
    RC err = compact->getVectorCopy(currentIndex, vec);
    if (err != RC::SUCCESS) {
        ICompact::getLogger()->severe(err, __FILE__, __func__, __LINE__);
        return err;
    }
    val->setData(currentIndex->getDim(), vec->getData());
    delete vec;
    return RC::SUCCESS;
}

ICompactControlBlock::~ICompactControlBlock() = default;