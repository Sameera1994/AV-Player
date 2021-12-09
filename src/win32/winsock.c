/*****************************************************************************
 * winsock.c: POSIX replacements for Winsock
 *****************************************************************************/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <vlc_common.h>
#include <vlc_network.h>

#if 0
ssize_t vlc_sendmsg (int s, struct msghdr *hdr, int flags)
{
    if ((hdr->msg_iovlen > 100) || (hdr->msg_controllen > 0))
    {
        errno = EINVAL;
        return -1;
    }

    WSABUF buf[hdr->msg_iovlen];
    for (size_t i = 0; i < sizeof (buf) / sizeof (buf[0]); i++)
        buf[i].buf = hdr->msg_iov[i].iov_base,
        buf[i].len = hdr->msg_iov[i].iov_len;

    DWORD sent;
    if (WSASendTo (s, buf, sizeof (buf) / sizeof (buf[0]), &sent, flags,
                   hdr->msg_name, hdr->msg_namelen, NULL, NULL) == 0)
        return sent;
    return -1;
}

ssize_t vlc_recvmsg (int s, struct msghdr *hdr, int flags)
{
    /* WSARecvMsg would be more straightforward, and would support ancillary
     * data, but it's not yet in mingw32. */
    if (hdr->msg_iovlen > 100)
    {
        errno = EINVAL;
        return -1;
    }

    WSABUF buf[hdr->msg_iovlen];
    for (size_t i = 0; i < sizeof (buf) / sizeof (buf[0]); i++)
        buf[i].buf = hdr->msg_iov[i].iov_base,
        buf[i].len = hdr->msg_iov[i].iov_len;

    DWORD recvd, dwFlags = flags;
    INT fromlen = hdr->msg_namelen;
    hdr->msg_controllen = 0;
    hdr->msg_flags = 0;

    int ret = WSARecvFrom (s, buf, sizeof (buf) / sizeof (buf[0]), &recvd,
                           &dwFlags, hdr->msg_name, &fromlen, NULL, NULL);
    hdr->msg_namelen = fromlen;
    hdr->msg_flags = dwFlags;
    if (ret == 0)
        return recvd;

#ifdef MSG_TRUNC
    if (WSAGetLastError() == WSAEMSGSIZE)
    {
        hdr->msg_flags |= MSG_TRUNC;
        return recvd;
    }
#else
# warning Out-of-date Winsock header files!
#endif
    return -1;
}
#endif
