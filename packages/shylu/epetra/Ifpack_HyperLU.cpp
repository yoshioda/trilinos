/** \file Ifpack_HyperLU.cpp

    \brief Use HyperLU as a preconditioner within IFPACK.

    \author Siva Rajamanickam

*/

#include "hyperlu.h"
#include "Ifpack_HyperLU.h"
#include "Teuchos_Time.hpp"

//#define DUMP_MATRICES

Ifpack_HyperLU::Ifpack_HyperLU(Epetra_CrsMatrix* A):
    A_(A),
    IsParallel_(true),
    IsInitialized_(false),
    IsComputed_(false),
    Label_(),
    NumApplyInverse_(0),
    Time_(A_->Comm())
{
}

void Ifpack_HyperLU::Destroy()
{
    if (IsInitialized_)
    {
        //delete A_;
        //delete partitioner_;
        //delete rd_;
    }
    if (IsComputed_)
    {
        delete hlu_data_.LP;
        delete hlu_data_.Solver;
        delete[] hlu_data_.DRowElems;
        delete[] hlu_data_.SRowElems;
    }
}

int Ifpack_HyperLU::Initialize()
{
    if(Comm().NumProc() != 1) 
        IsParallel_ = true;
    else 
        IsParallel_ = false;

    // TODO:
    // Need to enable partitioning here in Initialize once moving to Belos

    // Cannot call this method , need the partitioner around TODO : Can we 
    // avoid this
    //A_ = balanceAndRedistribute(A_, List_);

    // ==================== Symbolic factorization =========================
    // 1. Partition and redistribute [
    //partitioner_ = new Isorropia::Epetra::Partitioner(A_, List_, false);
    //partitioner_->partition();

    //rd_ = new Isorropia::Epetra::Redistributor(partitioner_);
    //Epetra_CrsMatrix *newA;
    //rd_->redistribute(*A_, newA);
    //A_ = newA;
    // ]

    double Sdiagfactor = 0.05; // hard code the diagonals
    /*HyperLU_factor(A_, 1, LP_, Solver_, C_, Dnr_, DRowElems_, Snr_, SRowElems_,
                    Sbar_, Sdiagfactor);*/
    hlu_config_.Sdiagfactor =  Teuchos::getParameter<double>(List_,
                                                "Diagonal Factor");
    hlu_config_.sym =  Teuchos::getParameter<int>(List_,
                                                "Symmetry");
    HyperLU_factor(A_, &hlu_data_, &hlu_config_);

    IsInitialized_ = true;
    return 0;
}

int Ifpack_HyperLU::SetParameters(Teuchos::ParameterList& parameterlist)
{
    List_ = parameterlist;
}

int Ifpack_HyperLU::Compute()
{
    Teuchos::Time ftime("setup time");
    ftime.start();

    libName_ = Teuchos::getParameter<string>(List_,
                                                "Outer Solver Library");
    if (libName_ == "Belos")
    {
        solver_  = new AztecOO() ;
        int err = solver_->SetUserMatrix(hlu_data_.Sbar.get());
        assert (err == 0);
        solver_->SetAztecOption(AZ_solver, AZ_gmres);
        solver_->SetAztecOption(AZ_precond, AZ_dom_decomp);
        solver_->SetAztecOption(AZ_keep_info, 1);
        solver_->SetMatrixName(999);

        double condest;
        err = solver_->ConstructPreconditioner(condest);
        assert (err == 0);
        cout << "Condition number of inner Sbar" << condest << endl;
    }
    else
    {
        // I suspect there is a bug in AztecOO. Doing what we do in the if case
        // here will cause an error when we use the solver in ApplyInverse
        // The error will not happen when we call the dummy JustTryIt() below
        solver_ = NULL;
    }

    ftime.stop();
    cout << "Time to ConstructPreconditioner" << ftime.totalElapsedTime() 
            << endl;
    IsComputed_ = true;
    cout << " Done with the compute" << endl ;
    return 0;
}

int Ifpack_HyperLU::JustTryIt()
{
    // Dummy function, To show the error in AztecOO, This works
    cout << "Entering JustTryIt" << endl;
    //cout << solver_ << endl;
    Epetra_Map BsMap(-1, hlu_data_.Snr, hlu_data_.SRowElems, 0, A_->Comm());
    Epetra_MultiVector Xs(BsMap, 1);
    Epetra_MultiVector Bs(BsMap, 1);
    Xs.PutScalar(0.0);
    solver_->SetLHS(&Xs);
    solver_->SetRHS(&Bs);
    solver_->Iterate(30, 1e-10);
}

int Ifpack_HyperLU::ApplyInverse(const Epetra_MultiVector& X, 
    Epetra_MultiVector& Y) const
{
#ifdef DUMP_MATRICES
    if (NumApplyInverse_ == 0)
    {
        EpetraExt::MultiVectorToMatlabFile("X.mat", X);
        cout << X;
    }
#endif
    cout << "Entering ApplyInvers" << endl;
    int err;
    assert(X.Map().SameAs(Y.Map()));
    assert(X.Map().SameAs(A_->RowMap()));
    const Epetra_MultiVector *newX; 
    newX = &X;
    //rd_->redistribute(X, newX);

    int nvectors = newX->NumVectors();

    // May have to use importer/exporter
    Epetra_Map BsMap(-1, hlu_data_.Snr, hlu_data_.SRowElems, 0, X.Comm());
    Epetra_Map BdMap(-1, hlu_data_.Dnr, hlu_data_.DRowElems, 0, X.Comm());

    Epetra_MultiVector Bs(BsMap, nvectors);
    Epetra_Import BsImporter(BsMap, newX->Map());

    assert(BsImporter.SourceMap().SameAs(newX->Map()));
    assert((newX->Map()).SameAs(BsImporter.SourceMap()));

    Bs.Import(*newX, BsImporter, Insert);
    Epetra_MultiVector Xs(BsMap, nvectors);
    Xs.PutScalar(0.0);

#ifdef DUMP_MATRICES
    if (NumApplyInverse_ == 0 && A_->Comm().MyPID() == 1)
    {
        EpetraExt::MultiVectorToMatlabFile("localXs.mat", localXs);
    }
#endif

    Epetra_LinearProblem Problem(hlu_data_.Sbar.get(), &Xs, &Bs);

#ifdef DUMP_MATRICES
    if (NumApplyInverse_ == 0 )
    {
        EpetraExt::RowMatrixToMatlabFile("Sbar.mat", *(hlu_data_.Sbar.get()));
    }
#endif

    if (libName_ == "Belos")
    {
        solver_->SetLHS(&Xs);
        solver_->SetRHS(&Bs);
    }
    else
    {
        // See the comment above on why we are not able to reuse the solver
        // when outer solve is AztecOO as well.
        solver_ = new AztecOO();
        //solver.SetPrecOperator(precop_);
        solver_->SetAztecOption(AZ_solver, AZ_gmres);
        // Do not use AZ_none
        solver_->SetAztecOption(AZ_precond, AZ_dom_decomp);
        //solver_->SetAztecOption(AZ_precond, AZ_none);
        //solver_->SetAztecOption(AZ_precond, AZ_Jacobi);
        ////solver_->SetAztecOption(AZ_precond, AZ_Neumann);
        //solver_->SetAztecOption(AZ_overlap, 3);
        //solver_->SetAztecOption(AZ_subdomain_solve, AZ_ilu);
        //solver_->SetAztecOption(AZ_output, AZ_all);
        //solver_->SetAztecOption(AZ_diagnostics, AZ_all);
        solver_->SetProblem(Problem);
    }
    solver_->Iterate(30, 1e-10);

    Epetra_MultiVector Bd(BdMap, nvectors);
    Epetra_Import BdImporter(BdMap, newX->Map());
    assert(BdImporter.SourceMap().SameAs(newX->Map()));
    assert((newX->Map()).SameAs(BdImporter.SourceMap()));
    Bd.Import(*newX, BdImporter, Insert);

    Epetra_MultiVector temp(BdMap, nvectors);
    hlu_data_.Cptr->Multiply(false, Xs, temp);
    temp.Update(1.0, Bd, -1.0);

    Epetra_SerialComm LComm;        // Use Serial Comm for the local vectors.
    Epetra_Map LocalBdMap(-1, hlu_data_.Dnr, hlu_data_.DRowElems, 0, LComm);
    Epetra_MultiVector localrhs(LocalBdMap, nvectors);
    Epetra_MultiVector locallhs(LocalBdMap, nvectors);

    int lda;
    double *values;
    err = temp.ExtractView(&values, &lda);
    assert (err == 0);
    int nrows = hlu_data_.Cptr->RowMap().NumMyElements();

    // copy to local vector
    assert(lda == nrows);
    for (int v = 0; v < nvectors; v++)
    {
       for (int i = 0; i < nrows; i++)
       {
           err = localrhs.ReplaceMyValue(i, v, values[i+v*lda]);
           assert (err == 0);
       }
    }

    hlu_data_.LP->SetRHS(&localrhs);
    hlu_data_.LP->SetLHS(&locallhs);
    hlu_data_.Solver->Solve();

    err = locallhs.ExtractView(&values, &lda);
    assert (err == 0);

    // copy to distributed vector
    assert(lda == nrows);
    for (int v = 0; v < nvectors; v++)
    {
       for (int i = 0; i < nrows; i++)
       {
           err = temp.ReplaceMyValue(i, v, values[i+v*lda]);
           assert (err == 0);
       }
    }

    // For checking faults
    //if (NumApplyInverse_ == 5)  temp.ReplaceMyValue(0, 0, 0.0);

    Epetra_Export XdExporter(BdMap, Y.Map());
    Y.Export(temp, XdExporter, Insert);

    Epetra_Export XsExporter(BsMap, Y.Map());
    Y.Export(Xs, XsExporter, Insert);


#ifdef DUMP_MATRICES
    if (NumApplyInverse_ == 0)
    {
        EpetraExt::MultiVectorToMatlabFile("Y.mat", Y);
        cout << Y;
    }
#endif
    NumApplyInverse_++;
    if (libName_ == "Belos")
    {
        // clean up
    }
    else
    {
        delete solver_;
    }
    cout << "Leaving ApplyInvers" << endl;
    return 0;
}

//! Computes the estimated condition number and returns the value.
double Ifpack_HyperLU::Condest(const Ifpack_CondestType CT, 
     const int MaxIters, const double Tol, Epetra_RowMatrix* Matrix_in)
{
    return -1.0;
}

//! Prints on stream basic information about \c this object.
ostream& Ifpack_HyperLU::Print(ostream& os) const
{
    os << " !!!!!!!!! " << endl;
    return os;
}
