#include "aosDB.h"

#include <fstream>
#include <assert.h>

/*
  The serialization format is <method desc> <optLevel> <counts>
*/
void AOSDatabase::readDB ()
{
    std::ifstream file(dbFilePath);
    assert (file.is_open());
    
    while (!file.eof ())
    {
        AOSDatabaseElement e;
        file >> e;
        if (e.getMethodFullDesc () == "")
            continue;
        methToElement.emplace (e.getMethodFullDesc (), e);
        //std::cout << e;
    }
    
    file.close ();
}

void AOSDatabase::writeDB ()
{
    std::ofstream file(dbFilePath, std::ios::trunc);
    
    for (auto it : methToElement)
    {        
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
    AOSDatabase db ("dbFile");
    
    db.readDB ();
    
    db.printDB ();
//    return 0;
    for (int i = 0; i < 1000; i++)
    {
        db.insertMethodInfo ("meth_"+ std::to_string (i), i, i);
    }
    
    db.writeDB ();
    return 0;
}
#endif
