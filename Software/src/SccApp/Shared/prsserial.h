/* Filename:  prsserial.h
 * Author:    Stefan Grimm, 2020-2025
 * Sentence:  Program memory Resource Strings Serial (PRSSerial)
 * Paragraph: Store string resources in the program memory and combine them into messages written to the Serial output.
 * License:   Unlicense (http://unlicense.org/)
 *            Released into the public domain
 */

#ifndef PRSSERIAL_H
#define PRSSERIAL_H

#ifdef __GNUC__
#define NOINLINE __attribute__((optimize("O2")))
 //__attribute__((noinline)) __attribute__((optimize("O2")))
#else
#define NOINLINE 
#endif


 /* Defines a resource string (RS) that is stored in the program memory and used in a message to the user.
  *  Usage: PRSDEFM(RSPRESSANYKEY, "Press any key to continue");
  *
  * To disable message output, use this definition in your code: #define PRSDEFM(N, V) const char* N = 0 */
#ifdef Arduino_h
#define PRSDEFM(N, V) const char N[] PROGMEM = V
#else
#define PRSDEFM(N, V) const char N[] = V
#endif

  /* Defines a resource string (RS) that is stored in the program memory and used as log information.
    *  Usage: PRSDEFL(RSSERICEREADY, "Service ready");
    *
    * To disable information output, use this definition in your code: #define PRSDEFL(N, V) const char* N = 0
    */
#ifdef Arduino_h
#define PRSDEFI(N, V) const char N[] PROGMEM = V
#else
#define PRSDEFI(N, V) const char N[] = V
#endif

    /* The maximal length for the whole serial output.
       * IF exceeded, the application will crash!
       */
#define SPOUTMAXSERIALOUTPUTLENTH 60

char buf[SPOUTMAXSERIALOUTPUTLENTH];

// Trick the compiler so it does not inline the function. This saves program memory.
const char* rs_cat(const char* rs1, const char* rs2, const char* rs3, const char* rs4);
//const char* (*rs_cat_ptr)(const char* rs1, const char* rs2, const char* rs3, const char* rs4) = rs_cat;

/* Writes the given resource string to the serial output buffer.
 *  Usage:
    PRSDEFL(RSHELLO, "Hello");
    PRSDEFL(RSWORLD, "World");
    rs_print(RSHELLO, RSWORLD);
    Output:
    HelloWorld
*/
void rs_print(const char* rs1, const char* rs2 = nullptr, const char* rs3 = nullptr, const char* rs4 = nullptr) {
#ifdef Arduino_h
  //if (rs1) strcpy_P(buf, rs1);
  //else return;
  //if (rs2) strcat_P(buf, rs2);
  //if (rs3) strcat_P(buf, rs3);
  //if (rs4) strcat_P(buf, rs4);
  Serial.print(rs_cat(rs1, rs2, rs3, rs4));
#else
  //if (rs1) strcpy_s(buf, rs1);
  //else return;
  //if (rs2) strcat_s(buf, rs2);
  //if (rs3) strcat_s(buf, rs3);
  //if (rs4) strcat_s(buf, rs4);
  printf(rs_cat(rs1, rs2, rs3, rs4));
#endif
}

/* Writes the given resource strings to the serial output buffer with an end-of-line.
 *  Usage:
    PRSDEFL(RSHELLO, "Hello");
    PRSDEFL(RSWORLD, "World");
    rs_println(RSHELLO, RSWORLD);
    Output:
    HelloWorld
....
*/
void rs_println(const char* rs1, const char* rs2 = nullptr, const char* rs3 = nullptr, const char* rs4 = nullptr) {
#ifdef Arduino_h
  //if (rs1) strcpy_P(buf, rs1);
  //else return;
  //if (rs2) strcat_P(buf, rs2);
  //if (rs3) strcat_P(buf, rs3);
  //if (rs4) strcat_P(buf, rs4);
  Serial.println(rs_cat(rs1, rs2, rs3, rs4));
#else
  //if (rs1) strcpy_s(buf, rs1);
  //else return;
  //if (rs2) strcat_s(buf, rs2);
  //if (rs3) strcat_s(buf, rs3);
  //if (rs4) strcat_s(buf, rs4);
  printf("%s\n", rs_cat(rs1, rs2, rs3, rs4));
#endif
}

NOINLINE const char* rs_cat(const char* rs1, const char* rs2 = nullptr, const char* rs3 = nullptr, const char* rs4 = nullptr) {
#ifdef Arduino_h
  if (rs1) strcpy_P(buf, rs1);
  else return buf;
  if (rs2) strcat_P(buf, rs2);
  if (rs3) strcat_P(buf, rs3);
  if (rs4) strcat_P(buf, rs4);
  return buf;
#else
  if (rs1) strcpy_s(buf, sizeof(buf), rs1);
  else return buf;
  if (rs2) strcat_s(buf, sizeof(buf), rs2);
  if (rs3) strcat_s(buf, sizeof(buf), rs3);
  if (rs4) strcat_s(buf, sizeof(buf), rs4);
  return buf;
#endif
}

#endif