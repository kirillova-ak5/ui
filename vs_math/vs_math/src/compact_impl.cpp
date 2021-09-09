#include <cmath>
#include <cstdlib>
#include <new>
#include <algorithm>
//#include <iostream>

#include "../include/ICompact.h"
#include "../include/IVector.h"

namespace
{
const double tolerance = 1e-6;

inline double getCoordUnsafe(IVector const *vec, size_t idx)
{
    double d;
    vec->getCord(idx, d);
    return d;
}

bool isLess( IVector const *lhs, IVector const *rhs )
{
    if (lhs->getDim() != rhs->getDim())
        return false;

    for (size_t i = 0; i < lhs->getDim(); i++)
        if (getCoordUnsafe(lhs, i) > getCoordUnsafe(rhs, i))
            return false;

    return true;
}

IVector const * min( IVector const *lhs, IVector const *rhs )
{
    if (isLess(lhs, rhs))
        return lhs;
    return rhs;
}

IVector const * max( IVector const *lhs, IVector const *rhs )
{
    if (isLess(lhs, rhs))
        return rhs;
    return lhs;
}

bool isValidData( IVector const * const begin, IVector const * const end )
{
    if (begin == nullptr || end == nullptr)
        return false;

    if (begin->getDim() != end->getDim())
        return false;

    return true;
}

bool isValidData( ICompact const * const begin, ICompact const * const end )
{
    if (begin == nullptr || end == nullptr)
        return false;

    if (begin->getDim() != end->getDim())
        return false;

    return true;
}


class CompactImpl : public ICompact
{
public:
    //class IIterator;

    CompactImpl( IVector *left, IVector *right, ILogger *logger ) :
         theLeft(left->clone()), theRight(right->clone()), dim(left->getDim())
    {
    }

    RC getLeftBoundary(IVector*& vec) const override
    {
        vec = theLeft->clone();
        return RC::SUCCESS;
    }

    RC getRightBoundary(IVector*& vec) const override
    {
        vec = theRight->clone();
        return RC::SUCCESS;
    }


    bool isInside(IVector const* const &vec) const override
    {
        if (!vec)
        {
            if (logger)
                logger->warning(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
            return false;
        }

        if (vec->getDim() != getDim())
        {
            if (logger)
                logger->warning(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
            return false;
        }

        return isLess(theLeft, vec) && isLess(vec, theRight);
    }
/*
    RC isSubSet(ICompact const* const other, bool& result) const override
    {
        if (!isValidData(this, other))
        {
            logger->severe(RC::BAD_REFERENCE, __FILE__, __func__, __LINE__);
            return RC::BAD_REFERENCE;
        }

        bool iscontains;
        auto rc = isInside(other->getLeftBoundary(), iscontains);
        if (rc != RC::SUCCESS)
        {
            if (logger != nullptr)
                logger->severe(RC, __FILE__, __func__, __LINE__);
            return rc;
        }

        if (iscontains)
        {
            rc = isInside(other->getEnd(), iscontains);
            if (rc != RC::SUCCESS)
            {
                if (logger != nullptr)
                    logger->severe(RC::BAD_REFERENCE, __FILE__, __func__, __LINE__);
                return rc;
            }

            result = iscontains;
        }

        return RC::SUCCESS;
    }
*/
    /*
    RC isIntersects(ICompact const* const other, bool& result) const override
    {
        if (!isValidData(this, other))
        {
            if (logger != nullptr)
                logger->severe(RC::, __FILE__, __func__, __LINE__);
            return RC::;
        }

        auto
                l = max(theLeft, other->getLeftBoundary()),
                r = min(theRight, other->getEnd());

        result = isLess(l, r);
        return RC::SUCCESS;
    }
*/
    size_t getDim() const override
    {
        return dim;
    }

    ICompact* clone() const override
    {
        auto c = new (std::nothrow) CompactImpl(theLeft, theRight);
        if (c == nullptr)
        {
            if (logger != nullptr)
                logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        }
        return c;
    }

    IIterator* getBegin(IMultiIndex const* const& step = nullptr) override
    {
        auto res = isCorrectStep(step, false);

        if (!res)
        {
            if (logger != nullptr)
                logger->severe(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
            return nullptr;
        }

        auto it = new (std::nothrow) IIterator(this, step);
        if (it == nullptr)
        {
            if (logger != nullptr)
                logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        }
        return it;
    }

    IIterator * getEnd(IVector const* const step = nullptr) override
    {
        auto res = isCorrectStep(step, true);

        if (!res)
        {
            if (logger != nullptr)
                logger->severe(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
            return nullptr;
        }

        auto it = new (std::nothrow) IIterator(this, step, logger, true);
        if (it == nullptr)
        {
            if (logger != nullptr)
                logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        }
        return it;
    }

    ~CompactImpl() override
    {
        delete theLeft;
        delete theRight;
    }

private:
    bool isCorrectStep( IVector const * const step, bool reverse )
    {
        if (step == nullptr)
            return false;

        if (step->getDim() != getDim())
            return false;

        for (size_t i = 0; i < dim; i++)
        {
            if (std::isnan(getCoordUnsafe(step, i)))
                return false;

            auto crd = getCoordUnsafe(step, i);
            if (crd < 0 && !reverse ||
                crd > 0 && reverse ||
                std::abs(crd) < tolerance)
                return false;
        }
        return true;
    }

public:
    class IIterator : public ICompact::IIterator
    {
        friend class CompactImpl;
    public:
        // compact - always available
        IIterator( ICompact const *compact, IVector const *step,
                  ILogger *logger, bool reverse = false ) :
            reverse(reverse),
            compact(compact->clone()),
            current(nullptr),
            logger(logger)
        {
            const double stp = tolerance * 10;
            auto dim = compact->getDim();
            double *data = new double[dim];

            if (!reverse)
                compact->getLeftBoundary(current);
            else
                compact->getRightBoundary(current);

            if (step == nullptr)
                for (size_t i = 0; i < dim; i++)
                    data[i] = stp;

            this->step = step->clone();

            for (size_t i = 0; i < dim; i++)
                data[i] = i;
            dir = IVector::createVector(compact->getDim(), data);
            delete []data;
        }

        //adds step to current value in IIterator
        RC next() override
        {
            bool done = false;
            RC rc;

            IVector* begin, * end;
            rc = compact->getLeftBoundary(begin);
            if (rc != RC::SUCCESS)
            {
                if (logger)
                    logger->severe(rc, __FILE__, __func__, __LINE__);
                return rc;
            }
            rc = compact->getRightBoundary(end);
            if (rc != RC::SUCCESS)
            {
                if (logger)
                    logger->severe(rc, __FILE__, __func__, __LINE__);
                return rc;
            }

            bool eq = !reverse ? IVector::equals(end, current, IVector::NORM::SECOND, tolerance) :
                                 IVector::equals(begin, current, IVector::NORM::SECOND, tolerance);

            if (eq)
            {
                delete begin;
                delete end;
                return RC::INDEX_OUT_OF_BOUND;
            }

            IVector *v = current->clone();

            auto dim = compact->getDim();

            // the goal idea:
            // check if it is available to do step along the axis number dir[0]
            // (current[dir[0]] += step[dir[0]])
            // if it is possible - step is done
            // if it it not possible (comapct does not contain such point) -
            //    if current[dir[0]] is end[dir[0]] - try axis dir[1] and so on until
            //    step is done
            //    if current[dir[0]] is not end[dir[0]] - set current[dir[0]] := end[dir[0]]
            // if step was not done - current == end

            for (size_t i = 0; i < dim && !done; i++)
            {
                size_t idx = static_cast<size_t>(std::round(getCoordUnsafe(dir, i)));

                // we stayed at bound, so go to begin
                if (!reverse)
                {
                    if (std::abs(getCoordUnsafe(v, idx) - getCoordUnsafe(end, idx)) < tolerance)
                    {
                        v->setCord(idx, getCoordUnsafe(begin, idx));
                        continue;
                    }
                }
                else
                {
                    if (std::abs(getCoordUnsafe(v, idx) - getCoordUnsafe(begin, idx)) < tolerance)
                    {
                        v->setCord(idx, getCoordUnsafe(end, idx));
                        continue;
                    }
                }

                v->setCord(idx, getCoordUnsafe(v, idx) + getCoordUnsafe(step, idx));

                if (!compact->isInside(v))
                {
                    if (!reverse)
                        v->setCord(idx, getCoordUnsafe(end, idx));
                    else
                        v->setCord(idx, getCoordUnsafe(begin, idx));
                }

                done = true;
            }

            if (done)
                for (size_t i = 0; i < dim; i++)
                    current->setCord(i, getCoordUnsafe(v, i));
            delete v;

            return RC::SUCCESS;
        }

        RC getVectorCopy(IVector*& val) const override
        {
            val = current->clone();
            return RC::SUCCESS;
        }

        //change order of step
        /*dtor*/
        ~IIterator() override
        {
            delete dir;
            delete step;
            delete current;
            delete compact;
        }
    private:
        bool reverse;
        ICompact const *compact;
        IVector *current;
        IVector const *step;
        IVector *dir;
        ILogger *logger;
    };

private:
    // begin is always left, end - right
    IVector *theLeft, *theRight;
    size_t dim;
    static ILogger *logger;
};
}

ICompact * ICompact::createCompact(IVector const* const begin, IVector const* const end, IMultiIndex const *nodeQuantities)
{
    ILogger* creationLogger = begin->getLogger();
    if (!creationLogger)
        creationLogger = end->getLogger();

    if (!isValidData(begin, end))
    {
        if (creationLogger)
            creationLogger->severe(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    auto lessBegEnd = isLess(begin, end);
    if (!lessBegEnd)
    {
        if (creationLogger)
            creationLogger->severe(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    // always should: begin < end!
    return new (std::nothrow) CompactImpl(const_cast<IVector *>(begin), const_cast<IVector *>(end));
}

ICompact * ICompact::intersection(ICompact const* const left, ICompact const* const right, ILogger* logger)
{
    if (!isValidData(left, right))
    {
        if (logger != nullptr)
            logger->severe(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    auto dim = left->getDim();

    bool inters;
    auto rc = left->isIntersects(right, inters);

    if (rc != RC::SUCCESS)
    {
        if (logger != nullptr)
            logger->severe(rc, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    if (inters)
    {
        auto *data = new (std::nothrow) double[dim]{0};
        if (data == nullptr)
        {
            if (logger != nullptr)
                logger->severe(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
            return nullptr;
        }
        IVector *l = IVector::createVector(dim, data), *r = IVector::createVector(dim, data);

	delete []data;

        IVector *lbeg, *rbeg, *lend, *rend;

        rc = left->getLeftBoundary(lbeg);
        if (rc != RC::SUCCESS)
        {
            if (logger)
                logger->severe(rc, __FILE__, __func__, __LINE__);
            return rc;
        }
        rc = right->getLeftBoundary(rbeg);
        if (rc != RC::SUCCESS)
        {
            if (logger)
                logger->severe(rc, __FILE__, __func__, __LINE__);
            return rc;
        }
        rc = left->getRightBoundary(lend);
        if (rc != RC::SUCCESS)
        {
            if (logger)
                logger->severe(rc, __FILE__, __func__, __LINE__);
            return rc;
        }
        rc = right->getRightBoundary(rend);
        if (rc != RC::SUCCESS)
        {
            if (logger)
                logger->severe(rc, __FILE__, __func__, __LINE__);
            return rc;
        }


        for (size_t i = 0; i < dim; i++)
        {
            l->setCord(i, std::max(lbeg->getCord(i), rbeg->getCord(i)));
            r->setCord(i, std::min(lend->getCord(i), rend->getCord(i)));
        }
        delete lbeg;
        delete lend;
        delete rbeg;
        delete rend;

        auto c = createCompact(l, r);

        delete l;
        delete r;

        return c;
    }

    if (logger != nullptr)
        logger->severe(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
    return nullptr;
}

ICompact * ICompact::createCompactSpan(ICompact const* const left, ICompact const* const right, IMultiIndex const * const grid)
{
    if (!isValidData(left, right))
    {
        if (logger)
            logger->severe(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    auto
            lbeg = left->getLeftBoundary(),
            rbeg = right->getLeftBoundary(),
            lend = left->getEnd(),
            rend = right->getEnd();

    auto c = createCompact(min(lbeg, rbeg),
                         max(lend, rend));

    delete lbeg;
    delete lend;
    delete rbeg;
    delete rend;

    return c;
}
