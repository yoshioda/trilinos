#include "TSFParameter.h"

#include "TSFError.h"

using namespace TSF;
using namespace std;


TSFParameter::TSFParameter(void) 
	: ptr_(0)
{}


TSFParameter::TSFParameter(const string& label, char parameter)  
	: ptr_(new TSFCharParameter(label, parameter))
{}


TSFParameter::TSFParameter(const string& label, const string& parameter)  
	: ptr_(new TSFCharStringParameter(label, parameter))
{}


TSFParameter::TSFParameter(const string& label, int parameter)  
	: ptr_(new TSFIntParameter(label, parameter))
{}


TSFParameter::TSFParameter(const string& label, const TSFArray<int>& parameter)  
	: ptr_(new TSFIntArrayParameter(label, parameter))
{}


TSFParameter::TSFParameter(const string& label, double parameter)  
	: ptr_(new TSFDoubleParameter(label, parameter))
{}


TSFParameter::TSFParameter(const string& label, const TSFArray<double>& parameter)  
	: ptr_(new TSFDoubleArrayParameter(label, parameter))
{}

void TSFParameter::print(ostream& os) const 
{
  os << getType() << " " << getLabel() << " " << getValueString();
}


