#!/bin/csh

#Phase 2: Create a compiling script
#Command: build_project <project_name>

#set globals

set GCC=/usr/bin/gcc
set GCCOPTS="-c"
set LD=/usr/bin/gcc
set LDOPTS="-o"
set PROJNAME=$argv[1]
set PROJPATH=/Users/jingu/Documents/University_of_Miami/Fall_2021/ECE322/ECE322Exam1
set MV=/bin/mv
set STAT=/usr/bin/stat
set CP=/bin/cp
set SRCPATH=/Users/jingu/Documents/University_of_Miami/Fall_2021/ECE322/ECE322Exam1/ex1_8004/src
set BINPATH=/Users/jingu/Documents/University_of_Miami/Fall_2021/ECE322/ECE322Exam1/ex1_8004/bin

if(${#argv}<1) then
    echo "usage $0 <project_name>"
    exit
endif

cd $SRCPATH

foreach file(*.c)
    if(! -e ${file:r}.o) then
        $GCC $GCCOPTS $file
        if(! -e ${file:r}.o) then
            echo "$file did not compile"
            exit
        else
            echo "$file compiled"
            $LD ${file:r}.o $LDOPTS $PROJNAME
            $MV $PROJNAME $BINPATH
        endif
    else
        if(`$STAT -f %m $file` >`$STAT -f %m ${file:r}.o` ) then
            $GCC $GCCOPTS $file
            if(! -e ${file:r}.o) then
                echo "$file did not compile"
                exit
            else
                echo "$file compiled"
                $LD ${file:r}.o $LDOPTS $PROJNAME
                $MV $PROJNAME $BINPATH
            endif
        else
            echo "$file cannot compile"
        endif
    endif
end



