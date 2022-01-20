#!/bin/csh

#Phase 1: Setup your environment – directory: ex1_<last_four_digits_of_your_C#>
#Command: cr_project <project_name>
#C12108004

#globals

set MKD=/bin/mkdir
set TOUCH=/usr/bin/touch
set CP=/bin/cp
set RM=/bin/rm
set PROJNAME=$argv[1]
set PROJPATH=/Users/jingu/Documents/University_of_Miami/Fall_2021/ECE322/ECE322Exam1

echo "you entered ${#argv} arguments"

if (${#argv} < 1) then 
	echo "usage: $0  <projectName>"
	exit
endif

#directory to store project
if (! -d $PROJPATH) then
	echo "$PROJPATH does not exist"
	$MKD $PROJPATH
else   
    echo "$PROJPATH exits"
    echo  " "
endif
  
echo "Making $PROJNAME inside main directory: $PROJPATH"
$MKD $PROJPATH/$PROJNAME
echo "Creating bin and src direcotries in $PROJNAME"
$MKD $PROJPATH/$PROJNAME/bin
$MKD $PROJPATH/$PROJNAME/src
$TOUCH $PROJPATH/$PROJNAME/src/main.c
exit

