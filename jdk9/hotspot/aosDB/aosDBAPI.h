#include <string>

#ifndef __AOSDBAPI_H__
#define __AOSDBAPI_H__

/*class AOSDBException
{
private:
    std::string msg;
public:
    AOSDBException (std::string _msg) : msg(_msg) {}
}*/

extern "C"{
int aosDBInit (bool verbose);
bool aosDBIsInit ();
void aosDBRead ();
bool aosDBVisitNextMethod ();
void aosDBVisitBegin ();
int aosDBGetNumberOfMethods ();
//mongo_aosdb_setVerbose ();
bool aosDBGetMethodInfo (std::string& methodFullDesc, int& optLevel, int& counts);
void aosDBAddMethodInfo (std::string& methodFullDesc, int optLevel, int counts);
void aosDBPrint ();
void aosDBWriteDB ();
void aosDBClearDB ();
}
#endif
