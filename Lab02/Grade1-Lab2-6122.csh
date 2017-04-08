#!/bin/csh
# Grading script for 6122 Lab1
echo "Working on user " $USER
if ( ! -e ThreadsTransform2D ) then
echo "Grade for user $USER is 0, No Submission found"
  exit(1)
endif
set user = $USER
cd ThreadsTransform2D
# Remove student's output files
rm -f *.txt
rm -f MyAfter1D.txt MyAfter2D.txt MyAfterInverse.txt
# Remove expected results
#rm -f after1d.txt  after2d.txt after2dInverse.txt
# Copy needed files 
cp /nethome/ECE6122/ThreadsTransform2D/after1d-correct.txt .
cp /nethome/ECE6122/ThreadsTransform2D/after2d-correct.txt .
cp /nethome/ECE6122/ThreadsTransform2D/after2dInverse.txt .
cp /nethome/ECE6122/ThreadsTransform2D/Tower.txt .
#cp /nethome/ECE6122/FourierTransform2D/Tower-Small.txt .

# Remove old builds and recompile
make clean
make
    ./threadDFT2d
#
@ grade = 100
set Comments = "Comment:"
# Check for M_PI usage
@ picount = `grep M_PI threadDFT2d.cc | wc -l`
if ( $picount == 0) then
  @ grade = $grade - 10
  set Comments = "$Comments Did not use M_PI"
endif
# # Check MPI usage
# @ mpicount = `grep MPI_Init threadDFT2d.cc | wc -l`
# if ($mpicount == 0) then
#   @ grade = $grade - 50
#   set Comments = "$Comments did NOT use MPI"
# endif
# Test if the Myafter1D file esists
if ( -e MyAfter1D.txt) then
  @ diffcount = `/nethome/ECE6122/compareFFT MyAfter1D.txt after1d-correct.txt`
  if ($diffcount > 10)   then
    @ grade = $grade - 25
    set Comments = "$Comments after1d mismatch"
  endif
else
  @ grade = $grade - 35
  set Comments = "$Comments missing MyAfter1D.txt"
endif

@ diffcount = 0
if ( -e MyAfter2D.txt) then
  @ diffcount = `/nethome/ECE6122/compareFFT MyAfter2D.txt after2d-correct.txt`
  if ($diffcount > 10)   then
    @ grade = $grade - 25
    set Comments = "$Comments after2d mismatch"
  endif
else
  @ grade = $grade - 35
  set Comments = "$Comments missing MyAfter2D.txt"
endif

@ diffcount = 0
if ( -e MyAfterInverse.txt) then
  @ diffcount = `/nethome/ECE6122/compareFFT MyAfterInverse.txt after2dInverse.txt`
  if ($diffcount > 10)   then
    @ grade = $grade - 25
    set Comments = "$Comments after2dInverse mismatch"
  endif
else
  @ grade = $grade - 35
  set Comments = "$Comments missing MyAfterInverse.txt"
endif



if ($grade < 0) then
  @ grade = 0
endif

echo "Grade for user $user is $grade, $Comments"
cd ..

