#ifndef _SL_FILE_H
#define _SL_FILE_H

#include <sl_types.h>
#include <sl_param_sys.h>
#include <sl_api_ksysctl.h>
#include <sl_api_filedev.h>

#ifdef __cplusplus
extern "C" {
#endif

SL_S32 openInitInFileCtrl(chanVideoPara_s *cvp, fileCtrl_s *ifc, const char *fname);
SL_S32 openInitOutFileCtrl(chanVideoPara_s *cvp, fileCtrl_s *ofc, const char *fname);
SL_S32 readFile(fileCtrl_s *fc, void *buf, SL_U32 size, SL_U32 *dlen);
SL_S32 writeFile(fileCtrl_s *fc, void *buf, SL_U32 dlen);
SL_S32 closeFile(fileCtrl_s *fc);

#ifdef __cplusplus
}
#endif

#endif /* _SL_FILE_H */
