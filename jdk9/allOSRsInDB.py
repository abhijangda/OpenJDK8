f = open ('./build/linux-x86_64-normal-server-release/jdk/bin/dbFile', 'r')
p = f.read()
f.close ()

import re

for q in re.findall(r'.+', p):
    if (q.find("-1") == -1):
        print q
