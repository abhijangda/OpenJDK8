#include "MongoMethodDatabaseAPI.h"
#include "aosDB.h"

#include <iostream>
#include <vector>

static MongoMethodDatabase* aosDB;
static bool initialized = false;

int mongo_aosdb_initialize ()
{
  if (initialized)
    return 0;
    
  aosDB = new MongoMethodDatabase ();
  initialized = true;

  return 1;
}

bool mongo_aosdb_isinitialized ()
{
  return initialized;
}

void mongo_aosdb_readAllDocuments ()
{
    aosDB->readAllDocuments ();
}

MongoMethodDatabaseElement* mongo_aosdb_getMethod (int i)
{
    return aosDB->getMethod (i);
}

int mongo_aosdb_getNMethods ()
{
    return aosDB->getNMethods ();
}

void mongo_aosdb_setVerbose ()
{
    aosDB->setVerbose ();
}

MongoMethodDatabaseElement* mongo_aosdb_find (string methodFullDesc)
{
    return aosDB->find (methodFullDesc);
}


/*int main ()
{
mongo_aosdb_initialize ();
    MongoMethodDatabaseElement* elem = mongo_aosdb_find ("Ljava/lang/Object;registerNatives()V");
if (elem == NULL)
	std::cout<<"elem null" << std::endl;
else
	std::cout<<elem->getOptLevel()<<std::endl;
}*/
