#pragma once
#include "CompactImplControlBlock.h"
#include "ICompact.h"

class LIB_EXPORT CompactImpl : public ICompact {
  public:
    static ICompact *createCompact(IVector const *vec1, IVector const *vec2, IMultiIndex const *nodeQuantities);

    ICompact *clone() const override;

    static RC setLogger(ILogger *const logger);
    static ILogger *getLogger();

    bool isInside(IVector const *const &vec) const override;
    RC getVectorCopy(IMultiIndex const *index, IVector *&val) const override;
    RC getVectorCoords(IMultiIndex const *index, IVector *const &val) const override;

    RC getLeftBoundary(IVector *&vec) const override;
    RC getRightBoundary(IVector *&vec) const override;
    size_t getDim() const override;
    IMultiIndex *getGrid() const override;

    class IteratorImpl : public IIterator {
      public:
        IteratorImpl(IVector *vec, IMultiIndex *idx, IMultiIndex *bypass_order, CompactImplControlBlock *cb);
        ~IteratorImpl();

        IIterator *getNext() override;
        IIterator *clone() const override;

        static RC setLogger(ILogger *const pLogger);
        static ILogger *getLogger();

        RC next() override;

        bool isValid() const override;

        RC getVectorCopy(IVector *&val) const override;
        RC getVectorCoords(IVector *const &val) const override;

      private:
        bool valid;
        IVector *vector;
        IMultiIndex *index;
        IMultiIndex *order;
        CompactImplControlBlock *control_block;
        static ILogger *logger;
    };

    IIterator *getIterator(IMultiIndex const *const &index, IMultiIndex const *const &bypassOrder) const override;
    IIterator *getBegin(IMultiIndex const *const &bypassOrder) const override;
    IIterator *getEnd(IMultiIndex const *const &bypassOrder) const override;

    ~CompactImpl();

  private:
    static ILogger *logger;
    const IVector *left_boundary;
    const IVector *right_boundary;
    const IMultiIndex *grid;
    size_t dim;
    CompactImplControlBlock *control_block;

    CompactImpl(const IVector *left, const IVector *right, const IMultiIndex *nodes);
};
