#include <cstring>
#include <cstdint>
#include <cmath>
#include "IMultiIndex.h"


namespace {
    class MultiindexImpl : public IMultiIndex {
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

        static IMultiIndex* createMultiIndex(size_t dim, const size_t *ptr_data) {
            if (dim == 0) {
                logger->severe(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
                return nullptr;
            }
            if (ptr_data == nullptr) {
                logger->warning(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
                return nullptr;
            }

            uint8_t* ptr = new (std::nothrow) uint8_t[sizeof(MultiindexImpl) + dim * sizeof(size_t)];
            if (ptr == nullptr) {
                logger->warning(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
                return nullptr;
            }
            IMultiIndex* vec = new (ptr) MultiindexImpl(dim);
            std::memcpy((uint8_t*)(ptr)+sizeof(MultiindexImpl), ptr_data, dim * sizeof(size_t));

            return vec;
        }

        IMultiIndex* clone() const override {
            return createMultiIndex(dim, RawData());
        }
        size_t const* getData() const override {
            return RawData();
        }
        RC setData(size_t dim, size_t const* const& ptr_data) override {
            if (this->dim != dim) {
                logger->severe(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
                return RC::MISMATCHING_DIMENSIONS;
            }
            if (ptr_data == nullptr) {
                logger->severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
                return RC::NULLPTR_ERROR;
            }

            memcpy(RawData(), ptr_data, dim * sizeof(size_t));

            return RC::SUCCESS;
        }

        RC getAxisIndex(size_t index, size_t& val) const override {
            if (index >= dim) {
                logger->severe(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
                return RC::INDEX_OUT_OF_BOUND;
            }

            val = RawData()[index];
            return RC::SUCCESS;
        }
        RC setAxisIndex(size_t index, size_t val) override {
            if (index >= dim) {
                logger->severe(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
                return RC::INDEX_OUT_OF_BOUND;
            }

            RawData()[index] = val;
            return RC::SUCCESS;
        }

        size_t getDim() const override {
            return dim;
        }

        RC incAxisIndex(size_t axisIndex, size_t val) override {
            if (axisIndex >= dim) {
                logger->severe(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
                return RC::INDEX_OUT_OF_BOUND;
            }
            RawData()[axisIndex] += val;
            return RC::SUCCESS;
        }

        void operator delete(void* ptr, size_t size) {
            if (ptr)
                delete[] reinterpret_cast<uint8_t*>(ptr);
        }

        ~MultiindexImpl() = default;

    private:
        static ILogger* logger;
        size_t dim;

        inline size_t* RawData() const
        {
            return (size_t*)((uint8_t*)(this) + sizeof(MultiindexImpl));
        }

        MultiindexImpl(size_t dim) : dim(dim) {}
    };
    ILogger* MultiindexImpl::logger = nullptr;
};

IMultiIndex* IMultiIndex::createMultiIndex(size_t dim, const size_t* indices) {
    return MultiindexImpl::createMultiIndex(dim, indices);
}


RC IMultiIndex::setLogger(ILogger* const logger) {
    return MultiindexImpl::setLogger(logger);
}

ILogger* IMultiIndex::getLogger() {
    return MultiindexImpl::getLogger();
}

IMultiIndex::~IMultiIndex() = default;
