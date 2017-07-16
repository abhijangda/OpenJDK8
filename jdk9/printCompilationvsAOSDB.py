f = open ('./build/linux-x86_64-normal-server-release/jdk/bin/printcompilation.txt', 'r')
p = f.read()
f.close ()

print p.count ("made not entrant")

f = open ('./build/linux-x86_64-normal-server-release/jdk/bin/dbFile', 'r')
d = f.read()
f.close ()

print d.count ("made not entrant")

import re


def common_in_print_compilation_and_db ():
    for q in re.findall(r'.+', p):
        e = re.findall(r'[\d\s]+([bsn\s]*)\s+\d\s+(.+)\(', q)
        if (e != []):
            t = e[0][0].strip()
            name = e[0][1].strip()
            if (d.find(name) == -1):
    #            print e[0]
                print name

def duplicate_in_print_compilation ():
    a = []
    for q in re.findall (r'.+', p):
        e = re.findall(r'[\d\s]+([bsn\s]*)\s+(\d)\s+(.+)\(', q)
        if (e != []):
            t = e[0][0].strip()
            l = e[0][1].strip()
            name = e[0][2].strip()
            if (a.count (name) > 0):
                print name, l
            else:
                a += [name]

    print ("number of distinct are ", len(a))
    
duplicate_in_print_compilation ()
