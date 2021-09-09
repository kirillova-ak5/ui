#include <cstddef>
#include "IVector.h"
#include "ISetControlBlock.h"
#include "ISet.h"

class SetImpl : public ISet {
public:
    RC getByUniqueIndex(IVector *const &vec, size_t &index, size_t indexInc);
    RC getFirstByUniqueIndex(IVector *const &vec, size_t &index);
    RC getLastByUniqueIndex(IVector *const &vec, size_t &index);
};

class SetImplControlBlock : public ISetControlBlock {
public:
    RC getNext(IVector *const &vec, size_t &index, size_t indexInc = 1) const {
        return set->getByUniqueIndex(vec, index, indexInc);
    }
    RC getPrevious(IVector *const &vec, size_t &index, size_t indexInc = 1) const {
        return set->getByUniqueIndex(vec, index, -(int)indexInc);
    }
    
    RC getBegin(IVector *const &vec, size_t &index) const {
        return set->getFirstByUniqueIndex(vec, index);
    }
    RC getEnd(IVector *const &vec, size_t &index) const {
        return set->getLastByUniqueIndex(vec, index);
    }

private:
    SetImpl* set;
protected:
    SetImplControlBlock(SetImpl* const& set) : set(set) {};
};

ISetControlBlock::~ISetControlBlock() = default;
