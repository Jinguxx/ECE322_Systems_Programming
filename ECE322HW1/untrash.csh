#!/bin/csh

# set some globals
set gardir = /Users/jingu/garbage
#set gardir = ~/garbage
set CP = /bin/cp
set RM = /bin/rm
set MKDIR = /bin/mkdir
set GREP = /usr/bin/grep #searches a file for a particular pattern of characters, and displays all lines that contain that pattern
set location = $PWD

echo "you entered ${#argv} arguments"

if (${#argv} < 1) then 
	echo "usage: $0 <filename>"
    echo "usage: -[df] <filename>"
    echo "usage: -[c]"
	exit
endif

# check if garbage dir exists
if (! -d $gardir) then
	echo "$gardir does not exist"
	mkdir gardir
else   
    echo "$gardir exits"
    echo  " "

endif

switch($argv[1])

    case -[dD]:
        set restore
        shift 
        breaksw

    case -[fF]:
        set force
        shift
        breaksw

    case -[cC]:
        set clear
        shift
        breaksw

    default:
        set untrash
        breaksw

endsw

echo "swich cases completed"

#if -[dD] is called Restore to original directory
if ( $?restore ) then
    while ( ${#argv}>0 )
        echo "$argv[1] $#argv"
        #will check if the file exits in garbage
        if (! -e ${gardir}/${argv[1]:t}) then
            echo "$argv[1] does not exist in garbage"

        else    
            echo "File $argv[1] exits in garbage"
            set pathgetter = `$GREP $gardir/index.txt -e "$argv[1]"`
            echo "${pathgetter[1]}"
            echo "Copying $gardir/$argv[1] ${pathgetter[2]}"
            echo "Removing $argv[1] from $gardir"
            $CP $gardir/$argv[1] ${pathgetter[2]} #copying file from trash to original
            $RM $gardir/$argv[1] #removing file from trash
         
        endif
        shift
     end
endif

#if -[fF] is called Force the overwrite of file(s) in the target directory
if ( $?force ) then
    while ( ${#argv}>0 )
        echo "$argv[1] $#argv"
        #will check if the file exits in garbage
        if (! -e ${gardir}/${argv[1]:t}) then
            echo "$argv[1] does not exist in garbage"

        else    
            echo "File $argv[1] exits in garbage"
            set pathgetter = `$GREP $gardir/index.txt -e "$argv[1]"`
            echo "${pathgetter[1]}"
            echo "Copying $gardir/$argv[1] ${pathgetter[2]}"
            echo "Removing $argv[1] from $gardir"
            $CP $gardir/$argv[1] ${PWD} #copying file from trash to original
            $RM $gardir/$argv[1] #removing file from trash
        
        endif
        shift
     end
endif

#if -[cC] is called then will clear the garbage directory
if ( $?clear ) then    
    echo "Deleting files in garbage directory"
    $RM $gardir/*
    echo " "
endif


# a regular untrash function that just move files out if garbage
if ( $?untrash ) then
    while (${#argv} > 0)
        #will check if the file exits in garbage
        if (! -e ${gardir}/${argv[1]:t}) then
            echo "$argv[1] does not exist in garbage or is a regular file"

        else    
            echo "File $argv[1] exists in garbage"
            set pathgetter = `$GREP $gardir/index.txt -e "$argv[1]"`
            echo "${pathgetter[1]}"
            echo "Copying $gardir/$argv[1] ${pathgetter[2]}"
            echo "Removing $argv[1] from $gardir"
            $CP $gardir/$argv[1] ${pathgetter[2]} #copying file from trash to original
            $RM $gardir/$argv[1] #removing file from trash

        endif
    shift
    end
endif

exit

