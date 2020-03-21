#include "MotionEstimationEngine.h"
#include "ME.h"

#define MAXSPEED 0
#define EXTRANEIGHBORS

const ASCMv
	zero = {0,0};

void MotionRangeDeliveryF(mfxI16 xLoc, mfxI16 yLoc, mfxI16 *limitXleft, mfxI16 *limitXright, mfxI16 *limitYup, mfxI16 *limitYdown, ASCImDetails dataIn) {
    mfxI32
        locX = 0,
        locY = 0;
    locY = yLoc / ((3 * (16 / dataIn.block_height)) / 2);
    locX = xLoc / ((3 * (16 / dataIn.block_width)) / 2);
    *limitXleft     = (mfxI16)NMAX(-8, -(xLoc * dataIn.block_width) - dataIn.horizontal_pad);
    *limitXright    = (mfxI16)NMIN(7, dataIn.Extended_Width - (((xLoc + 1) * dataIn.block_width) + dataIn.horizontal_pad));
    *limitYup       = (mfxI16)NMAX(-8, -(yLoc * dataIn.block_height) - dataIn.vertical_pad);
    *limitYdown     = (mfxI16)NMIN(7, dataIn.Extended_Height - (((yLoc + 1) * dataIn.block_width) + dataIn.vertical_pad));
}

bool MVcalcSAD(ASCMv MV, pmfxU8 curY, pmfxU8 refY, ASCImDetails dataIn, mfxU16 *bestSAD, mfxI32 *distance) {
    mfxU16
        SAD = 0;
    mfxI32
        preDist = (MV.x * MV.x) + (MV.y * MV.y);
    pmfxU8
        fRef = refY + MV.x + (MV.y * dataIn.Extended_Width);
    if(dataIn.block_width == 4 && dataIn.block_height == 4)
        SAD                    =    ME_SAD_4x4_Block(curY, fRef, dataIn.Extended_Width, dataIn.Extended_Width);
    else if(dataIn.block_width == 8 && dataIn.block_height == 8)
        SAD                    =    ME_SAD_8x8_Block(curY, fRef, dataIn.Extended_Width, dataIn.Extended_Width);
    else if(dataIn.block_width == 16 && dataIn.block_height == 8)
        SAD                    =    ME_SAD_16x8_Block(curY, fRef, dataIn.Extended_Width, dataIn.Extended_Width);
    else if(dataIn.block_width == 8 && dataIn.block_height == 16)
        SAD                    =    ME_SAD_8x16_Block(curY, fRef, dataIn.Extended_Width, dataIn.Extended_Width);
    else if(dataIn.block_width == 16 && dataIn.block_height == 16)
        SAD                    =    ME_SAD_16x16_Block(curY, fRef, dataIn.Extended_Width, dataIn.Extended_Width);
    if((SAD < *bestSAD) ||((SAD == *(bestSAD)) && *distance > preDist)) {
        *distance = preDist;
        *(bestSAD) = SAD;
        return true;
    }
    return false;
}

bool MVcalcSAD4x4(ASCMv MV, pmfxU8 curY, pmfxU8 refY, ASCImDetails dataIn, mfxU16 *bestSAD, mfxI32 *distance) {
    mfxI32
        preDist = (MV.x * MV.x) + (MV.y * MV.y);
    pmfxU8
        fRef = refY + MV.x + (MV.y * dataIn.Extended_Width);
    mfxU16
        SAD = ME_SAD_4x4_Block(curY, fRef, dataIn.Extended_Width, dataIn.Extended_Width);
    if((SAD < *bestSAD) ||((SAD == *(bestSAD)) && *distance > preDist)) {
        *distance = preDist;
        *(bestSAD) = SAD;
        return true;
    }
    return false;
}

bool MVcalcSAD8x8(ASCMv MV, pmfxU8 curY, pmfxU8 refY, ASCImDetails *dataIn, mfxU16 *bestSAD, mfxI32 *distance) {
    mfxI32
        preDist = (MV.x * MV.x) + (MV.y * MV.y),
        _fPos = MV.x + (MV.y * dataIn->Extended_Width);
    pmfxU8
        fRef = &refY[_fPos];
    mfxU16
        SAD = ME_SAD_8x8_Block(curY, fRef, dataIn->Extended_Width, dataIn->Extended_Width);
    if((SAD < *bestSAD) || ((SAD == *(bestSAD)) && *distance > preDist)) {
        *distance = preDist;
        *(bestSAD) = SAD;
        return true;
    }
    return false;
}

void MVcalcVar8x8(ASCMv MV, pmfxU8 curY, pmfxU8 refY, mfxI16 curAvg, mfxI16 refAvg, mfxI64 &var, mfxI64 &jtvar, mfxI64 &jtMCvar, ASCImDetails *dataIn) {
    mfxI32
        _fPos = MV.x + (MV.y * dataIn->Extended_Width);
    pmfxU8
        fRef = &refY[_fPos];

    ME_VAR_8x8_Block(curY, refY, fRef, curAvg, refAvg, dataIn->Extended_Width, dataIn->Extended_Width, var, jtvar, jtMCvar);
}

bool MVcalcSAD16x8(ASCMv MV, pmfxU8 curY, pmfxU8 refY, ASCImDetails dataIn, mfxU16 *bestSAD, mfxI32 *distance) {
    mfxI32
        preDist = (MV.x * MV.x) + (MV.y * MV.y);
    pmfxU8
        fRef = refY + MV.x + (MV.y * dataIn.Extended_Width);
    mfxU16
        SAD = ME_SAD_16x8_Block(curY, fRef, dataIn.Extended_Width, dataIn.Extended_Width);
    if((SAD < *bestSAD) || ((SAD == *(bestSAD)) && *distance > preDist)) {
        *distance = preDist;
        *(bestSAD) = SAD;
        return true;
    }
    return false;
}

bool MVcalcSAD8x16(ASCMv MV, pmfxU8 curY, pmfxU8 refY, ASCImDetails dataIn, mfxU16 *bestSAD, mfxI32 *distance) {
    mfxI32
        preDist = (MV.x * MV.x) + (MV.y * MV.y);
    pmfxU8
        fRef = refY + MV.x + (MV.y * dataIn.Extended_Width);
    mfxU16
        SAD = ME_SAD_8x16_Block(curY, fRef, dataIn.Extended_Width, dataIn.Extended_Width);
    if((SAD < *bestSAD) || ((SAD == *(bestSAD)) && *distance > preDist)) {
        *distance = preDist;
        *(bestSAD) = SAD;
        return true;
    }
    return false;
}

bool MVcalcSAD16x16(ASCMv MV, pmfxU8 curY, pmfxU8 refY, ASCImDetails dataIn, mfxU16 *bestSAD, mfxI32 *distance) {
    mfxI32
        preDist = (MV.x * MV.x) + (MV.y * MV.y);
    pmfxU8
        fRef = refY + MV.x + (MV.y * dataIn.Extended_Width);
    mfxU16
        SAD = ME_SAD_16x16_Block(curY, fRef, dataIn.Extended_Width, dataIn.Extended_Width);
    if((SAD < *bestSAD) || ((SAD == *(bestSAD)) && *distance > preDist)) {
        *distance = preDist;
        *(bestSAD) = SAD;
        return true;
    }
    return false;
}

void SearchLimitsCalcSqr(mfxI16 xLoc, mfxI16 yLoc, mfxI16 *limitXleft, mfxI16 *limitXright, mfxI16 *limitYup, mfxI16 *limitYdown, ASCImDetails *dataIn, mfxI32 range, ASCMv mv, ASCVidData *limits) {
    mfxI16
        locX = (mfxI16)((xLoc * dataIn->block_width) + dataIn->horizontal_pad + mv.x),
        locY = (mfxI16)((yLoc * dataIn->block_height) + dataIn->vertical_pad + mv.y);
    *limitXleft     = (mfxI16)NMAX(-locX,-range);
    *limitXright    = (mfxI16)NMIN(dataIn->Extended_Width - ((xLoc + 1) * dataIn->block_width) - dataIn->horizontal_pad - mv.x,range - 1);
    *limitYup       = (mfxI16)NMAX(-locY,-range);
    *limitYdown     = (mfxI16)NMIN(dataIn->Extended_Height - ((yLoc + 1) * dataIn->block_height) - dataIn->vertical_pad - mv.y,range - 1);
    if(limits->limitRange) {
        *limitXleft     = (mfxI16)NMAX(*limitXleft,-limits->maxXrange);
        *limitXright    = (mfxI16)NMIN(*limitXright,limits->maxXrange);
        *limitYup       = (mfxI16)NMAX(*limitYup,-limits->maxYrange);
        *limitYdown     = (mfxI16)NMIN(*limitYdown,limits->maxYrange);
    }
}

void SearchLimitsCalc(mfxI16 xLoc, mfxI16 yLoc, mfxI16 *limitXleft, mfxI16 *limitXright, mfxI16 *limitYup, mfxI16 *limitYdown, ASCImDetails *dataIn, mfxI32 range, ASCMv mv, ASCVidData *limits) {
    mfxI16
        locX = (mfxI16)((xLoc * dataIn->block_width) + dataIn->horizontal_pad + mv.x),
        locY = (mfxI16)((yLoc * dataIn->block_height) + dataIn->vertical_pad + mv.y);
    *limitXleft     = (mfxI16)NMAX(-locX,-range);
    *limitXright    = (mfxI16)NMIN(dataIn->Extended_Width - ((xLoc + 1) * dataIn->block_width) - dataIn->horizontal_pad - mv.x,range);
    *limitYup       = (mfxI16)NMAX(-locY,-range);
    *limitYdown     = (mfxI16)NMIN(dataIn->Extended_Height - ((yLoc + 1) * dataIn->block_height) - dataIn->vertical_pad - mv.y,range);
    if(limits->limitRange) {
        *limitXleft     = (mfxI16)NMAX(*limitXleft,-limits->maxXrange);
        *limitXright    = (mfxI16)NMIN(*limitXright,limits->maxXrange);
        *limitYup       = (mfxI16)NMAX(*limitYup,-limits->maxYrange);
        *limitYdown     = (mfxI16)NMIN(*limitYdown,limits->maxYrange);
    }
}

mfxF32 Dist(ASCMv vector) {
    return (mfxF32)sqrt((double)(vector.x*vector.x) + (double)(vector.y*vector.y));
}

mfxI32 DistInt(ASCMv vector) {
    return (vector.x*vector.x) + (vector.y*vector.y);
}

void MVpropagationCheck(mfxI32 xLoc, mfxI32 yLoc, ASCImDetails dataIn, ASCMv *propagatedMV) {
    mfxI16
        left    = (mfxI16)((xLoc * dataIn.block_width) + dataIn.horizontal_pad),
        right   = (mfxI16)(dataIn.Extended_Width - left - dataIn.block_width),
        up      = (mfxI16)((yLoc * dataIn.block_height) + dataIn.vertical_pad),
        down    = (mfxI16)(dataIn.Extended_Height - up - dataIn.block_height);
    if(propagatedMV->x < 0) {
        if(left + propagatedMV->x < 0)
            propagatedMV->x = -left;
    }
    else {
        if(right - propagatedMV->x < 0)
            propagatedMV->x = right;
    }

    if(propagatedMV->y < 0) {
        if(up + propagatedMV->y < 0)
            propagatedMV->y = -up;
    }
    else {
        if(down - propagatedMV->y < 0)
            propagatedMV->y = down;
    }
}

mfxU16 __cdecl ME_simple(ASCVidRead *videoIn, mfxI32 fPos, ASCImDetails *dataIn, ASCimageData *scale, ASCimageData *scaleRef, bool first, ASCVidData *limits) {
    ASCMv
        tMV,
        ttMV,
        *current,
        predMV = zero,
        Nmv    = zero;
    mfxU8
        *objFrame = NULL,
        *refFrame = NULL;
    mfxI16
        limitXleft  = 0,
        limitXright = 0,
        limitYup    = 0,
        limitYdown  = 0,
        xLoc = ((mfxI16)fPos % (mfxI16)dataIn->Width_in_blocks),
        yLoc = ((mfxI16)fPos / (mfxI16)dataIn->Width_in_blocks);
    mfxI32
        distance     = 0,
        mainDistance = 0,
        offset       = (yLoc * (mfxI16)dataIn->Extended_Width * (mfxI16)dataIn->block_height) + (xLoc * (mfxI16)dataIn->block_width);
    mfxU16
        *outSAD,
        zeroSAD = USHRT_MAX,
        bestSAD = USHRT_MAX;
    mfxU8
        neighbor_count = 0;
    bool
        foundBetter = false;

    (void)first;

    objFrame = &scale->Image.Y[offset];
    refFrame = &scaleRef->Image.Y[offset];

    current = scale->pInteger;
    outSAD  = scale->SAD;

    outSAD[fPos] = USHRT_MAX;

    MVcalcSAD8x8(zero, objFrame, refFrame, dataIn, &bestSAD, &distance);
    current[fPos] = zero;
    outSAD[fPos]  = bestSAD;
    zeroSAD       = bestSAD;
    mainDistance  = distance;
    if (bestSAD == 0)
        return bestSAD;

    if ((fPos > (mfxI32)dataIn->Width_in_blocks) && (xLoc > 0)) { //Top Left
        neighbor_count++;
        Nmv.x += current[fPos - dataIn->Width_in_blocks - 1].x;
        Nmv.y += current[fPos - dataIn->Width_in_blocks - 1].y;
    }
    if (fPos > (mfxI32)dataIn->Width_in_blocks) { // Top
        neighbor_count++;
        Nmv.x += current[fPos - dataIn->Width_in_blocks].x;
        Nmv.y += current[fPos - dataIn->Width_in_blocks].y;
    }
    if (xLoc > 0) {//Left
        neighbor_count++;
        Nmv.x += current[fPos - 1].x;
        Nmv.y += current[fPos - 1].y;
    }
    if (neighbor_count) {
        Nmv.x /= neighbor_count;
        Nmv.y /= neighbor_count;
        if ((Nmv.x + ((xLoc + 1) * MVBLK_SIZE)) > SMALL_WIDTH)
            Nmv.x -= (Nmv.x + ((xLoc + 1) * MVBLK_SIZE)) - SMALL_WIDTH;
        else if (((xLoc * MVBLK_SIZE) + Nmv.x) < 0)
            Nmv.x -= ((xLoc * MVBLK_SIZE) + Nmv.x);

        if ((Nmv.y + ((yLoc + 1) * MVBLK_SIZE)) > SMALL_HEIGHT)
            Nmv.y -= (Nmv.y + ((yLoc + 1) * MVBLK_SIZE)) - SMALL_HEIGHT;
        else if (((yLoc * MVBLK_SIZE) + Nmv.y) < 0)
            Nmv.y -= ((yLoc * MVBLK_SIZE) + Nmv.y);

        distance = mainDistance;
        if (Nmv.x != zero.x || Nmv.y != zero.y) {
            foundBetter = MVcalcSAD8x8(Nmv, objFrame, refFrame, dataIn, &bestSAD, &distance);
            if (foundBetter) {
                current[fPos] = Nmv;
                outSAD[fPos] = bestSAD;
                mainDistance = distance;
            }
        }
    }
    
    //Search around the best predictor (zero or Neighbor)
    SearchLimitsCalcSqr(xLoc, yLoc, &limitXleft, &limitXright, &limitYup, &limitYdown, dataIn, 8, current[fPos], limits);//Checks limits for +-8
    ttMV     = current[fPos];
    bestSAD  = outSAD[fPos];
    distance = mainDistance;

    {//Search area in steps of 2 for x and y
        mfxI32 _fPos = (limitYup + ttMV.y) * dataIn->Extended_Width + limitXleft + ttMV.x;
        mfxU8
            *ps = objFrame,
            *pr = &refFrame[_fPos];
        int xrange = limitXright - limitXleft/* + 1*/,
            yrange = limitYdown - limitYup/* + 1*/,
            bX     = 0,
            bY     = 0;
        ME_SAD_8x8_Block_Search(ps, pr, dataIn->Extended_Width, xrange, yrange, &bestSAD, &bX, &bY);
        if (bestSAD < outSAD[fPos]) {
            outSAD[fPos]    = bestSAD;
            current[fPos].x = (mfxI16)bX + limitXleft + ttMV.x;
            current[fPos].y = (mfxI16)bY + limitYup + ttMV.y;
            mainDistance    = DistInt(current[fPos]);
        }
    }
    //Final refinement +-1 search
    ttMV     = current[fPos];
    bestSAD  = outSAD[fPos];
    distance = mainDistance;
    SearchLimitsCalc(xLoc, yLoc, &limitXleft, &limitXright, &limitYup, &limitYdown, dataIn, 1, ttMV, limits);
    for (tMV.y = limitYup; tMV.y <= limitYdown; tMV.y++) {
        for (tMV.x = limitXleft; tMV.x <= limitXright; tMV.x++) {
            if (tMV.x != 0 || tMV.y != 0) {// don't search on center position
                predMV.x = tMV.x + ttMV.x;
                predMV.y = tMV.y + ttMV.y;
                foundBetter = MVcalcSAD8x8(predMV, objFrame, refFrame, dataIn, &bestSAD, &distance);
                if (foundBetter) {
                    current[fPos] = predMV;
                    outSAD[fPos]  = bestSAD;
                    mainDistance  = distance;
                    foundBetter = false;
                }
            }
        }
    }
    videoIn->average += (current[fPos].x * current[fPos].x) + (current[fPos].y * current[fPos].y);
    MVcalcVar8x8(current[fPos], objFrame, refFrame, scale->avgval, scaleRef->avgval, scale->var, scale->jtvar, scale->mcjtvar, dataIn);
    return(zeroSAD);
}
