// Support routines for the RAD package (Reverse Automatic Differentiation) --
// a package specialized for function and gradient evaluations.
// Written in 2004 by David M. Gay at Sandia National Labs, Albuquerque, NM.

#include "Sacado_rad.hpp"

//#ifndef SACADO_NO_NAMESPACE
//namespace Sacado {
//#endif

#ifdef RAD_AUTO_AD_Const
ADvari *ADvari::First_ADvari, **ADvari::Last_ADvari = &ADvari::First_ADvari;
#undef RAD_DEBUG_BLOCKKEEP
#else
#ifdef RAD_DEBUG_BLOCKKEEP
#if !(RAD_DEBUG_BLOCKKEEP > 0)
#undef RAD_DEBUG_BLOCKKEEP
#else
extern "C" void _uninit_f2c(void *x, int type, long len);
#define TYDREAL 5
static ADmemblock *rad_Oldcurmb;
static int rad_busy_blocks;
#endif /*RAD_DEBUG_BLOCKKEEP > 0*/
#endif /*RAD_DEBUG_BLOCKKEEP*/
#endif /*RAD_AUTO_AD_Const*/

Derp *Derp::LastDerp = 0;
ADcontext ADvari::adc;
CADcontext ConstADvari::cadc;
ConstADvari *ConstADvari::lastcad;
static int rad_need_reinit;
#ifdef RAD_DEBUG_BLOCKKEEP
static size_t rad_mleft_save;
#endif

const double CADcontext::One = 1., CADcontext::negOne = -1.;

ADcontext::ADcontext()
{
	First.next = 0;
	Busy = &First;
	Free = 0;
	Mbase = (char*)First.memblk;
	Mleft = sizeof(First.memblk);
#ifndef RAD_AUTO_AD_Const
	IVfirst = 0;
	IVnextp = &IVfirst;
#endif
	}

 void*
ADcontext::new_ADmemblock(size_t len)
{
	ADmemblock *mb, *mb0, *mb1, *mbf, *x;
#ifdef RAD_AUTO_AD_Const
	ADvari *a, *anext;
	IndepADvar *v;
#endif /* RAD_AUTO_AD_Const */

	if (rad_need_reinit && this == &ADvari::adc) {
		rad_need_reinit = 0;
		Derp::LastDerp = 0;
#ifdef RAD_DEBUG_BLOCKKEEP
		Mleft = rad_mleft_save;
		if (Mleft < sizeof(First.memblk))
			_uninit_f2c(Mbase + Mleft, TYDREAL,
			 (sizeof(First.memblk) - Mleft)/sizeof(double));
		if (mb = Busy->next) {
			if (!(mb0 = rad_Oldcurmb))
				mb0 = &First;
			for(;; mb = mb->next) {
				_uninit_f2c(mb->memblk, TYDREAL,
					sizeof(First.memblk)/sizeof(double));
				if (mb == mb0)
					break;
				}
			}
		rad_Oldcurmb = Busy;
		if (rad_busy_blocks >= RAD_DEBUG_BLOCKKEEP) {
			rad_busy_blocks = 0;
			rad_Oldcurmb = 0;
			mb0 = &First;
			mbf =  Free;
			for(mb = Busy; mb != mb0; mb = mb1) {
				mb1 = mb->next;
				mb->next = mbf;
				mbf = mb;
				}
			Free = mbf;
			Busy = mb;
			Mbase = (char*)First.memblk;
			Mleft = sizeof(First.memblk);
			*IVnextp = 0;
			IVnextp = &IVfirst;
			}

#else /* !RAD_DEBUG_BLOCKKEEP */

		mb0 = &ADvari::adc.First;
		mbf =  ADvari::adc.Free;
		for(mb = ADvari::adc.Busy; mb != mb0; mb = mb1) {
			mb1 = mb->next;
			mb->next = mbf;
			mbf = mb;
			}
		ADvari::adc.Free = mbf;
		ADvari::adc.Busy = mb;
		ADvari::adc.Mbase = (char*)ADvari::adc.First.memblk;
		ADvari::adc.Mleft = sizeof(ADvari::adc.First.memblk);
#ifdef RAD_AUTO_AD_Const
		*ADvari::Last_ADvari = 0;
		ADvari::Last_ADvari = &ADvari::First_ADvari;
		if (anext = ADvari::First_ADvari) {
			while(a = anext) {
				anext = a->Next;
				if (v = a->padv)
					v->cv = new ADvari(v, a->Val);
				}
			}
#else
		*ADvari::adc.IVnextp = 0;
		ADvari::adc.IVnextp = &ADvari::adc.IVfirst;
#endif /*RAD_AUTO_AD_Const*/
#endif /* RAD_DEBUG_BLOCKKEEP */
		if (Mleft >= len)
			return Mbase + (Mleft -= len);
		}

	if (x = Free)
		Free = x->next;
	else
		x = new ADmemblock;
#ifdef RAD_DEBUG_BLOCKKEEP
	rad_busy_blocks++;
#endif
	x->next = Busy;
	Busy = x;
	return (Mbase = (char*)x->memblk) +
		(Mleft = sizeof(First.memblk) - len);
	}

 void
ADcontext::Gradcomp()
{
	Derp *d;

	if (rad_need_reinit) {
		for(d = Derp::LastDerp; d; d = d->next)
			d->c->aval = 0;
		}
	else {
		rad_need_reinit = 1;
#ifdef RAD_DEBUG_BLOCKKEEP
		rad_mleft_save = ADvari::adc.Mleft;
#endif
		ADvari::adc.Mleft = 0;
		}

	if (d = Derp::LastDerp) {
		d->b->aval = 1;
		do d->c->aval += *d->a * d->b->aval;
		while(d = d->next);
		}
	}

 void
ADcontext::Weighted_Gradcomp(int n, ADvar **v, double *w)
{
	Derp *d;
	int i;

	if (rad_need_reinit) {
		for(d = Derp::LastDerp; d; d = d->next)
			d->c->aval = 0;
		}
	else {
		rad_need_reinit = 1;
#ifdef RAD_DEBUG_BLOCKKEEP
		rad_mleft_save = ADvari::adc.Mleft;
#endif
		ADvari::adc.Mleft = 0;
		}
	if (d = Derp::LastDerp) {
		for(i = 0; i < n; i++)
			v[i]->cv->aval = w[i];
		do d->c->aval += *d->a * d->b->aval;
		while(d = d->next);
		}
	}

#ifdef RAD_AUTO_AD_Const

IndepADvar::IndepADvar(double d)
{
	cv = new ADvari(this,d);
	}

IndepADvar::IndepADvar(int d)
{
	cv = new ADvari(this,d);
	}

IndepADvar::IndepADvar(long d)
{
	cv = new ADvari(this,d);
	}

#else /*!RAD_AUTO_AD_Const*/

 AD_IndepVlist *ADcontext::AD_Indep_vars()
{ return ADvari::adc.IVfirst; }

 void
IndepADvar::AD_Indep(const IndepADvar &v)
{
	AD_IndepVlist *x = (AD_IndepVlist *)ADvari::adc.Memalloc(sizeof(AD_IndepVlist));
	*ADvari::adc.IVnextp = x;
	ADvari::adc.IVnextp = &x->next;
	x->v = (ADvari*)v.cv;
	}

IndepADvar::IndepADvar(double d)
{
	ADvari *x = new ADvari(d);
	cv = x;
	AD_Indep(*(ADvar*)this);
	}

IndepADvar::IndepADvar(int d)
{
	ADvari *x = new ADvari((double)d);
	cv = x;
	AD_Indep(*(ADvar*)this);
	}

IndepADvar::IndepADvar(long d)
{
	ADvari *x = new ADvari((double)d);
	cv = x;
	AD_Indep(*(ADvar*)this);
	}

#endif /*RAD_AUTO_AD_Const*/

 void
ADvar::ADvar_ctr(double d)
{
#ifdef RAD_AUTO_AD_Const
	ADvari *x = new ADvari(this,d);
#else
	ADvari *x = ConstADvari::cadc.fpval_implies_const
		? new ConstADvari(d)
		: new ADvari(d);
#endif
	cv = x;
	}

ConstADvar::ConstADvar()
{
	ConstADvari *x = new ConstADvari(0.);
	cv = x;
	}

 void
ConstADvar::ConstADvar_ctr(double d)
{
	ConstADvari *x = new ConstADvari(d);
	cv = x;
	}
ConstADvar::ConstADvar(ADvari &x)
{
	ConstADvari *y = new ConstADvari(x.Val);
	Derp *d = new Derp(&CADcontext::One, y, &x);
	cv = y;
	}

 void
IndepADvar::AD_Const(const IndepADvar &v)
{
	ConstADvari *ncv = new ConstADvari(v.val());
	((IndepADvar*)&v)->cv = ncv;
	}

 void
ConstADvari::aval_reset()
{
	ConstADvari *x = ConstADvari::lastcad;
	while(x) {
		x->aval = 0;
		x = x->prevcad;
		}
	}

#ifdef RAD_AUTO_AD_Const

ADvari::ADvari(const IndepADvar *x, double d): Val(d), aval(0.)
{
	*Last_ADvari = this;
	Last_ADvari = &Next;
	padv = (IndepADvar*)x;
	}

ADvar1::ADvar1(const IndepADvar *x, const IndepADvar &y):
	ADvari(y.cv->Val), d(&CADcontext::One, this, y.cv)
{
	*ADvari::Last_ADvari = this;
	ADvari::Last_ADvari = &Next;
	padv = (IndepADvar*)x;
	}

ADvar1::ADvar1(const IndepADvar *x, ADvari &y):
	ADvari(y.Val), d(&CADcontext::One, this, &y)
{
	*ADvari::Last_ADvari = this;
	ADvari::Last_ADvari = &Next;
	padv = (IndepADvar*)x;
	}

#else

 ADvar&
ADvar_operatoreq(ADvar *This, const ADvari &x)
{ This->cv = new ADvar1(x.Val, &CADcontext::One, (ADvari*)&x); return *This; }

 IndepADvar&
ADvar_operatoreq(IndepADvar *This, const ADvari &x)
{ This->cv = new ADvar1(x.Val, &CADcontext::One, (ADvari*)&x); return *This; }

#endif /*RAD_AUTO_AD_Const*/

 IndepADvar&
IndepADvar::operator=(double d)
{
#ifdef RAD_AUTO_AD_Const
	if (cv)
		cv->padv = 0;
	cv = new ADvari(this,d);
#else
	cv = new ADvari(d);
	AD_Indep(*this);
#endif
	return *this;
	}

 ADvar&
ADvar::operator=(double d)
{
#ifdef RAD_AUTO_AD_Const
	if (cv)
		cv->padv = 0;
	cv = new ADvari(this,d);
#else
	cv = ConstADvari::cadc.fpval_implies_const
		? new ConstADvari(d)
		: new ADvari(d);
#endif
	return *this;
	}

 ADvari&
operator-(ADvari &T) {
	return *(new ADvar1(-T.Val, &CADcontext::negOne, &T));
	}

 ADvari&
operator+(ADvari &L, ADvari &R) {
	return *(new ADvar2(L.Val + R.Val, &L, &CADcontext::One, &R, &CADcontext::One));
	}

 ADvar&
ADvar::operator+=(ADvari &R) {
	ADvari *Lcv = cv;
#ifdef RAD_AUTO_AD_Const
	Lcv->padv = 0;
#endif
	cv = new ADvar2(Lcv->Val + R.Val, Lcv, &CADcontext::One, &R, &CADcontext::One);
	return *this;
	}

 ADvari&
operator+(ADvari &L, double R) {
	return *(new ADvar1(L.Val + R, &CADcontext::One, &L));
	}

 ADvar&
ADvar::operator+=(double R) {
	ADvari *tcv = cv;
#ifdef RAD_AUTO_AD_Const
	tcv->padv = 0;
#endif
	cv = new ADvar1(tcv->Val + R, &CADcontext::One, tcv);
	return *this;
	}

 ADvari&
operator+(double L, ADvari &R) {
	return *(new ADvar1(L + R.Val, &CADcontext::One, &R));
	}

 ADvari&
operator-(ADvari &L, ADvari &R) {
	return *(new ADvar2(L.Val - R.Val, &L, &CADcontext::One, &R, &CADcontext::negOne));
	}

 ADvar&
ADvar::operator-=(ADvari &R) {
	ADvari *Lcv = cv;
#ifdef RAD_AUTO_AD_Const
	Lcv->padv = 0;
#endif
	cv = new ADvar2(Lcv->Val - R.Val, Lcv, &CADcontext::One, &R, &CADcontext::negOne);
	return *this;
	}

 ADvari&
operator-(ADvari &L, double R) {
	return *(new ADvar1(L.Val - R, &CADcontext::One, &L));
	}

 ADvar&
ADvar::operator-=(double R) {
	ADvari *tcv = cv;
#ifdef RAD_AUTO_AD_Const
	tcv->padv = 0;
#endif
	cv = new ADvar1(tcv->Val - R, &CADcontext::One, tcv);
	return *this;
	}

 ADvari&
operator-(double L, ADvari &R) {
	return *(new ADvar1(L - R.Val, &CADcontext::negOne, &R));
	}

 ADvari&
operator*(ADvari &L, ADvari &R) {
	return *(new ADvar2(L.Val * R.Val, &L, &R.Val, &R, &L.Val));
	}

 ADvar&
ADvar::operator*=(ADvari &R) {
	ADvari *Lcv = cv;
#ifdef RAD_AUTO_AD_Const
	Lcv->padv = 0;
#endif
	cv = new ADvar2(Lcv->Val * R.Val, Lcv, &R.Val, &R, &Lcv->Val);
	return *this;
	}

 ADvari&
operator*(ADvari &L, double R) {
	return *(new ADvar1s(L.Val * R, R, &L));
	}

 ADvar&
ADvar::operator*=(double R) {
	ADvari *Lcv = cv;
#ifdef RAD_AUTO_AD_Const
	Lcv->padv = 0;
#endif
	cv = new ADvar1s(Lcv->Val * R, R, Lcv);
	return *this;
	}

 ADvari&
operator*(double L, ADvari &R) {
	return *(new ADvar1s(L * R.Val, L, &R));
	}

 ADvari&
operator/(ADvari &L, ADvari &R) {
	double Lv = L.Val, Rv = R.Val, pL = 1. / Rv, q = Lv/Rv;
	return *(new ADvar2q(q, pL, -q*pL, &L, &R));
	}

 ADvar&
ADvar::operator/=(ADvari &R) {
	ADvari *Lcv = cv;
#ifdef RAD_AUTO_AD_Const
	Lcv->padv = 0;
#endif
	double Lv = Lcv->Val, Rv = R.Val, pL = 1. / Rv, q = Lv/Rv;
	cv = new ADvar2q(q, pL, -q*pL, Lcv, &R);
	return *this;
	}

 ADvari&
operator/(ADvari &L, double R) {
	return *(new ADvar1s(L.Val / R, 1./R, &L));
	}

 ADvari&
operator/(double L, ADvari &R) {
	double recip = 1. / R.Val;
	double q = L * recip;
	return *(new ADvar1s(q, -q*recip, &R));
	}

 ADvar&
ADvar::operator/=(double R) {
	ADvari *Lcv = cv;
#ifdef RAD_AUTO_AD_Const
	Lcv->padv = 0;
#endif
	cv = new ADvar1s(Lcv->Val / R, 1./R, Lcv);
	return *this;
	}

 ADvari&
atan(ADvari &v) {
	double t = v.Val;
	return *(new ADvar1s(atan(t), 1./(1. + t*t), &v));
	}

 ADvari&
atan2(ADvari &L, ADvari &R) {
	double x = L.Val, y = R.Val, t = x*x + y*y;
	return *(new ADvar2q(atan2(x,y), y/t, -x/t, &L, &R));
	}

 ADvari&
atan2(double x, ADvari &R) {
	double y = R.Val, t = x*x + y*y;
	return *(new ADvar1s(atan2(x,y), -x/t, &R));
	}

 ADvari&
atan2(ADvari &L, double y) {
	double x = L.Val, t = x*x + y*y;
	return *(new ADvar1s(atan2(x,y), y/t, &L));
	}

 ADvari&
cos(ADvari &v) {
	return *(new ADvar1s(cos(v.Val), -sin(v.Val), &v));
	}

 ADvari&
exp(ADvari &v) {
	ADvar1* rcv = new ADvar1(exp(v.Val), &v);
	rcv->d.a = &rcv->Val;
	rcv->d.b = rcv;
	return *rcv;
	}

 ADvari&
log(ADvari &v) {
	double x = v.Val;
	return *(new ADvar1s(log(x), 1. / x, &v));
	}

 ADvari&
pow(ADvari &L, ADvari &R) {
	double x = L.Val, y = R.Val, t = pow(x,y);
	return *(new ADvar2q(t, y*t/x, t*log(x), &L, &R));
	}

 ADvari&
pow(double x, ADvari &R) {
	double t = pow(x,R.Val);
	return *(new ADvar1s(t, t*log(x), &R));
	}

 ADvari&
pow(ADvari &L, double y) {
	double x = L.Val, t = pow(x,y);
	return *(new ADvar1s(t, y*t/x, &L));
	}

 ADvari&
sin(ADvari &v) {
	return *(new ADvar1s(sin(v.Val), cos(v.Val), &v));
	}

 ADvari&
sqrt(ADvari &v) {
	double t = sqrt(v.Val);
	return *(new ADvar1s(t, 0.5/t, &v));
	}

 ADvari&
tan(ADvari &v) {
	double t = cos(v.Val);
	return *(new ADvar1s(tan(v.Val), 1./(t*t), &v));
	}

 ADvari&
fabs(ADvari &v) {	// "fabs" is not the best choice of name,
			// but this name is used at Sandia.
	double t, p;
	p = 1;
	if ((t = v.Val) < 0) {
		t = -t;
		p = -p;
		}
	return *(new ADvar1s(t, p, &v));
	}

 ADvari&
ADf1(double f, double g, ADvari &x) {
	return *(new ADvar1s(f, g, &x));
	}

 ADvari&
ADf2(double f, double gx, double gy, ADvari &x, ADvari &y) {
	return *(new ADvar2q(f, gx, gy, &x, &y));
	}

 ADvarn::ADvarn(double val1, int n1, const ADvar *x, const double *g): n(n1), ADvari(val1)
{
	Derp *d1, *dlast;
	double *a1;
	int i;

	a1 = a = (double*)ADvari::adc.Memalloc(n*sizeof(*a));
	d1 = Da =  (Derp*)ADvari::adc.Memalloc(n*sizeof(Derp));
	dlast = Derp::LastDerp;
	for(i = 0; i < n1; i++, d1++) {
		d1->next = dlast;
		dlast = d1;
		a1[i] = g[i];
		d1->a = &a1[i];
		d1->b = this;
		d1->c = x[i].cv;
		}
	Derp::LastDerp = dlast;
	}

 ADvari&
ADfn(double f, int n, const ADvar *x, const double *g) {
	return *(new ADvarn(f, n, x, g));
	}

//#ifndef SACADO_NO_NAMESPACE
//} /* namespace Sacado */
//#endif
