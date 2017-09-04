#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>

#include <assert.h>

#include <pthread.h>

#ifndef __AOSDB_H__
#define __AOSDB_H__

#define NUM_OPT_LEVELS 5

class AOSDatabase;

enum { MorphismLimit = 2 }; // Max call site's morphism we care about
enum ProfileType 
{
    Invalid = -1,
    CallProfile = 0,
    CountProfile = 1,
    //Update Max and Min when new profile is added and always add new profiletype
    //before Min and Max
    Min = CallProfile,
    Max = CountProfile 
};
    
class AOSDatabaseElementProfile
{
public:
    
    ProfileType profileType;
    
    AOSDatabaseElementProfile ()
    {
        profileType = Invalid;
    }
    
    AOSDatabaseElementProfile (ProfileType _profileType) : profileType (_profileType)
    {
    }
    
    ProfileType getProfileType () const {return profileType;}
    
    AOSDatabaseElementProfile (const AOSDatabaseElementProfile& a)
    {
        profileType = a.profileType;
    }
};

class AOSDatabaseElementCountProfile : public AOSDatabaseElementProfile
{
private:
    int count;

public:
    AOSDatabaseElementCountProfile () : AOSDatabaseElementProfile (ProfileType::CountProfile),
    count (0)
    {
    }
    
    AOSDatabaseElementCountProfile (int c) : AOSDatabaseElementProfile (ProfileType::CountProfile),
    count (c)
    {
    }
    
    int getCount () const {return count;}
    int setCount (int c) {count = c;}
    
    friend std::ostream &operator<< (std::ostream &os, const AOSDatabaseElementCountProfile& e) 
    {
        os << (int)e.getProfileType () << " " << e.getCount () << " ";
    }
};

class AOSDatabaseElementCallProfile : public AOSDatabaseElementProfile
{
private:
    int  limit;                // number of receivers have been determined
    int  morphism;             // determined call site's morphism
    int  count;                // # times has this call been executed
    int  receiver_count[MorphismLimit + 1]; // # times receivers have been seen
    
public:

    AOSDatabaseElementCallProfile () : AOSDatabaseElementProfile (ProfileType::CallProfile)
    {
        limit=0;
        morphism = 0;
        count = -1;
        receiver_count [0] = -1;
        receiver_count [1] = -1;
        receiver_count [2] = -1;
    }
    
    AOSDatabaseElementCallProfile (int _limit, int _morphism, int _count, int _receiver_count[MorphismLimit+1]):
         AOSDatabaseElementProfile (ProfileType::CallProfile), limit(_limit), morphism(_morphism), count (_count)
    {
        for (int i = 0; i < MorphismLimit +1; i++)
            receiver_count [i] = _receiver_count[i];
    }

    int getLimit () const {return limit;}
    int getMorphism () const {return morphism;}
    int getCount() const {return count;}
    int getReceiverCountAtIndex (int i) const {return receiver_count[i];}
    void getReceiverCount (int* _receiver_count)
    {
        for (int i = 0; i < MorphismLimit+1; i++)
        {
            _receiver_count[i] = receiver_count[i];
        }
    }
    
    void setLimit (int _l) {limit = _l;}
    void setMorphism (int _m) {morphism = _m;}
    void setCount (int _c) {count = _c;}
    void setReceiverCountAtIndex (int i, int v) {assert (i < MorphismLimit+1); receiver_count[i] = v;}
    void setReceiverCount (int _receiver_count[MorphismLimit+1])
    {
        for (int i = 0; i < MorphismLimit+1; i++)
        {
            receiver_count[i] = _receiver_count[i];
        }
    }
    
    friend std::ostream &operator<< (std::ostream &os, const AOSDatabaseElementCallProfile& e) 
    { 
        os << (int)e.getProfileType () << " " << e.getLimit () << " " << e.getCount () << 
            " " << e.getMorphism () << " ";
        int i;
        
        for (i = 0; i < MorphismLimit + 1; i++)
        {
            os << e.getReceiverCountAtIndex (i) << " "; 
        }
    }
};

class AOSDatabaseElement
{
public:
    static pthread_mutex_t _mutex;

private:
    std::string methodFullDesc;
    int highestOptLevel;
    int counts;
    int bci;
    bool empty;
    int highestOsrBci; //-1 if OSR never happened, if OSR happened once represents the bci
    int highestOsrLevel;
    int interpreterInvocationCount;
    int interpreterThrowoutCount;
    int invocationCount;
    int backedgeCount;
    
    //bci to an vector of AOSDatabaseElementProfile, such that 
    //v[profileType] is particular AOSDBProfile. For example,
    //v[0] is CallProfile, v[1] is CounterProfile
    std::unordered_map <int, std::vector<AOSDatabaseElementProfile*> > bciToProfile;
    //TODO: bci to counter data
    
    AOSDatabaseElement ()
    {
        //pthread_mutex_init (&__mutex, NULL);
    }
    
    AOSDatabaseElementProfile* getProfileForBci (int bci, 
                                                 ProfileType type)
    {
        auto it = bciToProfile.find (bci);
        if (it == bciToProfile.end ())
            return NULL;
        
        if (it->second[(int)type] == nullptr || 
            it->second[(int)type]->getProfileType () == ProfileType::Invalid)
            return NULL;
            
        return it->second [(int)type];
    }

    void setProfileForBci (int bci, AOSDatabaseElementProfile* profile)
    {
        assert (profile->getProfileType () != ProfileType::Invalid);
        assert (profile != nullptr);
        
        auto it = bciToProfile.find (bci);
        
        if (it == bciToProfile.end ())
        {
            auto v = std::vector <AOSDatabaseElementProfile*> ();
            for (int i = ProfileType::Min; i <= ProfileType::Max; i++)
            {
                v.push_back (nullptr);
            }
            
            bciToProfile[bci] = v;
        }
        
        it = bciToProfile.find (bci);
        
        if (it->second.size () != (ProfileType::Max+1))
        {
            std::cout << "ASSERTION FAILED: it->second.size () == (ProfileType::Max+1) "<< std::endl;
            assert (it->second.size () == (ProfileType::Max+1));
        }
        
        if (it->second [(int)profile->getProfileType ()] != nullptr)
            delete it->second [(int)profile->getProfileType ()];
            
        it->second [(int)profile->getProfileType ()] = profile;
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
    int getInterpreterInvocationCount () {return interpreterInvocationCount;}
    int getInterpreterThrowoutCount () {return interpreterThrowoutCount;}
    int getInvocationCount () {return invocationCount;}
    int getBackedgeCount () {return backedgeCount;}
    
    void setHighestOptLevel (int l){highestOptLevel=l;}
    void setCounts (int c){counts = c;}
    //void setBci (int b) {bci=b;}
    void setHighestOsrBci (int o) {highestOsrBci = o;}
    void setHighestOsrLevel (int o) {highestOsrLevel = o;}
    void setInterpreterInvocationCount (int count) {interpreterInvocationCount = count;}
    void setInterpreterThrowoutCount (int count) {interpreterThrowoutCount = count;}
    void setInvocationCount (int count) {invocationCount = count;}
    void setBackedgeCount (int b) {backedgeCount = b;}
    
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
    
    AOSDatabaseElementCallProfile* getCallProfileForBci (int bci)
    {
        AOSDatabaseElementProfile* prof = getProfileForBci (bci, 
                                                            ProfileType::CallProfile);
        if (prof->getProfileType () != ProfileType::CallProfile)
            return NULL;

        return (AOSDatabaseElementCallProfile*)(prof);
    }
    
    AOSDatabaseElementCountProfile* getCountProfileForBci (int bci)
    {
        AOSDatabaseElementProfile* prof = getProfileForBci (bci, 
                                                            ProfileType::CountProfile);
        if (prof->getProfileType () != ProfileType::CountProfile)
            return NULL;

        return (AOSDatabaseElementCountProfile*)(prof);
    }
    
    /*void setCallProfileLimitForBci (int bci, int _limit)
    {
        
    }
    
    void setCallProfileMorphismForBci (int bci, int _morphism)
    {
        
    }
    
    void setCallProfileCountForBci (int bci, int _count)
    {
        
    }
    
    void setCallProfileReceiverCountForBci (int bci, int _receiver_count)
    {
        
    }*/
    
    void setCallProfileForBci (int bci, int _limit, int _morphism, int _count, 
                               int _receiver_count[MorphismLimit+1])
    {
        pthread_mutex_lock (&_mutex);
        //std::cout << "Insert Call Profile at bci " << bci << std::endl;
        AOSDatabaseElementCallProfile* profile = new AOSDatabaseElementCallProfile (_limit, _morphism,
                                                                                    _count, _receiver_count);
        //std::cout << "setProfileForBci " << bci << std::endl;
        setProfileForBci (bci, profile);
        pthread_mutex_unlock (&_mutex);
        //std::cout << "setProfileForBci Done" << std::endl;
    }
    
    void setCountProfileForBci (int bci, int count)
    {
        pthread_mutex_lock (&_mutex);
        //std::cout << "Insert Count Profile at bci " << bci << " count " << count << std::endl;
        AOSDatabaseElementCountProfile* profile = new AOSDatabaseElementCountProfile (count);
        //std::cout << "setProfileForBci " << bci << " profile " << profile << std::endl;
        setProfileForBci (bci, profile);
        pthread_mutex_unlock (&_mutex);
        //std::cout << "setProfileForBci Done" << std::endl;
    }
    
    AOSDatabaseElement (std::string _methodDesc, int _highestOptLevel, int _counts, 
                        int _highestOsrBci, int _highestOsrLevel):
        methodFullDesc (_methodDesc), highestOptLevel (_highestOptLevel), counts (_counts), 
        highestOsrBci(_highestOsrBci), highestOsrLevel(_highestOsrLevel), interpreterInvocationCount(-1),
        interpreterThrowoutCount(-1), invocationCount(-1), backedgeCount(-1)
    {
        //pthread_mutex_init (&__mutex, NULL);
    }
    
    AOSDatabaseElement (const AOSDatabaseElement& a)
    {
        methodFullDesc = a.methodFullDesc;
        highestOptLevel = a.highestOptLevel;
        counts = a.counts;
        highestOsrBci = a.highestOsrBci;
        highestOsrLevel = a.highestOsrLevel;
        bciToProfile = a.bciToProfile;
        interpreterInvocationCount = a.interpreterInvocationCount;
        interpreterThrowoutCount = a.interpreterThrowoutCount;
        invocationCount = a.invocationCount;
        backedgeCount = a.backedgeCount;
    }
    
    friend std::ostream &operator<< (std::ostream &os, const AOSDatabaseElement& e) { 
         os << e.methodFullDesc << " " << e.highestOptLevel << " " << e.counts << " " 
            << e.highestOsrBci << " " << e.highestOsrLevel << " " << e.interpreterInvocationCount
            << " " << e.interpreterThrowoutCount << " " << e.invocationCount << " " <<
            e.backedgeCount << " ";
         
         os << "[" << " ";
         
         os << e.bciToProfile.size () << " ";
         
         for (auto it : e.bciToProfile)
         {
             os << "{ ";
             
             os << it.first << " ";
             int i = 0;
             for (auto profile : it.second)
             {
                 //std::cout << "Profile at i " << i << " is " << profile << std::endl;
                 i++;
                 
                if (profile == nullptr)
                {
                    os << -1 << " ";
                }
                else if (profile->getProfileType () == ProfileType::CountProfile)
                {
                    os << *(AOSDatabaseElementCountProfile*) profile;
                }
                else if (profile->getProfileType () == ProfileType::CallProfile)
                {
                    os << *(AOSDatabaseElementCallProfile*) profile;
                }
             }
             
             os << "} ";
         }
         
         os << "]\n";
         
         return os;            
      }

      friend std::istream &operator>>(std::istream &is, AOSDatabaseElement& e) { 
         is >> e.methodFullDesc;
         //std::cout << "Reading for e.methodFullDesc " << e.methodFullDesc << " ";
         is >> e.highestOptLevel >> e.counts >> 
            e.highestOsrBci >> e.highestOsrLevel >> e.interpreterInvocationCount
            >> e.interpreterThrowoutCount >> e.invocationCount >>
            e.backedgeCount;
         
         std::string c; //for brackets
         is >> c; // [
         
         int sites;
         is >> sites;
         //std::cout << sites << " ";
         for (int i = 0; i < sites; i++)
         {
             is >> c; //{
             //std::cout << " c " << c << " "; 
             int _bci;
             is >> _bci;
             //std::cout << " bci " << _bci << " ";
             for (int i = ProfileType::Min; i <= ProfileType::Max; i++)
             {
                 int type;
                 
                 is >> type;
                 //std::cout << " i " << i << " type " << type;
                 if (type == ProfileType::Invalid)
                 {
                 }
                 else if (type == ProfileType::CallProfile)
                 {
                     int limits, count, morphism, receiver_count[MorphismLimit+1];
                     
                     is >> limits >> count >> morphism;
                     
                     for (int j = 0; j < MorphismLimit + 1; j++)
                     {
                         is >> receiver_count[j];
                     } 
                     
                     e.setCallProfileForBci (_bci, limits, count, morphism, receiver_count);
                 }
                 else if (type == ProfileType::CountProfile)
                 {
                     int count;
                     
                     is >> count;
                     
                     e.setCountProfileForBci (_bci, count);
                 }
                 else
                 {
                    //std::cout << "Invalid ProfileType detected " << type << std::endl;
                    //assert (false);
                 }
             }
             
             is >> c;
         }
         
         is >> c;

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
    
    void insertHotDataForMethod (std::string& methodFullDesc, 
                                 int _interpreterInvocationCount, int _throwoutCount,
                                 int _invocationCount, int _backedgeCount)
    {
        if (verbose)
        {
            std::cout << "Insert HotData For " << methodFullDesc << " IntInvokeCnt "<< 
                _interpreterInvocationCount << " ThrowCount "<< _throwoutCount <<
                " InvokeCount " <<  _invocationCount << " BackedgeCount " << 
                _backedgeCount << std::endl;
        }
        
        auto it = methToElement.find (methodFullDesc);
        
        if (it == methToElement.end ())
        {
            AOSDatabaseElement elem (methodFullDesc, -1, -1, -1, -1);
            methToElement.emplace (methodFullDesc, elem);
            elem.setInterpreterInvocationCount (_interpreterInvocationCount);
            elem.setInterpreterThrowoutCount (_throwoutCount);
            elem.setInvocationCount (_invocationCount);
            elem.setBackedgeCount (_backedgeCount);
            
            if (verbose)
            {
                std::cout << "Inserted Method " << elem << std::endl;
            }
        }
        else
        {
            if (_interpreterInvocationCount != -1)
                it->second.setInterpreterInvocationCount (_interpreterInvocationCount);
                
            if (_throwoutCount != -1)
                it->second.setInterpreterThrowoutCount (_throwoutCount);
            
            if (_invocationCount != -1)
                it->second.setInvocationCount (_invocationCount);
            
            if (_backedgeCount != -1)
                it->second.setBackedgeCount (_backedgeCount);
                
            if (verbose)
            {
                std::cout << "Inserted Method " << it->second << std::endl;
            }
        }
    }
    
    void insertCallProfileForMethod (std::string& methodFullDesc, int bci,
                                         int _limit, int _morphism, int _count, 
                                         int _receiver_count[MorphismLimit+1])
    {
        auto it = methToElement.find (methodFullDesc);
        
        if (it == methToElement.end ())
        {
            AOSDatabaseElement elem (methodFullDesc, -1, -1, -1, -1);
            methToElement.emplace (methodFullDesc, elem);
            //std::cout << "FOUND: setCallProfileForBci " << methodFullDesc << " at bci " << bci << std::endl;
            elem.setCallProfileForBci (bci, _limit, _morphism, _count, 
                                           _receiver_count);
        }
        else
        {
            //std::cout << "NOT found: setCallProfileForBci " << methodFullDesc << " at bci " << bci << std::endl;
            it->second.setCallProfileForBci (bci, _limit, _morphism, _count, 
                                                 _receiver_count);
        }
    }
    
    void insertCountProfileForMethod (std::string& methodFullDesc, int bci,
                                      int _count)
    {
        auto it = methToElement.find (methodFullDesc);
        
        if (it == methToElement.end ())
        {
            AOSDatabaseElement elem (methodFullDesc, -1, -1, -1, -1);
            methToElement.emplace (methodFullDesc, elem);
            elem.setCountProfileForBci (bci, _count);
        }
        else
        {
            it->second.setCountProfileForBci (bci, _count);
        }
    }
    
    bool findHotDataForMethod (const std::string& methodFullDesc, 
                               int& interpreterInvocationCount, int& throwoutCount,
                               int& invocationCount, int& backedgeCount)
    {
        auto it = methToElement.find (methodFullDesc);
        
        if (it == methToElement.end ())
        {
            return false;
        }
        else
        {
            interpreterInvocationCount = it->second.getInterpreterInvocationCount ();
            throwoutCount = it->second.getInterpreterThrowoutCount ();
            invocationCount = it->second.getInvocationCount ();
            backedgeCount = it->second.getBackedgeCount ();
            
            return true;
        }
    }
    
    bool findCallProfileForMethod (const std::string& methodFullDesc, int bci,
                                         int& _limit, int& _morphism, int& _count, 
                                         int* _receiver_count)
    {
        auto it = methToElement.find (methodFullDesc);
        
        if (it == methToElement.end ())
        {
            return false;
        }
        else
        {
            AOSDatabaseElementCallProfile* p = it->second.getCallProfileForBci(bci);
            if (p == NULL)
                return false;
                
            _limit = p->getLimit();
            _morphism = p->getMorphism();
            _count = p->getCount();
            p->getReceiverCount(_receiver_count);
            
            return true;
        }
    }

    bool findCountProfileForMethod (const std::string& methodFullDesc, int bci,
                                    int& _count)
    {
        auto it = methToElement.find (methodFullDesc);
        
        if (it == methToElement.end ())
        {
            return false;
        }
        else
        {
            AOSDatabaseElementCountProfile* p = it->second.getCountProfileForBci(bci);
            
            if (p == NULL)
                return false;
                
            _count = p->getCount();
            
            return true;
        }
    }
    
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
