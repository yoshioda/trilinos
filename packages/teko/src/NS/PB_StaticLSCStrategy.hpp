#ifndef __PB_StaticLSCStrategy_hpp__
#define __PB_StaticLSCStrategy_hpp__

#include "NS/PB_LSCStrategy.hpp"

namespace PB {
namespace NS {

class LSCPrecondState; // forward declaration

// constant, not very flexible strategy for driving LSCPreconditioenrFactory
class StaticLSCStrategy : public LSCStrategy {
public:

   // Staiblized constructor
   StaticLSCStrategy(const LinearOp & invF,
                     const LinearOp & invBQBtmC,
                     const LinearOp & invD,
                     const LinearOp & invMass);
 
   // Stable constructor
   StaticLSCStrategy(const LinearOp & invF,
                     const LinearOp & invBQBtmC,
                     const LinearOp & invMass);

   /** This informs the strategy object to build the state associated
     * with this operator.
     *
     * \param[in] A The linear operator to be preconditioned by LSC.
     * \param[in] state State object for storying reusable information about
     *                  the operator A.
     */
   virtual void buildState(BlockedLinearOp & A,BlockPreconditionerState & state) const {}

   /** Get the inverse of the \f$F\f$ block.
     *
     * \param[in] A The linear operator to be preconditioned by LSC.
     * \param[in] state State object for storying reusable information about
     *                  the operator A.
     *
     * \returns An (approximate) inverse of \f$F\f$.
     */
   virtual LinearOp getInvF(const BlockedLinearOp & A,BlockPreconditionerState & state) const
   { return invF_; }

   /** Get the inverse of \f$B Q_u^{-1} B^T\f$. 
     *
     * \param[in] A The linear operator to be preconditioned by LSC.
     * \param[in] state State object for storying reusable information about
     *                  the operator A.
     *
     * \returns An (approximate) inverse of \f$B Q_u^{-1} B^T\f$.
     */
   virtual LinearOp getInvBQBt(const BlockedLinearOp & A,BlockPreconditionerState & state) const
   { return invBQBtmC_; }

   /** Get the inverse for stabilizing the whole schur complement approximation.
     *
     * \param[in] A The linear operator to be preconditioned by LSC.
     * \param[in] state State object for storying reusable information about
     *                  the operator A.
     *
     * \returns The operator to stabilize the whole Schur complement.
     */
   virtual LinearOp getInvD(const BlockedLinearOp & A,BlockPreconditionerState & state) const
   { return invD_; }

   /** Get the inverse mass matrix.
     *
     * \param[in] A The linear operator to be preconditioned by LSC.
     * \param[in] state State object for storying reusable information about
     *                  the operator A.
     *
     * \returns The inverse of the mass matrix \f$Q_u\f$.
     */
   virtual LinearOp getInvMass(const BlockedLinearOp & A,BlockPreconditionerState & state) const
   { return invMass_; }

   /** Should the approximation of the inverse use a full LDU decomposition, or
     * is a upper triangular approximation sufficient.
     *
     * \returns True if the full LDU decomposition should be used, otherwise
     *          only an upper triangular version is used.
     */
   virtual bool useFullLDU() const { return false; }

protected:
   // protected memebers
   LinearOp invF_;
   LinearOp invBQBtmC_;
   LinearOp invD_;
   LinearOp invMass_;
};

} // end namespace NS
} // end namespace PB

#endif