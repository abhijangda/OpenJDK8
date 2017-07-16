#include "aosDB.h"
#include "aosDBAPI.h"

#include <assert.h>

static AOSDatabase* aosdb = nullptr;
static AOSDatabase::iterator* currentIter = nullptr;

int aosDBInit (bool verbose, bool recordStats)
{
    if (aosdb != nullptr)
        return 1;
    
    aosdb = new AOSDatabase ("dbFile", verbose, recordStats);
    
    if (aosdb == nullptr)
        return 0;
    
    return 1;
}

bool aosDBIsInit ()
{
    return aosdb != nullptr;
}

void aosDBPrint ()
{
    assert (aosdb != nullptr);
    
    aosdb->printDB ();
}

void aosDBRead ()
{
    assert (aosdb != nullptr);
    aosdb->readDB ();
}

bool aosDBVisitNextMethod ()
{
    assert (currentIter != nullptr && aosdb != nullptr);
    
    if (*currentIter == aosdb->end())
        return false;
    
    AOSDatabase::iterator * prev_iter = currentIter;
    currentIter = new AOSDatabase::iterator (++(*(prev_iter)));
    delete prev_iter;
    return true;
}

int aosDBGetNumberOfMethods ()
{
    assert (aosdb != nullptr);
    return aosdb->getNMethods();
}

bool aosDBGetCurrMethodInfo (std::string& methodFullDesc, int& optLevel, int& counts, int& bci)
{
    assert (currentIter != nullptr);
    
    if (*currentIter == aosdb->end ())
        return false;

    methodFullDesc = (**currentIter).getMethodFullDesc ();
    optLevel = (**currentIter).getOptLevel ();
    counts = (**currentIter).getCounts ();
    bci = (**currentIter).getBci ();
    
    return true;
}

void aosDBAddMethodInfo (std::string& methodFullDesc, int optLevel, int counts, int bci)
{
    //assert (currentIter != nullptr && *currentIter == aosdb->end ());
    
    if (aosdb->is_verbose ())
    {
        std::cout << "aosDBAddMethodInfo "<< methodFullDesc << " " << optLevel
                << " " << counts << std::endl;
    }
    
    aosdb->insertMethodInfo (methodFullDesc, optLevel, counts, bci);
}

void aosDBVisitBegin ()
{
    currentIter = new AOSDatabase::iterator (aosdb->begin());
}

void aosDBWriteDB ()
{
    assert (aosdb != nullptr);
    
    aosdb->writeDB ();
}

void aosDBClearDB ()
{
    assert (aosdb != nullptr);
    
    aosdb->clearDB ();
}

bool aosDBFindMethodInfo (std::string& methodFullDesc, int& optLevel, int& counts, int& bci)
{
    return aosdb->findMethodInfo (methodFullDesc, optLevel, counts, bci);
}

int aosDBGetMethodsFoundInDB ()
{
    assert (aosdb->getRecordStats ());
    return aosdb->getMethodsFoundInDB();
}

int aosDBGetMethodsNotFoundInDB ()
{
    assert (aosdb->getRecordStats ());
    return aosdb->getMethodsNotFoundInDB();
}

int aosDBGetMethodsFoundAtOptLevelInDB (int l)
{
    assert (0 <= l && l <= NUM_OPT_LEVELS);
    return aosdb->getMethodsFoundAtOptLevelInDB (l);
}

//TEST

#ifdef __TEST_API__
int main ()
{
    aosDBInit ();
    std::cout << "Initialize"<<std::endl;
    aosDBRead ();
    std::cout << "DB Read"<<std::endl;
    //aosPrintDB ();
    aosDBVisitBegin ();
    std::cout << "Visit Begin"<<std::endl;
    do
    {
        std::string desc;
        int optLevel;
        int counts;
        
        aosDBGetCurrMethodInfo (desc, optLevel, counts);
        
        std::cout << desc << " " << optLevel << " " << counts << std::endl;
    }
    while (aosDBVisitNextMethod ());
    
    return 0;
}
#endif
