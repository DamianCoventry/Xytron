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
#include "stdafx.h"
#include "fontfile.h"
#include "../util/throw.h"

#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace Gfx;

///////////////////////////////////////////////////////////////////////////////
FontFile::FontFile()
: swatch_size_(0)
{
}

///////////////////////////////////////////////////////////////////////////////
FontFile::~FontFile()
{
}

///////////////////////////////////////////////////////////////////////////////
void FontFile::Load(const std::string& filename)
{
    LOG("Loading FONT file [" << filename << "]");

    std::ostringstream oss;
    std::ifstream file(filename.c_str(), std::ios_base::binary);
    if(!file)
    {
        THROW("Couldn't open the file [" << filename << "] for reading");
    }

    char id[4];
    file.read(id, 4);
    if(id[0] != 'F' || id[1] != 'O' || id[2] != 'N' || id[3] != 'T')
    {
        THROW("The file [" << filename << "] is not a valid font file");
    }

    LOGFONT log_font;
    file.read((char*)&log_font, sizeof(LOGFONT));
    face_name_ = log_font.lfFaceName;

    rects_.reset(new RECT[NUM_CHARS]);
    file.read((char*)rects_.get(), sizeof(RECT)*NUM_CHARS);

    file.read((char*)&swatch_size_, sizeof(int));

    int num_pixels = sizeof(unsigned char)*3*swatch_size_*swatch_size_;
    pixels_.reset(new unsigned char[num_pixels]);
    file.read((char*)pixels_.get(), sizeof(unsigned char)*num_pixels);
}
