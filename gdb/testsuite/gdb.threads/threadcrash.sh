#! /bin/bash

# NOTE: threadcrash.c *must* be built with debugging symbols
#
# The point of this shell script is to crash treadcrash.c, load the
# resulting core file into gdb and verify that gdb can extract enough
# information from the core file.
#
# The return code from this script is the number of failed tests.

LOG=gdbresult.log

if [ $# = 0 ] ; then
    echo >&2 Syntax: $0 \<name of threadcrash binary\> [\<gdb binary\> \<args...\>]
    exit 1
fi
RUNME="$1"
shift
GDB="${*:-gdb}"


pf_prefix=""
function pf_prefix() {
	pf_prefix="$*"
}

set_test=""
function set_test() {
	if [ -n "$set_test" ] ; then
		echo >&2 "DEJAGNU-BASH ERROR: set_test already set"
		exit 1
	fi
	set_test="$*"
	if [ -n "$pf_prefix" ] ; then
		set_test="$pf_prefix: $set_test"
	fi
}

# INTERNAL
function record_test {
	if [ -z "$set_test" ] ; then
		echo >&2 "DEJAGNU-BASH ERROR: set_test not set"
		exit 1
	fi
	# Provide the leading whitespace delimiter:
	echo " $1: $set_test"
	set_test=""
}

function pass() {
	record_test PASS
}
function fail() {
	record_test FAIL
}


# Verify that the gdb output doesn't contain $1.
function mustNotHave() {
    local BADWORD=$1
    set_test gdb output contains "$BADWORD"
    if grep -q "$BADWORD" $LOG ; then
        fail
        return 1
    fi
    pass
    return 0
}

# Verify that the gdb output contains exactly $1 $2s.
function mustHaveCorrectAmount() {
    local WANTEDNUMBER=$1
    local GOODWORD=$2
    local ACTUALNUMBER=$(grep "$GOODWORD" $LOG | wc -l)
    set_test gdb output contained $ACTUALNUMBER \""$GOODWORD"\", not $WANTEDNUMBER as expected
    if [ $ACTUALNUMBER != $WANTEDNUMBER ] ; then
        fail
        return 1
    fi
    pass
    return 0
}

# Verify that the gdb output contains seven threads
function mustHaveSevenThreads() {
    NTHREADS=$(egrep "^Thread [1-7] \(" $LOG | wc -l)
    set_test gdb output contains $NTHREADS threads, not 7 as expected
    if [ $NTHREADS != 7 ] ; then
        fail
        return 1
    fi
    pass
    return 0
}

# Verify that the gdb output has all parameters on consecutive lines
function mustHaveSequence() {
    SEQUENCE="$*"
    NPARTS=$#
    grep "$1" -A$((NPARTS - 1)) $LOG > matches.log

    while [ $# -gt 1 ] ; do
        shift
        ((NPARTS--))
        grep "$1" -A$((NPARTS - 1)) matches.log > temp.log
        mv temp.log matches.log
    done
    LASTPART=$1

    set_test gdb output does not contain the sequence: $SEQUENCE
    if ! grep -q "$LASTPART" matches.log ; then
        fail
        return 1
    fi
    pass
    return 0
}

# Verify that $LOG contains all information we want
function verifyLog() {
    local FAILURES=0
    
    mustNotHave '??' || ((FAILURES++))
    mustHaveCorrectAmount 11 threadcrash.c: || ((FAILURES++))
    
    mustHaveSevenThreads || ((FAILURES++))
    mustHaveSequence sleep "makeSyscall (ignored=" || ((FAILURES++))
    
    mustHaveSequence sleep "syscallingSighandler (signo=" "signal handler called" 0x || ((FAILURES++))
    mustHaveSequence pthread_kill "makeSyscallFromSighandler (ignored=" || ((FAILURES++))
    
    mustHaveSequence sleep "syscallingAltSighandler (signo=" "signal handler called" 0x || ((FAILURES++))
    mustHaveSequence pthread_kill "makeSyscallFromAltSighandler (ignored=" || ((FAILURES++))
    
    mustHaveSequence Thread "spin (ignored=" || ((FAILURES++))
    
    mustHaveSequence "spinningSighandler (signo=" "signal handler called" 0x || ((FAILURES++))
    mustHaveSequence pthread_kill "spinFromSighandler (ignored=" || ((FAILURES++))
    
    mustHaveSequence "spinningAltSighandler (signo=" "signal handler called" 0x || ((FAILURES++))
    mustHaveSequence pthread_kill "spinFromAltSighandler (ignored=" || ((FAILURES++))
    
    mustHaveSequence Thread "main (argc=1, argv=" || ((FAILURES++))

    return $FAILURES
}

# Put result of debugging a core file in $LOG
function getLogFromCore() {
    # Make sure we get a core file
    set_test Make sure we get a core file
    if ! ulimit -c unlimited ; then
        fail
        exit 1
    fi
    pass

    # Run the crasher
    ./$(basename "$RUNME")
    EXITCODE=$?

    # Verify that we actually crashed
    set_test $RUNME should have been killed by a signal, got non-signal exit code $EXITCODE
    if [ $EXITCODE -lt 128 ] ; then
        fail
        exit 1
    fi
    pass

    # Verify that we got a core file
    set_test $RUNME did not create a core file
    if [ ! -r core* ] ; then
        fail
        exit 1
    fi
    pass

    # Run gdb
    cat > gdbscript.gdb <<EOF
set width 0
t a a bt 100
quit
EOF
    cat gdbscript.gdb /dev/zero | $GDB -nx "./$(basename "$RUNME")" core* > $LOG
    EXITCODE=$?

    set_test gdb exited with error code
    if [ $EXITCODE != 0 ] ; then
        ((FAILURES++))
        echo >&2 gdb exited with error code $EXITCODE
	fail
    fi
    pass
}

# Put result of debugging a gcore file in $LOG
function getLogFromGcore() {
    # Create the core file
    rm -f core*
    cat > gdbscript.gdb <<EOF
handle SIGQUIT pass noprint nostop
handle SIGUSR1 pass noprint nostop
handle SIGUSR2 pass noprint nostop
handle SIGALRM pass noprint nostop
run
gcore
quit
EOF
    cat gdbscript.gdb /dev/zero | $GDB -nx "./$(basename "$RUNME")" > /dev/null
    EXITCODE=$?

    set_test gdb exited with error code when creating gcore file
    if [ $EXITCODE != 0 ] ; then
        ((FAILURES++))
        echo >&2 gdb exited with error code $EXITCODE when creating gcore file
	fail
    fi
    pass
    
    # Verify that we got a core file from gcore
    set_test gdb gcore did not create a core file
    if [ ! -r core* ] ; then
        fail
        exit 1
    fi
    pass

    # Run gdb on the gcore file
    cat > gdbscript.gdb <<EOF
set width 0
t a a bt 100
quit
EOF
    cat gdbscript.gdb /dev/zero | $GDB -nx "./$(basename "$RUNME")" core* > $LOG
    EXITCODE=$?

    set_test gdb exited with error code when examining gcore file
    if [ $EXITCODE != 0 ] ; then
        ((FAILURES++))
        echo >&2 gdb exited with error code $EXITCODE when examining gcore file
	fail
    fi
    pass
}

# Put result of debugging a core file in $LOG
function getLogFromLiveProcess() {
    # Run gdb
    cat > gdbscript.gdb <<EOF
handle SIGQUIT pass noprint nostop
handle SIGUSR1 pass noprint nostop
handle SIGUSR2 pass noprint nostop
handle SIGALRM pass noprint nostop
set width 0
run
t a a bt 100
quit
EOF
    cat gdbscript.gdb /dev/zero | $GDB -nx "./$(basename "$RUNME")" > $LOG
    EXITCODE=$?

    set_test gdb exited with error code
    if [ $EXITCODE != 0 ] ; then
        ((FAILURES++))
        echo >&2 gdb exited with error code $EXITCODE
	fail
    fi
    pass
}

####### Main program follows #####################

# Make sure we don't clobber anybody else's (core) file(s)
WORKDIR=/tmp/$PPID
mkdir -p $WORKDIR
cp "$RUNME" $WORKDIR
cd $WORKDIR

# Count problems
FAILURES=0

echo === Testing gdb vs core file...
pf_prefix core file
getLogFromCore
verifyLog
((FAILURES+=$?))
pf_prefix
echo === Core file tests done.

echo

echo === Testing gdb vs gcore file...
pf_prefix gcore file
getLogFromGcore
verifyLog
((FAILURES+=$?))
pf_prefix
echo === Gcore file tests done.

echo

echo === Testing gdb vs live process...
pf_prefix live process
getLogFromLiveProcess
verifyLog
((FAILURES+=$?))
pf_prefix
echo === Live process tests done.

# Executive summary
echo
if [ $FAILURES == 0 ] ; then
    echo All tests passed!
else
    echo $FAILURES tests failed!
    echo
    echo Make sure the threadcrash binary contains debugging information \(build with \"gcc -g\"\).
fi

# Clean up
cd /
rm -rf $WORKDIR

exit $FAILURES
