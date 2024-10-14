#if !defined(_IntF_)
#define _IntF_

#include "math.h"
//#include "..\common\globals.h"
extern int Interp;
class IntFilter
{
public:
	void Init(float Lev)
	{
		InitIntegr(Lev);
	}
		void InitIntegr(float Lev)
		{
			  FirstPass=true;
			  Diff=0;
			  CurrVal=0;
			  Level=Lev;
			  OldVal=0;
			  Counter=0;
		};
		void Done()
		{
		};
		void DoneIntegr()
		{
			Counter++;
		};
		float Compute(float X)
		{
			  if ( FirstPass ) CurrVal=X;
			  else
			  {
				CurrVal+=Diff;
				if ( fabs(X-OldVal)<Level )
				{
				  Diff=X-OldVal;
				}
				else
					Counter++;
			  }
			  OldVal=X;
			  FirstPass=false;
			  if (!Interp) return X;
			  return CurrVal;
		};
protected:
        bool   FirstPass;    
        float Level;        // поpог отбpаковки
        float Diff;         // последняя пpавильная pазность
        float OldVal;       // пpедыдущее значение сигнала
        float CurrVal;      // текущее значение фильтpованного сигнала
		int Counter;
};



#endif