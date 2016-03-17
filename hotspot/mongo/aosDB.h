#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/database.hpp>

#include <unordered_map>
#include <vector>

#include "MongoMethodDatabaseElement.h"

//To compile g++ *.cpp *.h -c -fPIC --std=c++11 -I/usr/include/bsoncxx/v_noabi/ -I/usr/include/mongocxx/v_noabi/ -lmongocxx -lbsoncxx

class MongoMethodDatabase
{
private:
    mongocxx::client conn;
    mongocxx::database aosDB;
    mongocxx::collection aosCollection;
    bool verbose;
    int bulkCount;
    vector <MongoMethodDatabaseElement*> methods;
    unordered_map<string, MongoMethodDatabaseElement*> methodToDescMap;
    
public:
    MongoMethodDatabase ();
    
    void readAllDocuments ();
    
    void setVerbose ()
    {
        verbose = true;
    }
    
    int getNMethods ()
    {
        return methods.size ();
    }
    
    MongoMethodDatabaseElement* getMethod (int i)
    {
        return methods[i];
    }

    MongoMethodDatabaseElement* find (string methodFullDesc);
};
