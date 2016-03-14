#include <iostream>
#include <string>

using namespace std;
//To compile g++ -c -fPIC --std=c++11 mongo.cpp -I/usr/include/bsoncxx/v_noabi/ -I/usr/include/mongocxx/v_noabi/ -lmongocxx -lbsoncxx
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
void __mongo_xxxx_xxxx(bool verbose) {
    mongocxx::instance inst{};
    if (verbose)
        std::cout<<"MongoCXX::Instance created"<<std::endl;
    
    mongocxx::client conn{mongocxx::uri{}};
    
    if (verbose)
        std::cout<<"Mongo Connection created"<<std::endl;    
  
    auto database = conn["AOSDatabase"];
    if (verbose)
        std::cout<<"Obtained AOSDatabase"<<std::endl;
    
    auto aosCollection = database["AOSCollection"];
    if (verbose)
        std::cout<<"Obtained AOSCollection"<<std::endl;

    auto cursor = aosCollection.find({});
    
    for (auto&& doc : cursor) {
        int optLevel = doc["optLevel"].get_int32();
        bsoncxx::types::b_utf8 s = doc["methodFullDesc"].get_utf8();
        std::cout<<((bsoncxx::stdx::string_view)s)[0] <<std::endl;      
//wstring methodFullDesc = doc["methodFullDesc"].get_utf8();
    }
}

int main ()
{
  __mongo_xxxx_xxxx (true);
}

/*void insert ()
{
    collection.insert_one(document.view());
    auto cursor = collection.find({});

    for (auto&& doc : cursor) {
        std::cout << bsoncxx::to_json(doc) << std::endl;
    }
}*/

