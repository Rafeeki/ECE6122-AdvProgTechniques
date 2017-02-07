import urllib
import sys
from signal import signal, SIGPIPE, SIG_DFL
mypath = "http://users.ece.gatech.edu/~riley/ece6122/"
mylines = urllib.urlopen(mypath).readlines()
for item in mylines:
	if "http" in item: 
		#print item[item.index("http"):item.find("</A")]
		last = item[item.index("http"):item.find("\">")]
	elif "./handout" in item: 
		#print item[item.index("./handout"):item.find("</A")] 
		last = mypath + item[item.index("handout"):item.find("\">")]
#print last
sys.stdout.write(last)
sys.stdout.flush()
signal(SIGPIPE,SIG_DFL)#&
