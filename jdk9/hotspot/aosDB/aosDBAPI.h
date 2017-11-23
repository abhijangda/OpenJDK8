#include <string>

#ifndef __AOSDBAPI_H__
#define __AOSDBAPI_H__
#define NUM_OPT_LEVELS 5

/*class AOSDBException
{
private:
    std::string msg;
public:
    AOSDBException (std::string _msg) : msg(_msg) {}
}*/

extern "C"{
int aosDBInit (bool verbose, bool recordStats);
bool aosDBIsInit ();
void aosDBRead ();
bool aosDBVisitNextMethod ();
void aosDBVisitBegin ();
int aosDBGetNumberOfMethods ();
//mongo_aosdb_setVerbose ();
bool aosDBGetCurrMethodInfo (std::string& methodFullDesc, int& highestOptLevel, int& counts,
                             int& highestOsrBci, int& highestOsrLevel);
bool aosDBGetCurrMethodCallProfile (int bci, int& limits, int& morphism, 
                                        int& count, int* receiver_count);
void aosDBAddMethodInfo (std::string& methodFullDesc, int optLevel, int counts, int bci);
void aosDBAddMethodCallProfileForBci (std::string& methodFullDesc, int bci, 
                                          int limit, int morphism, int count,
                                          int receiver_count[]);
void aosDBAddMethodCountProfileForBci (std::string& methodFullDesc, int bci, int count);
void aosDBAddHotDataForMethod (std::string& methodFullDesc,
                                int interpreterInvocationCount, int throwoutCount,
                                int invocationCount, int backedgeCount);
void aosDBPrint ();
void aosDBWriteDB ();
void aosDBClearDB ();
bool aosDBFindMethodInfo (std::string& methodFullDesc, int& highestOptLevel, int& counts,
                         int& highestOsrBci, int& highestOsrLevel);
bool aosDBFindMethodCallProfile (std::string& methodFullDesc, int& bci, 
                                     int& limit, int& morphism, int& count,
                                     int receiver_count[]);
bool aosDBFindMethodCountProfile (std::string& methodFullDesc, int& bci, int& count);
bool aosDBFindHotDataForMethod (std::string& methodFullDesc,
                                int& interpreterInvocationCount, int& throwoutCount,
                                int& invocationCount, int& backedgeCount);
void aosDBAddIsInlinedForMethod (std::string& methodFullDesc, int bci, bool inlined);
bool aosDBFindIsInlinedForMethod (std::string& methodFullDesc, int bci, bool& inlined);
int aosDBGetMethodsFoundInDB ();
int aosDBGetMethodsNotFoundInDB ();
int aosDBGetMethodsFoundAtOptLevelInDB (int l);
}
#endif
