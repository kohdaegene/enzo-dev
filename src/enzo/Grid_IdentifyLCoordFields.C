/***********************************************************************
/
/  GRID CLASS (IDENTIFY LAGRANGIAN COORDINATES FIELDS)
/
/  written by: Tom Abel
/  date:       February, 2018
/  modified1:
/
/  PURPOSE:
/
/  NOTE:
/
************************************************************************/

#include <stdio.h>
#include "ErrorExceptions.h"
#include "macros_and_parameters.h"
#include "typedefs.h"
#include "global_data.h"
#include "Fluxes.h"
#include "GridList.h"
#include "ExternalBoundary.h"
#include "Grid.h"
 
/* function prototypes */
 
int FindField(int f, int farray[], int n);

int grid::IdentifyLCoordFields(int &LCoord1Num, int &LCoord2Num, int &LCoord3Num)
{
 
  LCoord1Num, LCoord2Num, LCoord3Num =0 ;
 
  LCoord1Num = FindField(LCoord1, FieldType, NumberOfBaryonFields);
  LCoord2Num = FindField(LCoord2, FieldType, NumberOfBaryonFields);
  LCoord3Num = FindField(LCoord3, FieldType, NumberOfBaryonFields);

  return SUCCESS;
}

