#include "CompactImpl.h"

#include <cmath>

ILogger *CompactImpl::logger = nullptr;

CompactImpl::CompactImpl(const IVector *left, const IVector *right, const IMultiIndex *nodes) {
    left_boundary = left->clone();
    right_boundary = right->clone();
    grid = nodes->clone();
    dim = left_boundary->getDim();
    control_block = new CompactImplControlBlock(this);
}

CompactImpl::~CompactImpl() {
    delete left_boundary;
    delete right_boundary;
    delete grid;
    delete control_block;
}

ICompact *CompactImpl::createCompact(IVector const *vec1, IVector const *vec2, IMultiIndex const *nodeQuantities) {
    if (vec1 == nullptr || vec2 == nullptr || nodeQuantities == nullptr) {
        logger->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    if (vec1->getDim() != vec2->getDim() || vec2->getDim() != nodeQuantities->getDim()) {
        logger->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    return new (std::nothrow) CompactImpl(vec1, vec2, nodeQuantities);
}

RC CompactImpl::setLogger(ILogger *const pLogger) {
    if (pLogger == nullptr)
        return RC::NULLPTR_ERROR;

    logger = pLogger;
    return RC::SUCCESS;
}

ILogger *CompactImpl::getLogger() { return logger; }

ICompact *CompactImpl::clone() const { return createCompact(left_boundary, right_boundary, grid); }

bool CompactImpl::isInside(IVector const *const &vec) const {
    if (vec == nullptr) {
        logger->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return false;
    }
    if (vec->getDim() != dim) {
        logger->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return false;
    }

    const double *left_data = left_boundary->getData();
    const double *right_data = right_boundary->getData();
    const double *vec_data = vec->getData();

    for (size_t idx = 0; idx < dim; ++idx)
        if (vec_data[idx] > right_data[idx] || vec_data[idx] < left_data[idx])
            return false;
    return true;
}

RC CompactImpl::getLeftBoundary(IVector *&vec) const {
    IVector *copy = left_boundary->clone();
    vec = copy;
    if (copy == nullptr) {
        logger->warning(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    return RC::SUCCESS;
}
RC CompactImpl::getRightBoundary(IVector *&vec) const {
    IVector *copy = right_boundary->clone();
    vec = copy;
    if (copy == nullptr) {
        logger->warning(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    return RC::SUCCESS;
}

size_t CompactImpl::getDim() const { return dim; }

IMultiIndex *CompactImpl::getGrid() const { return grid->clone(); }

RC CompactImpl::getVectorCopy(IMultiIndex const *index, IVector *&val) const {
    double *empty_data = new double[dim];
    IVector *tmp = IVector::createVector(dim, empty_data);
    delete[] empty_data;

    if (tmp == nullptr) {
        logger->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        val = nullptr;
        return RC::NULLPTR_ERROR;
    }

    RC err = getVectorCoords(index, tmp);
    if (err != RC::SUCCESS) {
        logger->severe(err, __FILE__, __func__, __LINE__);
        val = nullptr;
        delete tmp;
        return err;
    }

    val = tmp;
    return RC::SUCCESS;
}

RC CompactImpl::getVectorCoords(IMultiIndex const *index, IVector *const &val) const {
    if (index == nullptr) {
        logger->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    if (index->getDim() != dim || val->getDim() != dim) {
        logger->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    const size_t *index_data = index->getData();
    const size_t *grid_data = grid->getData();
    for (size_t idx = 0; idx < dim; ++idx)
        if (index_data[idx] > grid_data[idx]) {
            logger->severe(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
            return RC::INDEX_OUT_OF_BOUND;
        }

    const double *left_data = left_boundary->getData();
    const double *right_data = right_boundary->getData();
    for (size_t idx = 0; idx < dim; ++idx) {
        double lambda = (double)(index_data[idx]) / (grid_data[idx] - 1);
        val->setCord(idx, (1.0 - lambda) * left_data[idx] + lambda * right_data[idx]);
    }

    return RC::SUCCESS;
}

ICompact *ICompact::createCompact(IVector const *vec1, IVector const *vec2, IMultiIndex const *nodeQuantities) {
    return CompactImpl::createCompact(vec1, vec2, nodeQuantities);
}

RC ICompact::setLogger(ILogger *const logger) { return CompactImpl::setLogger(logger); }

ILogger *ICompact::getLogger() { return CompactImpl::getLogger(); }

static bool checkIntersection(const double arr1[], const double arr2[], double& resLeft, double& resRight)
{
    if (arr1[1] < arr2[0] || arr2[1] < arr1[0])
    {
        resLeft = 0;
        resRight = 0;
        return false;
    }
    if (arr2[0] < arr1[0] && arr1[1] < arr2[1])
    {
        resLeft = arr1[0];
        resRight = arr1[1];
        return true;
    }
    if (arr1[0] < arr2[0] && arr2[1] < arr1[1])
    {
        resLeft = arr2[0];
        resRight = arr2[1];
        return true;
    }
    if (arr1[0] < arr2[0] && arr2[0] < arr1[1] && arr1[1] < arr2[1])
    {
        resLeft = arr2[0];
        resRight = arr1[1];
        return true;
    }
    if (arr2[0] < arr1[0] && arr1[0] < arr2[1] && arr2[1] < arr1[1])
    {
        resLeft = arr1[0];
        resRight = arr2[1];
        return true;
    }
    else 
        return false;
}


ICompact *ICompact::createIntersection(ICompact const *op1, ICompact const *op2, IMultiIndex const *const grid, double tol)
{
    if (op1 == nullptr || op2 == nullptr || grid == nullptr) {
        getLogger()->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    if (std::isnan(tol) || std::isinf(tol)) {
        getLogger()->severe(RC::NOT_NUMBER, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    if (op1->getDim() != op2->getDim() || op1->getDim() != grid->getDim()) {
        getLogger()->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return nullptr;
    }
/*
    const size_t projection = 2;
    size_t dim = op1->getDim();
    IVector *com1_left_bound, *com1_right_bound, *com2_left_bound, *com2_right_bound;

    RC err = op1->getLeftBoundary(com1_left_bound);
    if (err != RC::SUCCESS) {
        getLogger()->severe(err, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    err = op1->getRightBoundary(com1_right_bound);
    if (err != RC::SUCCESS) {
        delete com1_left_bound;
        getLogger()->severe(err, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    err = op2->getLeftBoundary(com2_left_bound);
    if (err != RC::SUCCESS) {
        delete com1_left_bound;
        delete com1_right_bound;
        getLogger()->severe(err, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    err = op2->getRightBoundary(com2_right_bound);
    if (err != RC::SUCCESS) {
        delete com1_left_bound;
        delete com1_right_bound;
        delete com2_left_bound;
        getLogger()->severe(err, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    IMultiIndex *grid_copy = grid->clone();
    if (grid_copy == nullptr) {
        delete com1_left_bound;
        delete com1_right_bound;
        delete com2_left_bound;
        delete com2_right_bound;
        getLogger()->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    double left_data1, right_data1, left_data2, right_data2, res_left[dim], res_right[dim];

    for (size_t idx = 0; idx < dim; ++idx) {
        left_data1 = com1_left_bound->getData()[idx];
        right_data1 = com1_right_bound->getData()[idx];
        left_data2 = com2_left_bound->getData()[idx];
        right_data2 = com2_right_bound->getData()[idx];
        if (!intersectionPossible(left_data1, right_data1, left_data2, right_data2, res_left[idx], res_right[idx])) {
            delete com1_left_bound;
            delete com1_right_bound;
            delete com2_left_bound;
            delete com2_right_bound;
            delete grid_copy;
            return nullptr;
        }

        double tmp_data[projection] = {res_left[idx], res_right[idx]};
        IVector *tmp_inter = IVector::createVector(projection, tmp_data);
        if (tmp_inter == nullptr) {
            delete com1_left_bound;
            delete com1_right_bound;
            delete com2_left_bound;
            delete com2_right_bound;
            delete grid_copy;
            getLogger()->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
            return nullptr;
        }
        if (tmp_inter->norm(IVector::NORM::FIRST) < tol) {
            grid_copy->setAxisIndex(idx, 1);
        }
        delete tmp_inter;
    }

    IVector *new_left = IVector::createVector(dim, res_left);
    if (new_left == nullptr) {
        delete com1_left_bound;
        delete com1_right_bound;
        delete com2_left_bound;
        delete com2_right_bound;
        delete grid_copy;
        getLogger()->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    IVector *new_right = IVector::createVector(dim, res_right);
    if (new_right == nullptr) {
        delete com1_left_bound;
        delete com1_right_bound;
        delete com2_left_bound;
        delete com2_right_bound;
        delete grid_copy;
        delete new_left;
        getLogger()->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    ICompact *inter = ICompact::createCompact(new_left, new_right, grid_copy);
    if (inter == nullptr) {
        delete com1_left_bound;
        delete com1_right_bound;
        delete com2_left_bound;
        delete com2_right_bound;
        delete grid_copy;
        delete new_left;
        delete new_right;
        getLogger()->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    delete com1_left_bound;
    delete com1_right_bound;
    delete com2_left_bound;
    delete com2_right_bound;
    delete grid_copy;
    delete new_left;
    delete new_right;

    return inter;*/        return nullptr;

}

ICompact *ICompact::createCompactSpan(ICompact const *op1, ICompact const *op2, IMultiIndex const *const grid)
{/*
    if (op1 == nullptr || op2 == nullptr || grid == nullptr) {
        getLogger()->warning(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    if (op1->getDim() != op2->getDim()) {
        getLogger()->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    auto checkUnion = [](const double *arr, double &resLeft, double &resRight) -> void {
        resLeft = arr[0];
        resRight = arr[0];
        for (size_t i = 1; i < 4; ++i) {
            if (arr[i] < resLeft) {
                resLeft = arr[i];
            }
            if (arr[i] > resRight) {
                resRight = arr[i];
            }
        }
    };

    const size_t projection = 2;
    size_t dim = op1->getDim();
    IVector *com1_left_bound, *com1_right_bound, *com2_left_bound, *com2_right_bound;

    RC err = op1->getLeftBoundary(com1_left_bound);
    if (err != RC::SUCCESS) {
        getLogger()->severe(err, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    err = op1->getRightBoundary(com1_right_bound);
    if (err != RC::SUCCESS) {
        delete com1_left_bound;
        getLogger()->severe(err, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    err = op2->getLeftBoundary(com2_left_bound);
    if (err != RC::SUCCESS) {
        delete com1_left_bound;
        delete com1_right_bound;
        getLogger()->severe(err, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    err = op2->getRightBoundary(com2_right_bound);
    if (err != RC::SUCCESS) {
        delete com1_left_bound;
        delete com1_right_bound;
        delete com2_left_bound;
        getLogger()->severe(err, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    double res_left[dim], res_right[dim];
    for (size_t idx = 0; idx < dim; ++idx) {
        double tmp[] = {com1_left_bound->getData()[idx], com1_right_bound->getData()[idx],
                        com1_left_bound->getData()[idx], com2_right_bound->getData()[idx]};
        checkUnion(tmp, res_left[idx], res_right[idx]);
    }
    IVector *new_left = IVector::createVector(dim, res_left);
    if (new_left == nullptr) {
        delete com1_left_bound;
        delete com1_right_bound;
        delete com2_left_bound;
        delete com2_right_bound;
        getLogger()->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    IVector *new_right = IVector::createVector(dim, res_right);
    if (new_right == nullptr) {
        delete com1_left_bound;
        delete com1_right_bound;
        delete com2_left_bound;
        delete com2_right_bound;
        delete new_left;
        getLogger()->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    ICompact *union_res = ICompact::createCompact(new_left, new_right, grid);
    if (union_res == nullptr) {
        delete com1_left_bound;
        delete com1_right_bound;
        delete com2_left_bound;
        delete com2_right_bound;
        delete new_left;
        delete new_right;
        getLogger()->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    delete com1_left_bound;
    delete com1_right_bound;
    delete com2_left_bound;
    delete com2_right_bound;
    delete new_left;
    delete new_right;

    return union_res;*/        return nullptr;

}

ICompact::~ICompact() = default;
