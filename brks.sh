

prog="brks"

options="/home/mt/thrift/brks/conf/log.conf"

bin="/home/mt/thrift/brks/src/$prog"

pidfile="/home/mt/thrift/brks/run/${prog}.pid"

RETVAL=0



#check if requirments are met
if [ ! -x "${options}" ]; then
    echo "no such file ${options}"
    exit 1
fi

if [ ! -x "${bin}" ]; then
    echo "no such file ${bin}" 
    exit 1
fi


start()
{
    echo "Starting brks ... "
    [ -x $bin ] || exit 5
    if [ -f $pidfile ]; then 
        echo -ne "$prog is running...\n"
        exit 6
    fi
    #Start daemons.
    ${bin} ${options} &
    RETVAL=$?

    if [ $RETVAL -eq 0 ] ; then
        echo "started success "
    else
	echo "started failed"
	exit 7
    fi
    PID=$!
    [ ! -z "${PID}" ] && echo ${PID} > ${pidfile} 

    return $RETVAL
}

stop(){
    echo "Shutting down $prog ... "
    if [ ! -f "$pidfile" ];
    then 
	echo "no $prog to stop (cound not find file $pidfile)"
    else
	kill -9 $(cat "$pidfile")
	rm -rf "$pidfile"
	echo -n " stopped "
    fi
    RETVAL=$?
    return $RETVAL
}

restart()
{
    stop
#    sleep 3
    start 
}

case "$1" in 
    start)
        start 
        ;;

    stop)
        stop
        ;;

    restart)
        restart
        ;;

    status)
        status $exec
        RETVAL=$?
        ;;
    *)

        echo $"Usage: $0 {start|stop|restart|status}"
        RETVAL=1
esac

exit $RETVAL
