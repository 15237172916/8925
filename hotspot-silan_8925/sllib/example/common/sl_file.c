/*
 * This file includes functions used to read to mdev and write from mdev
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sl_types.h>
#include <sl_error.h>
#include <sl_debug.h>
#include <sl_mmutil.h>
#include <sl_param_sys.h>
#include <sl_api_filedev.h>
#include <sl_file.h>

/* Open a file */
static SL_S32 openFile(fileCtrl_s *fc, const char *name)
{
    SL_S32 ret;
    int fd;
    struct stat st;

    fd = open(name, O_CREAT | O_RDWR, 0755);
    if (fd < 0) {
        log_err("Failed to open file %s", name);
        return SL_ERROR_UNKNOWN_DEVICE;
    }

    ret = fstat(fd, &st);
    if (ret < 0) {
        log_err("Failed to 'fstat' file %d\n", fd);
        close(fd);
        return SL_ERROR_GENERIC_IO;
    }

    fc->fd = fd;
    fc->fsize = st.st_size;

    return SL_NO_ERROR;
}

/* Close a file */
SL_S32 closeFile(fileCtrl_s *fc)
{
    close(fc->fd);
    fc->fd = -1;

    return SL_NO_ERROR;
}

/* Init control block of the input file */
SL_S32 openInitInFileCtrl(chanVideoPara_s *cvp, fileCtrl_s *ifc, const char *fname)
{
    SL_S32 ret;
    videoPara_s vp;

    if (!ifc) {
        log_err("NULL pointer of ifc");
        return SL_ERROR_BAD_PARAMETER;
    }

    ret = openFile(ifc, fname);
    if (SL_NO_ERROR != ret)
        return ret;

    vp.format = cvp->format;
    vp.reso = cvp->reso;
    vp.reso_wi = cvp->reso_wi;
    vp.reso_hi = cvp->reso_hi;
    ret = MMUTIL_resoRatioId2Size(vp.reso, &vp.reso_wi, &vp.reso_hi);
    if (SL_NO_ERROR != ret)
        return ret;
    ret = MMUTIL_getFrameSize(&vp, &ifc->rlen);
    if (SL_NO_ERROR != ret)
        return ret;
    ifc->wlen = ifc->rlen;
    ifc->rptr = 0;
    ifc->wptr = 0;
    log_debug("Size of per IO of input file %d, wi %d, hi %d", ifc->rlen,
            vp.reso_wi, vp.reso_hi);

    return SL_NO_ERROR;
}

/* Init control block of the output file */
SL_S32 openInitOutFileCtrl(chanVideoPara_s *cvp, fileCtrl_s *ofc, const char *fname)
{
    SL_S32 ret;
    videoPara_s vp;

    if (!ofc) {
        log_err("NULL pointer of ofc");
        return SL_ERROR_BAD_PARAMETER;
    }

    ret = openFile(ofc, fname);
    if (SL_NO_ERROR != ret)
        return ret;

    vp.format = cvp->format;
    vp.reso = cvp->reso;
    ofc->rlen = 0;
    ofc->wlen = 0;
    ofc->rptr = 0;
    ofc->wptr = 0;
    log_debug("Size of per IO of output file=%d", ofc->wlen);

    return SL_NO_ERROR;
}

/* Read data from the file and write to a MDEV memory block */
SL_S32 readFile(fileCtrl_s *fc, void *buf, SL_U32 size, SL_U32 *dlen)
{
    SL_S32 ret;
    SL_S32 unfinished;

    if ((!buf) || (!dlen)) {
        log_err("NULL 'buf'");
        return SL_ERROR_BAD_PARAMETER;
    }
    if (fc->rptr >= fc->fsize) {
        log_note("End of file");
        return SL_ERROR_NO_MORE;
    }

    unfinished = fc->rlen;
    if (unfinished > size) {
        log_warn("Cut size of read from %d to buf size %d", unfinished, size);
        unfinished = size;
    }
    if ((fc->rptr + unfinished) > fc->fsize)
        unfinished = fc->fsize - fc->rptr;

    ret = lseek(fc->fd, fc->rptr, SEEK_SET);
    if (ret < 0) {
        log_err("'lseek' failed, request offset=%d", fc->rptr);
        return SL_ERROR_GENERIC_IO;
    }

    /* Copy data from file to the buffer */
    log_debug("Read file, buf %p, size %d", buf, unfinished);
    ret = read(fc->fd, buf, unfinished);
    if (ret < 0) {
        log_err("'read' failed, request %d bytes", unfinished);
        return SL_ERROR_GENERIC_IO;
    }
    else if (ret < unfinished) {
        log_warn("Request %d bytes, but only %d bytes finished",
                unfinished, ret);
    }
    fc->rptr += ret;
    *dlen = unfinished;

    return SL_NO_ERROR;
}

/* Read data from a memory block and write data to the file */
SL_S32 writeFile(fileCtrl_s *fc, void *buf, SL_U32 dlen)
{
    SL_S32 ret;

    ret = lseek(fc->fd, fc->wptr, SEEK_SET);
    if (ret < 0) {
        log_err("'lseek' failed, request offset %d", fc->wptr);
        return SL_ERROR_GENERIC_IO;
    }

    /* Read data from buffer and write it to the file */
    log_debug("write file, buf %p, dlen %d", buf, dlen);
    ret = write(fc->fd, buf, dlen);
    if (ret < 0) {
        log_err("'write' failed, request %d bytes", dlen);
        return SL_ERROR_GENERIC_IO;
    }
    else if (ret < dlen) {
        log_warn("Request to write %d bytes, but only %d bytes were finished",
                dlen, ret);
    }
    fc->wptr += ret;

    return SL_NO_ERROR;
}
