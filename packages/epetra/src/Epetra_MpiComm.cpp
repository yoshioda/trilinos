
//@HEADER
// ************************************************************************
// 
//               Epetra: Linear Algebra Services Package 
//                 Copyright (2001) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//  
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ************************************************************************
//@HEADER

#include "Epetra_MpiComm.h"

//=============================================================================
Epetra_MpiComm::Epetra_MpiComm(MPI_Comm Comm) : 
	Epetra_Object("Epetra::MpiComm"),
	MpiCommData_(new Epetra_MpiCommData(Comm))
{
}

//=============================================================================
Epetra_MpiComm::Epetra_MpiComm(const Epetra_MpiComm & Comm) : 
  Epetra_Object(Comm.Label()), 
  MpiCommData_(Comm.MpiCommData_)
{
  MpiCommData_->IncrementReferenceCount();
}

//=============================================================================
void Epetra_MpiComm::Barrier() const {
  MPI_Barrier(MpiCommData_->Comm_);
}
//=============================================================================
int Epetra_MpiComm::Broadcast(double * Values, int Count, int Root) const {
  EPETRA_CHK_ERR(CheckInput(Values,Count));
  EPETRA_CHK_ERR(MPI_Bcast(Values, Count, MPI_DOUBLE, Root, MpiCommData_->Comm_));
  return(0);
}
//=============================================================================
int Epetra_MpiComm::Broadcast(int * Values, int Count, int Root) const {
  EPETRA_CHK_ERR(CheckInput(Values,Count));
  EPETRA_CHK_ERR(MPI_Bcast(Values, Count, MPI_INT, Root, MpiCommData_->Comm_));
  return(0);
}
//=============================================================================
int Epetra_MpiComm::Broadcast(long * Values, int Count, int Root) const {
  EPETRA_CHK_ERR(CheckInput(Values,Count));
  EPETRA_CHK_ERR(MPI_Bcast(Values, Count, MPI_LONG, Root, MpiCommData_->Comm_));
  return(0);
}
//=============================================================================
int Epetra_MpiComm::Broadcast(char * Values, int Count, int Root) const {
  EPETRA_CHK_ERR(CheckInput(Values,Count));
  EPETRA_CHK_ERR(MPI_Bcast(Values, Count, MPI_CHAR, Root, MpiCommData_->Comm_));
  return(0);
}
//=============================================================================
int Epetra_MpiComm::GatherAll(double * MyVals, double * AllVals, int Count) const {
  EPETRA_CHK_ERR(CheckInput(MyVals,Count));
  EPETRA_CHK_ERR(CheckInput(AllVals,Count));
  EPETRA_CHK_ERR(MPI_Allgather(MyVals, Count, MPI_DOUBLE, AllVals, Count, MPI_DOUBLE, MpiCommData_->Comm_));
  return(0);
}
//=============================================================================
int Epetra_MpiComm::GatherAll(int * MyVals, int * AllVals, int Count) const {
  EPETRA_CHK_ERR(CheckInput(MyVals,Count));
  EPETRA_CHK_ERR(CheckInput(AllVals,Count));
  EPETRA_CHK_ERR(MPI_Allgather(MyVals, Count, MPI_INT, AllVals, Count, MPI_INT, MpiCommData_->Comm_)); 
  return(0);
}
//=============================================================================
int Epetra_MpiComm::GatherAll(long * MyVals, long * AllVals, int Count) const {
  EPETRA_CHK_ERR(CheckInput(MyVals,Count));
  EPETRA_CHK_ERR(CheckInput(AllVals,Count));
  EPETRA_CHK_ERR(MPI_Allgather(MyVals, Count, MPI_LONG, AllVals, Count, MPI_LONG, MpiCommData_->Comm_)); 
  return(0);
}
//=============================================================================
int Epetra_MpiComm::SumAll(double * PartialSums, double * GlobalSums, int Count) const {
  EPETRA_CHK_ERR(CheckInput(PartialSums,Count));
  EPETRA_CHK_ERR(CheckInput(GlobalSums,Count));
  EPETRA_CHK_ERR(MPI_Allreduce(PartialSums, GlobalSums, Count, MPI_DOUBLE, MPI_SUM, MpiCommData_->Comm_));
  return(0);
}
//=============================================================================
int Epetra_MpiComm::SumAll(int * PartialSums, int * GlobalSums, int Count) const {
  EPETRA_CHK_ERR(CheckInput(PartialSums,Count));
  EPETRA_CHK_ERR(CheckInput(GlobalSums,Count));
  EPETRA_CHK_ERR(MPI_Allreduce(PartialSums, GlobalSums, Count, MPI_INT, MPI_SUM, MpiCommData_->Comm_));
  return(0);
}
//=============================================================================
int Epetra_MpiComm::SumAll(long * PartialSums, long * GlobalSums, int Count) const {
  EPETRA_CHK_ERR(CheckInput(PartialSums,Count));
  EPETRA_CHK_ERR(CheckInput(GlobalSums,Count));
  EPETRA_CHK_ERR(MPI_Allreduce(PartialSums, GlobalSums, Count, MPI_LONG, MPI_SUM, MpiCommData_->Comm_));
  return(0);
}
//=============================================================================
int Epetra_MpiComm::MaxAll(double * PartialMaxs, double * GlobalMaxs, int Count) const {
  EPETRA_CHK_ERR(CheckInput(PartialMaxs,Count));
  EPETRA_CHK_ERR(CheckInput(GlobalMaxs,Count));
  EPETRA_CHK_ERR(MPI_Allreduce(PartialMaxs, GlobalMaxs, Count, MPI_DOUBLE, MPI_MAX, MpiCommData_->Comm_));
  return(0);
}
//=============================================================================
int Epetra_MpiComm::MaxAll(int * PartialMaxs, int * GlobalMaxs, int Count) const {
  EPETRA_CHK_ERR(CheckInput(PartialMaxs,Count));
  EPETRA_CHK_ERR(CheckInput(GlobalMaxs,Count));
  EPETRA_CHK_ERR(MPI_Allreduce(PartialMaxs, GlobalMaxs, Count, MPI_INT, MPI_MAX, MpiCommData_->Comm_));
  return(0);
}
//=============================================================================
int Epetra_MpiComm::MaxAll(long * PartialMaxs, long * GlobalMaxs, int Count) const {
  EPETRA_CHK_ERR(CheckInput(PartialMaxs,Count));
  EPETRA_CHK_ERR(CheckInput(GlobalMaxs,Count));
  EPETRA_CHK_ERR(MPI_Allreduce(PartialMaxs, GlobalMaxs, Count, MPI_LONG, MPI_MAX, MpiCommData_->Comm_));
  return(0);
}
//=============================================================================
int Epetra_MpiComm::MinAll(double * PartialMins, double * GlobalMins, int Count) const {
  EPETRA_CHK_ERR(CheckInput(PartialMins,Count));
  EPETRA_CHK_ERR(CheckInput(GlobalMins,Count));
  EPETRA_CHK_ERR(MPI_Allreduce(PartialMins, GlobalMins, Count, MPI_DOUBLE, MPI_MIN, MpiCommData_->Comm_));
  return(0);
}
//=============================================================================
int Epetra_MpiComm::MinAll(int * PartialMins, int * GlobalMins, int Count) const {
  EPETRA_CHK_ERR(CheckInput(PartialMins,Count));
  EPETRA_CHK_ERR(CheckInput(GlobalMins,Count));
  EPETRA_CHK_ERR(MPI_Allreduce(PartialMins, GlobalMins, Count, MPI_INT, MPI_MIN, MpiCommData_->Comm_));
  return(0);
}
//=============================================================================
int Epetra_MpiComm::MinAll(long * PartialMins, long * GlobalMins, int Count) const {
  EPETRA_CHK_ERR(CheckInput(PartialMins,Count));
  EPETRA_CHK_ERR(CheckInput(GlobalMins,Count));
  EPETRA_CHK_ERR(MPI_Allreduce(PartialMins, GlobalMins, Count, MPI_LONG, MPI_MIN, MpiCommData_->Comm_));
  return(0);
}
//=============================================================================
int Epetra_MpiComm::ScanSum(double * MyVals, double * ScanSums, int Count) const {
  EPETRA_CHK_ERR(CheckInput(MyVals,Count));
  EPETRA_CHK_ERR(CheckInput(ScanSums,Count));
  EPETRA_CHK_ERR(MPI_Scan(MyVals, ScanSums, Count, MPI_DOUBLE, MPI_SUM, MpiCommData_->Comm_));
  return(0);
}
//=============================================================================
int Epetra_MpiComm::ScanSum(int * MyVals, int * ScanSums, int Count) const {
  EPETRA_CHK_ERR(CheckInput(MyVals,Count));
  EPETRA_CHK_ERR(CheckInput(ScanSums,Count));
  EPETRA_CHK_ERR(MPI_Scan(MyVals, ScanSums, Count, MPI_INT, MPI_SUM, MpiCommData_->Comm_));
  return(0);
}
//=============================================================================
int Epetra_MpiComm::ScanSum(long * MyVals, long * ScanSums, int Count) const {
  EPETRA_CHK_ERR(CheckInput(MyVals,Count));
  EPETRA_CHK_ERR(CheckInput(ScanSums,Count));
  EPETRA_CHK_ERR(MPI_Scan(MyVals, ScanSums, Count, MPI_LONG, MPI_SUM, MpiCommData_->Comm_));
  return(0);
}
//=============================================================================
Epetra_Distributor * Epetra_MpiComm:: CreateDistributor() const {

  Epetra_Distributor * dist = new Epetra_MpiDistributor(*this);
  return(dist);
}
//=============================================================================
Epetra_Directory * Epetra_MpiComm:: CreateDirectory(const Epetra_BlockMap & map) const {

  Epetra_Directory * dir = new Epetra_BasicDirectory(map);
  return(dir);
}
//=============================================================================
Epetra_MpiComm::~Epetra_MpiComm()  {
	CleanupData();
}
//=============================================================================
void Epetra_MpiComm::CleanupData() {
	if(MpiCommData_ != 0) {
		MpiCommData_->DecrementReferenceCount();
		if(MpiCommData_->ReferenceCount() == 0) {
			delete MpiCommData_;
			MpiCommData_ = 0;
		}
	}
}
//=============================================================================
Epetra_MpiComm & Epetra_MpiComm::operator= (const Epetra_MpiComm & Comm) {
	if((this != &Comm) && (MpiCommData_ != Comm.MpiCommData_)) {
		CleanupData();
		MpiCommData_ = Comm.MpiCommData_;
		MpiCommData_->IncrementReferenceCount();
	}
	return(*this);
}
