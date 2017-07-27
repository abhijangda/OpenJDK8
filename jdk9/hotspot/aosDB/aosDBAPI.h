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
void aosDBAddMethodInfo (std::string& methodFullDesc, int optLevel, int counts, int bci);
void aosDBPrint ();
void aosDBWriteDB ();
void aosDBClearDB ();
bool aosDBFindMethodInfo (std::string& methodFullDesc, int& highestOptLevel, int& counts,
                         int& highestOsrBci, int& highestOsrLevel);
int aosDBGetMethodsFoundInDB ();
int aosDBGetMethodsNotFoundInDB ();
int aosDBGetMethodsFoundAtOptLevelInDB (int l);
}
#endif
