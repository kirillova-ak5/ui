//#include <QStringList>
//#include <QRegExp>
//
//#include "ISolver.h"
//#include "IBrocker.h"
//
//namespace
//{
//
///*
// * Solver param: compact step
// */
//class SolverImpl : public ISolver
//{
//public:
//    SolverImpl();
//
//    RC setParams(IVector const* params) override;
//
//    RC setParams(QString& str) override;
//
//    RC setProblem(IProblem *pProblem) override;
//
//    RC setProblemParams(IVector const* params) override;
//
//    RC setCompact(ICompact *pCompact) override;
//
//    size_t getParamsDim() const override;
//
//    RC solve() override;
//
//    RC getSolution(IVector * &vec) const override;
//
//    ~SolverImpl() override;
//
//private:
//    IVector *solution;
//    IVector *params;
//    IVector *problemParams;
//    ILogger *logger;
//    IProblem *problem;
//    ICompact *compact;
//};
//
//SolverImpl::SolverImpl() :
//    solution(nullptr), params(nullptr),
//    problemParams(nullptr), problem(nullptr),
//    compact(nullptr)
//{
//    logger = ILogger::createLogger(this);
//}
//
//RC SolverImpl::setParams(const IVector *params)
//{
//    if (params == nullptr)
//    {
//        if (logger != nullptr)
//            logger->severe("solver::setParams: null param", RC::NULLPTR_ERROR);
//        return RC::NULLPTR_ERROR;
//    }
//
//
//    if (this->params != nullptr)
//        delete this->params;
//    this->params = params->clone();
//
//    return RC::SUCCESS;
//}
//
///* Grammar:
// * dim = {INT}; step = {DOUBLE[DIM]}
// * example:
// * dim = 3; step = 0.01, 0.02, 0.03
// */
//RC SolverImpl::setParams(QString &str)
//{
//    struct Grammar
//    {
//        const QString paramSeparator = ";";
//        const QString coordSeparator = ",";
//        const QString assign = "=";
//        const QString dimension = "dim";
//        const QString step = "step";
//        const QRegExp ws = QRegExp(QString("\\s"));
//        const int paramsCount = 2;
//        const int dummy = 0; // for better padding!
//    } g;
//    auto res = str.split(g.paramSeparator, QString::SplitBehavior::SkipEmptyParts);
//    if (res.size() != g.paramsCount)
//    {
//        if (logger != nullptr)
//            logger->severe("Bad grammar of params string", RC::INVALID_ARGUMENT);
//        return RC::INVALID_ARGUMENT;
//    }
//
//    bool ok;
//    uint dim = 0;
//    double *data = nullptr;
//    for (auto str : res)
//    {
//        auto args = str.split(g.assign, QString::SplitBehavior::SkipEmptyParts);
//
//        if (args.size() != 2)
//        {
//            if (logger != nullptr)
//                logger->severe("Bad param grammar: should be KEY = VALUE", RC::INVALID_ARGUMENT);
//            return RC::INVALID_ARGUMENT;
//        }
//
//        if (args[0].remove(g.ws) == g.dimension)
//        {
//            dim = args[1].toUInt(&ok);
//            if (!ok)
//            {
//                if (logger != nullptr)
//                    logger->severe("Bad grammar: dimension should be uint", RC::INVALID_ARGUMENT);
//                return RC::INVALID_ARGUMENT;
//            }
//        }
//        else if (args[0].remove(g.ws) == g.step)
//        {
//            auto stp = args[1].split(g.coordSeparator);
//
//            // here could be check dim == str.size() but we allow user to choose args order
//            // if this condition won't be passed, vector will not be created and we will detect it
//
//            auto sz = stp.size();
//            data = new (std::nothrow) double[sz];
//
//            for (int i = 0; i < sz; i++)
//            {
//                data[i] = stp[i].toDouble(&ok);
//
//                if (!ok)
//                {
//                    if (logger != nullptr)
//                        logger->severe("Bad grammar: coord should be double", RC::INVALID_ARGUMENT);
//
//                    delete []data;
//                    return RC::INVALID_ARGUMENT;
//                }
//            }
//        }
//        else
//        {
//            if (logger != nullptr)
//                logger->severe("Bad grammar: wrong param name", RC::INVALID_ARGUMENT);
//            return RC::INVALID_ARGUMENT;
//        }
//    }
//
//    IVector *v = IVector::createVector(dim, data, logger);
//    delete []data;
//
//    if (v != nullptr)
//    {
//        params = v;
//        return RC::SUCCESS;
//    }
//
//    if (logger != nullptr)
//        logger->severe("Something went wrong while creating param vector", RC::INVALID_ARGUMENT);
//    return RC::INVALID_ARGUMENT;
//}
//
//RC SolverImpl::setProblem(IProblem *pProblem)
//{
//    if (pProblem == nullptr)
//    {
//        if (logger != nullptr)
//            logger->severe("solver::setProblem: null param", RC::NULLPTR_ERROR);
//        return RC::NULLPTR_ERROR;
//    }
//    problem = pProblem;
//    return RC::SUCCESS;
//}
//
//RC SolverImpl::setProblemParams(const IVector *params)
//{
//    if (params == nullptr)
//    {
//        if (logger != nullptr)
//            logger->severe("solver::setProblemParams: null param", RC::NULLPTR_ERROR);
//        return RC::NULLPTR_ERROR;
//    }
//
//    if (this->problemParams != nullptr)
//        delete this->problemParams;
//    this->problemParams = params->clone();
//
//    return RC::SUCCESS;
//}
//
//RC SolverImpl::setCompact(ICompact *pCompact)
//{
//    if (pCompact == nullptr)
//    {
//        if (logger != nullptr)
//            logger->severe("solver::setCompact: null param", RC::NULLPTR_ERROR);
//        return RC::NULLPTR_ERROR;
//    }
//
//    if (compact != nullptr)
//        delete compact;
//
//    compact = pCompact->clone();
//
//    return RC::SUCCESS;
//}
//
//size_t SolverImpl::getParamsDim() const
//{
//    if (params == nullptr)
//        return 0;
//    return params->getDim();
//}
//
//RC SolverImpl::solve()
//{
//    if (!problem->isCompactValid(compact))
//    {
//        if (logger != nullptr)
//            logger->severe("solve: not valid compact", RC::INVALID_ARGUMENT);
//        return RC::INVALID_ARGUMENT;
//    }
//
//    if (params->getDim() != compact->getDim())
//    {
//        if (logger != nullptr)
//            logger->severe("solver::solve: params dimension should beq equal to compact", RC::MISMATCHING_DIMENSIONS);
//        return RC::MISMATCHING_DIMENSIONS;
//    }
//
//    auto it = compact->begin(params);
//    if (it == nullptr)
//    {
//        if (logger != nullptr)
//            logger->severe("solve: not valid step", RC::INVALID_ARGUMENT);
//        return RC::INVALID_ARGUMENT;
//    }
//
//    if (problemParams != nullptr)
//        problem->setParams(problemParams);
//
//    auto end = compact->getEnd();
//    const double tolerance = 1e-6;
//
//    auto dim = compact->getDim();
//    double *data = new (std::nothrow) double[dim];
//
//    if (data == nullptr)
//    {
//        if (logger != nullptr)
//            logger->severe("solve: no memory", RC::ALLOCATION_ERROR);
//        return RC::ALLOCATION_ERROR;
//    }
//
//    IVector *bestSolution = IVector::createVector(dim, data, logger);
//    delete []data;
//
//    if (bestSolution == nullptr)
//    {
//        if (logger != nullptr)
//            logger->severe("solve: smth bad with vector created", RC::INVALID_ARGUMENT);
//        return RC::INVALID_ARGUMENT;
//    }
//
//    const double startBestRes = 1e10;
//    double bestRes = startBestRes;
//    double curRes;
//
//    for (auto rcf = RC::SUCCESS; rcf == RC::SUCCESS; rcf = it->doStep())
//    {
//        
//        auto rc = problem->goalFunctionByArgs(it->getPoint(), curRes);
//
//        if (rc != RC::SUCCESS)
//        {
//            if (logger != nullptr)
//                logger->severe("solve: smth wrong with goalFunctionByArgs", RC::INVALID_ARGUMENT);
//            return RC::INVALID_ARGUMENT;
//        }
//
//        if (curRes < bestRes)
//        {
//            bestRes = curRes;
//            auto pt = it->getPoint();
//            if (pt == nullptr)
//            {
//                if (logger != nullptr)
//                    logger->severe("solve: smth wrong with goalFunctionByArgs", RC::INVALID_ARGUMENT);
//                return RC::INVALID_ARGUMENT;
//            }
//
//            for (size_t i = 0; i < dim; i++)
//                if (bestSolution->setCord(i, pt->getCord(i)) != RC::SUCCESS)
//                {
//                    if (logger != nullptr)
//                        logger->severe("solve: smth wrong with setCord", RC::INVALID_ARGUMENT);
//                    return RC::INVALID_ARGUMENT;
//                }
//        }
//
//    }
//    delete it;
//
//    if (std::abs(startBestRes - bestRes) < tolerance)
//    {
//        logger->severe("solve: solution not found", RC::NOT_FOUND);
//        return RC::NOT_FOUND;
//    }
//
//    if (solution != nullptr)
//        delete solution;
//
//    solution = bestSolution->clone();
//    delete bestSolution;
//
//    return RC::SUCCESS;
//}
//
//RC SolverImpl::getSolution(IVector *&vec) const
//{
//    if (solution == nullptr)
//    {
//        logger->severe("getSolution: no solution yet", RC::NOT_FOUND);
//        return RC::NOT_FOUND;
//    }
//
//    vec = solution->clone();
//    return RC::SUCCESS;
//}
//
//SolverImpl::~SolverImpl()
//{
//    delete solution;
//    delete params;
//    delete problemParams;
//    delete compact;
//}
//
//class BrockerImpl : public IBrocker
//{
//private:
//    BrockerImpl()
//    {
//        impl = new (std::nothrow) SolverImpl();
//    }
//
//public:
//    Type getType() const override
//    {
//        return Type::SOLVER;
//    }
//
//    void * getInterfaceImpl(Type type) const override
//    {
//        switch (type)
//        {
//        case Type::SOLVER:
//            return impl;
//        default:
//            return nullptr;
//        }
//    }
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
//    SolverImpl *impl;
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
