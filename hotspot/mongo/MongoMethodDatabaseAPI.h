#include "MongoMethodDatabaseElement.h"

int mongo_aosdb_initialize ();
bool mongo_aosdb_isinitialized ();
void mongo_aosdb_readAllDocuments ();
MongoMethodDatabaseElement* mongo_aosdb_getMethod (int i);
int mongo_aosdb_getNMethods ();
void mongo_aosdb_setVerbose ();
MongoMethodDatabaseElement* mongo_aosdb_find (string methodFullDesc);
