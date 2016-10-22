/*============================================================================
 * Set advanced numerical parameters for the current simulation when the CDO
 * kernel is used
 *============================================================================*/

/* Code_Saturne version 4.2.0 */

/*
  This file is part of Code_Saturne, a general-purpose CFD tool.

  Copyright (C) 1998-2015 EDF S.A.

  This program is free software; you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation; either version 2 of the License, or (at your option) any later
  version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 51 Franklin
  Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

/*----------------------------------------------------------------------------*/

#include "cs_defs.h"

/*----------------------------------------------------------------------------
 * Standard C library headers
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *  Local headers
 *----------------------------------------------------------------------------*/

#include "cs_equation.h"
#include "cs_domain.h"

/*----------------------------------------------------------------------------
 * Header for the current file
 *----------------------------------------------------------------------------*/

#include "cs_prototypes.h"

/*----------------------------------------------------------------------------*/

BEGIN_C_DECLS

/*=============================================================================
 * Additional doxygen documentation
 *============================================================================*/

/*!
  \file cs_user_cdo_numerics.c

  \brief Set advanced parameters about the numerical schemes for each
         equation to solve.
         Useful to change the default behaviour.
*/

/*! \cond DOXYGEN_SHOULD_SKIP_THIS */

/*=============================================================================
 * Local Macro definitions and structure definitions
 *============================================================================*/

/*============================================================================
 * Private user function definitions
 *============================================================================*/

/*============================================================================
 * User function definitions
 *============================================================================*/

/*----------------------------------------------------------------------------*/
/*!
 * \brief  Setup advanced features concerning the way geometric quantities
 *         are built
 *
 * \return the type of computation to evaluate the cell center
 */
/*----------------------------------------------------------------------------*/

cs_cdo_cc_algo_t
cs_user_cdo_geometric_settings(void)
{
  /* Algorithm for computing cell centers */
  /* ==================================== */

  /* Choice between:
     CS_CDO_CC_MEANV:   Cell center is computed as the mean of cell vertices
     CS_CDO_CC_BARYC:   Cell center is computed as the real cell barycenter
     CS_CDO_CC_SATURNE: Cell center is given by Code_Saturne
     CS_CDO_CC_ORTHO:   Cell center is optimized to enforce orthogonality
                        between cell-face edge and face plane
   */

  return CS_CDO_CC_BARYC;
}

/*----------------------------------------------------------------------------*/
/*!
 * \brief  Setup advanced features concerning the numerical parameters
 *         of the equation resolved during the computation
 *
 * \param[in, out]  domain  pointer to a cs_domain_t structure
 */
/*----------------------------------------------------------------------------*/

void
cs_user_cdo_numeric_settings(cs_domain_t   *domain)
{
  return; /* REMOVE_LINE_FOR_USE_OF_SUBROUTINE */

  /* Modify the setting of an equation using a generic process

     ***********  cs_equation_set_option(eq, key, val)   ************

     the couple (key,val) are strings among the following choices:

     KEY = "scheme_space"
     >> val: "cdo_vb" for CDO vertex-based scheme
     >> val: "cdo_fb" for CDO face-based scheme

     KEY = "verbosity"
     >> val: "0" (default), "1", "2", ...
     The higher the more detailed information is displayed
     - "1" detailed setup resume and coarse grain timer stats
     - "2" fine grain timer stats

     KEY = "hodge_diff_algo" or "hodge_time_algo"
     >> val: "voronoi" (default for time), "cost" (default for diffusion)
             or "wbs"
     - "voronoi" leads to diagonal discrete Hodge operator but is not
     consistent for all meshes
     - "cost" is more robust (i.e. it handles more general meshes but is is
     less efficient)
     - "wbs" is robust and accurate but is limited to the reconstruction of
     potential-like degrees of freedom

     KEY = "hodge_diff_coef" or "hodge_time_coef"
     This key is only useful if "cost" is set as algorithm
     >> val: "dga", "sushi", "gcr" or "1.5", "9"..
     val is either a name or a value. Notice that
     - "dga" corresponds to the value 1./3.
     - "sushi" corresponds to the value 1./sqrt(3.)
     - "gcr" corresponds to the value 1.

     KEY = "solver_family"
     >> val: "cs" (default), "petsc", "newton" (not implemented yet)
     For using "petsc" one needs to compile Code_Saturne with the PETSc
     library

     KEY = "itsol"
     >> val: "cg" (default), "bicg", "gmres", "amg"
     - "cg" is the standard conjuguate gradient algorithm
     - "bicg" is BiCG-Stab2 algorithm (for non-symmetric linear systems)
     - "gmres" is a robust iterative solver but not as efficient
     - "amg" is an algebraic multigrid iterative solver

     KEY = "precond"
     >> val: "jacobi", "poly1", "ssor", "ilu0", "icc0", "amg", "as"
     - "jacobi" diagonal preconditoner
     - "poly1"  neumann polynomial of order 1
     - "ssor"   symmetric successive over-relaxation (only with PETSC)
     - "ilu0"   incomplete LU factorization
     - "icc0"   incomplete Cholesky factorization (for symmetric matrices)
     - "amg"    algebraic multigrid

     KEY = "itsol_max_iter"
     >> val: "2000" for instance

     KEY = "itsol_eps"
     >> val:  "1e-10" for instance

     KEY = "itsol_resnorm"
     >> val: "true" or "false"

     Set the type of enforcement of the boundary conditions
     KEY = "bc_enforcement"
        >> val: "strong", "penalization", "nitsche", "sym_nitsche"
     "strong"       remove unknowns attached to a BC
     "penalization" weak enforcement using a huge penalization coefficient
     "weak"         weak enforcement using the Nitsche method
     "weak_sym"     weak enforcement keeping the symmetry of the system

     KEY = "bc_quadrature"
        >> val: "subdiv", "bary", "higher", "highest"
     Set the quadrature algorithm used for evaluating boundary conditions
     "subdiv"  used a subdivision into tetrahedra
     "bary"    used the barycenter approximation
     "higher"  used 4 Gauss points for approximating the integral
     "highest" used 5 Gauss points for approximating the integral

     Remark: "higher" and "highest" implies automatically a subdivision into
     tetrahedra

     Set time scheme:
     KEY = "time_scheme"
     >> val: "implicit", "explicit", "crank_nicolson", "theta_scheme"
     "implicit": first-order in time (inconditionnally stable)
     "explicit":
     "crank_nicolson": second_order in time
     "theta_scheme": generic time scheme. One recovers:
                     - "implicit" with theta = 1,
                     - "explicit" with theta = 0
                     - "crank_nicolson" with theta = 0.5

     KEY = "time_theta" (only useful if "time_scheme" is set to "theta_scheme"
     >> val: "0.75" for instance (must be between 0 <=val<= 1)

     Post-processing options:
     KEY = "post_freq"
     >> val: "10" for instance  ("0" for only the initial state)

     KEY = "post"
     >> val: "peclet", "upwind_coef"
     "peclet" to post-process an estimation of the Peclet number in each cell
     "upwind_coef" to post-process an estimation of the upwinding coefficient
     related a given Peclet number

     Advection options:
     KEY = "adv_weight"
     >> val: "upwind", "centered", "samarskii", "sg", "d10g5"

     KEY = "adv_weight_criterion"
     >> val: "xexc" or "flux"
  */

  cs_equation_t  *eq = cs_domain_get_equation(domain, "FVCA6.1");

  cs_equation_set_option(eq, "space_scheme", "cdo_fb");
  cs_equation_set_option(eq, "verbosity", "2");
  cs_equation_set_option(eq, "hodge_diff_algo", "cost");
  cs_equation_set_option(eq, "hodge_diff_coef", "dga");
  cs_equation_set_option(eq, "solver_family", "petsc");
  cs_equation_set_option(eq, "itsol", "cg");
  cs_equation_set_option(eq, "precond", "amg");
  cs_equation_set_option(eq, "itsol_max_iter", "2500");
  cs_equation_set_option(eq, "itsol_eps", "1e-12");
  cs_equation_set_option(eq, "itsol_resnorm", "false");

}
