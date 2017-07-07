#include <string>
#include <unordered_map>
#include <iostream>

#ifndef __AOSDB_H__
#define __AOSDB_H__

class AOSDatabase;

class AOSDatabaseElement
{
private:
    std::string methodFullDesc;
    int optLevel;
    int counts;
    bool empty;
    
    AOSDatabaseElement ()
    {
    }
    
public:
    friend AOSDatabase;
    std::string& getMethodFullDesc () {return methodFullDesc;}
    int getOptLevel () {return optLevel;}
    int getCounts () {return counts;}
    
    std::string getClassName ()
    {
        return methodFullDesc.substr (0, methodFullDesc.find (";")+1);
    }
    
    std::string getMethodName ()
    {
        int pos = methodFullDesc.find (";") + 1;
        int size = methodFullDesc.find ("(") - pos;
        
        return methodFullDesc.substr (pos, size);  
    }
    
    AOSDatabaseElement (std::string _methodDesc, int _optLevel, int _counts):
        methodFullDesc (_methodDesc), optLevel (_optLevel), counts (_counts)
    {
    }
    
    AOSDatabaseElement (const AOSDatabaseElement& a)
    {
        methodFullDesc = a.methodFullDesc;
        optLevel = a.optLevel;
        counts = a.counts;
    }
    
     friend std::ostream &operator<< (std::ostream &os, const AOSDatabaseElement& e) { 
         os << e.methodFullDesc << " " << e.optLevel << " " << e.counts << "\n";
         return os;            
      }

      friend std::istream &operator>>(std::istream &is, AOSDatabaseElement& e) { 
         is >> e.methodFullDesc >> e.optLevel >> e.counts;
         return is;            
      }
};

class AOSDatabase
{
private:   
    std::string dbFilePath;
    std::unordered_map <std::string, AOSDatabaseElement> methToElement;
    bool verbose;
    
public:
    bool is_verbose () {return verbose;}
    
    class iterator
    {
        private:
            std::unordered_map <std::string, AOSDatabaseElement>::iterator mapIter;
            
        public:
            iterator (std::unordered_map <std::string, AOSDatabaseElement>::iterator _iter):
                mapIter (_iter)
            {}
            
            AOSDatabaseElement& operator* ()
            {
                return mapIter->second;
            }
            
            iterator operator++ ()
            {
                auto it = ++mapIter;
                return iterator (it);
            }
            
            bool operator== (iterator it2)
            {
                return this->mapIter == it2.mapIter;
            }
    };
    
    AOSDatabase (std::string _dbFile, bool _verbose = false) : dbFilePath(_dbFile), verbose (_verbose)
    {
        if (verbose)
        {
            std::cout << "methToElement initialized with " << methToElement.size () << " elements" << std::endl;
        }
    }
    
    AOSDatabase (std::unordered_map <std::string, AOSDatabaseElement> _methToElement, 
                 bool _verbose = false):
        methToElement (_methToElement), verbose (_verbose)
    {
    }
    
    void readDB ();
    void writeDB ();
    void clearDB () {methToElement.clear ();}
    void printDB ();
    
    int getNMethods ()
    {
        return methToElement.size ();
    }

    iterator begin () 
    {
        auto it = methToElement.begin();
        return iterator (it);
    } 
    
    iterator end () 
    {
        auto it = methToElement.end();
        return iterator (it);
    }
    
    bool setVerbose (bool v) {verbose = v;}
    
    void insertDBElement (AOSDatabaseElement& elem)
    {
        methToElement.emplace (elem.getMethodFullDesc (), elem);
    }
    
    void insertMethodInfo (std::string methodFullDesc, int optLevel,
                           int counts)
    {
        AOSDatabaseElement elem (methodFullDesc, optLevel, counts);
        if (verbose)
        {
            std::cout << "AOSDatabaseElement created " << elem << std::endl;
        }
        
        methToElement.emplace (methodFullDesc, elem);
        if (verbose)
        {
            std::cout << "AOSDatabaseElement added " << std::endl;
        }
    }
};
#endif 
