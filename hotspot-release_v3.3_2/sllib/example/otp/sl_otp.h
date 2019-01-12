#ifndef _SILAN_OTP_H
#define _SILAN_OTP_H


#ifdef _cplusplus
    extern "C" {
#endif

#define OTP_IOC_MAGIC 'O'
#define OTP_GET_SIZE _IOR(OTP_IOC_MAGIC, 0, int)

#ifdef _cplusplus
}
#endif



#endif /* _SILAN_OTP_H */
