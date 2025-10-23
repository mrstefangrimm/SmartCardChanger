/* Filename:  prsserial.h
 * Author:    Stefan Grimm, 2020-2025
 * Sentence:  Program memory Resource Strings Serial (PRSSerial)
 * Paragraph: Store string resources in the program memory and combine them into messages written to the Serial output.
 * License:   Unlicense (http://unlicense.org/)
 *            Released into the public domain
 */

#ifndef PRSSERIAL_H
#define PRSSERIAL_H


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
  if (rs1) Serial.print(reinterpret_cast<const __FlashStringHelper*>(rs1));
  else return;
  if (rs2) Serial.print(reinterpret_cast<const __FlashStringHelper*>(rs2));
  if (rs3) Serial.print(reinterpret_cast<const __FlashStringHelper*>(rs3));
  if (rs4) Serial.print(reinterpret_cast<const __FlashStringHelper*>(rs4));
#else
  if (rs1) printf(rs1);
  else return;
  if (rs2) printf(rs2);
  if (rs3) printf(rs3);
  if (rs4) printf(rs4);
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
  rs_print(rs1, rs2, rs3, rs4);
#ifdef Arduino_h
  Serial.println();
#else
  printf("\n");
#endif
}

#endif