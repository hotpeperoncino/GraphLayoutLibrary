#ifndef CLIPPINGPOINTS_H
#define CLIPPINGPOINTS_H

/**
 * @brief The ClippingPoints class
 *
 * The class provides helpers to manage clipping points in force based layouts.
 */
class ClippingPoints
{
public:
/** @name Creators
  *  The methods under this section are responsible for constructing or
  *  destructing an instance of type ClippingPoints.
  */
 //@{
	/**
     Constructs new object of type ClippingPoints.
    @pre none

    @param none

    @return none

    @throw none
    */
    ClippingPoints();
//@}

/** @name Queries
 *  The methods under this section are responsible for accessing
 *  an instance of type ClippingPoints.
 */
//@{
    /** Returns Clipping Points between two rectangles

    @pre
        -# dRectU and dRectV should be valid

    @param dRectU
        Ponter to rectangle U dimensions

    @param dRectV
        Ponter to rectangle V dimensions

    @param iCordinates
        Cordinates of edge

    @param iClipPoints
        Cordinates of found clip points

    @return true for intersection otherwise false

    @throws none

    */
	bool  getClippingPoints(double* dRectU, double* dRectV, int* iCordinates, int* iClipPoints );

    
    /** Returns Cardinal Directions between two rectangles given Slope and Prime

    @pre
        -# Slope  and Prime should be valid

    @param Slope
        Slope of the edge

    @param dSlopePrime
        Slope Prome for edge

    @param iLine
        Pointer to cordinates of line


    @return 0,1 or 2 as cardinal directions

    @throws none

    */
	int  getCardinalDirections(double dSlope,double dSlopePrime, int iLine);
    
    /** Returns true if two line segments intersect

    @pre
        -# P and Q shold be valid

    @param P and Q
       Pointer to cordinates of  Edges P and Q

    @return bool

    @throws none

    */
	bool  doIntersect(int *iP , int*iQ);

    /** Returns Orientation between two rectangles.

    @pre
        -# Co ordinates should be consistent

    @param P , Q and R points


    @return int
            orientation value of rectangle

    @throws none

    */
	int  orientation(int ipx, int ipy ,int iqx ,int iqy ,int irx ,int iry );

    /** Calculates intersection between two edges .

    @pre
        -# Edges  should be valid

    @param Slope
        Slope of the edge

    @param dSlopePrime
        Slope Prome for edge

    @param iLine
        Pointer to cordinates of line

    @return none

    @throws none

    */
	void  calcIntersection(int *iP , int* iEdge,  int* iIntersection );
//@}
};

#endif // CLIPPINGPOINTS_H
