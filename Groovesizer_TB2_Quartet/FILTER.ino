// based on Groovuino filter.h http://groovuino.blogspot.tw/

void setFilterCutoff(unsigned char cutoff)
{
  f = cutoff;
  setFeedbackf((int)cutoff);
}

void setFilterResonance(unsigned char resonance)
{
  q = resonance;
  setFeedbackq((int)resonance);
}

void setFilterType(unsigned char filtType)
{
  if (filtType == 0) fType = 0; // LP
  if (filtType == 1) fType = 1; // BP
  if (filtType == 2) fType = 2; // HP
}

inline

int32_t filterNextL(int32_t in)
{
  if (filterBypass)
    return in;
  else
  {
    if (fType == 0)
      in >>= 1; // the lowpass filter seems to need more headroom 
    int32_t ret;
    hp = in - bufL0;
    bp = bufL0 - bufL1;
    bufL0 += fxmul(f,  (hp  + fxmul(fb, bp)));
    bufL1 += fxmul(f, bufL0 - bufL1);
    if (fType == 0) ret = bufL1;
    if (fType == 1) ret = bp;
    if (fType == 2) ret = hp;
    return ret + 2048;
  }
}

/*
inline

int32_t filterNextR(int32_t in)
{
  if (filterBypass)
    return in;
  else
  {
    int32_t ret;
    hp = in - bufR0;
    bp = bufR0 - bufR1;
    bufR0 += fxmul(f,  (hp  + fxmul(fb, bp)));
    bufR1 += fxmul(f, bufR0 - bufR1);
    if (fType == 0) ret = bufR1;
    if (fType == 1) ret = bp;
    if (fType == 2) ret = hp;
    return ret + 2048;
  }
}
*/

void setFeedbackf(long f)
{
  fb = q + fxmul(q, (int)SHIFTED_1 - (f / 128));
}

void setFeedbackq(long q)
{
  fb = q + fxmul(q, (int)SHIFTED_1 - (f / 128));
}

// convert an int into to its fixed representation
inline
long fx(int i)
{
  return (i << FX_SHIFT);
}

inline
long fxmul(long a, int b)
{
  return ((a * b) >> FX_SHIFT);
}
