#include "MongoMethodDatabaseElement.h"

int mongo_aosdb_initialize ();
void mongo_aosdb_readAllDocuments (int i);
MongoMethodDatabaseElement* mongo_aosdb_getMethod (int j, int i);
int mongo_aosdb_getNMethods (int i);
void mongo_aosdb_setVerbose (int i);
int mongo_aosdb_getOptLevelForMethod (int i, string methodFullDesc);
