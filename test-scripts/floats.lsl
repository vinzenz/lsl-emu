float min = 1.175494351E-38;
float max = 3.402823466E+38;
float sci = 2.6E-5;
float sci_a = 2.6E+3;
float sci_b = 2.6E3;
float sci_c = 26000.E-1;
float f = 2600;//implicitly typecast to a float
float E = 85.34859;
float cast = (float)"42";//explicit typecast to a float
float Infintity = (float)"inf"; //-- may be negative, will cause a math error if evaluated in LSO, see 'caveats' below
float NotANumber = (float)"nan"; //- will cause math error when evaluated in LSO

default {
   state_entry() {}
}
