/*
 * get_sn.h
 *
 *  Created on: 2018Äê11ÔÂ14ÈÕ
 *      Author: ccore-1
 */

#ifndef GET_SN_H_
#define GET_SN_H_

/*
 * serial number length is 8 bytes.
 * so the buff must be 8 bytes at length.
 */

/*
 * Description:
 *    serial number length is 8 bytes.
 *    so the buff must be 8 bytes at least.
 *
 * Input:
 *     @buff: serial number buf, must be 8 bytes at least.
 *
 * Output:
 *     @buff: 8 bytes length sn.
 */
void get_serial_number(unsigned char * buff);

#endif /* GET_SN_H_ */
