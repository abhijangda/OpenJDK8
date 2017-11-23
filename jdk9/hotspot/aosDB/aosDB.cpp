#include "aosDB.h"

#include <fstream>
#include <assert.h>

pthread_mutex_t AOSDatabaseElement::_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t AOSDatabase::_mutex = PTHREAD_MUTEX_INITIALIZER;

/*
  The serialization format is <method desc> <optLevel> <counts>
*/
void AOSDatabase::readDB ()
{
    std::ifstream file(dbFilePath);
    std::cout << "Reading from dbFile " << dbFilePath << std::endl;
    assert (file.is_open());
    
    while (!file.eof ())
    {
        //std::cout << "tellg " << file.tellg () << std::endl;
        AOSDatabaseElement e;
        file >> e;
        if (e.getMethodFullDesc () == "")
            continue;
            
        methToElement.emplace (e.getMethodFullDesc (), e);
        
        if (verbose)
            std::cout << e;
    }
    
    file.close ();
}

void AOSDatabase::writeDB ()
{
    std::ofstream file(dbFilePath, std::ios::trunc);
    
    for (auto it : methToElement)
    {        
        if (verbose)
            std::cout << "Writing " << it.second;
        file << it.second;
    }
    
    file.close ();
}

void AOSDatabase::printDB ()
{
    for (auto it : methToElement)
    {
        std::cout << it.second;
    }
}

//Test
#ifdef __TEST_AOSDB__
int main ()
{
    AOSDatabase db ("dbFiletest", true, true);
    //db.readDB ();
    
    //db.printDB ();
    //return 0;
    for (int i = 0; i < 1000; i++)
    {
        db.insertMethodInfo ("meth_"+ std::to_string (i), i, i, i);
        for (int j = 0; j < 10; j++)
        {
            int c[MorphismLimit+1] = {i,i,i};
            auto q = "meth_"+ std::to_string (i);
            db.insertCallProfileForMethod (q, i, i+j, i+j, i+j, c);
        }
        for (int j = 0; j < 10; j++)
        {
            auto q = "meth_"+ std::to_string (i);
            db.insertCountProfileForMethod (q, i, 10);
        }
    }
    
    db.writeDB ();
    return 0;
}
#endif
