#include "aosDB.h"

#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/options/find.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::finalize;
mongocxx::instance inst{};

MongoMethodDatabase::MongoMethodDatabase () :
conn{mongocxx::uri{}}, aosDB(conn["AOSDatabase"]),
aosCollection (aosDB["AOSCollection"])
{
    bulkCount = 0;
    
    if (verbose)
        std::cout<<"Mongo Connection created"<<std::endl;    
  
    //aosDB = conn["AOSDatabase"];
    if (verbose)
        std::cout<<"Obtained AOSDatabase"<<std::endl;
    
    //aosCollection = aosDB["AOSCollection"];
    if (verbose)
        std::cout<<"Obtained AOSCollection"<<std::endl;
}

void MongoMethodDatabase::readAllDocuments()
{
    auto cursor = aosCollection.find({});
    
    for (auto&& doc : cursor) {
        int optLevel = doc["optLevel"].get_int32();
        double counts = doc["count"].get_double ();
        bsoncxx::types::b_utf8 s = doc["methodFullDesc"].get_utf8();
        bsoncxx::stdx::string_view str = ((bsoncxx::stdx::string_view)s);
        string methodFullDesc = "";
        
        for (int i = 0; i < str.length (); i++)
        {
            methodFullDesc += str[i];
        }
        
        if (verbose)
            std::cout <<"Read: " << methodFullDesc << " optLevel " << optLevel << " counts " << counts << std::endl;
        methods.push_back (new MongoMethodDatabaseElement (methodFullDesc, optLevel, counts));        
    }
}

MongoMethodDatabaseElement* MongoMethodDatabase::find (std::string methodFullDesc)
{
    try
    {
        auto cursor = aosCollection.find({document{} << "methodFullDesc" << methodFullDesc << finalize});
        
        for (auto&& doc : cursor) {
            int optLevel = doc["optLevel"].get_int32();
            double counts = doc["count"].get_double();
            return new MongoMethodDatabaseElement(methodFullDesc, optLevel, counts);
        }
        
        return NULL;
    }
    catch(...)
    {
    }
    
    return 0;
}
