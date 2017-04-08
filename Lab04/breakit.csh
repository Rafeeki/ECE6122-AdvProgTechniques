#!/bin/csh
set user = $1
set Expected = HelloFrom.rgentry8.RNqAPvuGoCFmiVqBoZvuVQOFcfvatPpTwtEmzZFNr
set Actual = `./BreakRSA  3732361943063156869 2353158482122596563  1144396136088474149 3118136502542392450 2566135240754842358 1508646099011884447 106553438845203707 1471110284560132878 854070655717207707 1485903517577302497 1998238202659219549 3098145005932033064`
echo "expected is $Expected" 
echo "actal    is $Actual"
if ( "$Expected" == "$Actual" ) then
echo "Grade for user $user is 100"
else
echo "Grade for user $user is 50"
endif
