#include "ClippingPoints.h"

ClippingPoints::ClippingPoints()
{

}

bool ClippingPoints :: getClippingPoints(double* dRectU , double* dRectV,
                                         int* iCordinates, int* iClipPoints )
{
    // https://code.google.com/p/chilay/source/browse/src/main/java/org/ivis/util/IGeometry.java?name=
    //SBGN+Layout&r=e3f8360f41dfd86012603d678861b8f462b58659

    //dRect[i] = centrex,i+1 = centrey,i+2 = height,i+3 = width,i+4 = lefttopx,i+5 = lefttopy;
    int i=0;
    double dVXCoord =dRectV[i],dVYCoord = dRectV[i+1],dUXCoord = dRectU[i],
           dUYCoord = dRectU[i+1] ;

    // variables for rectangle U

    double dUHeight = dRectU[i+2], dUWidth=dRectU[i+3], dVHeight = dRectV[i+2],
           dVWidth = dRectV[i+3],  dUXTopLeft = dRectU[4], dUYTopLeft = dRectU[5],
           dVXTopLeft = dRectV[4], dVYTopLeft = dRectV[5];

    bool bClipA = false, bClipB = false ;

    // line is vertical
    if(dUXCoord == dVXCoord)
    {
        if(dUYCoord > dUYCoord)
        {
            iClipPoints[0] = (int) dUXCoord;
            iClipPoints[1] = (int) dUYTopLeft;
            iClipPoints[2] = (int) dVXCoord;
            iClipPoints[3] = (int) dVYTopLeft - dVHeight;
            return false ;

        }

        else if(dUYCoord < dVYCoord)
        {
            iClipPoints[0] = (int) dUXCoord;
            iClipPoints[1] = (int) dUYTopLeft - dUHeight;
            iClipPoints[2] = (int) dVXCoord;
            iClipPoints[3] = (int) dVYTopLeft;
            return false;

        }
        else
        {
            // not a line
            return false;
        }

    }
     else if( dUYCoord == dVYCoord )
    {
        if(dUXCoord > dVXCoord)
        {
            iClipPoints[0] = (int) dUXTopLeft;
            iClipPoints[1] = (int) dUYCoord;
            iClipPoints[2] = (int) dVXTopLeft+ dVWidth ;
            iClipPoints[3] = (int) dVYCoord;
            return false;
        }
        else if(dUXCoord < dVXCoord)
        {
            iClipPoints[0] = (int) dUXTopLeft + dUWidth;
            iClipPoints[1] = (int) dUYCoord;
            iClipPoints[2] = (int) dVXTopLeft ;
            iClipPoints[3] = (int) dVYCoord;
            return false;

        }
        else
        {
            return false;
        }

    }

    else
    {

        double dSlopeU = dUHeight / dUWidth ;
        double dSlopeV = dVHeight / dVWidth ;

        double dSlopePrime = (dVYCoord - dUYCoord) / (dVXCoord - dUXCoord);
        int iCardinalDirecU , iCardinalDirecV;
        double dTempUX, dTempUY , dTempVX , dTempVY;

        if( - dSlopeU == dSlopePrime)
        {
            if(dUXCoord > dVXCoord)
            {
                iClipPoints[0] = dUXTopLeft;
                iClipPoints[1] = dUYTopLeft - dUHeight;
                bClipA = true;

            }
            else
            {
                iClipPoints[0] = dUXTopLeft + dUWidth;
                iClipPoints[1] = dVYTopLeft;
                bClipA = true;

            }
        }
        else if (dSlopeU == dSlopePrime )
        {
            if(dUXCoord > dVXCoord)
            {
                iClipPoints[0] = dUXTopLeft;
                iClipPoints[1] = dUYTopLeft;
                bClipA = true;

            }
            else
            {
                iClipPoints[0] = dUXTopLeft + dUWidth ;
                iClipPoints[1] = dUYTopLeft - dUHeight;
                bClipA = true;

            }

        }


        //
        if( (- dSlopeV ) == dSlopePrime )
        {
            if( dVXCoord > dUXCoord )
            {
                iClipPoints[2] = dVXTopLeft ;
                iClipPoints[3] = dVYTopLeft - dVHeight;
                bClipB = true;

            }
            else
            {
                iClipPoints[2] = dVXTopLeft + dVWidth ;
                iClipPoints[3] = dVYTopLeft ;
                bClipB = true;

            }
        }
        else if( dSlopeV  == dSlopePrime )
        {
            if( dVXCoord > dUXCoord )
            {
                iClipPoints[2] = dVXTopLeft ;
                iClipPoints[3] = dVYTopLeft ;
                bClipB = true;

            }
            else
            {
                iClipPoints[2] = dVXTopLeft + dVWidth ;
                iClipPoints[3] = dVYTopLeft - dVWidth ;
                bClipB = true;

            }

        }
        //

        if( bClipA && bClipB )
        {
            return false;
        }


        // determine cardinal direction of rectangles .

        if(dUXCoord > dVXCoord )
        {
            if(dUYCoord > dVYCoord )
            {
                iCardinalDirecU = getCardinalDirections(dSlopeU,dSlopePrime,4);
                iCardinalDirecV = getCardinalDirections(dSlopeV,dSlopePrime,2);

            }
            else
            {
                iCardinalDirecU = getCardinalDirections(-dSlopeU,dSlopePrime,3);
                iCardinalDirecV = getCardinalDirections(-dSlopeV,dSlopePrime,1);

            }
        }
        else
        {
            if(dUYCoord > dVYCoord )
            {
                iCardinalDirecU = getCardinalDirections(-dSlopeU,dSlopePrime,1);
                iCardinalDirecV = getCardinalDirections(-dSlopeV,dSlopePrime,3);

            }
            else
            {
                iCardinalDirecU = getCardinalDirections(dSlopeU,dSlopePrime,2);
                iCardinalDirecV = getCardinalDirections(dSlopeV,dSlopePrime,4);

            }


        }
        // calculate clipping pt if not calculated earlier

        if(!bClipA)
        {
            switch (iCardinalDirecU) {
            case 1:
                dTempUX  =  dUXCoord + (- dUHeight /2)/ dSlopePrime;
                dTempUY = dUYTopLeft;
                iClipPoints[0] = dTempUX;
                iClipPoints[1] = dTempUY;
                break;

            case 2:
                dTempUX  = dUXTopLeft + dUWidth;
                dTempUY = dUYCoord + (dUWidth/2)*dSlopePrime ;
                iClipPoints[0] = dTempUX;
                iClipPoints[1] = dTempUY;
                break;

            case 3:
                dTempUX  =  dUXCoord + (dUHeight /2)/ dSlopePrime;
                dTempUY =   dUYTopLeft - dUHeight;
                iClipPoints[0] = dTempUX;
                iClipPoints[1] = dTempUY;
                break;

            case 4:
                dTempUX  =  dUXTopLeft;
                dTempUY =   dUYCoord + (-dUWidth /2)*dSlopePrime ;
                iClipPoints[0] = dTempUX;
                iClipPoints[1] = dTempUY;
                break;

            default:
                break;
            }
        }
        if(!bClipB)
        {
            switch (iCardinalDirecV) {
            case 1:
                dTempVX = dVXCoord + (-dVHeight/2)*dSlopePrime;
                dTempVY = dUYTopLeft;
                iClipPoints[2] = dTempVX;
                iClipPoints[3] = dTempVY;
                break;

            case 2:
                dTempVX  = dVXTopLeft + dVWidth;
                dTempVY =  dVYCoord + (dVWidth/2)*dSlopePrime ;
                iClipPoints[2] = dTempVX;
                iClipPoints[3] = dTempVY;
                break;

            case 3:
                dTempVX  =  dVXCoord + (dVHeight /2)/ dSlopePrime;
                dTempVY =   dVYTopLeft - dVHeight;
                iClipPoints[2] = dTempVX;
                iClipPoints[3] = dTempVY;
                break;

            case 4:
                dTempVX  =  dVXTopLeft;
                dTempVY =   dVYCoord + (-dVWidth /2)*dSlopePrime ;
                iClipPoints[2] = dTempVX;
                iClipPoints[3] = dTempVY;
                break;

            default:
                break;
            }
        }




    }

    return false;

}


int ClippingPoints :: getCardinalDirections(double dSlope,double dSlopePrime, int iLine)
{
    if(dSlope > dSlopePrime)
    {
        return iLine;
    }
    else
    {
        return (1 + iLine % 4 ) ;
    }
}


bool ClippingPoints :: doIntersect(int *iP , int*iQ)
{
    /*
    // returns true if two line segments intersect
    // ip ={ x1,y1,x2,y2}
    // Find the four orientations needed for general and
       // special cases
       int o1 = orientation( iP[0],iP[1],iP[2],iP[3],iQ[0],iQ[1]);
       int o2 = orientation( iP[0],iP[1],iP[2],iP[3],iQ[2],iQ[3]);
       int o3 = orientation( iQ[0],iQ[1],iQ[2],iQ[3],iP[0],iP[1]);
       int o4 = orientation(iQ[0],iQ[1],iQ[2],iQ[3] ,iP[2],iP[3]);

       // General case
       if (o1 != o2 && o3 != o4)
       {
           return true;
       }
       return false; // Doesn't fall in any of the above cases */
     float a1, a2, b1, b2, c1, c2;
     float r1, r2 , r3, r4;

     // Compute a1, b1, c1, where line joining points 1 and 2
     // is "a1 x + b1 y + c1 = 0".
     a1 = iP[1] - iP[3];
     b1 = iP[0] - iP[2];
     c1 = (iP[2] * iP[1]) - (iP[0] * iP[3]);

     // Compute r3 and r4.
     r3 = ((a1 * iQ[0]) + (b1 * iQ[1]) + c1);
     r4 = ((a1 * iQ[2]) + (b1 * iQ[3]) + c1);

     // Check signs of r3 and r4. If both point 3 and point 4 lie on
     // same side of line 1, the line segments do not intersect.
     if ((r3 != 0) && (r4 != 0) && ((r3 > 0 && r4> 0)|| (r3<0 && r4 <0))){
       return false;
     }
       else
       {
           return true;
       }


}

int ClippingPoints :: orientation(int ipx, int ipy ,int iqx ,int iqy ,int irx ,int iry )
{
      int val =  (iqy - ipy) * (irx - iqx) -
                 (iqx - ipx) * (iry - iqy);

       if (val == 0) return 0;  // colinear

       return (val > 0)? 1: 2;
}


void ClippingPoints :: calcIntersection(int *iP , int* iEdge,  int* iIntersection )
{
    float iDx1 , iDy1 , iDx2 , iDy2 , im1, im2 , iC1 , iC2 ;
    iDx1 = iP[0] - iP[2] +0.00001;
    iDy1 = iP[1] - iP[3] + 0.00001 ;

    im1 = iDy1/iDx1;
    iC1 = iP[1] - (im1 *iP[0]);


    iDx2 = iEdge[0] - iEdge[2] +0.00001;  // to remove divide by zero
    iDy2 = iEdge[1] - iEdge[3] + 0.00001;

    im2 = iDy2/iDx2;
    iC2 = iEdge[3] - (im2 * iEdge[2]);

    iIntersection[0] = (iC2 - iC1)/(im1 - im2);
    iIntersection[1] = im1*iIntersection[0] + iC1;





}




























