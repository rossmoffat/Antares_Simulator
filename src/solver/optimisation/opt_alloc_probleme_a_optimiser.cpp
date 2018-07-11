/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

# include "opt_structure_probleme_a_resoudre.h"

# include "../simulation/simulation.h"
# include "../simulation/sim_structure_donnees.h"
# include "../simulation/sim_structure_probleme_economique.h"
# include "../simulation/sim_structure_probleme_adequation.h"
# include "../simulation/sim_extern_variables_globales.h"

# include "opt_fonctions.h"

# include "../ext/Sirius_Solver/simplexe/spx_definition_arguments.h"
# include "../ext/Sirius_Solver/simplexe/spx_fonctions.h"
# include <stdio.h>

static void optimisationAllocateProblem( PROBLEME_HEBDO * ProblemeHebdo, const int mxPaliers )
{
	int NbTermes; int NbIntervalles; int NumIntervalle; int i;
	size_t szNbVarsDouble; size_t szNbVarsint; size_t szNbContint;
  int NombreDePasDeTempsPourUneOptimisation;
	
	PROBLEME_ANTARES_A_RESOUDRE * ProblemeAResoudre;
	PROBLEMES_SIMPLEXE ** ProblemesSpxDUneClasseDeManoeuvrabilite;

	ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

  NombreDePasDeTempsPourUneOptimisation = ProblemeHebdo->NombreDePasDeTempsPourUneOptimisation;
	
	szNbVarsDouble = ProblemeAResoudre->NombreDeVariables   * sizeof(double);
	szNbVarsint    = ProblemeAResoudre->NombreDeVariables   * sizeof(int  );
	szNbContint    = ProblemeAResoudre->NombreDeContraintes * sizeof(int  );

	ProblemeAResoudre->Sens                    = (char *) MemAlloc(ProblemeAResoudre->NombreDeContraintes * sizeof( char ));
	ProblemeAResoudre->IndicesDebutDeLigne     = (int *)  MemAlloc(szNbContint);
	ProblemeAResoudre->NombreDeTermesDesLignes = (int *)  MemAlloc(szNbContint);

  NbTermes  = (int) mxPaliers;
	NbTermes += 3; 
	NbTermes += (int)(0.25 * ProblemeHebdo->NombreDInterconnexions);
	NbTermes *= ProblemeAResoudre->NombreDeContraintes;
	NbTermes += ProblemeHebdo->NombreDePays * NombreDePasDeTempsPourUneOptimisation;

	NbTermes += ProblemeHebdo->NombreDePays * NombreDePasDeTempsPourUneOptimisation * 4;

	
  NbTermes += ProblemeHebdo->NbTermesContraintesPourLesCoutsDeDemarrage;
		
	ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes = (double *) MemAlloc(NbTermes * sizeof( double ));
	ProblemeAResoudre->IndicesColonnes                       = (int *)    MemAlloc(NbTermes * sizeof( int ));

	ProblemeAResoudre->NombreDeTermesAllouesDansLaMatriceDesContraintes = NbTermes;
	ProblemeAResoudre->IncrementDAllocationMatriceDesContraintes        = (int)(0.1 * NbTermes);
	
	ProblemeAResoudre->CoutQuadratique= (double *) MemAlloc(szNbVarsDouble);
	ProblemeAResoudre->CoutLineaire   = (double *) MemAlloc(szNbVarsDouble);
	ProblemeAResoudre->TypeDeVariable = (int *)    MemAlloc(szNbVarsint);
	ProblemeAResoudre->Xmin   = (double *) MemAlloc(szNbVarsDouble);
	ProblemeAResoudre->Xmax   = (double *) MemAlloc(szNbVarsDouble);
	ProblemeAResoudre->X      = (double *) MemAlloc(szNbVarsDouble);

	ProblemeAResoudre->SecondMembre = (double *) MemAlloc(ProblemeAResoudre->NombreDeContraintes * sizeof( double ));

	ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees = (double **) MemAlloc(ProblemeAResoudre->NombreDeVariables   * sizeof( void * ));
	ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits        = (double **) MemAlloc(ProblemeAResoudre->NombreDeVariables   * sizeof( void * ));
	ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux      = (double **) MemAlloc(ProblemeAResoudre->NombreDeContraintes * sizeof( void * )); 

	ProblemeAResoudre->CoutsMarginauxDesContraintes = (double *) MemAlloc(ProblemeAResoudre->NombreDeContraintes * sizeof( double ));
	ProblemeAResoudre->CoutsReduits                 = (double *) MemAlloc(szNbVarsDouble);

	
  NbIntervalles = (int) (ProblemeHebdo->NombreDePasDeTemps / NombreDePasDeTempsPourUneOptimisation );

	ProblemeAResoudre->ProblemesSpxDUneClasseDeManoeuvrabilite =
		(PROBLEMES_SIMPLEXE **) MemAlloc(ProblemeHebdo->NombreDeClassesDeManoeuvrabiliteActives * sizeof( void * ));

	ProblemesSpxDUneClasseDeManoeuvrabilite = ProblemeAResoudre->ProblemesSpxDUneClasseDeManoeuvrabilite;

	if ( ProblemesSpxDUneClasseDeManoeuvrabilite != NULL) {
		for ( i = 0; i < ProblemeHebdo->NombreDeClassesDeManoeuvrabiliteActives; i++ ) {

			ProblemesSpxDUneClasseDeManoeuvrabilite[i] = (PROBLEMES_SIMPLEXE*) MemAlloc( sizeof( PROBLEMES_SIMPLEXE ));

			ProblemesSpxDUneClasseDeManoeuvrabilite[i]->ProblemeSpx = (void **) MemAlloc(NbIntervalles * sizeof( void * ));
			for ( NumIntervalle = 0; NumIntervalle < NbIntervalles ; NumIntervalle++ ) {
				ProblemesSpxDUneClasseDeManoeuvrabilite[i]->ProblemeSpx[NumIntervalle] = NULL;
			}
		}
	}

	
	ProblemeAResoudre->PositionDeLaVariable = (int *) MemAlloc( ProblemeAResoudre->NombreDeVariables   * sizeof( int ) );
	ProblemeAResoudre->ComplementDeLaBase   = (int *) MemAlloc( ProblemeAResoudre->NombreDeContraintes * sizeof( int ) );

	
	ProblemeAResoudre->Pi      = (double *) MemAlloc( ProblemeAResoudre->NombreDeVariables * sizeof( double ) );
	ProblemeAResoudre->Colonne = (int *)    MemAlloc( ProblemeAResoudre->NombreDeVariables * sizeof( int   ) );
	
}



void OPT_AllocDuProblemeAOptimiser( PROBLEME_HEBDO * ProblemeHebdo )
{
	int mxPaliers; PROBLEME_ANTARES_A_RESOUDRE * ProblemeAResoudre;
	ProblemeAResoudre = (PROBLEME_ANTARES_A_RESOUDRE *) MemAllocMemset(sizeof( PROBLEME_ANTARES_A_RESOUDRE ));
	ProblemeHebdo->ProblemeAResoudre = ProblemeAResoudre;

	
	
	OPT_DecompteDesVariablesEtDesContraintesDuProblemeAOptimiser( ProblemeHebdo, &mxPaliers);
	
	optimisationAllocateProblem( ProblemeHebdo, mxPaliers );
}



void OPT_AugmenterLaTailleDeLaMatriceDesContraintes( PROBLEME_ANTARES_A_RESOUDRE * ProblemeAResoudre )
{
	int NbTermes;

	NbTermes = ProblemeAResoudre->NombreDeTermesAllouesDansLaMatriceDesContraintes;
	NbTermes+= ProblemeAResoudre->IncrementDAllocationMatriceDesContraintes;

	ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes =
		(double *) MemRealloc(ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes, NbTermes * sizeof( double ));

	ProblemeAResoudre->IndicesColonnes =
		(int *)   MemRealloc(ProblemeAResoudre->IndicesColonnes, NbTermes * sizeof( int   ));

	ProblemeAResoudre->NombreDeTermesAllouesDansLaMatriceDesContraintes = NbTermes;
	
}




void OPT_LiberationMemoireDuProblemeAOptimiser( PROBLEME_HEBDO * ProblemeHebdo )
{
int i; PROBLEME_ANTARES_A_RESOUDRE * ProblemeAResoudre;

ProblemeAResoudre = ProblemeHebdo->ProblemeAResoudre;

if (ProblemeAResoudre)
{
	MemFree(ProblemeAResoudre->Sens);
	MemFree(ProblemeAResoudre->IndicesDebutDeLigne);
	MemFree(ProblemeAResoudre->NombreDeTermesDesLignes);
	MemFree(ProblemeAResoudre->CoefficientsDeLaMatriceDesContraintes);
	MemFree(ProblemeAResoudre->IndicesColonnes);
	MemFree(ProblemeAResoudre->CoutQuadratique);
	MemFree(ProblemeAResoudre->CoutLineaire);
	MemFree(ProblemeAResoudre->TypeDeVariable);
	MemFree(ProblemeAResoudre->Xmin);
	MemFree(ProblemeAResoudre->Xmax);
	MemFree(ProblemeAResoudre->X);
	MemFree(ProblemeAResoudre->SecondMembre);
	MemFree(ProblemeAResoudre->AdresseOuPlacerLaValeurDesVariablesOptimisees);
	MemFree(ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsReduits);
	MemFree(ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux);
	MemFree(ProblemeAResoudre->CoutsMarginauxDesContraintes);
	MemFree(ProblemeAResoudre->CoutsReduits);

	if (ProblemeAResoudre->ProblemesSpxDUneClasseDeManoeuvrabilite) {
		for ( i = 0; i < ProblemeHebdo->NombreDeClassesDeManoeuvrabiliteActives; ++i) {
			MemFree(ProblemeAResoudre->ProblemesSpxDUneClasseDeManoeuvrabilite[i]->ProblemeSpx);
			MemFree(ProblemeAResoudre->ProblemesSpxDUneClasseDeManoeuvrabilite[i]);
		}
		MemFree(ProblemeAResoudre->ProblemesSpxDUneClasseDeManoeuvrabilite);
	}
	
	MemFree( ProblemeAResoudre->PositionDeLaVariable );
	MemFree( ProblemeAResoudre->ComplementDeLaBase );
	MemFree( ProblemeAResoudre->Pi );
	MemFree( ProblemeAResoudre->Colonne );

	MemFree( ProblemeAResoudre );

	ProblemeAResoudre = nullptr;
}
return;
}

