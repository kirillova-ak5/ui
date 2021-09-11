#include "CompactImpl.h"

ILogger *CompactImpl::IteratorImpl::logger = nullptr;

CompactImpl::IteratorImpl::IteratorImpl(IVector *vec, IMultiIndex *idx, IMultiIndex *bypass_order,
                                        CompactImplControlBlock *cb) {
    vector = vec;
    index = idx;
    order = bypass_order;
    control_block = cb;
    valid = true;
}

CompactImpl::IteratorImpl::~IteratorImpl() {
    delete vector;
    delete index;
    delete order;
}

ILogger *CompactImpl::IteratorImpl::getLogger() { return logger; }

RC CompactImpl::IteratorImpl::setLogger(ILogger *const pLogger) {
    if (pLogger == nullptr)
        return RC::NULLPTR_ERROR;

    logger = pLogger;
    return RC::SUCCESS;
}

ICompact::IIterator *CompactImpl::IteratorImpl::getNext() {
    IIterator *copy = clone();
    if (copy == nullptr) {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    RC err = copy->next();
    if (err != RC::SUCCESS) {
        logger->severe(err, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    return copy;
}

ICompact::IIterator *CompactImpl::IteratorImpl::clone() const {
    IVector *vec_copy = vector->clone();
    if (vec_copy == nullptr) {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    IMultiIndex *index_copy = index->clone();
    if (index_copy == nullptr) {
        delete vec_copy;
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    IMultiIndex *order_copy = order->clone();
    if (order_copy == nullptr) {
        delete vec_copy;
        delete index_copy;
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    IIterator *iter_copy = new (std::nothrow) IteratorImpl(vec_copy, index_copy, order_copy, control_block);
    if (iter_copy == nullptr) {
        delete vec_copy;
        delete index_copy;
        delete order_copy;
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    return iter_copy;
}

RC CompactImpl::IteratorImpl::next() {
    RC err = control_block->get(index, order);
    if (err != RC::SUCCESS) {
        if (err == RC::INDEX_OUT_OF_BOUND)
            valid = false;
        return err;
    }
    return control_block->get(index, vector);
}

bool CompactImpl::IteratorImpl::isValid() const { return valid; }

RC CompactImpl::IteratorImpl::getVectorCopy(IVector *&val) const {
    val = vector->clone();
    if (val == nullptr) {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return RC::ALLOCATION_ERROR;
    }
    return RC::SUCCESS;
}

RC CompactImpl::IteratorImpl::getVectorCoords(IVector *const &val) const {
    if (val == nullptr) {
        logger->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    if (val->getDim() != vector->getDim()) {
        logger->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }
    return val->setData(vector->getDim(), vector->getData());
}

ICompact::IIterator *CompactImpl::getIterator(IMultiIndex const *const &index,
                                              IMultiIndex const *const &bypassOrder) const {
    if (index == nullptr || bypassOrder == nullptr) {
        logger->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    if (index->getDim() != bypassOrder->getDim()) {
        logger->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
    }

    IVector *vec_copy = nullptr;
    RC err = getVectorCopy(index, vec_copy);
    if (err != RC::SUCCESS) {
        logger->severe(err, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    IMultiIndex *index_copy = index->clone();
    if (index_copy == nullptr) {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        delete vec_copy;
        return nullptr;
    }
    IMultiIndex *order_copy = bypassOrder->clone();
    if (order_copy == nullptr) {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        delete vec_copy;
        delete index_copy;
        return nullptr;
    }

    IteratorImpl *new_iter = new (std::nothrow) IteratorImpl(vec_copy, index_copy, order_copy, control_block);
    if (new_iter == nullptr) {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        delete vec_copy;
        delete index_copy;
        delete order_copy;
        return nullptr;
    }
    IteratorImpl::setLogger(logger);

    return new_iter;
}

ICompact::IIterator *CompactImpl::getBegin(IMultiIndex const *const &bypassOrder) const {
    size_t *data = new (std::nothrow) size_t[dim];
    if (data == nullptr) {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    std::fill_n(data, dim, 1);
    IMultiIndex *index = IMultiIndex::createMultiIndex(dim, data);
    delete[] data;

    if (index == nullptr) {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    IIterator *new_iter = getIterator(index, bypassOrder);
    delete index;
    return new_iter;
}

ICompact::IIterator *CompactImpl::getEnd(IMultiIndex const *const &bypassOrder) const {
    size_t *data = new (std::nothrow) size_t[dim];
    if (data == nullptr) {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    const size_t *grid_data = grid->getData();
    for (size_t idx = 0; idx < dim; ++idx)
        data[idx] = grid_data[idx];
    IMultiIndex *index = IMultiIndex::createMultiIndex(dim, data);
    delete[] data;

    if (index == nullptr) {
        logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    IIterator *new_iter = getIterator(index, bypassOrder);
    delete index;
    return new_iter;
}

ILogger *ICompact::IIterator::getLogger() { return CompactImpl::IteratorImpl::getLogger(); }

RC ICompact::IIterator::setLogger(ILogger *const logger) { return CompactImpl::IteratorImpl::setLogger(logger); }

ICompact::IIterator::~IIterator() = default;
