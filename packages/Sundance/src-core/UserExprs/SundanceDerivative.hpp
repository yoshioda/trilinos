/* @HEADER@ */
/* @HEADER@ */

#ifndef SUNDANCE_DERIVATIVE_H
#define SUNDANCE_DERIVATIVE_H

#include "SundanceDefs.hpp"
#include "SundanceScalarExpr.hpp"
#include "SundanceMultiIndex.hpp"
#include "Teuchos_XMLObject.hpp"

namespace SundanceCore
{
  using namespace SundanceUtils;
  using namespace Teuchos;

  using std::string;
  using std::ostream;

  /** */
  class Derivative : public Internal::ScalarExpr
    {
    public:
      /** ctor */
      Derivative(int direction);

      /** virtual destructor */
      virtual ~Derivative() {;}

      /** */
      virtual XMLObject toXML() const ;

#ifndef DOXYGEN_DEVELOPER_ONLY

      /** Indicate whether this expression is a "hungry"
       * differential operator that is awaiting an argument. */
      virtual bool isHungryDiffOp() const {return true;}

      /** */
      virtual ostream& toText(ostream& os, bool paren) const ;

      /** */
      virtual ostream& toLatex(ostream& os, bool paren) const ;

      /** */
      const Internal::MultiIndex& multiIndex() const {return m_;}

      /** */
      virtual RefCountPtr<Internal::ExprBase> getRcp() {return rcp(this);}


    private:
      Internal::MultiIndex m_;

#endif /* DOXYGEN_DEVELOPER_ONLY */
    };
}

#endif
