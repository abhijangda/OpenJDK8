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
    int highestOptLevel;
    int counts;
    int bci;
    bool empty;
    int highestOsrBci; //-1 if OSR never happened, if OSR happened once represents the bci
    int highestOsrLevel;
    
    AOSDatabaseElement ()
    {
    }
    
public:
    static int getInvalidBciValue () {return -2;}
    friend AOSDatabase;
    std::string& getMethodFullDesc () {return methodFullDesc;}
    int getHighestOptLevel () {return highestOptLevel;}
    int getCounts () {return counts;}
    //int getBci () {return bci;}
    int getHighestOsrBci () {return highestOsrBci;}
    int getHighestOsrLevel () {return  highestOsrLevel;}
    void setHighestOptLevel (int l){highestOptLevel=l;}
    void setCounts (int c){counts = c;}
    //void setBci (int b) {bci=b;}
    void setHighestOsrBci (int o) {highestOsrBci = o;}
    void setHighestOsrLevel (int o) {highestOsrLevel = o;}
    
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
    
    AOSDatabaseElement (std::string _methodDesc, int _highestOptLevel, int _counts, 
                        int _highestOsrBci, int _highestOsrLevel):
        methodFullDesc (_methodDesc), highestOptLevel (_highestOptLevel), counts (_counts), 
        highestOsrBci(_highestOsrBci), highestOsrLevel(_highestOsrLevel)
    {
    }
    
    AOSDatabaseElement (const AOSDatabaseElement& a)
    {
        methodFullDesc = a.methodFullDesc;
        highestOptLevel = a.highestOptLevel;
        counts = a.counts;
        highestOsrBci = a.highestOsrBci;
        highestOsrLevel = a.highestOsrLevel;
    }
    
     friend std::ostream &operator<< (std::ostream &os, const AOSDatabaseElement& e) { 
         os << e.methodFullDesc << " " << e.highestOptLevel << " " << e.counts << " " 
            << e.highestOsrBci << " " << e.highestOsrLevel << "\n";
         return os;            
      }

      friend std::istream &operator>>(std::istream &is, AOSDatabaseElement& e) { 
         is >> e.methodFullDesc >> e.highestOptLevel >> e.counts >> 
            e.highestOsrBci >> e.highestOsrLevel;
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
        /*insertMethodInfo (elem.getMethodFullDesc (), elem.getHighestOptLevel (), 
                          elem.getCounts (), elem.getHighestOsrBci (), elem.getHighestOsrLevel ());*/
    }
    
    void insertMethodInfo (std::string methodFullDesc, int optLevel,
                           int counts, int bci)
    {
        if (verbose)
        {
            if (bci == -1)
            {
                AOSDatabaseElement elem (methodFullDesc, optLevel, counts, bci, -1);
                std::cout << "AOSDatabaseElement created " << elem << std::endl;
            }
            else
            {
                AOSDatabaseElement elem (methodFullDesc, optLevel, counts, -1, bci);
                std::cout << "AOSDatabaseElement created " << elem << std::endl;
            }
        }
        
        int highestOsrBci = -1;
        int highestOsrLevel = -1;
        int highestOptLevel = -1;
        auto it = methToElement.find (methodFullDesc);
        
        if (it == methToElement.end ())
        {           
            if (bci == -1)
            {
                highestOptLevel = optLevel;
            }
            else
            {
                highestOsrBci = bci;
                highestOsrLevel = optLevel;
            }
            
            AOSDatabaseElement elem (methodFullDesc, highestOptLevel, counts, 
                                     highestOsrBci, highestOsrLevel);
            methToElement.emplace (methodFullDesc, elem);
        }
        else
        {
            highestOsrBci = it->second.getHighestOsrBci ();
            highestOsrLevel = it->second.getHighestOsrLevel ();
            highestOptLevel = it->second.getHighestOptLevel ();
            
            if (bci == -1)
            {
                highestOptLevel = optLevel;
            }
            else
            {
                highestOsrLevel = optLevel;
                highestOsrBci = bci;
            }
            
            it->second.setHighestOptLevel (highestOptLevel);
            it->second.setCounts (counts);
            it->second.setHighestOsrBci (highestOsrBci);
            it->second.setHighestOsrLevel (highestOsrLevel);
            //methToElement.erase (it);
            //AOSDatabaseElement elem (methodFullDesc, optLevel, counts, bci, osr_once_bci);
            //methToElement.emplace (methodFullDesc, elem);
        }
        
        if (verbose)
        {
            AOSDatabaseElement elem (methodFullDesc, highestOptLevel, counts, highestOsrBci, highestOsrLevel);
            std::cout << "AOSDatabaseElement added " << elem << std::endl;
        }
    }
    
    bool findMethodInfo (const std::string& methodFullDesc, int& highestOptLevel, 
                         int& counts, int& highestOsrBci, int& highestOsrLevel)
    {
        auto it = methToElement.find (methodFullDesc);
        if (it == methToElement.end ())
        {
            if (recordStats)
                methodsNotFoundInDB++;
            return false;
        }
        
        highestOptLevel = it->second.getHighestOptLevel ();
        counts = it->second.getCounts ();
        highestOsrBci = it->second.getHighestOsrBci ();
        highestOsrLevel = it->second.getHighestOsrLevel ();
        
        if (recordStats)
        {
            methodsFoundInDB++;
            methodsFoundAtOptLevelInDB[highestOptLevel]++;
        }
        
        return true;
    }
};
#endif 
