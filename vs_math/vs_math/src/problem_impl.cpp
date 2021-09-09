//#include <cmath>
//#include <new>
//#include "IProblem.h"
//#include "IBroker.h"
//
//
//namespace
//{
//
///* Sofa problem for rectangular shape
// * Problem: find such D and B that rectangular ship
// * with length D and width B will be able to pass
// * 90-degree turn of channel with width W (param)
// *
// * arg: (D, B)
// * param: (W, Tc)
// * where
// * Tc - count of positions to be cheked in the turn
// */
//class ProblemImpl : public IProblem
//{
//public:
//    //args - x , params - coef
//    // args - width and heigh
//    RC goalFunction(IVector const* args, IVector const* params, double& res) const override;
//    RC goalFunctionByArgs(IVector const*  args, double& res) const override;
//    size_t getArgsDim() const override;
//    size_t getParamsDim() const override;
//
//    RC setParams(IVector const* params) override;
//    bool isCompactValid(ICompact const * const & compact) const override;
//
//    ~ProblemImpl() override;
//
//    ProblemImpl();
//
//private:
//    ILogger *logger;
//    IVector *params;
//};
//
//
//RC ProblemImpl::goalFunction(const IVector *args, const IVector *params, double &res) const
//{
//    if (args == nullptr || params == nullptr)
//    {
//        if (logger != nullptr)
//            logger->severe("Problem::goalFunction: null params or args", RC::NULLPTR_ERROR);
//        return RC::NULLPTR_ERROR;
//    }
//
//    if (args->getDim() != getArgsDim() || params->getDim() != getParamsDim())
//    {
//        if (logger != nullptr)
//            logger->severe("Problem::goalFunction: wrong dimension of arg or param", RC::MISMATCHING_DIMENSIONS);
//        return RC::MISMATCHING_DIMENSIONS;
//    }
//
//    // t is iterator over uniform grid [0; 2 * sqrt(2) * W]
//    // - position along vertical wall
//    const double sq2 = sqrt(2);
//    auto checkAvailable = [sq2, args, params]( double t ) -> bool
//    {
//        double
//                b = args->getCoord(1),
//                d = args->getCoord(0);
//        double c = d + b * d * d / (t * sqrt(d * d - t * t));
//        double y = t + b * d / sqrt(d * d - t * t);
//        double x = sqrt(c * c - y * y);
//
//        double ly = sq2 * y * (x * x + x * y) / (c * c + 2 * x * y);
//        return ly <= params->getCoord(0) * sq2;
//    };
//
//    bool available = true;
//    for (double maxt = args->getCoord(0),
//            step = maxt / params->getCoord(1), t = step;
//         t < maxt; t += step)
//        if (!checkAvailable(t))
//        {
//            available = false;
//            break;
//        }
//
//    if (available)
//        res = -args->getCoord(0) * args->getCoord(1);
//    else
//        res = 10;
//
//    return RC::SUCCESS;
//}
//
//RC ProblemImpl::goalFunctionByArgs(const IVector *args, double &res) const
//{
//    return goalFunction(args, params, res);
//}
//
//size_t ProblemImpl::getArgsDim() const
//{
//    return 2;
//}
//
//size_t ProblemImpl::getParamsDim() const
//{
//    return 2;
//}
//
//RC ProblemImpl::setParams(const IVector *params)
//{
//    if (params == nullptr)
//    {
//        if (logger != nullptr)
//            logger->severe("Problem::setParams: null params", RC::NULLPTR_ERROR);
//        return RC::NULLPTR_ERROR;
//    }
//
//    auto dim = getParamsDim();
//
//    if (params->getDim() != dim)
//    {
//        if (logger != nullptr)
//            logger->severe("Problem::setParams: wrong dimension of param", RC::MISMATCHING_DIMENSIONS);
//        return RC::MISMATCHING_DIMENSIONS;
//    }
//
//    if (this->params == nullptr)
//        this->params = params->clone();
//    else
//        for (size_t i = 0; i < dim; i++)
//            this->params->setCoord(i, params->getCoord(i));
//
//    return RC::SUCCESS;
//}
//
//bool ProblemImpl::isCompactValid(const ICompact * const &compact) const
//{
//    if (compact == nullptr)
//    {
//        if (logger != nullptr)
//            logger->severe("Problem::isCompactValid - null compact", RC::NULLPTR_ERROR);
//        return false;
//    }
//
//    auto dim = compact->getDim();
//    if (dim != getArgsDim())
//    {
//        if (logger != nullptr)
//            logger->severe("Problem::isCompactValid - wrong dim", RC::MISMATCHING_DIMENSIONS);
//        return false;
//    }
//
//    auto
//            beg = compact->getLeftBoundary(),
//            end = compact->getEnd();
//
//    const double sq2 = sqrt(2);
//
//    return end->getCoord(0) <= 2 * sq2 * params->getCoord(0) && beg->getCoord(0) >= 0 &&
//            end->getCoord(1) <= params->getCoord(0) && beg->getCoord(1) >= 0;
//}
//
//ProblemImpl::~ProblemImpl()
//{
//    delete params;
//}
//
//ProblemImpl::ProblemImpl() : params(nullptr)
//{
//    logger = ILogger::createLogger(this);
//}
//
//class BrockerImpl : public IBrocker
//{
//private:
//    BrockerImpl()
//    {
//        impl = new (std::nothrow) ProblemImpl();
//    }
//
//public:
//    Type getType() const override
//    {
//        return Type::PROBLEM;
//    }
//
//    void * getInterfaceImpl(Type type) const override
//    {
//        switch (type)
//        {
//        case Type::PROBLEM:
//            return impl;
//        default:
//            return nullptr;
//        }
//    }
//
//    void release() override // harakiri
//    {
//        delete impl;
//        impl = nullptr;
//    }
//
//    static BrockerImpl * getInstance()
//    {
//        return instance;
//    }
//
//    /*dtor*/
//    ~BrockerImpl() override
//    {
//        delete impl;
//    }
//private:
//    ProblemImpl *impl;
//    static BrockerImpl *instance;
//};
//
//BrockerImpl * BrockerImpl::instance = new BrockerImpl;
//}
//
//extern "C"
//{
//
//DECLSPEC void * getBrocker()
//{
//    return BrockerImpl::getInstance();
//}
//
//}
