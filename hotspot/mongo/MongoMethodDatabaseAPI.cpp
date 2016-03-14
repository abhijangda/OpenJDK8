#include "MongoMethodDatabaseAPI.h"
#include "aosDB.h"
#include <iostream>
static vector<MongoMethodDatabase*> aosDBs;

int mongo_aosdb_initialize ()
{
    aosDBs.push_back (new MongoMethodDatabase ());
    
    return aosDBs.size () - 1;
}

void mongo_aosdb_readAllDocuments (int i)
{
    aosDBs[i]->readAllDocuments ();
}

MongoMethodDatabaseElement* mongo_aosdb_getMethod (int j, int i)
{
    return aosDBs[j]->getMethod (i);
}

int mongo_aosdb_getNMethods (int i)
{
    return aosDBs[i]->getNMethods ();
}

void mongo_aosdb_setVerbose (int i)
{
    aosDBs[i]->setVerbose ();
}

int mongo_aosdb_getOptLevelForMethod (int i, string methodFullDesc)
{
    return aosDBs[i]->getOptLevelMethodForDesc (methodFullDesc);
}


int main ()
{
mongo_aosdb_initialize ();mongo_aosdb_initialize ();
    //std::cout<<aosDB.getOptLevelMethodForDesc ("java.lang.String::lastIndexOf(II)I");
}
