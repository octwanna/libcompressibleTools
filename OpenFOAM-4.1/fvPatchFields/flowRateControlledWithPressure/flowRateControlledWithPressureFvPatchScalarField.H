/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2013 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::flowRateControlledWithPressureFvPatchScalarField

Group
    grpInletBoundaryConditions grpOutletBoundaryConditions

Description
    This boundary condition provides a total pressure condition.  Four
    variants are possible:

    1. incompressible subsonic:
        \f[
            p_p = p_0 - 0.5 |U|^2
        \f]
        where
        \vartable
            p_p     | incompressible pressure at patch [m2/s2]
            p_0     | incompressible total pressure [m2/s2]
            U       | velocity
        \endvartable

    2. compressible subsonic:
        \f[
            p_p = p_0 - 0.5 \rho |U|^2
        \f]
        where
        \vartable
            p_p     | pressure at patch [Pa]
            p_0     | total pressure [Pa]
            \rho    | density [kg/m3]
            U       | velocity
        \endvartable

    3. compressible transonic (\gamma <= 1):
        \f[
            p_p = \frac{p_0}{1 + 0.5 \psi |U|^2}
        \f]
        where
        \vartable
            p_p     | pressure at patch [Pa]
            p_0     | total pressure [Pa]
            G       | coefficient given by \f$\frac{\gamma}{1-\gamma}\f$
        \endvartable

    4. compressible supersonic (\gamma > 1):
        \f[
            p_p = \frac{p_0}{(1 + 0.5 \psi G |U|^2)^{\frac{1}{G}}}
        \f]
        where
        \vartable
            p_p     | pressure at patch [Pa]
            p_0     | total pressure [Pa]
            \gamma  | ratio of specific heats (Cp/Cv)
            \psi    | compressibility [m2/s2]
            G       | coefficient given by \f$\frac{\gamma}{1-\gamma}\f$
        \endvartable

    The modes of operation are set via the combination of \c phi, \c rho, and
    \c psi entries:
    \table
        Mode                    | phi   | rho   | psi
        incompressible subsonic | phi   | none  | none
        compressible subsonic   | phi   | rho   | none
        compressible transonic  | phi   | none  | psi
        compressible supersonic | phi   | none  | psi
    \endtable


    \heading Patch usage

    \table
        Property     | Description             | Required    | Default value
        U            | velocity field name     | no          | U
        phi          | flux field name         | no          | phi
        rho          | density field name      | no          | none
        psi          | compressibility field name | no       | none
        gamma        | ratio of specific heats (Cp/Cv) | yes |
        p0           | total pressure          | yes       |
    \endtable

    Example of the boundary condition specification:
    \verbatim
    myPatch
    {
        type            flowRateControlledWithPressure;
        U               U;
        phi             phi;
        rho             none;
        psi             none;
        gamma           1.4;
        p0              uniform 1e5;
    }
    \endverbatim

Note
    The default boundary behaviour is for subsonic, incompressible flow.

SeeAlso
    Foam::fixedValueFvPatchField

SourceFiles
    flowRateControlledWithPressureFvPatchScalarField.C

\*---------------------------------------------------------------------------*/

#ifndef flowRateControlledWithPressureFvPatchScalarField_H
#define flowRateControlledWithPressureFvPatchScalarField_H

#include "fixedValueFvPatchFields.H"
#include "Function1.H"
#include "controllerModel.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
              Class flowRateControlledWithPressureFvPatchScalarField Declaration
\*---------------------------------------------------------------------------*/

class flowRateControlledWithPressureFvPatchScalarField
:
    public fixedValueFvPatchScalarField
{
    // Private data

        //- Name of the velocity field
        word UName_;

        //- Name of the flux transporting the field
        word phiName_;

        //- Name of the density field used to normalise the mass flux
        //  if neccessary
        word rhoName_;

        //- Name of the compressibility field used to calculate the wave speed
        word psiName_;

        //- Heat capacity ratio
        scalar gamma_;
        
        //-
        scalar p0_;

        //- Flow rate
        autoPtr<Function1<scalar> > flowRate_;
        
        //-
        autoPtr<Foam::fv::controllerModel> flowController_;


public:

    //- Runtime type information
    TypeName("flowRateControlledWithPressure");


    // Constructors

        //- Construct from patch and internal field
        flowRateControlledWithPressureFvPatchScalarField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&
        );

        //- Construct from patch, internal field and dictionary
        flowRateControlledWithPressureFvPatchScalarField
        (
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const dictionary&
        );

        //- Construct by mapping given flowRateControlledWithPressureFvPatchScalarField
        //  onto a new patch
        flowRateControlledWithPressureFvPatchScalarField
        (
            const flowRateControlledWithPressureFvPatchScalarField&,
            const fvPatch&,
            const DimensionedField<scalar, volMesh>&,
            const fvPatchFieldMapper&
        );

        //- Construct as copy
        flowRateControlledWithPressureFvPatchScalarField
        (
            const flowRateControlledWithPressureFvPatchScalarField&
        );

        //- Construct and return a clone
        virtual tmp<fvPatchScalarField> clone() const
        {
            return tmp<fvPatchScalarField>
            (
                new flowRateControlledWithPressureFvPatchScalarField(*this)
            );
        }

        //- Construct as copy setting internal field reference
        flowRateControlledWithPressureFvPatchScalarField
        (
            const flowRateControlledWithPressureFvPatchScalarField&,
            const DimensionedField<scalar, volMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual tmp<fvPatchScalarField> clone
        (
            const DimensionedField<scalar, volMesh>& iF
        ) const
        {
            return tmp<fvPatchScalarField>
            (
                new flowRateControlledWithPressureFvPatchScalarField(*this, iF)
            );
        }


    // Member functions

        // Access

            //- Return the name of the velocity field
            const word& UName() const
            {
                return UName_;
            }

            //- Return reference to the name of the velocity field
            //  to allow adjustment
            word& UName()
            {
                return UName_;
            }

            //- Return the name of the flux field
            const word& phiName() const
            {
                return phiName_;
            }

            //- Return reference to the name of the flux field
            //  to allow adjustment
            word& phiName()
            {
                return phiName_;
            }

            //- Return the name of the density field
            const word& rhoName() const
            {
                return rhoName_;
            }

            //- Return reference to the name of the density field
            //  to allow adjustment
            word& rhoName()
            {
                return rhoName_;
            }

            //- Return the name of the compressibility field
            const word& psiName() const
            {
                return psiName_;
            }

            //- Return reference to the name of the compressibility field
            //  to allow adjustment
            word& psiName()
            {
                return psiName_;
            }

            //- Return the heat capacity ratio
            scalar gamma() const
            {
                return gamma_;
            }

            //- Return reference to the heat capacity ratio to allow adjustment
            scalar& gamma()
            {
                return gamma_;
            }


        // Mapping functions

            //- Map (and resize as needed) from self given a mapping object
            virtual void autoMap
            (
                const fvPatchFieldMapper&
            );

            //- Reverse map the given fvPatchField onto this fvPatchField
            virtual void rmap
            (
                const fvPatchScalarField&,
                const labelList&
            );


        // Evaluation functions

            //- Update the coefficients associated with the patch field
            //  using the given patch total pressure and velocity fields
            virtual void updateCoeffs
            (
                const scalarField& p0p,
                const vectorField& Up
            );

            //- Update the coefficients associated with the patch field
            virtual void updateCoeffs();


        //- Write
        virtual void write(Ostream&) const;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
