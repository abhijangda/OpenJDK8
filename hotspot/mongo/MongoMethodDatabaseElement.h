#include <string>
using namespace std;

#ifndef __MONGO_METHOD_DATABASE_ELEMENT_H__
#define __MONGO_METHOD_DATABASE_ELEMENT_H__

class MongoMethodDatabaseElement
{
private:
    string methodFullDesc;
    int optLevel;
    double counts;
    
public:
    MongoMethodDatabaseElement (string _desc, int _optLevel, double _counts)
    {
        methodFullDesc = _desc;
        optLevel = _optLevel;
        counts = _counts;
    }
    
    string getMethodFullDesc ()
    {
        return methodFullDesc;
    }
    
    int getOptLevel ()
    {
        return optLevel;
    }
    
    double getCounts ()
    {
        return counts;
    }
    
    string getClassName ()
    {
        return methodFullDesc.substr (0, methodFullDesc.find (";")+1);
    }
    
    string getMethodName ()
    {
        int pos = methodFullDesc.find (";") + 1;
        int size = methodFullDesc.find ("(") - pos;
        
        return methodFullDesc.substr (pos, size);  
    }
};

#endif 
