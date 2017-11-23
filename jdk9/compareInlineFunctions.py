import commands

d = commands.getoutput ('cat build/linux-x86_64-normal-server-release/jdk/bin/inlineBaselineAvrora.txt')

import re

lines = set(re.findall(r'.+', d))

if False:
  print "Distinct lines "
  print "\n".join(list(lines))
  print "Number: ", len(lines)
else:
  f = open ('build/linux-x86_64-normal-server-release/jdk/bin/inline7.txt', 'r+')
  p = f.read()
  f.close ()
  
  c = 0
  for line in lines:
    if (p.find (line[:line.find ('OptLevel')]) == -1):
      c += 1
      print line
  
  print "Count", c
