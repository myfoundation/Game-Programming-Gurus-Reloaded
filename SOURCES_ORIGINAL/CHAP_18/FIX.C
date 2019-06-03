
/// I N C L U D E S ///////////////////////////////////////////////////////////

#include <math.h>
#include <stdio.h>

// define our new magical fixed point data type

typedef long fixed;

// F U N C T I O N S //////////////////////////////////////////////////////////

fixed Assign_Integer(long integer)
{

return((fixed)integer << 8);


} // end Assign_Integer

///////////////////////////////////////////////////////////////////////////////

fixed Assign_Float(float number)
{

return((fixed)(number * 256));

} // end Assign_Float

///////////////////////////////////////////////////////////////////////////////

fixed Mul_Fixed(fixed f1,fixed f2)
{

return((f1*f2) >> 8);

} // end Mul_Fixed

///////////////////////////////////////////////////////////////////////////////

fixed Add_Fixed(fixed f1,fixed f2)
{

return(f1+f2);

} // end Add_Fixed

///////////////////////////////////////////////////////////////////////////////

Print_Fixed(fixed f1)
{

printf("%ld.%ld",f1 >> 8, 100*(unsigned long)(f1 & 0x00ff)/256);

} // end Print_Fixed


//M A I N //////////////////////////////////////////////////////////////////////


main()
{


fixed f1,f2,f3;

f1 = Assign_Float(15);
f2 = Assign_Float(233.45);

f3 = Mul_Fixed(f1,f2);

printf("\nf1:");
Print_Fixed(f1);

printf("\nf2:");
Print_Fixed(f2);

printf("\nf3:");
Print_Fixed(f3);


} // end main

