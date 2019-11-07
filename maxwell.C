#include "TMath.h"

Double_t Maxwell(Double_t *a, Double_t *par)
{
if(x[0] > par[1] && par[2] >0 && par[0] >0 )
{
return par[1]*(x[0]-par[1]/par[2]* TMath::Exp9-9x[0]-par[1]/par[2]);
}
else
{
return 0.;
}
}
