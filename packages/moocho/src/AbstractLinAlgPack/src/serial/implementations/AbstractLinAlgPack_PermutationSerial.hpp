// ///////////////////////////////////////////////////////////////////////
// AbstractLinAlgPack_PermutationSerial.hpp
//
// Copyright (C) 2001 Roscoe Ainsworth Bartlett
//
// This is free software; you can redistribute it and/or modify it
// under the terms of the "Artistic License" (see the web site
//   http://www.opensource.org/licenses/artistic-license.html).
// This license is spelled out in the file COPYING.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// above mentioned "Artistic License" for more details.

#ifndef SLAP_PERMUTATION_SERIAL_H
#define SLAP_PERMUTATION_SERIAL_H

#include "AbstractLinAlgPack_VectorSpaceSerial.hpp"
#include "AbstractLinAlgPack_Permutation.hpp"

namespace AbstractLinAlgPack {

///
/** Subclass for permutations of serial vectors.
 *
 * This subclass is really not tied to any specific vector implementation and
 * instead uses the <tt>Vector::get_sub_vector()</tt> and 
 * <tt>VectorMutable::get_sub_vector()</tt> to get explicit views of the
 * elements that are permuted.
 *
 * ToDo: Finish documentation.
 */
class PermutationSerial
	: public AbstractLinAlgPack::Permutation
{
public:

	/** @name Public types */
	//@{

	///
	typedef Teuchos::RefCountPtr<const IVector>  i_vector_ptr_t;

	//@}

	/** @name Constructors / initializers */
	//@{

	///
	/** Calls <tt>this->initialize_identity()</tt>
	 */
	PermutationSerial( size_type dim = 0 );

	///
	/** Calls <tt>this->initialize()</tt>
	 */
	PermutationSerial(
		const i_vector_ptr_t      &perm
		,const i_vector_ptr_t     &inv_perm
		,bool                     allocate_missing_perm = true
		,bool                     check_inv_perm        = false
		);
	
	///
	/** Initialize the identity permutations.
	 *
	 * Postconditions:<ul>
	 * <li> <tt>this->perm().get() == NULL</tt>
	 * <li> <tt>this->inv_perm().get() == NULL</tt>
	 * <li> <tt>this->dim() == dim</tt>
	 * <li> <tt>this->is_identity() == true</tt>
	 * </ul>
	 */
	void initialize_identity( size_type dim );

	///
	/** Initialize given permutation vectors.
	 *
	 * @param  perm [in] Defines the permutation as:
	 *              \verbatim P*x -> y  =>  y(i) = x(perm(i))\endverbatim
	 *              It is allowed for <tt>perm.get() == NULL</tt> in which
	 *              case the permutation in \c this will be defined in terms
	 *              of \c inv_perm (which can't be NULL).
	 * @param  inv_perm
	 *              [in] Defines the permutation as:
	 *              \verbatim P*x -> y  =>  y(inv_perm(i)) = x(i)\endverbatim
	 *              It is allowed for <tt>inv_perm.get() == NULL</tt> in which
	 *              case the permutation in \c this will be defined in terms
	 *              of \c perm (which can't be NULL).
	 * @param  allocate_missing_perm
	 *              [in] If true, then a missing permutation will be allocated
	 *              and initialized.
	 * @param  check_inv_perm
	 *              [in] If <tt>perm.get() != NULL && inv_perm.get() != NULL</tt> and
	 *              <tt>check_inv_perm == true</tt> then, a check is performed to see
	 *              if <tt>*inv_perm</tt> really is the inverse permutation of <tt>*perm</tt>.
	 *              The default value is \c false.
	 *
	 * Preconditions:<ul>
	 * <li> <tt>perm.get() != NULL || inv_perm.get() != NULL</tt> (throw <tt>std::invalid_argument</tt>)
	 * <li> [<tt>perm.get() != NULL && inv_perm.get() != NULL</tt>] <tt>perm->size() == inv_perm->size()</tt>
	 *      (throw <tt>std::invalid_argument</tt>)
	 * <li> [<tt>perm.get() != NULL && inv_perm.get() != NULL</tt>] <tt>*inv_perm</tt> is the inverse permutation
	 *      of <tt>*perm</tt>.  This would be guaranteed if <tt>inv_perm.get()</tt> was initialized as
	 *      <tt>DenseLinAlgPack::inv_perm(*perm,inv_perm.get()</tt>.  This is checked for if <tt>check_inv_perm == true</tt>
	 *      is passed in.  If this check fails, then a <tt>std::invalid_argument</tt> exception is thrown.
	 * </ul>
	 *
	 * Postconditions:<ul>
	 * <li> If <tt>perm.get() != NULL</tt>
	 *      <ul><li> <tt>this->perm().get() == perm.get()</tt></ul>
	 *      else
	 *      <ul>
	 *      <li> If <tt>allocate_missing_perm == true</tt>
	 *           <ul><li><tt>this->perm().get() != NULL</tt></ul>
	 *           else
	 *           <ul><li><tt>this->perm().get() == NULL</tt></ul>
	 *      </ul>
	 * <li> If <tt>inv_perm.get() != NULL</tt>
	 *      <ul><li> <tt>this->inv_perm().get() == inv_perm.get()</tt></ul>
	 *      else
	 *      <ul>
	 *      <li> If <tt>allocate_missing_perm == true</tt>
	 *           <ul><li><tt>this->inv_perm().get() != NULL</tt></ul>
	 *           else
	 *           <ul><li><tt>this->inv_perm().get() == NULL</tt></ul>
	 *      </ul>
	 * <li> <tt>this->dim() == perm.size()</tt>
	 * <li> <tt>this->is_identity() == false</tt>
	 * <li> The behavior of <tt>this->permute()</tt> is defined above.
	 * </ul>
	 *
	 * It may seem strange to define both the permutation and the inverse permutation since only one is needed
	 * to implement the <tt>permute()</tt> methods.  However, clients may need to \c dynamic_cast<> to get at
	 * the <tt>IVector</tt> objects in order to perform other specialized operations.
	 */
	void initialize(
		const i_vector_ptr_t      &perm
		,const i_vector_ptr_t     &inv_perm
		,bool                     allocate_missing_perm = true
		,bool                     check_inv_perm        = false
		);

	//@}

	/** @name Access */
	//@{
	
	///
	/** Get smart pointer to the permutation vector.
	 *
	 * If <tt>this->perm().count() == 1</tt> then <tt>*this->perm()</tt>
	 * is only referenced by \c this.
	 */
	const i_vector_ptr_t& perm() const;
	///
	/** Get smart pointer to the inverse permutation vector.
	 *
	 * If <tt>this->inv_perm().count() == 1</tt> then <tt>*this->inv_perm()</tt>
	 * is only referenced by \c this.
	 */
	const i_vector_ptr_t& inv_perm() const;

	//@}

	/** @name Overridden from Permutation */
	//@{
	
	///
	const VectorSpace& space() const;
	///
	size_type dim() const;
	///
	bool is_identity() const;
	///
	std::ostream& output(std::ostream& out) const;
	///
	void permute( 
		BLAS_Cpp::Transp          P_trans
		,const Vector       &x
		,VectorMutable      *y
		) const;
	///
	void permute( 
		BLAS_Cpp::Transp          P_trans
		,VectorMutable      *y
		) const;

	//@}

private:
	
#ifdef DOXYGEN_COMPILE
	VectorSpaceSerial     space;
	DenseLinAlgPack::IVector   *perm;
	DenseLinAlgPack::IVector   *inv_perm;
#else
	VectorSpaceSerial     space_;
	i_vector_ptr_t        perm_;
	i_vector_ptr_t        inv_perm_;
#endif

}; // end class PermutationSerial

// //////////////////////////////////////////////
// Inline members

inline
const PermutationSerial::i_vector_ptr_t&
PermutationSerial::perm() const
{
	return perm_;
}

inline
const PermutationSerial::i_vector_ptr_t&
PermutationSerial::inv_perm() const
{
	return inv_perm_;
}

} // end namespace AbstractLinAlgPack

#endif // SLAP_PERMUTATION_SERIAL_H
