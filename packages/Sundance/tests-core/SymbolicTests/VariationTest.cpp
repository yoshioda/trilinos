#include "SundanceExpr.hpp"
#include "SundanceStdMathOps.hpp"
#include "SundanceDerivative.hpp"
#include "SundanceUnknownFunctionStub.hpp"
#include "SundanceTestFunctionStub.hpp"
#include "SundanceDiscreteFunctionStub.hpp"
#include "SundanceCoordExpr.hpp"
#include "SundanceZeroExpr.hpp"
#include "SundanceSymbolicTransformation.hpp"
#include "SundanceDeriv.hpp"
#include "SundanceParameter.hpp"
#include "SundanceOut.hpp"
#include "Teuchos_Time.hpp"
#include "Teuchos_MPISession.hpp"
#include "Teuchos_TimeMonitor.hpp"
#include "SundanceDerivSet.hpp"
#include "SundanceRegionQuadCombo.hpp"
#include "SundanceEvalManager.hpp"
#include "SundanceEvalVector.hpp"
#include "SundanceSymbPreprocessor.hpp"
#include "SundanceStringEvalMediator.hpp"

using namespace SundanceUtils;
using SundanceCore::List;
using namespace SundanceCore::Internal;
using namespace Teuchos;
using namespace TSFExtended;

static Time& totalTimer() 
{
  static RefCountPtr<Time> rtn 
    = TimeMonitor::getNewTimer("total"); 
  return *rtn;
}

static Time& doitTimer() 
{
  static RefCountPtr<Time> rtn 
    = TimeMonitor::getNewTimer("doit"); 
  return *rtn;
}



void doit(const Expr& e, 
          const Expr& vars,
          const Expr& varEvalPt,
          const Expr& unks,
          const Expr& unkEvalPt, 
          const Expr& fixed,
          const Expr& fixedEvalPt, 
          const EvalContext& region)
{
  TimeMonitor t0(doitTimer());
  EvalManager mgr;
  mgr.setRegion(region);

  static RefCountPtr<AbstractEvalMediator> mediator 
    = rcp(new StringEvalMediator());

  mgr.setMediator(mediator);

  const EvaluatableExpr* ev 
    = dynamic_cast<const EvaluatableExpr*>(e[0].ptr().get());

  DerivSet d = SymbPreprocessor::setupVariations(e[0], 
                                                 vars,
                                                 varEvalPt,
                                                 unks,
                                                 unkEvalPt,
                                                 fixed,
                                                 fixedEvalPt,
                                                 region);

  Tabs tab;
  //  cerr << tab << *ev->sparsitySuperset(region) << endl;
  //  ev->showSparsity(cerr, region);

  // RefCountPtr<EvalVectorArray> results;

  Array<double> constantResults;
  Array<RefCountPtr<EvalVector> > vectorResults;

  ev->evaluate(mgr, constantResults, vectorResults);

  ev->sparsitySuperset(region)->print(cerr, vectorResults, constantResults);

  
  // results->print(cerr, ev->sparsitySuperset(region).get());
}

void doit(const Expr& e, 
          const Expr& vars,
          const Expr& varEvalPt,
          const Expr& fixed,
          const Expr& fixedEvalPt, 
          const EvalContext& region)
{
  TimeMonitor t0(doitTimer());
  EvalManager mgr;
  mgr.setRegion(region);

  static RefCountPtr<AbstractEvalMediator> mediator 
    = rcp(new StringEvalMediator());

  mgr.setMediator(mediator);

  const EvaluatableExpr* ev 
    = dynamic_cast<const EvaluatableExpr*>(e[0].ptr().get());

  DerivSet d = SymbPreprocessor::setupGradient(e[0], 
                                               vars,
                                               varEvalPt,
                                               fixed,
                                               fixedEvalPt,
                                               region);

  Tabs tab;
  //  cerr << tab << *ev->sparsitySuperset(region) << endl;
  //  ev->showSparsity(cerr, region);

  // RefCountPtr<EvalVectorArray> results;

  Array<double> constantResults;
  Array<RefCountPtr<EvalVector> > vectorResults;

  ev->evaluate(mgr, constantResults, vectorResults);

  ev->sparsitySuperset(region)->print(cerr, vectorResults, constantResults);

  
  // results->print(cerr, ev->sparsitySuperset(region).get());
}



void testExpr(const Expr& e,  
              const Expr& vars,
              const Expr& varEvalPt,
              const Expr& unks,
              const Expr& unkEvalPt, 
              const Expr& fixed,
              const Expr& fixedEvalPt,  
              const EvalContext& region)
{
  cerr << endl 
       << "------------------------------------------------------------- " << endl;
  cerr  << "-------- testing " << e.toString() << " -------- " << endl;
  cerr << endl 
       << "------------------------------------------------------------- " << endl;

  try
    {
      doit(e, vars, varEvalPt, unks, unkEvalPt, fixed, fixedEvalPt, region);
    }
  catch(exception& ex)
    {
      cerr << "EXCEPTION DETECTED!" << endl;
      cerr << ex.what() << endl;
      // cerr << "repeating with increased verbosity..." << endl;
      //       cerr << "-------- testing " << e.toString() << " -------- " << endl;
      //       Evaluator::verbosity() = 2;
      //       EvalVector::verbosity() = 2;
      //       EvaluatableExpr::verbosity() = 2;
      //       Expr::showAllParens() = true;
      //       doit(e, region);
      exit(1);
    }
}

void testExpr(const Expr& e,  
              const Expr& vars,
              const Expr& varEvalPt,
              const Expr& fixed,
              const Expr& fixedEvalPt,  
              const EvalContext& region)
{
  cerr << endl 
       << "------------------------------------------------------------- " << endl;
  cerr  << "-------- testing " << e.toString() << " -------- " << endl;
  cerr << endl 
       << "------------------------------------------------------------- " << endl;

  try
    {
      doit(e, vars, varEvalPt, fixed, fixedEvalPt, region);
    }
  catch(exception& ex)
    {
      cerr << "EXCEPTION DETECTED!" << endl;
      cerr << ex.what() << endl;
      // cerr << "repeating with increased verbosity..." << endl;
      //       cerr << "-------- testing " << e.toString() << " -------- " << endl;
      //       Evaluator::verbosity() = 2;
      //       EvalVector::verbosity() = 2;
      //       EvaluatableExpr::verbosity() = 2;
      //       Expr::showAllParens() = true;
      //       doit(e, region);
      exit(1);
    }
}

int main(int argc, void** argv)
{
  
  try
		{
      MPISession::init(&argc, &argv);

      TimeMonitor t(totalTimer());

      int maxDiffOrder = 2;

      verbosity<SymbolicTransformation>() = VerbSilent;
      //      verbosity<Evaluator>() = VerbExtreme;
      verbosity<EvalVector>() = VerbSilent;
      // verbosity<EvaluatableExpr>() = VerbExtreme;
      Expr::showAllParens() = true;

      EvalVector::shadowOps() = true;

      Expr dx = new Derivative(0);
      Expr dy = new Derivative(1);

			Expr u = new UnknownFunctionStub("u");
			Expr lambda = new UnknownFunctionStub("lambda");
			Expr alpha = new UnknownFunctionStub("alpha");

      cerr << "u=" << u << endl;
      cerr << "lambda=" << lambda << endl;

      Expr x = new CoordExpr(0);
      Expr y = new CoordExpr(1);

      Expr u0 = new DiscreteFunctionStub("u0");
      Expr lambda0 = new DiscreteFunctionStub("lambda0");
      Expr zero = new ZeroExpr();
      Expr alpha0 = new DiscreteFunctionStub("alpha0");

      Array<Expr> tests;



      tests.append(0.5*(u-cos(x))*(u-cos(x)) + 0.5*(dx*alpha)*(dx*alpha) 
                     + exp(alpha)* (dx*lambda)*(dx*u));

      cerr << "STATE EQUATIONS " << endl;
      for (int i=0; i<tests.length(); i++)
        {
          RegionQuadCombo rqc(rcp(new CellFilterStub()), 
                              rcp(new QuadratureFamilyStub(0)));
          EvalContext context(rqc, maxDiffOrder, EvalContext::nextID());
          testExpr(tests[i], 
                   lambda,
                   zero,
                   u,
                   zero,
                   alpha,
                   alpha0,
                   context);
        }
      
      cerr << "ADJOINT EQUATIONS " << endl;
      for (int i=0; i<tests.length(); i++)
        {
          RegionQuadCombo rqc(rcp(new CellFilterStub()), 
                              rcp(new QuadratureFamilyStub(0)));
          EvalContext context(rqc, maxDiffOrder, EvalContext::nextID());
          testExpr(tests[i], 
                   u,
                   u0, 
                   lambda,
                   lambda0,
                   alpha,
                   alpha0,
                   context);
        }

      //   verbosity<Evaluator>() = VerbExtreme;
      //      verbosity<EvaluatableExpr>() = VerbExtreme;
      cerr << "REDUCED GRADIENT " << endl;
      for (int i=0; i<tests.length(); i++)
        {
          RegionQuadCombo rqc(rcp(new CellFilterStub()), 
                              rcp(new QuadratureFamilyStub(0)));
          EvalContext context(rqc, 1, EvalContext::nextID());
          testExpr(tests[i], 
                   alpha, 
                   alpha0,
                   List(u, lambda),
                   List(u0, lambda0),
                   context);
        }

      

    }
	catch(exception& e)
		{
			Out::println(e.what());
		}
  TimeMonitor::summarize();

  MPISession::finalize();
}
