/***********************************************************************
/
/  GRID CLASS (INITIALIZE COLOR FIELDS OF LAGRANGIAN COORDS)
/
/  written by: Tom Abel
/  date:       February, 2018
/  modified1:
/
/  PURPOSE:
/
/  RETURNS: FAIL or SUCCESS
/
************************************************************************/
#include <stdio.h>
#include <math.h>
#include "ErrorExceptions.h"
#include "macros_and_parameters.h"
#include "typedefs.h"
#include "global_data.h"
#include "Fluxes.h"
#include "GridList.h"
#include "ExternalBoundary.h"
#include "Grid.h"

int grid::InitializeLCoord()
{
  int dim, i, j, k, n, index, size, field, oNumB;
  
  PrepareGridDerivedQuantities();
  int L1, L2, L3 = -1;
  IdentifyLCoordFields(L1,L2,L3);

  int DensNum, GENum, Vel1Num, Vel2Num, Vel3Num, TENum;
  DensNum = GENum = Vel1Num = Vel2Num = Vel3Num = TENum = 0;
  IdentifyPhysicalQuantities(DensNum, GENum, Vel1Num, Vel2Num, Vel3Num, TENum);
  //fprintf(stderr,"\n\nYESYESYES %i,%i, %i, %i,%i,%i\n\n", GridRank,DensNum, L1, L2, ProcessorNumber,MyProcessorNumber );

  n = 0;

  float L0 = 1e-6; //prevent field values getting weird when normalizing species

  if (ProcessorNumber == MyProcessorNumber) {
    for (k = 0; k < GridDimension[2]; k++)
      for (j = 0; j < GridDimension[1]; j++)
        for (i = 0; i < GridDimension[0]; i++, n++) {
  	/* Compute position */
      BaryonField[L1][n] = L0 * BaryonField[DensNum][n] * 
  	  (CellLeftEdge[0][i] + 0.5*CellWidth[0][i] - DomainLeftEdge[0]);
  	if (GridRank > 1)
  	  BaryonField[L2][n] = L0 * BaryonField[DensNum][n] *
  	    (CellLeftEdge[1][j] + 0.5*CellWidth[1][j] - DomainLeftEdge[1]);
  	if (GridRank > 2) 
  	  BaryonField[L3][n] = L0 * BaryonField[DensNum][n] *
  	    (CellLeftEdge[2][k] + 0.5*CellWidth[2][k] - DomainLeftEdge[2]);
        }
  }

  return SUCCESS;
} /* End InitializeLCoord */
