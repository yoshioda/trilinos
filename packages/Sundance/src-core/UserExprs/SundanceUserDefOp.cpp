/* @HEADER@ */
// ************************************************************************
// 
//                             Sundance
//                 Copyright 2011 Sandia Corporation
// 
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Kevin Long (kevin.long@ttu.edu)
// 

/* @HEADER@ */

#include "SundanceUserDefOp.hpp"
#include "SundanceUserDefOpElement.hpp"
#include "SundanceUserDefFunctor.hpp"
#include "SundanceUserDefFunctorElement.hpp"
#include "PlayaTabs.hpp"
#include "SundanceOut.hpp"

using namespace Sundance;
using namespace Sundance;

using namespace Sundance;
using namespace Teuchos;


UserDefOp::UserDefOp(const Expr& args,
                     const RCP<const UserDefFunctor>& functor)
  : ListExpr()
{
  int nElems = functor->rangeDim();
  Array<RCP<ScalarExpr> > scalarArgs = getScalarArgs(args);

  RCP<Sundance::Map<EvalContext, RCP<const UserDefOpCommonEvaluator> > > 
    commonEvaluatorsMap = rcp(new Sundance::Map<EvalContext, RCP<const UserDefOpCommonEvaluator> >());
  
  for (int i=0; i<nElems; i++)
    {
      RCP<UserDefFunctorElement> e 
        = rcp(new UserDefFunctorElement(functor, i));
      Expr elem = new UserDefOpElement(scalarArgs, commonEvaluatorsMap, e);
      append(elem);
    }
}



Array<RCP<ScalarExpr> > UserDefOp::getScalarArgs(const Expr& args)
{
  Expr fargs = args.flatten();
  Array<RCP<ScalarExpr> > sargs(fargs.size());
  
  for (int i=0; i<fargs.size(); i++)
    {
      sargs[i] = rcp_dynamic_cast<ScalarExpr>(fargs[i].ptr());
    }
  return sargs;
}

