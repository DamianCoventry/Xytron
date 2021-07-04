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
#ifndef INCLUDED_ISTATECONTROLLER
#define INCLUDED_ISTATECONTROLLER

///////////////////////////////////////////////////////////////////////////////
struct IStateController
{
    virtual void ChangeToStatePreGame(bool new_highscore = false, unsigned int level = 0, unsigned long score = 0) = 0;
    virtual void ChangeToStatePreWave(bool warping) = 0;
    virtual void ChangeToStateWave(bool resumed) = 0;
    virtual void ChangeToStatePostWave() = 0;
    virtual void ChangeToStatePaused() = 0;
    virtual void ChangeToStateGetReady(bool respawn_player) = 0;
    virtual void ChangeToStateGameOver() = 0;
    virtual void ChangeToStateGameWon() = 0;
};

#endif  // INCLUDED_ISTATECONTROLLER
