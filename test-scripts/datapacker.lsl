vector openedrot = < 90.113922, -27.865593, -0.008368 >;
vector openedpos = < -2.071502, -3.781746, 2.626869 >;
vector closedrot = < -39.037622, 89.589369, 128.926167 >;
vector closedpos = < -2.071502, -3.781746, 2.626869 >;

string hexc = "0123456789ABCDEF";//faster
string Hex  = "0123456789abcdef";

integer UTF8ToInt(string input)
{
   // by Strife
   integer result = llBase64ToInteger(llStringToBase64(input)); //input = llGetSubString(input,0,0)));

   if(result & 0x80000000){
      integer end   = (integer)("0x" + llGetSubString(input = (string)llParseString2List(llEscapeURL(input), (list)"%", []), -8, -1));
      integer begin = (integer)("0x" + llDeleteSubString(input, -8, -1));
      return(((0x0000003f & end) | ((0x00003f00 & end) >> 2) |
              ((0x003f0000 & end) >> 4) | ((0x3f000000 & end) >> 6) |
              ((0x0000003f & begin) << 24) | ((0x00000100 & begin) << 22)) &
             (0x7FFFFFFF >> (5 * ((integer)(llLog(~result) / 0.69314718055994530941723212145818) - 25))));
      }
   return(result >> 24);
}

string IntToUTF8(integer input)  // by Strife
{
   integer bytes = llCeil(llLog(input) / 0.69314718055994530941723212145818);

   bytes = (input >= 0x80) * (bytes + ~(((1 << bytes) - input) > 0)) / 5;
   string result = "%" + byte2hex((input >> (6 * bytes)) | ((0x3F80 >> bytes) << !bytes));
   while(bytes){
         result += "%" + byte2hex((((input >> (6 * (bytes = ~-bytes))) | 0x80) & 0xBF));
         }
   return(llUnescapeURL(result));
}

string byte2hex(integer x)
{
   integer y = (x >> 4) & 0xF;                          // By Strife

   return(llGetSubString(Hex, y, y) + llGetSubString(Hex, x & 0xF, x & 0xF));
}

key Ints2Key(integer a, integer b, integer c, integer d)
{
   return(byte2hex(a >> 24) + byte2hex(a >> 16) + byte2hex(a >> 8) + byte2hex(a) + "-" +
          byte2hex(b >> 24) + byte2hex(b >> 16) + "-" + byte2hex(b >> 8) + byte2hex(b) + "-" +
          byte2hex(c >> 24) + byte2hex(c >> 16) + "-" + byte2hex(c >> 8) + byte2hex(c) +
          byte2hex(d >> 24) + byte2hex(d >> 16) + byte2hex(d >> 8) + byte2hex(d));
}

integer fui(float input) // by Strife
{
   if((input) != 0.0){
      integer sign = (input < 0) << 31;
      if((input = llFabs(input)) < 2.3509887016445750159374730744445e-38){
         return(sign | (integer)(input / 1.4012984643248170709237295832899e-45));
         }
      integer exp = llFloor((llLog(input) / 0.69314718055994530941723212145818));
      return((0x7FFFFF & (integer)(input * (0x1000000 >> sign))) | (((exp + 126 + (sign = ((integer)input - (3 <= (input /= (float)("0x1p" + (string)(exp -= ((exp >> 31) | 1)))))))) << 23) | sign));
      }
   return(((string)input == (string)(-0.0)) << 31);
}

float iuf(integer input) // by Strife
{
   return(llPow(2.0, (input | !input) - 150) * (((!!(input = (0xff & (input >> 23)))) << 23) | ((input & 0x7fffff))) * (1 | (input >> 31)));
}

list Unusable = [0x00, 1, 0x0D, 1, 0x7C, 1, 0x7E, 1, 0xD800, 2047, 0xFDD0, 31, 0xFFFE, 2];

// Unusable: 0000, 000D, D800-DFFF, FDD0-FDEF, FFFE and FFFF.
//              0(1), 13(1), 124(1), 126(1), 55296(2047), 64976(31), 65534(2),
// Reserved for delimiters: '~' and '|'.
// 1-byte: Base123, 2-byte: Base2043, 3-byte: Base1112025

string IntToBaseToUTF8(integer Int, integer Base)
{
   integer Original = Int;
   // Integer to Base
   integer Sign = (Int < 0);

   if(Sign){
      Int = -Int;
      }
   integer M     = Int % (Base / 2);
   list    Coded = [M + (Base / 2) * Sign];

   Int = Int / (Base / 2);
   while(Int > Base){
         Coded = [Int % Base] + Coded;
         Int  /= Base;
         }
   if(Int){
      Coded = [Int] + Coded;
      }

   // Base to UTF8
   integer a;
   integer b = llGetListLength(Coded);
   list    lEnc;
   for(; a < b; ++a){
       Int = llList2Integer(Coded, a);
       integer c;
       integer d = llGetListLength(Unusable);
       for(; c < d; c += 2){
           if(Int >= llList2Integer(Unusable, c)){
              Int += llList2Integer(Unusable, c + 1);
              }
           else{
               c = d;
               }
           }
       lEnc += IntToUTF8(Int);
       }

   if(Base > 60000){
      return(llDumpList2String(lEnc, "|"));
      }
   else{
       return((string)lEnc);
       }
}

integer UTF8ToBaseToInt(string UTF8, integer Base)
{
   // UTF8 to Base
   list lDec;

   if(Base > 60000){
      lDec = llParseString2List(UTF8, ["|"], []);
      }
   else{
       integer h = llStringLength(UTF8);
       while(h--){
             lDec = llGetSubString(UTF8, h, h) + lDec;
             }
       }

   integer i;
   integer j = llGetListLength(lDec);
   list    Coded;
   for(; i < j; ++i){
       string  Char = llList2String(lDec, i);
       integer Int  = UTF8ToInt(Char);
       integer l    = llGetListLength(Unusable) - 1;
       for(; l > 0; l -= 2){
           if(Int >= llList2Integer(Unusable, l - 1)){
              Int -= llList2Integer(Unusable, l);
              }
           }
       Coded += [Int];
       }

   // Base to Integer
   integer Int;
   integer k   = llGetListLength(Coded);
   integer Exp = Base / 2;
   integer Sign;
   Int = llList2Integer(Coded, --k);
   if(Int > Exp){
      Sign = 1;
      Int -= Exp;
      }
   while(k--){
         Int += llList2Integer(Coded, k) * Exp;
         Exp *= Base;
         }
   if(Sign){
      Int = -Int;
      }
   return(Int);
}

string Compress(integer Bytes, list Input)
{
   integer Base;

   if(Bytes == 1){
      Base = 123;
      }
   else
   if(Bytes == 2){
           Base = 2043;
           }
   else
   if(Bytes == 3){
           Base = 1112025;
           }
   else
   if(Bytes == -1){
           Base = 251;        // Use for web with "charset=ISO-8859-1"
           }
   list    Types;
   list    Compressed;
   integer x;
   integer y = llGetListLength(Input);
   for(; x < y; ++x){
       // Get as Integer(s) from List Entry, and compress
       integer Type = llGetListEntryType(Input, x);
       if(Type == TYPE_INTEGER){
          integer Int0 = llList2Integer(Input, x);
          Compressed += [IntToBaseToUTF8(Int0, Base)];
          }
       else if(Type == TYPE_FLOAT){
               integer Int0 = fui(llList2Float(Input, x));
               Compressed += [IntToBaseToUTF8(Int0, Base)];
               }
       else if(Type == TYPE_KEY){
               string  s    = llDumpList2String(llParseString2List(llList2String(Input, x), ["-"], []), "");
               integer Int0 = (integer)("0x" + llGetSubString(s, 0, 7));
               integer Int1 = (integer)("0x" + llGetSubString(s, 8, 15));
               integer Int2 = (integer)("0x" + llGetSubString(s, 16, 23));
               integer Int3 = (integer)("0x" + llGetSubString(s, 24, 31));
               Compressed += [IntToBaseToUTF8(Int0, Base),
                              IntToBaseToUTF8(Int1, Base),
                              IntToBaseToUTF8(Int2, Base),
                              IntToBaseToUTF8(Int3, Base)];
               }
       else if(Type == TYPE_VECTOR){
               vector  v    = llList2Vector(Input, x);
               integer Int0 = fui(v.x);
               integer Int1 = fui(v.y);
               integer Int2 = fui(v.z);
               Compressed += [IntToBaseToUTF8(Int0, Base),
                              IntToBaseToUTF8(Int1, Base),
                              IntToBaseToUTF8(Int2, Base)];
               }
       else if(Type == TYPE_ROTATION){
               rotation v    = llList2Rot(Input, x);
               integer  Int0 = fui(v.x);
               integer  Int1 = fui(v.y);
               integer  Int2 = fui(v.z);
               integer  Int3 = fui(v.s);
               Compressed += [IntToBaseToUTF8(Int0, Base),
                              IntToBaseToUTF8(Int1, Base),
                              IntToBaseToUTF8(Int2, Base),
                              IntToBaseToUTF8(Int3, Base)];
               }
       else if(Type == TYPE_STRING){
               Compressed += llList2String(Input, x);
               }

       // Add to header
       integer Row = x % 10;
       if(!Row){
          Types += Type;
          }
       else{
           integer Col = x / 10;
           integer t   = llList2Integer(Types, Col);
           t     = t | (Type << (Row * 3));
           Types = llListReplaceList(Types, [t], Col, Col);
           }
       }
   // Compress header
   y = llGetListLength((Types = y + Types));
   for(x = 0; x < y; ++x){
       Types = llListReplaceList(Types, [IntToBaseToUTF8(llList2Integer(Types, x), Base)], x, x);
       }

   return(llDumpList2String(Types + Compressed, "~"));
}

list Decompress(integer Bytes, string Encrypted)
{
   integer Base;

   if(Bytes == 1){
      Base = 123;
      }
   else
   if(Bytes == 2){
           Base = 2043;
           }
   else
   if(Bytes == 3){
           Base = 1112025;
           }
   else
   if(Bytes == -1){
           Base = 251;        // Use for web with "charset=ISO-8859-1"
           }
   list Input = llParseString2List(Encrypted, ["~"], []);

   integer Total = UTF8ToBaseToInt(llList2String(Input, 0), Base);
   integer x;
   integer y;
   list    Types;
   for(y = 1 + (Total / 10); x < y; ++x){
       Types += UTF8ToBaseToInt(llList2String(Input, x + 1), Base);
       }

   list    Output;
   integer Ptr = y + 1;
   for(x = 0; x < Total; ++x){
       integer Row  = x % 10;
       integer Col  = x / 10;
       integer Type = (llList2Integer(Types, Col) >> (Row * 3)) & 7;

       if(Type == TYPE_INTEGER){
          Output += [UTF8ToBaseToInt(llList2String(Input, Ptr++), Base)];
          }
       else if(Type == TYPE_FLOAT){
               Output += [iuf(UTF8ToBaseToInt(llList2String(Input, Ptr++), Base))];
               }
       else if(Type == TYPE_KEY){
               integer Int0 = UTF8ToBaseToInt(llList2String(Input, Ptr++), Base);
               integer Int1 = UTF8ToBaseToInt(llList2String(Input, Ptr++), Base);
               integer Int2 = UTF8ToBaseToInt(llList2String(Input, Ptr++), Base);
               integer Int3 = UTF8ToBaseToInt(llList2String(Input, Ptr++), Base);
               Output += [Ints2Key(Int0, Int1, Int2, Int3)];
               }
       else if(Type == TYPE_VECTOR){
               integer Int0 = UTF8ToBaseToInt(llList2String(Input, Ptr++), Base);
               integer Int1 = UTF8ToBaseToInt(llList2String(Input, Ptr++), Base);
               integer Int2 = UTF8ToBaseToInt(llList2String(Input, Ptr++), Base);
               Output += [< iuf(Int0), iuf(Int1), iuf(Int2) >];
               }
       else if(Type == TYPE_ROTATION){
               integer Int0 = UTF8ToBaseToInt(llList2String(Input, Ptr++), Base);
               integer Int1 = UTF8ToBaseToInt(llList2String(Input, Ptr++), Base);
               integer Int2 = UTF8ToBaseToInt(llList2String(Input, Ptr++), Base);
               integer Int3 = UTF8ToBaseToInt(llList2String(Input, Ptr++), Base);
               Output += [< iuf(Int0), iuf(Int1), iuf(Int2), iuf(Int3) >];
               }
       else if(Type == TYPE_STRING){
               Output += [llList2String(Input, Ptr++)];
               }
       }
   return(Output);
}

integer UTF8Length(string msg)  // by kimmie Loveless
{
   integer rNum = llStringLength(msg);

   return(rNum + ((llStringLength(llEscapeURL(msg)) - rNum) / 4));
}

string Float2Hex(float input) // Doubles Unsupported, LSO Safe, Mono Safe
{                             // Copyright Strife Onizuka, 2006-2007, LGPL, http://www.gnu.org/copyleft/lesser.html or (cc-by) http://creativecommons.org/licenses/by/3.0/
   if(input != (integer)input){//LL screwed up hex integers support in rotation & vector string typecasting
      string str = (string)input;
      if(!~llSubStringIndex(str, ".")){
         return(str);                                                                     //NaN and Infinities, it's quick, it's easy.
         }
      float unsigned = llFabs(input);                                                     //logs don't work on negatives.
      integer exponent = llFloor((llLog(unsigned) / 0.69314718055994530941723212145818)); //floor(log2(b)) + rounding error

      integer e2       = (exponent -= ((exponent >> 31) | 1));
      integer mantissa = (integer)((unsigned / (float)("0x1p" + (string)e2)) * 0x4000000);             //shift up into integer range
      integer index    = (integer)(llLog(mantissa & - mantissa) / 0.69314718055994530941723212145818); //index of first 'on' bit
      str      = "." + (string)(exponent + index - 26);
      mantissa = mantissa >> index;
      do {
         str = llGetSubString(hexc, 15 & mantissa, 15 & mantissa) + str;
         } while(mantissa = mantissa >> 4);

      while(llGetSubString(str, 0, 0) == "0"){
            str = llGetSubString(str, 1, -1);
            }

      if(input < 0){
         return("-" + str);
         }
      return(str);
      }//integers pack well so anything that qualifies as an integer we dump as such, supports negative zero
   return(llDeleteSubString((string)input, -7, -1));//trim off the float portion, return an integer
}

string pack_vector(vector v)
{
   return(Float2Hex(v.x) + "%" + Float2Hex(v.y) + "%" + Float2Hex(v.z));
}

string help_prefix(string with, string what)
{
   if(llGetSubString(what, 0, 0) == "-"){
      return("-" + with + llGetSubString(what, 1, -1));
      }
   return(with + what);
}

float Hex2Float(string h)
{
   list   parts = llParseStringKeepNulls(h, ["."], []);
   string transformed;

   if(llGetListLength(parts) == 2){
      transformed = (help_prefix("0x", llList2String(parts, 0)) + "." + help_prefix("0p", llList2String(parts, 1)));
      }
   else{
       transformed = help_prefix("0x", h) + ".";
       }
   llSay(0, "Transformed: " + h + " => " + transformed);
   return((float)transformed);
}

vector unpack_vector(string v)
{
   list e = llParseStringKeepNulls(v, ["%"], []);

   llSay(0, "Length: " + (string)llGetListLength(e));
   if(llGetListLength(e) == 3){
      return(< Hex2Float(llList2String(e, 0)), Hex2Float(llList2String(e, 1)), Hex2Float(llList2String(e, 2)) >);
      }
   return(ZERO_VECTOR);
}

list unpack_data(string d)
{
   list l = llCSV2List(d);

   return( [
              unpack_vector(llList2String(l, 0)),
              unpack_vector(llList2String(l, 1)),
              unpack_vector(llList2String(l, 2)),
              unpack_vector(llList2String(l, 3))
           ]);
}

default
{
   state_entry()
   {
      integer bytes = 1;
      list    data  = [openedpos, openedrot, closedpos, closedrot];
      // list packed_data = [pack_vector(openedpos), pack_vector(openedrot), pack_vector(closedpos), pack_vector(closedrot)];
      string original = llList2CSV(data);

      llSay(0, "Original length: " + (string)llStringLength(original));
      llSay(0, "Original: " + original);
      // string packed = llList2CSV(packed_data);
      string packed = llStringToBase64(Compress(bytes, data));
      llSay(0, "Packed length: " + (string)UTF8Length(packed));
      llSay(0, "Packed:   " + packed);
      llSay(0, "Unpacked: " + llList2CSV(Decompress(bytes, llBase64ToString(packed))));
   }
}
