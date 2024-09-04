/* Auto generated file */
#pragma once
#include <errno.h>

/* PANDIO custom errors: */
#define PD_OK (0)
#define PD_UNKNOWN (-7000)
#define PD_EOF (-7002)

/* Unix system error codes: */
#ifdef EPERM
#define PD_EPERM (-EPERM)
#else
#define PD_EPERM (-1)
#endif

#ifdef ENOENT
#define PD_ENOENT (-ENOENT)
#else
#define PD_ENOENT (-2)
#endif

#ifdef ESRCH
#define PD_ESRCH (-ESRCH)
#else
#define PD_ESRCH (-3)
#endif

#ifdef EINTR
#define PD_EINTR (-EINTR)
#else
#define PD_EINTR (-4)
#endif

#ifdef EIO
#define PD_EIO (-EIO)
#else
#define PD_EIO (-5)
#endif

#ifdef ENXIO
#define PD_ENXIO (-ENXIO)
#else
#define PD_ENXIO (-6)
#endif

#ifdef E2BIG
#define PD_E2BIG (-E2BIG)
#else
#define PD_E2BIG (-7)
#endif

#ifdef ENOEXEC
#define PD_ENOEXEC (-ENOEXEC)
#else
#define PD_ENOEXEC (-8)
#endif

#ifdef EBADF
#define PD_EBADF (-EBADF)
#else
#define PD_EBADF (-9)
#endif

#ifdef ECHILD
#define PD_ECHILD (-ECHILD)
#else
#define PD_ECHILD (-10)
#endif

#ifdef EAGAIN
#define PD_EAGAIN (-EAGAIN)
#else
#define PD_EAGAIN (-11)
#endif

#ifdef ENOMEM
#define PD_ENOMEM (-ENOMEM)
#else
#define PD_ENOMEM (-12)
#endif

#ifdef EACCES
#define PD_EACCES (-EACCES)
#else
#define PD_EACCES (-13)
#endif

#ifdef EFAULT
#define PD_EFAULT (-EFAULT)
#else
#define PD_EFAULT (-14)
#endif

#ifdef ENOTBLK
#define PD_ENOTBLK (-ENOTBLK)
#else
#define PD_ENOTBLK (-15)
#endif

#ifdef EBUSY
#define PD_EBUSY (-EBUSY)
#else
#define PD_EBUSY (-16)
#endif

#ifdef EEXIST
#define PD_EEXIST (-EEXIST)
#else
#define PD_EEXIST (-17)
#endif

#ifdef EXDEV
#define PD_EXDEV (-EXDEV)
#else
#define PD_EXDEV (-18)
#endif

#ifdef ENODEV
#define PD_ENODEV (-ENODEV)
#else
#define PD_ENODEV (-19)
#endif

#ifdef ENOTDIR
#define PD_ENOTDIR (-ENOTDIR)
#else
#define PD_ENOTDIR (-20)
#endif

#ifdef EISDIR
#define PD_EISDIR (-EISDIR)
#else
#define PD_EISDIR (-21)
#endif

#ifdef EINVAL
#define PD_EINVAL (-EINVAL)
#else
#define PD_EINVAL (-22)
#endif

#ifdef ENFILE
#define PD_ENFILE (-ENFILE)
#else
#define PD_ENFILE (-23)
#endif

#ifdef EMFILE
#define PD_EMFILE (-EMFILE)
#else
#define PD_EMFILE (-24)
#endif

#ifdef ENOTTY
#define PD_ENOTTY (-ENOTTY)
#else
#define PD_ENOTTY (-25)
#endif

#ifdef ETXTBSY
#define PD_ETXTBSY (-ETXTBSY)
#else
#define PD_ETXTBSY (-26)
#endif

#ifdef EFBIG
#define PD_EFBIG (-EFBIG)
#else
#define PD_EFBIG (-27)
#endif

#ifdef ENOSPC
#define PD_ENOSPC (-ENOSPC)
#else
#define PD_ENOSPC (-28)
#endif

#ifdef ESPIPE
#define PD_ESPIPE (-ESPIPE)
#else
#define PD_ESPIPE (-29)
#endif

#ifdef EROFS
#define PD_EROFS (-EROFS)
#else
#define PD_EROFS (-30)
#endif

#ifdef EMLINK
#define PD_EMLINK (-EMLINK)
#else
#define PD_EMLINK (-31)
#endif

#ifdef EPIPE
#define PD_EPIPE (-EPIPE)
#else
#define PD_EPIPE (-32)
#endif

#ifdef EDOM
#define PD_EDOM (-EDOM)
#else
#define PD_EDOM (-33)
#endif

#ifdef ERANGE
#define PD_ERANGE (-ERANGE)
#else
#define PD_ERANGE (-34)
#endif

#ifdef EDEADLK
#define PD_EDEADLK (-EDEADLK)
#else
#define PD_EDEADLK (-35)
#endif

#ifdef ENAMETOOLONG
#define PD_ENAMETOOLONG (-ENAMETOOLONG)
#else
#define PD_ENAMETOOLONG (-36)
#endif

#ifdef ENOLCK
#define PD_ENOLCK (-ENOLCK)
#else
#define PD_ENOLCK (-37)
#endif

#ifdef ENOSYS
#define PD_ENOSYS (-ENOSYS)
#else
#define PD_ENOSYS (-38)
#endif

#ifdef ENOTEMPTY
#define PD_ENOTEMPTY (-ENOTEMPTY)
#else
#define PD_ENOTEMPTY (-39)
#endif

#ifdef ELOOP
#define PD_ELOOP (-ELOOP)
#else
#define PD_ELOOP (-40)
#endif

#ifdef EWOULDBLOCK
#define PD_EWOULDBLOCK (-EWOULDBLOCK)
#else
#define PD_EWOULDBLOCK (-11)
#endif

#ifdef ENOMSG
#define PD_ENOMSG (-ENOMSG)
#else
#define PD_ENOMSG (-42)
#endif

#ifdef EIDRM
#define PD_EIDRM (-EIDRM)
#else
#define PD_EIDRM (-43)
#endif

#ifdef ECHRNG
#define PD_ECHRNG (-ECHRNG)
#else
#define PD_ECHRNG (-44)
#endif

#ifdef EL2NSYNC
#define PD_EL2NSYNC (-EL2NSYNC)
#else
#define PD_EL2NSYNC (-45)
#endif

#ifdef EL3HLT
#define PD_EL3HLT (-EL3HLT)
#else
#define PD_EL3HLT (-46)
#endif

#ifdef EL3RST
#define PD_EL3RST (-EL3RST)
#else
#define PD_EL3RST (-47)
#endif

#ifdef ELNRNG
#define PD_ELNRNG (-ELNRNG)
#else
#define PD_ELNRNG (-48)
#endif

#ifdef EUNATCH
#define PD_EUNATCH (-EUNATCH)
#else
#define PD_EUNATCH (-49)
#endif

#ifdef ENOCSI
#define PD_ENOCSI (-ENOCSI)
#else
#define PD_ENOCSI (-50)
#endif

#ifdef EL2HLT
#define PD_EL2HLT (-EL2HLT)
#else
#define PD_EL2HLT (-51)
#endif

#ifdef EBADE
#define PD_EBADE (-EBADE)
#else
#define PD_EBADE (-52)
#endif

#ifdef EBADR
#define PD_EBADR (-EBADR)
#else
#define PD_EBADR (-53)
#endif

#ifdef EXFULL
#define PD_EXFULL (-EXFULL)
#else
#define PD_EXFULL (-54)
#endif

#ifdef ENOANO
#define PD_ENOANO (-ENOANO)
#else
#define PD_ENOANO (-55)
#endif

#ifdef EBADRQC
#define PD_EBADRQC (-EBADRQC)
#else
#define PD_EBADRQC (-56)
#endif

#ifdef EBADSLT
#define PD_EBADSLT (-EBADSLT)
#else
#define PD_EBADSLT (-57)
#endif

#ifdef EDEADLOCK
#define PD_EDEADLOCK (-EDEADLOCK)
#else
#define PD_EDEADLOCK (-35)
#endif

#ifdef EBFONT
#define PD_EBFONT (-EBFONT)
#else
#define PD_EBFONT (-59)
#endif

#ifdef ENOSTR
#define PD_ENOSTR (-ENOSTR)
#else
#define PD_ENOSTR (-60)
#endif

#ifdef ENODATA
#define PD_ENODATA (-ENODATA)
#else
#define PD_ENODATA (-61)
#endif

#ifdef ETIME
#define PD_ETIME (-ETIME)
#else
#define PD_ETIME (-62)
#endif

#ifdef ENOSR
#define PD_ENOSR (-ENOSR)
#else
#define PD_ENOSR (-63)
#endif

#ifdef ENONET
#define PD_ENONET (-ENONET)
#else
#define PD_ENONET (-64)
#endif

#ifdef ENOPKG
#define PD_ENOPKG (-ENOPKG)
#else
#define PD_ENOPKG (-65)
#endif

#ifdef EREMOTE
#define PD_EREMOTE (-EREMOTE)
#else
#define PD_EREMOTE (-66)
#endif

#ifdef ENOLINK
#define PD_ENOLINK (-ENOLINK)
#else
#define PD_ENOLINK (-67)
#endif

#ifdef EADV
#define PD_EADV (-EADV)
#else
#define PD_EADV (-68)
#endif

#ifdef ESRMNT
#define PD_ESRMNT (-ESRMNT)
#else
#define PD_ESRMNT (-69)
#endif

#ifdef ECOMM
#define PD_ECOMM (-ECOMM)
#else
#define PD_ECOMM (-70)
#endif

#ifdef EPROTO
#define PD_EPROTO (-EPROTO)
#else
#define PD_EPROTO (-71)
#endif

#ifdef EMULTIHOP
#define PD_EMULTIHOP (-EMULTIHOP)
#else
#define PD_EMULTIHOP (-72)
#endif

#ifdef EDOTDOT
#define PD_EDOTDOT (-EDOTDOT)
#else
#define PD_EDOTDOT (-73)
#endif

#ifdef EBADMSG
#define PD_EBADMSG (-EBADMSG)
#else
#define PD_EBADMSG (-74)
#endif

#ifdef EOVERFLOW
#define PD_EOVERFLOW (-EOVERFLOW)
#else
#define PD_EOVERFLOW (-75)
#endif

#ifdef ENOTUNIQ
#define PD_ENOTUNIQ (-ENOTUNIQ)
#else
#define PD_ENOTUNIQ (-76)
#endif

#ifdef EBADFD
#define PD_EBADFD (-EBADFD)
#else
#define PD_EBADFD (-77)
#endif

#ifdef EREMCHG
#define PD_EREMCHG (-EREMCHG)
#else
#define PD_EREMCHG (-78)
#endif

#ifdef ELIBACC
#define PD_ELIBACC (-ELIBACC)
#else
#define PD_ELIBACC (-79)
#endif

#ifdef ELIBBAD
#define PD_ELIBBAD (-ELIBBAD)
#else
#define PD_ELIBBAD (-80)
#endif

#ifdef ELIBSCN
#define PD_ELIBSCN (-ELIBSCN)
#else
#define PD_ELIBSCN (-81)
#endif

#ifdef ELIBMAX
#define PD_ELIBMAX (-ELIBMAX)
#else
#define PD_ELIBMAX (-82)
#endif

#ifdef ELIBEXEC
#define PD_ELIBEXEC (-ELIBEXEC)
#else
#define PD_ELIBEXEC (-83)
#endif

#ifdef EILSEQ
#define PD_EILSEQ (-EILSEQ)
#else
#define PD_EILSEQ (-84)
#endif

#ifdef ERESTART
#define PD_ERESTART (-ERESTART)
#else
#define PD_ERESTART (-85)
#endif

#ifdef ESTRPIPE
#define PD_ESTRPIPE (-ESTRPIPE)
#else
#define PD_ESTRPIPE (-86)
#endif

#ifdef EUSERS
#define PD_EUSERS (-EUSERS)
#else
#define PD_EUSERS (-87)
#endif

#ifdef ENOTSOCK
#define PD_ENOTSOCK (-ENOTSOCK)
#else
#define PD_ENOTSOCK (-88)
#endif

#ifdef EDESTADDRREQ
#define PD_EDESTADDRREQ (-EDESTADDRREQ)
#else
#define PD_EDESTADDRREQ (-89)
#endif

#ifdef EMSGSIZE
#define PD_EMSGSIZE (-EMSGSIZE)
#else
#define PD_EMSGSIZE (-90)
#endif

#ifdef EPROTOTYPE
#define PD_EPROTOTYPE (-EPROTOTYPE)
#else
#define PD_EPROTOTYPE (-91)
#endif

#ifdef ENOPROTOOPT
#define PD_ENOPROTOOPT (-ENOPROTOOPT)
#else
#define PD_ENOPROTOOPT (-92)
#endif

#ifdef EPROTONOSUPPORT
#define PD_EPROTONOSUPPORT (-EPROTONOSUPPORT)
#else
#define PD_EPROTONOSUPPORT (-93)
#endif

#ifdef ESOCKTNOSUPPORT
#define PD_ESOCKTNOSUPPORT (-ESOCKTNOSUPPORT)
#else
#define PD_ESOCKTNOSUPPORT (-94)
#endif

#ifdef EOPNOTSUPP
#define PD_EOPNOTSUPP (-EOPNOTSUPP)
#else
#define PD_EOPNOTSUPP (-95)
#endif

#ifdef ENOTSUP
#define PD_ENOTSUP (-ENOTSUP)
#else
#define PD_ENOTSUP (-95)
#endif

#ifdef EPFNOSUPPORT
#define PD_EPFNOSUPPORT (-EPFNOSUPPORT)
#else
#define PD_EPFNOSUPPORT (-96)
#endif

#ifdef EAFNOSUPPORT
#define PD_EAFNOSUPPORT (-EAFNOSUPPORT)
#else
#define PD_EAFNOSUPPORT (-97)
#endif

#ifdef EADDRINUSE
#define PD_EADDRINUSE (-EADDRINUSE)
#else
#define PD_EADDRINUSE (-98)
#endif

#ifdef EADDRNOTAVAIL
#define PD_EADDRNOTAVAIL (-EADDRNOTAVAIL)
#else
#define PD_EADDRNOTAVAIL (-99)
#endif

#ifdef ENETDOWN
#define PD_ENETDOWN (-ENETDOWN)
#else
#define PD_ENETDOWN (-100)
#endif

#ifdef ENETUNREACH
#define PD_ENETUNREACH (-ENETUNREACH)
#else
#define PD_ENETUNREACH (-101)
#endif

#ifdef ENETRESET
#define PD_ENETRESET (-ENETRESET)
#else
#define PD_ENETRESET (-102)
#endif

#ifdef ECONNABORTED
#define PD_ECONNABORTED (-ECONNABORTED)
#else
#define PD_ECONNABORTED (-103)
#endif

#ifdef ECONNRESET
#define PD_ECONNRESET (-ECONNRESET)
#else
#define PD_ECONNRESET (-104)
#endif

#ifdef ENOBUFS
#define PD_ENOBUFS (-ENOBUFS)
#else
#define PD_ENOBUFS (-105)
#endif

#ifdef EISCONN
#define PD_EISCONN (-EISCONN)
#else
#define PD_EISCONN (-106)
#endif

#ifdef ENOTCONN
#define PD_ENOTCONN (-ENOTCONN)
#else
#define PD_ENOTCONN (-107)
#endif

#ifdef ESHUTDOWN
#define PD_ESHUTDOWN (-ESHUTDOWN)
#else
#define PD_ESHUTDOWN (-108)
#endif

#ifdef ETOOMANYREFS
#define PD_ETOOMANYREFS (-ETOOMANYREFS)
#else
#define PD_ETOOMANYREFS (-109)
#endif

#ifdef ETIMEDOUT
#define PD_ETIMEDOUT (-ETIMEDOUT)
#else
#define PD_ETIMEDOUT (-110)
#endif

#ifdef ECONNREFUSED
#define PD_ECONNREFUSED (-ECONNREFUSED)
#else
#define PD_ECONNREFUSED (-111)
#endif

#ifdef EHOSTDOWN
#define PD_EHOSTDOWN (-EHOSTDOWN)
#else
#define PD_EHOSTDOWN (-112)
#endif

#ifdef EHOSTUNREACH
#define PD_EHOSTUNREACH (-EHOSTUNREACH)
#else
#define PD_EHOSTUNREACH (-113)
#endif

#ifdef EALREADY
#define PD_EALREADY (-EALREADY)
#else
#define PD_EALREADY (-114)
#endif

#ifdef EINPROGRESS
#define PD_EINPROGRESS (-EINPROGRESS)
#else
#define PD_EINPROGRESS (-115)
#endif

#ifdef ESTALE
#define PD_ESTALE (-ESTALE)
#else
#define PD_ESTALE (-116)
#endif

#ifdef EUCLEAN
#define PD_EUCLEAN (-EUCLEAN)
#else
#define PD_EUCLEAN (-117)
#endif

#ifdef ENOTNAM
#define PD_ENOTNAM (-ENOTNAM)
#else
#define PD_ENOTNAM (-118)
#endif

#ifdef ENAVAIL
#define PD_ENAVAIL (-ENAVAIL)
#else
#define PD_ENAVAIL (-119)
#endif

#ifdef EISNAM
#define PD_EISNAM (-EISNAM)
#else
#define PD_EISNAM (-120)
#endif

#ifdef EREMOTEIO
#define PD_EREMOTEIO (-EREMOTEIO)
#else
#define PD_EREMOTEIO (-121)
#endif

#ifdef EDQUOT
#define PD_EDQUOT (-EDQUOT)
#else
#define PD_EDQUOT (-122)
#endif

#ifdef ENOMEDIUM
#define PD_ENOMEDIUM (-ENOMEDIUM)
#else
#define PD_ENOMEDIUM (-123)
#endif

#ifdef EMEDIUMTYPE
#define PD_EMEDIUMTYPE (-EMEDIUMTYPE)
#else
#define PD_EMEDIUMTYPE (-124)
#endif

#ifdef ECANCELED
#define PD_ECANCELED (-ECANCELED)
#else
#define PD_ECANCELED (-125)
#endif

#ifdef ENOKEY
#define PD_ENOKEY (-ENOKEY)
#else
#define PD_ENOKEY (-126)
#endif

#ifdef EKEYEXPIRED
#define PD_EKEYEXPIRED (-EKEYEXPIRED)
#else
#define PD_EKEYEXPIRED (-127)
#endif

#ifdef EKEYREVOKED
#define PD_EKEYREVOKED (-EKEYREVOKED)
#else
#define PD_EKEYREVOKED (-128)
#endif

#ifdef EKEYREJECTED
#define PD_EKEYREJECTED (-EKEYREJECTED)
#else
#define PD_EKEYREJECTED (-129)
#endif

#ifdef EOWNERDEAD
#define PD_EOWNERDEAD (-EOWNERDEAD)
#else
#define PD_EOWNERDEAD (-130)
#endif

#ifdef ENOTRECOVERABLE
#define PD_ENOTRECOVERABLE (-ENOTRECOVERABLE)
#else
#define PD_ENOTRECOVERABLE (-131)
#endif

#ifdef ERFKILL
#define PD_ERFKILL (-ERFKILL)
#else
#define PD_ERFKILL (-132)
#endif

#ifdef EHWPOISON
#define PD_EHWPOISON (-EHWPOISON)
#else
#define PD_EHWPOISON (-133)
#endif


#define PD_ERR_STR_MAPPING (X)       \
   X(PD_EPERM, "Operation not permitted")       \
   X(PD_ENOENT, "No such file or directory")       \
   X(PD_ESRCH, "No such process")       \
   X(PD_EINTR, "Interrupted system call")       \
   X(PD_EIO, "I/O error")       \
   X(PD_ENXIO, "No such device or address")       \
   X(PD_E2BIG, "Argument list too long")       \
   X(PD_ENOEXEC, "Exec format error")       \
   X(PD_EBADF, "Bad file number")       \
   X(PD_ECHILD, "No child processes")       \
   X(PD_EAGAIN, "Try again")       \
   X(PD_ENOMEM, "Out of memory")       \
   X(PD_EACCES, "Permission denied")       \
   X(PD_EFAULT, "Bad address")       \
   X(PD_ENOTBLK, "Block device required")       \
   X(PD_EBUSY, "Device or resource busy")       \
   X(PD_EEXIST, "File exists")       \
   X(PD_EXDEV, "Cross-device link")       \
   X(PD_ENODEV, "No such device")       \
   X(PD_ENOTDIR, "Not a directory")       \
   X(PD_EISDIR, "Is a directory")       \
   X(PD_EINVAL, "Invalid argument")       \
   X(PD_ENFILE, "File table overflow")       \
   X(PD_EMFILE, "Too many open files")       \
   X(PD_ENOTTY, "Not a typewriter")       \
   X(PD_ETXTBSY, "Text file busy")       \
   X(PD_EFBIG, "File too large")       \
   X(PD_ENOSPC, "No space left on device")       \
   X(PD_ESPIPE, "Illegal seek")       \
   X(PD_EROFS, "Read-only file system")       \
   X(PD_EMLINK, "Too many links")       \
   X(PD_EPIPE, "Broken pipe")       \
   X(PD_EDOM, "Math argument out of domain of func")       \
   X(PD_ERANGE, "Math result not representable")       \
   X(PD_EDEADLK, "Resource deadlock would occur")       \
   X(PD_ENAMETOOLONG, "File name too long")       \
   X(PD_ENOLCK, "No record locks available")       \
   X(PD_ENOSYS, "Function not implemented")       \
   X(PD_ENOTEMPTY, "Directory not empty")       \
   X(PD_ELOOP, "Too many symbolic links encountered")       \
   X(PD_EWOULDBLOCK, "Operation would block")       \
   X(PD_ENOMSG, "No message of desired type")       \
   X(PD_EIDRM, "Identifier removed")       \
   X(PD_ECHRNG, "Channel number out of range")       \
   X(PD_EL2NSYNC, "Level 2 not synchronized")       \
   X(PD_EL3HLT, "Level 3 halted")       \
   X(PD_EL3RST, "Level 3 reset")       \
   X(PD_ELNRNG, "Link number out of range")       \
   X(PD_EUNATCH, "Protocol driver not attached")       \
   X(PD_ENOCSI, "No CSI structure available")       \
   X(PD_EL2HLT, "Level 2 halted")       \
   X(PD_EBADE, "Invalid exchange")       \
   X(PD_EBADR, "Invalid request descriptor")       \
   X(PD_EXFULL, "Exchange full")       \
   X(PD_ENOANO, "No anode")       \
   X(PD_EBADRQC, "Invalid request code")       \
   X(PD_EBADSLT, "Invalid slot")       \
   X(PD_EDEADLOCK, "Resource deadlock would occur")       \
   X(PD_EBFONT, "Bad font file format")       \
   X(PD_ENOSTR, "Device not a stream")       \
   X(PD_ENODATA, "No data available")       \
   X(PD_ETIME, "Timer expired")       \
   X(PD_ENOSR, "Out of streams resources")       \
   X(PD_ENONET, "Machine is not on the network")       \
   X(PD_ENOPKG, "Package not installed")       \
   X(PD_EREMOTE, "Object is remote")       \
   X(PD_ENOLINK, "Link has been severed")       \
   X(PD_EADV, "Advertise error")       \
   X(PD_ESRMNT, "Srmount error")       \
   X(PD_ECOMM, "Communication error on send")       \
   X(PD_EPROTO, "Protocol error")       \
   X(PD_EMULTIHOP, "Multihop attempted")       \
   X(PD_EDOTDOT, "RFS specific error")       \
   X(PD_EBADMSG, "Not a data message")       \
   X(PD_EOVERFLOW, "Value too large for defined data type")       \
   X(PD_ENOTUNIQ, "Name not unique on network")       \
   X(PD_EBADFD, "File descriptor in bad state")       \
   X(PD_EREMCHG, "Remote address changed")       \
   X(PD_ELIBACC, "Can not access a needed shared library")       \
   X(PD_ELIBBAD, "Accessing a corrupted shared library")       \
   X(PD_ELIBSCN, ".lib section in a.out corrupted")       \
   X(PD_ELIBMAX, "Attempting to link in too many shared libraries")       \
   X(PD_ELIBEXEC, "Cannot exec a shared library directly")       \
   X(PD_EILSEQ, "Illegal byte sequence")       \
   X(PD_ERESTART, "Interrupted system call should be restarted")       \
   X(PD_ESTRPIPE, "Streams pipe error")       \
   X(PD_EUSERS, "Too many users")       \
   X(PD_ENOTSOCK, "Socket operation on non-socket")       \
   X(PD_EDESTADDRREQ, "Destination address required")       \
   X(PD_EMSGSIZE, "Message too long")       \
   X(PD_EPROTOTYPE, "Protocol wrong type for socket")       \
   X(PD_ENOPROTOOPT, "Protocol not available")       \
   X(PD_EPROTONOSUPPORT, "Protocol not supported")       \
   X(PD_ESOCKTNOSUPPORT, "Socket type not supported")       \
   X(PD_EOPNOTSUPP, "Operation not supported on transport endpoint")       \
   X(PD_ENOTSUP, "Operation not supported on transport endpoint")       \
   X(PD_EPFNOSUPPORT, "Protocol family not supported")       \
   X(PD_EAFNOSUPPORT, "Address family not supported by protocol")       \
   X(PD_EADDRINUSE, "Address already in use")       \
   X(PD_EADDRNOTAVAIL, "Cannot assign requested address")       \
   X(PD_ENETDOWN, "Network is down")       \
   X(PD_ENETUNREACH, "Network is unreachable")       \
   X(PD_ENETRESET, "Network dropped connection because of reset")       \
   X(PD_ECONNABORTED, "Software caused connection abort")       \
   X(PD_ECONNRESET, "Connection reset by peer")       \
   X(PD_ENOBUFS, "No buffer space available")       \
   X(PD_EISCONN, "Transport endpoint is already connected")       \
   X(PD_ENOTCONN, "Transport endpoint is not connected")       \
   X(PD_ESHUTDOWN, "Cannot send after transport endpoint shutdown")       \
   X(PD_ETOOMANYREFS, "Too many references: cannot splice")       \
   X(PD_ETIMEDOUT, "Connection timed out")       \
   X(PD_ECONNREFUSED, "Connection refused")       \
   X(PD_EHOSTDOWN, "Host is down")       \
   X(PD_EHOSTUNREACH, "No route to host")       \
   X(PD_EALREADY, "Operation already in progress")       \
   X(PD_EINPROGRESS, "Operation now in progress")       \
   X(PD_ESTALE, "Stale file handle")       \
   X(PD_EUCLEAN, "Structure needs cleaning")       \
   X(PD_ENOTNAM, "Not a XENIX named type file")       \
   X(PD_ENAVAIL, "No XENIX semaphores available")       \
   X(PD_EISNAM, "Is a named type file")       \
   X(PD_EREMOTEIO, "Remote I/O error")       \
   X(PD_EDQUOT, "Quota exceeded")       \
   X(PD_ENOMEDIUM, "No medium found")       \
   X(PD_EMEDIUMTYPE, "Wrong medium type")       \
   X(PD_ECANCELED, "Operation Canceled")       \
   X(PD_ENOKEY, "Required key not available")       \
   X(PD_EKEYEXPIRED, "Key has expired")       \
   X(PD_EKEYREVOKED, "Key has been revoked")       \
   X(PD_EKEYREJECTED, "Key was rejected by service")       \
   X(PD_EOWNERDEAD, "Owner died")       \
   X(PD_ENOTRECOVERABLE, "State not recoverable")       \
   X(PD_ERFKILL, "Operation not possible due to RF-kill")       \
   X(PD_EHWPOISON, "Memory page has hardware error")       \
   X(PD_PD_OK, "Success")       \
   X(PD_PD_UNKNOWN, "Unknown error")       \
   X(PD_PD_EOF, "End of the stream")
