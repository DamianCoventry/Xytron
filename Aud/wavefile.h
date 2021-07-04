///////////////////////////////////////////////////////////////////////////////
//
//  PROJECT: Sage
//
//  AUTHOR: Damian Coventry
//
//  CREATION DATE: June-July 2007
// 
//  COPYRIGHT NOTICE:
//
//      (C) Omenware
//      Created in 2007 as an unpublished copyright work.  All rights reserved.
//      This document and the information it contains is confidential and
//      proprietary to Omenware.  Hence, it may not be  used, copied, reproduced,
//      transmitted, or stored in any form or by any means, electronic,
//      recording, photocopying, mechanical or otherwise, without the prior
//      written permission of Omenware
//
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
#ifndef INCLUDED_AUD_WAVEFILE
#define INCLUDED_AUD_WAVEFILE

#include <string>
#include <boost/shared_array.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace Aud
{

///////////////////////////////////////////////////////////////////////////////
class WaveFile
{
public:
    WaveFile();
    ~WaveFile();

    void Load(const std::string& filename);
    void Unload();

    int GetBps() const      { return bps_; }
    int GetSps() const      { return sps_; }
    int GetChannels() const { return channels_; }
    int GetLength() const   { return length_; }

    boost::shared_array<unsigned char> GetBytes() const { return bytes_; }

private:
    void ProcessFormatChunk(std::ifstream& file, unsigned long chunk_size);
    void ProcessDataChunk(std::ifstream& file, unsigned long chunk_size);

private:
    std::string filename_;
    int bps_;
    int sps_;
    int channels_;
    int length_;

    boost::shared_array<unsigned char> bytes_;
};

}       // namespace Aud

#endif  // INCLUDED_AUD_WAVEFILE
