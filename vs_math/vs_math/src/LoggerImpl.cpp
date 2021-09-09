#include <ostream>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <memory>
#include "ILogger.h"

namespace {
    class LoggerImpl : public ILogger {
    public:
        static ILogger* createLogger() { 
            return new LoggerImpl;
        }
        static ILogger *createLogger(const char* const& filename, bool overwrite = true) {
            return new LoggerImpl(filename, overwrite);
        }

        RC log(RC code, Level level, const char* const& srcfile, const char* const& function, int line) override {
            if (srcfile == nullptr || function == nullptr) {
                severe(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
                return RC::NULLPTR_ERROR;
            }
            
            *logstream << "[" << srcfile << "] " << "[" << function << "] " << "[" << line << "] ";
            return log(code, level);
        }
        RC log(RC code, Level level) override {
            *logstream << "[" << levelToStrMap[level] << "] " << errMsgMap[code] << std::endl;
            return RC::SUCCESS;
        }

    private:
        std::ostream* logstream;
        std::map<RC, std::string> errMsgMap;
        std::map<Level, std::string> levelToStrMap;

        LoggerImpl() { 
            logstream = &std::cout;
            InitLevelToStrMap();
            InitErrMsgMap();
        }

        LoggerImpl(const std::string& filename, bool overwrite = true) {
            std::ios_base::openmode writeMode;
            if (overwrite)
                writeMode = std::ios::out;
            else
                writeMode = std::ios::app;
            
            logstream = new std::ofstream(filename, writeMode);

            InitLevelToStrMap();
            InitErrMsgMap();
        }

        void InitErrMsgMap() {
            errMsgMap.insert({RC::UNKNOWN, "Unknown error occured"});
            errMsgMap.insert({RC::SUCCESS, "Successful"});
            errMsgMap.insert({RC::INVALID_ARGUMENT, "Received invalid argument"});
            errMsgMap.insert({RC::MISMATCHING_DIMENSIONS, "Cannot operate on two vectors with different dimensions"});
            errMsgMap.insert({RC::INDEX_OUT_OF_BOUND, "Index out of bound"});
            errMsgMap.insert({RC::INFINITY_OVERFLOW, "Received number greater than infinity"});
            errMsgMap.insert({RC::NOT_NUMBER, "Argument declared a number but turned out not to be one"});
            errMsgMap.insert({RC::ALLOCATION_ERROR, "Couldn't allocate more memory"});
            errMsgMap.insert({RC::NULLPTR_ERROR, "Received nullptr instead of a valid pointer"});
            errMsgMap.insert({RC::FILE_NOT_FOUND, "Couldn't open file, maybe it doesn't exist"});
            errMsgMap.insert({RC::VECTOR_NOT_FOUND, "Couldn't find matching IVector instance"});
            errMsgMap.insert({RC::IO_ERROR, "Input or output stream is unavailable"});
            errMsgMap.insert({RC::MEMORY_INTERSECTION, "Found intersecting memory parts"});
            errMsgMap.insert({RC::SOURCE_SET_DESTROYED, "Iterator trying to work with dead set"});
            errMsgMap.insert({RC::SOURCE_COMPACT_DESTROYED, "Iterator trying to work with dead compact"});
            errMsgMap.insert({RC::SOURCE_SET_EMPTY, "Iterator trying to move to the begin/end of empty set"});
            errMsgMap.insert({RC::VECTOR_ALREADY_EXIST, "Given vector already exists in current set"});
            errMsgMap.insert({RC::SET_INDEX_OVERFLOW, "Set index too big"});
            errMsgMap.insert({RC::NO_ARGS_SET, "No arguments set for problem to evaluate or for solver to solve"});
            errMsgMap.insert({RC::NO_PARAMS_SET, "No params set for problem to evaluate or for solver to solve"});
            errMsgMap.insert({RC::NO_PROBLEM_SET, "No problem set for solver to solve"});
        }
        void InitLevelToStrMap() {
            levelToStrMap.insert({Level::SEVERE, "SEVERE"});
            levelToStrMap.insert({ILogger::Level::WARNING, "WARNING"});
            levelToStrMap.insert({Level::INFO, "INFO"});
        }
        
        ~LoggerImpl() {
            if (logstream != &std::cout)
                delete logstream;
        };
    };
};

ILogger* ILogger::createLogger() {
    return LoggerImpl::createLogger();
}

ILogger* ILogger::createLogger(const char* const& filename, bool overwrite) {
    return LoggerImpl::createLogger(filename, overwrite);
}

ILogger::~ILogger() = default;