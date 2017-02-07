#!/usr/bin/bash
firefox http://users.ece.gatech.edu/~riley/ece6122/
firefox https://helix.gtpe.gatech.edu/local/kalturamediagallery/index.php?courseid=11327
python $PWD/LinkReader.py | wget &
#ssh -vv -l rgentry8 deepthought-login.cc.gatech.edu
