
CPPFLAGS=-std=c++11 -fPIC

all: aosDBAPI.so

aosDBAPI.so:  aosDBAPI.cpp aosDB.cpp aosDBAPI.h aosDB.h
	g++ $(CPPFLAGS) $< -c -o $@

testAPI:  aosDBAPI.cpp
	g++ $(CPPFLAGS) aosDB.cpp -D__TEST_API__ $< -o $@

testAOSDB:  aosDB.cpp 
	g++ $(CPPFLAGS) -D__TEST_AOSDB__ $< -o $@

clean:
	rm aosDBAPI.so
	rm testAPI
	rm testAOSDB
