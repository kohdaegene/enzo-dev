/***********************************************************************
/
/  INITIALIZE LAGRANGIAN COORDINATE FIELDS
/
/  written by: Daegene Koh
/  date:       July, 2018
/  modified1:
/
/ PURPOSE: This routine initializes everything that is needed by the
/          Lagrangian Coordinates fields that is not included in the normal routines.
/
************************************************************************/
#include "preincludes.h"

#include <stdlib.h>
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
#include "Hierarchy.h"
#include "TopGridData.h"
#include "LevelHierarchy.h"
#include "ImplicitProblemABC.h"

int LagrangianCoordinatesInitialize(char *ParameterFile, 
				HierarchyEntry &TopGrid, 
				TopGridData &MetaData,
				ExternalBoundary &Exterior, 
				ImplicitProblemABC* &ImplicitSolver,
				LevelHierarchyEntry *LevelArray[], int restart)
{ 
  /* declarations */
  const char *Lcoord1Name = "x_lcoord";
  const char *Lcoord2Name = "y_lcoord";
  const char *Lcoord3Name = "z_lcoord";

  int i, j, k, level;
  FILE *fptr;
  LevelHierarchyEntry *Temp;
  int NumberOfObsoleteFields;
  int ObsoleteFields[MAX_NUMBER_OF_BARYON_FIELDS];

  if (LagrangianCoordinates == FALSE) {

    /* Check for fields and delete them */

    NumberOfObsoleteFields = 1;
    ObsoleteFields[0] = LCoord1;
    if (MetaData.TopGridRank > 1) {
      NumberOfObsoleteFields++;
      ObsoleteFields[1] = LCoord2;
    } if (MetaData.TopGridRank > 2) {
      NumberOfObsoleteFields++;
      ObsoleteFields[2] = LCoord3;
    }

    for (level = 0; level < MAX_DEPTH_OF_HIERARCHY; level++)
      for (Temp = LevelArray[level]; Temp; Temp = Temp->NextGridThisLevel)
	Temp->GridData->DeleteObsoleteFields(ObsoleteFields, 
					     NumberOfObsoleteFields);

    Exterior.DeleteObsoleteFields(ObsoleteFields, NumberOfObsoleteFields);

    return SUCCESS;
  }
  /* If we're restarting from a non-Lagrangian run, create
     fields.  We will check if the field already exists inside the
     grid routine. */

  int OldNumberOfBaryonFields = 0, FieldsToAdd = 0;
  int TypesToAdd[MAX_NUMBER_OF_BARYON_FIELDS];
  int ExistingTypes[MAX_NUMBER_OF_BARYON_FIELDS];

  for (i = 0; i < MAX_NUMBER_OF_BARYON_FIELDS; i++)
    ExistingTypes[i] = FieldUndefined;
  
  if (LagrangianCoordinates) {

    TypesToAdd[FieldsToAdd++] = LCoord1;
    if (MetaData.TopGridRank > 1) 
      TypesToAdd[FieldsToAdd++] = LCoord2;
    if (MetaData.TopGridRank > 2) 
      TypesToAdd[FieldsToAdd++] = LCoord3;
    
    
    // don't use the rest
    for (i = FieldsToAdd; i < MAX_NUMBER_OF_BARYON_FIELDS; i++)
      TypesToAdd[i] = FieldUndefined;

    /* Check if the fields already exist. */
    OldNumberOfBaryonFields = LevelArray[0]->GridData->
      ReturnNumberOfBaryonFields();
    LevelArray[0]->GridData->ReturnFieldType(ExistingTypes);

    for (i = 0; i < FieldsToAdd; i++)
      for (j = 0; j < OldNumberOfBaryonFields; j++)
	if (TypesToAdd[i] == ExistingTypes[j]) {
	  for (k = i; k < FieldsToAdd; k++)
	    TypesToAdd[k] = TypesToAdd[k+1];
	  i--;
	  break;
	} // ENDIF matching type
    FieldsToAdd = 0;
    while (TypesToAdd[FieldsToAdd] != FieldUndefined)
      FieldsToAdd++;

  } 
  // Add the necessary fields
  if (FieldsToAdd > 0 && debug)
    fprintf(stdout, "LagrangianCoordinatesInitialize: Increasing baryon fields "
	    "from %"ISYM" to %"ISYM"\n", OldNumberOfBaryonFields, 
	    OldNumberOfBaryonFields+FieldsToAdd);

  // Add an extra 1 because we will need it for flagging/marking cells.
  if (OldNumberOfBaryonFields+FieldsToAdd+1 > MAX_NUMBER_OF_BARYON_FIELDS)
    ENZO_FAIL("Exceeds MAX_NUMBER_OF_BARYON_FIELDS.  "
        "Please increase and re-compile.");

  for (level = 0; level < MAX_DEPTH_OF_HIERARCHY; level++)
    for (Temp = LevelArray[level]; Temp; Temp = Temp->NextGridThisLevel)
      Temp->GridData->AddFields(TypesToAdd, FieldsToAdd);

  // Add external boundaries
  for (i = 0; i < FieldsToAdd; i++) {
    Exterior.AddField(TypesToAdd[i]);
  } // ENDFOR fields

  if (!restart) { 
  //Initialize Lagrangian Field Values
    for (level = 0; level < MAX_DEPTH_OF_HIERARCHY; level++)
      for (Temp = LevelArray[level]; Temp; Temp = Temp->NextGridThisLevel)
        Temp->GridData->InitializeLCoord();

     for (level = MAX_DEPTH_OF_HIERARCHY; level > 0; level--)
      for (Temp = LevelArray[level]; Temp; Temp = Temp->NextGridThisLevel)
        Temp->GridData->ProjectSolutionToParentGrid(*LevelArray[level-1]->GridData);
  }

  // Assign the radiation field DataLabels
  for (i = 0; i < FieldsToAdd; i++) {
    switch (TypesToAdd[i]) {
    case LCoord1:
      DataLabel[OldNumberOfBaryonFields+i] = (char*) Lcoord1Name;
      break;
    case LCoord2:
      DataLabel[OldNumberOfBaryonFields+i] = (char*) Lcoord2Name;
      break;
    case LCoord3:
      DataLabel[OldNumberOfBaryonFields+i] = (char*) Lcoord3Name;
      break;
      } // ENDSWITCH
    if (debug) 
      printf("Adding new field %s\n", DataLabel[OldNumberOfBaryonFields+i]);
  } // ENDFOR fields


  return SUCCESS;

}
