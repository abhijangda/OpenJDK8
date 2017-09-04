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

bool aosDBGetCurrMethodInfo (std::string& methodFullDesc, int& highestOptLevel, int& counts,
                         int& highestOsrBci, int& highestOsrLevel)
{
    assert (currentIter != nullptr);
    
    if (*currentIter == aosdb->end ())
        return false;

    methodFullDesc = (**currentIter).getMethodFullDesc ();
    highestOptLevel = (**currentIter).getHighestOptLevel ();
    counts = (**currentIter).getCounts ();
    highestOsrBci = (**currentIter).getHighestOsrBci ();
    highestOsrLevel = (**currentIter).getHighestOsrLevel ();
    return true;
}

bool aosDBGetCurrMethodCallProfile (int bci, int& limits, int& morphism, 
                                        int& count, int* receiver_count)
{
    assert (currentIter != nullptr);
    
    if (*currentIter == aosdb->end ())
        return false;
    
    AOSDatabaseElementCallProfile* p = (**currentIter).getCallProfileForBci (bci);
    
    if (p == nullptr)
        return false;
    
    limits = p->getLimit ();
    count = p->getCount ();
    morphism = p->getMorphism ();
    p->getReceiverCount (receiver_count);
    
    return true;
}

void aosDBAddMethodInfo (std::string& methodFullDesc, int optLevel, int counts, 
                         int bci)
{
    //assert (currentIter != nullptr && *currentIter == aosdb->end ());
    
    if (aosdb->is_verbose ())
    {
        std::cout << "aosDBAddMethodInfo "<< methodFullDesc << " " << optLevel
                << " " << counts << std::endl;
    }
    
    aosdb->insertMethodInfo (methodFullDesc, optLevel, counts, bci);
}

void aosDBAddMethodCallProfileForBci (std::string& methodFullDesc, int bci, 
                                          int limit, int morphism, int count,
                                          int receiver_count[MorphismLimit+1])
{
    aosdb->insertCallProfileForMethod (methodFullDesc, bci, limit, morphism,
                                           count, receiver_count);
}

void aosDBAddMethodCountProfileForBci (std::string& methodFullDesc, int bci, 
                                       int count)
{
    aosdb->insertCountProfileForMethod (methodFullDesc, bci, count);
}

void aosDBAddHotDataForMethod (std::string& methodFullDesc,
                                int interpreterInvocationCount, int throwoutCount,
                                int invocationCount, int backedgeCount)
{
    aosdb->insertHotDataForMethod (methodFullDesc, interpreterInvocationCount, 
                                   throwoutCount, invocationCount,
                                   backedgeCount);
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

bool aosDBFindMethodInfo (std::string& methodFullDesc, int& highestOptLevel, int& counts,
                         int& highestOsrBci, int& highestOsrLevel)
{
    return aosdb->findMethodInfo (methodFullDesc, highestOptLevel, counts, 
                                  highestOsrBci, highestOsrLevel);
}

bool aosDBFindMethodCallProfile (std::string& methodFullDesc, int& bci, 
                                     int& limit, int& morphism, int& count,
                                     int receiver_count[MorphismLimit+1])
{
    return aosdb->findCallProfileForMethod (methodFullDesc, bci, limit,
                                                morphism, count, receiver_count);
}

bool aosDBFindMethodCountProfile (std::string& methodFullDesc, int& bci, 
                                  int& count)
{
    return aosdb->findCountProfileForMethod (methodFullDesc, bci, count);
}

bool aosDBFindHotDataForMethod (std::string& methodFullDesc,
                                int& interpreterInvocationCount, int& throwoutCount,
                                int& invocationCount, int& backedgeCount)
{
    return aosdb->findHotDataForMethod (methodFullDesc, interpreterInvocationCount, 
                                        throwoutCount, invocationCount, backedgeCount);
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
    aosDBInit (true, true);
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
        int bci;
        aosDBGetCurrMethodInfo (desc, optLevel, counts, bci);
        
        std::cout << desc << " " << optLevel << " " << counts << std::endl;
    }
    while (aosDBVisitNextMethod ());
    
    return 0;
}
#endif
