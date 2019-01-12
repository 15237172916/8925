#ifndef     H264_FRAMED_LIVE_SOURCE_H__
#define     H264_FRAMED_LIVE_SOURCE_H__


#include     "FramedSource.hh"



class H264FramedLiveSource: public FramedSource{
    public:
        static H264FramedLiveSource  *createNew(UsageEnvironment& env,
                char const* fileName, int  vch,
                unsigned preferredFrameSize = 0,
                unsigned playTimePerFrame = 0);
    protected:
        H264FramedLiveSource(UsageEnvironment& env,
                char const* fileName,int vch,
                unsigned preferredFrameSize,
                unsigned playTimePerFrame);
        ~H264FramedLiveSource();
    private:
        virtual  void   doGetNextFrame(void);
        int TransportData( unsigned char* to, unsigned maxSize );
        virtual unsigned maxFrameSize() const;
    private:
        FILE    *fp;  //only for test
        const  int  mVideoChannel;  //add by Andy_xi, WAC,PTZ,WAC_PTZ
};




#endif
