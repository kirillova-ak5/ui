#include <cstring>
#include <cmath>
#include "IVector.h"


namespace {
    class VectorImpl : public IVector {
    public:
        static RC setLogger(ILogger* const pLogger) {
            if (pLogger == nullptr)
                return RC::NULLPTR_ERROR;
            
            logger = pLogger;
            return RC::SUCCESS;
        }
        static ILogger* getLogger() {
            return logger;
        }

        static IVector* createVector(size_t dim, double const* const& ptr_data) {
            if (dim == 0) {
                logger->severe(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
                return nullptr;
            }
            if (ptr_data == nullptr) {
                logger->warning(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
                return nullptr;
            }
            
            uint8_t* ptr = new (std::nothrow) uint8_t[sizeof(VectorImpl) + dim * sizeof(double)];
            if (ptr == nullptr) {
                logger->warning(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
                return nullptr;
            }
            IVector* vec = new (ptr) VectorImpl(dim);
            std::memcpy((uint8_t*)(ptr) + sizeof(VectorImpl), ptr_data, dim * sizeof(double));

            return vec;
        }

        IVector* clone() const override {
            return createVector(dim, RawData());
        }
        double const* getData() const override {
            return RawData();
        }
        RC setData(size_t dim, double const* const& ptr_data) override {
            if (this->dim != dim) {
                logger->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
                return RC::MISMATCHING_DIMENSIONS;
            }
            if (ptr_data == nullptr) {
                logger->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
                return RC::NULLPTR_ERROR;
            }
            for (size_t idx = 0; idx < dim; ++idx)
            {
                if (std::isnan(ptr_data[idx]) || std::isinf(ptr_data[idx])) {
                    logger->severe(RC::NOT_NUMBER, __FILE__, __func__, __LINE__);
                    return RC::NOT_NUMBER;
                }
            }

            memcpy(RawData(), ptr_data, dim * sizeof(double));

            return RC::SUCCESS;
        }

        RC getCord(size_t index, double& val) const override {
            if (index >= dim) {
                logger->severe(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
                return RC::INDEX_OUT_OF_BOUND;
            }
            
            val = RawData()[index];
            return RC::SUCCESS;
        }
        RC setCord(size_t index, double val) override {
            if (index >= dim) {
                logger->severe(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
                return RC::INDEX_OUT_OF_BOUND;
            }
            
            RawData()[index] = val;
            return RC::SUCCESS;
        }
        RC scale(double multiplier) override {
            applyFunction([multiplier](double val)->double{ return val * multiplier; });
            return RC::SUCCESS;
        }
        size_t getDim() const override {
            return dim;
        }

        RC inc(IVector const* const& op) override {
            if (op == nullptr) {
                logger->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
                return RC::NULLPTR_ERROR;
            }
            if (op->getDim() != dim) {
                logger->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
                return RC::MISMATCHING_DIMENSIONS;
            }

            for (size_t idx = 0; idx < getDim(); ++idx)
                RawData()[idx] += op->getData()[idx];
            return RC::SUCCESS;
        }
        RC dec(IVector const* const& op) override {
            if (op == nullptr) {
                logger->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
                return RC::NULLPTR_ERROR;
            }
            if (op->getDim() != dim) {
                logger->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
                return RC::MISMATCHING_DIMENSIONS;
            }

            for (size_t idx = 0; idx < getDim(); ++idx)
                RawData()[idx] -= op->getData()[idx];
            return RC::SUCCESS;
        }

        double norm(NORM norm) const override {
            double sum = 0;
            double max = *(RawData());

            switch (norm) {
                case NORM::FIRST:
                    for (size_t idx = 0; idx < dim; ++idx)
                        sum += fabs(RawData()[idx]);
                    return sum;

                case NORM::SECOND:
                    for (size_t idx = 0; idx < dim; ++idx)
                        sum += RawData()[idx] * RawData()[idx];
                    return sqrt(sum);

                case NORM::CHEBYSHEV:
                    for (size_t idx = 1; idx < dim; ++idx)
                        if (RawData()[idx] - max > 1e-6)
                            max = RawData()[idx];
                    return max;

                default:
                    logger->warning(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
                    return 0;
            }
        }

        RC applyFunction(const std::function<double(double)>& fun) override {
            for (size_t idx = 0; idx < dim; ++idx)
                RawData()[idx] = fun(RawData()[idx]);
            return RC::SUCCESS;
        }
        RC foreach(const std::function<void(double)>& fun) const override {
            for (size_t idx = 0; idx < dim; ++idx)
                fun(RawData()[idx]);
            return RC::SUCCESS;
        }

        size_t sizeAllocated() const override {
            return dim * sizeof(double) + sizeof(VectorImpl);
        }

        void operator delete(void* ptr, size_t size) {
            if (ptr)
                delete[] reinterpret_cast<uint8_t*>(ptr);
        }

        ~VectorImpl() = default;
    
    private:
        static ILogger* logger;
        size_t dim;

        inline double* RawData() const
        {
            return (double *)((uint8_t*)(this) + sizeof(VectorImpl));
        }

        VectorImpl(size_t dim) : dim(dim) {}
    };
    ILogger* VectorImpl::logger = nullptr;
};

IVector* IVector::createVector(size_t dim, double const* const& ptr_data) {
    return VectorImpl::createVector(dim, ptr_data);
}

RC IVector::copyInstance(IVector* const dest, IVector const* const& src) {
    if (!(abs((int)((uint8_t*)src - (uint8_t*)dest)) >= src->sizeAllocated())) {
        src->getLogger()->severe(RC::MEMORY_INTERSECTION, __FILE__, __func__, __LINE__);
        return RC::MEMORY_INTERSECTION;
    }
    if (src->sizeAllocated() != dest->sizeAllocated()) {
        src->getLogger()->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    std::memcpy((uint8_t*)dest, (uint8_t*)src, src->sizeAllocated());
    return RC::SUCCESS;
}

RC IVector::moveInstance(IVector* const dest, IVector*& src) {
    if (!(abs((int)((uint8_t*)src - (uint8_t*)dest)) >= src->sizeAllocated())) {
        src->getLogger()->severe(RC::MEMORY_INTERSECTION, __FILE__, __func__, __LINE__);
        return RC::MEMORY_INTERSECTION;
    }
    if (src->sizeAllocated() != dest->sizeAllocated()) {
        src->getLogger()->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    std::memmove((uint8_t*)dest, (uint8_t*)src, src->sizeAllocated());
    delete src;
    src = nullptr;
    return RC::SUCCESS;
}

RC IVector::setLogger(ILogger* const logger) {
    return VectorImpl::setLogger(logger);
}

ILogger* IVector::getLogger() {
    return VectorImpl::getLogger();
}

IVector* IVector::add(IVector const* const& op1, IVector const* const& op2) {
    if (op1 == nullptr || op2 == nullptr) {
        op1->getLogger()->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    if (op1->getDim() != op2->getDim()) {
        op1->getLogger()->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    IVector* newVec = op1->clone();
    newVec->inc(op2);
    return newVec;
}

IVector* IVector::sub(IVector const* const& op1, IVector const* const& op2) {
    if (op1 == nullptr || op2 == nullptr) {
        op1->getLogger()->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    if (op1->getDim() != op2->getDim()) {
        op1->getLogger()->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    
    IVector* newVec = op1->clone();
    newVec->dec(op2);
    return newVec;
}

double IVector::dot(IVector const* const& op1, IVector const* const& op2) {
    if (op1 == nullptr || op2 == nullptr) {
        op1->getLogger()->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return 0;
    }
    if (op1->getDim() != op2->getDim()) {
        op1->getLogger()->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return 0;
    }

    double rez = 0;
    for (size_t idx = 0; idx < op1->getDim(); ++idx)
        rez += op1->getData()[idx] * op2->getData()[idx];
    return rez;
}

bool IVector::equals(IVector const* const& op1, IVector const* const& op2, NORM n, double tol) {
    if (op1 == nullptr || op2 == nullptr) {
        op1->getLogger()->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return false;
    }
    if (op1->getDim() != op2->getDim()) {
        op1->getLogger()->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return false;
    }
    if (tol < 0) {
        op1->getLogger()->severe(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
        return false;
    }

    IVector* sub = IVector::sub(op1, op2);
    
    if (sub->norm(n) >= tol) {
        delete sub;
        return false;
    }

    delete sub;
    return true;
}

IVector::~IVector() = default;
