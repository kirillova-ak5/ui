#pragma once
#include "ICompactControlBlock.h"

class CompactImpl;

class CompactImplControlBlock : public ICompactControlBlock {
  public:
    CompactImplControlBlock(CompactImpl *compact);
    /*
     * Control block moves iterator forward in grid of compact. When reached last node of an axis, next axis position
     * will be incremented. Next axis defined by bypassOrder
     *
     * @param [in] currentIndex Multi-index of current iterator position
     *
     * @param [in] bypassOrder Multi-index, that defining bypass orred of axis
     */
    RC get(IMultiIndex *const &currentIndex, IMultiIndex const *const &bypassOrder) const override;
    /*
     * Control block clculates vector corresponding to multi-index
     *
     * @param [in] currentIndex Multi-index of current iterator position
     *
     * @param [in] val Buffer for vector data
     */
    RC get(IMultiIndex const *const &currentIndex, IVector *const &val) const override;

  private:
    CompactImpl *compact;
};
