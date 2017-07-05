#include "aosDB.h"
#include "aosDBAPI.h"

static AOSDatabase* aosdb = nullptr;
static AOSDatabase::iterator* currentIter = nullptr;

int aosDBInit ()
{
    if (aosdb != nullptr)
        return 1;
    
    aosdb = new AOSDatabase ("dbFile");
    
    if (aosdb == nullptr)
        return 0;
    
    return 1;
}

bool aosDBIsInit ()
{
    return aosDBInit != nullptr;
}

void aosPrintDB ()
{
    aosdb->printDB ();
}

void aosDBRead ()
{
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
    return aosdb->getNMethods();
}

bool aosDBGetCurrMethodInfo (std::string& methodFullDesc, int& optLevel, int& counts)
{
    assert (currentIter != nullptr);
    
    if (*currentIter == aosdb->end ())
        return false;

    methodFullDesc = (**currentIter).getMethodFullDesc ();
    optLevel = (**currentIter).getOptLevel ();
    counts = (**currentIter).getCounts ();
    
    return true;
}

void aosDBAddMethodInfo (std::string& methodFullDesc, int optLevel, int counts)
{
    assert (currentIter != nullptr && *currentIter == aosdb->end ());
    
    aosdb->insertMethodInfo (methodFullDesc, optLevel, counts);
}

void aosDBVisitBegin ()
{
    currentIter = new AOSDatabase::iterator (aosdb->begin());
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
