f = open ('./build/linux-x86_64-normal-server-release/jdk/bin/printcompilation1.txt', 'r')
p = f.read()
f.close ()

#print p.count ("made not entrant")
print p.count("inline (hot)")

f = open ('./build/linux-x86_64-normal-server-release/jdk/bin/out1.txt', 'r')
d = f.read()
f.close ()

#print d.count ("made not entrant")
print d.count ("inline (hot)")
p, d = d, p
print p == d
import re


def common_in_print_compilation_and_db ():
    l = 0
    for q in re.findall(r'.+', p):
        e = re.findall(r'[\d\s]+([bsn\s]*)\s+\d\s+(.+)\(', q)
        if (e != []):
            t = e[0][0].strip()
            name = e[0][1].strip()
            if (d.find(name) == -1):
    #            print e[0]
                print name, "at line ", l
        l+=1
        
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

def compare_inline_functions ():
    l = 0
    for q in re.findall(r'.+', p):
        e = q
        if (e.find ("@") != -1 and e.find ("inline (hot)") != -1):
            try:
                e = re.findall (r'\d+\s+([\w\.\:\$\d]+)\s+\(', e [e.find ("@") + 1:e.find("inline (hot)")])[0]
                if (e not in d):
                    print e.strip(), " at line ", l
            except Exception as ex:
                print e, ex
        l+=1
    
compare_inline_functions ()
