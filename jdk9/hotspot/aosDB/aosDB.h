#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>

#ifndef __AOSDB_H__
#define __AOSDB_H__

#define NUM_OPT_LEVELS 5

class AOSDatabase;

class AOSDatabaseElement
{
private:
    std::string methodFullDesc;
    int optLevel;
    int counts;
    int bci;
    bool empty;
    
    AOSDatabaseElement ()
    {
    }
    
public:
    friend AOSDatabase;
    std::string& getMethodFullDesc () {return methodFullDesc;}
    int getOptLevel () {return optLevel;}
    int getCounts () {return counts;}
    int getBci () {return bci;}
    
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
    
    AOSDatabaseElement (std::string _methodDesc, int _optLevel, int _counts, int _bci):
        methodFullDesc (_methodDesc), optLevel (_optLevel), counts (_counts), bci (_bci)
    {
    }
    
    AOSDatabaseElement (const AOSDatabaseElement& a)
    {
        methodFullDesc = a.methodFullDesc;
        optLevel = a.optLevel;
        counts = a.counts;
        bci = a.bci;
    }
    
     friend std::ostream &operator<< (std::ostream &os, const AOSDatabaseElement& e) { 
         os << e.methodFullDesc << " " << e.optLevel << " " << e.counts << " " << e.bci << "\n";
         return os;            
      }

      friend std::istream &operator>>(std::istream &is, AOSDatabaseElement& e) { 
         is >> e.methodFullDesc >> e.optLevel >> e.counts >> e.bci;
         return is;            
      }
};

class AOSDatabase
{
private:   
    std::string dbFilePath;
    std::unordered_map <std::string, AOSDatabaseElement> methToElement;
    bool verbose;
    bool recordStats;
    int methodsAdded;
    int methodsInDB;
    int methodsFoundInDB;
    int methodsNotFoundInDB;
    std::vector<int> methodsFoundAtOptLevelInDB;
    
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
    
    AOSDatabase (std::string _dbFile, bool _verbose, bool _recordStats) : dbFilePath(_dbFile), verbose (_verbose),
    recordStats (_recordStats), methodsFoundAtOptLevelInDB (std::vector <int> (NUM_OPT_LEVELS, 0))
    {
        if (verbose)
        {
            std::cout << "methToElement initialized with " << methToElement.size () << " elements" << std::endl;
        }
    }
    
    AOSDatabase (std::unordered_map <std::string, AOSDatabaseElement> _methToElement, 
                 bool _verbose, bool _recordStats):
        methToElement (_methToElement), verbose (_verbose), 
        recordStats (_recordStats), methodsFoundAtOptLevelInDB (std::vector <int> (NUM_OPT_LEVELS, 0))
    {
    }
    
    void readDB ();
    void writeDB ();
    void clearDB () {methToElement.clear ();}
    void printDB ();
    int getMethodsFoundInDB () {return methodsFoundInDB;}
    int getMethodsNotFoundInDB () {return methodsNotFoundInDB;}
    bool getRecordStats () {return recordStats;}
    int getMethodsFoundAtOptLevelInDB (int l) {return methodsFoundAtOptLevelInDB[l];}
    
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
                           int counts, int bci)
    {
        AOSDatabaseElement elem (methodFullDesc, optLevel, counts, bci);
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
    
    bool findMethodInfo (const std::string& methodFullDesc, int& optLevel, int& counts, int& bci)
    {
        auto it = methToElement.find (methodFullDesc);
        if (it == methToElement.end ())
        {
            if (recordStats)
                methodsNotFoundInDB++;
            return false;
        }
        optLevel = it->second.getOptLevel ();
        counts = it->second.getCounts ();
        bci = it->second.getBci ();
        if (recordStats)
        {
            methodsFoundInDB++;
            methodsFoundAtOptLevelInDB[optLevel]++;
        }
        
        return true;
    }
};
#endif 
