#!/bin/csh

# set some globals
#set gardir = /Users/jingu/garbage
set gardir = ~/garbage
set CP = /bin/cp
set RM = /bin/rm
set location = $PWD

echo "you entered ${#argv} arguments"

if (${#argv} < 1) then 
	echo "usage: $0 <filename>"
    echo "usage: -[lL]"
    echo "usage: -[fF] <filename>"
	exit
endif

# check if garbage dir exists
if (! -d $gardir) then
	echo "$gardir does not exist"
	mkdir gardir
else   
    echo "$gardir exits"

endif

switch($argv[1])

    case -[fF]:
        set force
        shift 
        breaksw

    case -[lL]:
        set list
        shift
        breaksw

    default:
        set trash
        breaksw

endsw

echo "swich cases completed"

if ( $?force ) then
    while ( ${#argv}>0 )
        echo "$argv[1] $#argv"
        #will check if the file exits in directory
        if (! -e $argv[1]) then
            echo "$argv[1] does not exist in directory or is a regular file"
        else    
            echo "File $argv[1] exits in directory"
            if (! -e ${gardir}/${argv[1]:t}) then
                echo "$argv[1] does not exists in garbage"
                echo "Copying $argv[1] into $gardir"
                echo "Removing $argv[1] from $gardir"
                $CP $argv[1] $gardir #copying file from directory to trash
                $RM $argv[1] #removing file from original local
                echo "$argv[1] $location" >> ${gardir}/index.txt
            else
                echo "File $argv[1] does exist in garbage"
                echo "Overwritting existing file in garbage"
                $CP -i $argv[1] $gardir
                $RM -i $argv[1]
                echo "$argv[1] $location" >> ${gardir}/index.txt
            endif
        endif
        shift
     end
endif

#if -[lL] is called then will acces the garbage directory and print list what is in it
if ( $?list ) then    
    ls ${gardir}
endif


# a regular trash function that just move nonexisting files into garbage
if ( $?trash ) then
    while ( ${#argv}>0 )
        #will check if the file exits in directory
        echo " $argv eeee"
        if (! -e $argv[1]) then
            echo "$argv[1] does not exist in directory or is a regular file"

        else    
            echo "File $argv[1] exists in directory"
            if (! -e ${gardir}/${argv[1]:t}) then
                echo "$argv[1] does not exists in garbage"
                echo "Copying $argv[1] into $gardir"
                echo "Removing $argv[1] from $gardir"
                $CP $argv[1] $gardir #copying file from directory to trash
                $RM $argv[1] #removing file from original local
                echo "$argv[1] $location" >> ${gardir}/index.txt
            
            else
                echo "File $argv[1] does exist in garbage"
            endif
        endif
        shift
    end
endif

exit









