(./GradeRSA RSA-output.txt $USER) > grade_output.txt
tail -1 grade_output.txt | mail -s "Grading finished" "$USER@gatech.edu"
