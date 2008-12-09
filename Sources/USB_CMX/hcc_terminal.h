/****************************************************************************
*
*            Copyright (c) 2006-2007 by CMX Systems, Inc.
*
* This software is copyrighted by and is the sole property of
* CMX.  All rights, title, ownership, or other interests
* in the software remain the property of CMX.  This
* software may only be used in accordance with the corresponding
* license agreement.  Any unauthorized use, duplication, transmission,
* distribution, or disclosure of this software is expressly forbidden.
*
* This Copyright notice may not be removed or modified without prior
* written consent of CMX.
*
* CMX reserves the right to modify this software without notice.
*
* CMX Systems, Inc.
* 12276 San Jose Blvd. #511
* Jacksonville, FL 32223
* USA
*
* Tel:  (904) 880-1840
* Fax:  (904) 880-1632
* http: www.cmx.com
* email: cmx@cmx.com
*
***************************************************************************/

#ifndef _TERMINAL_H_
#define _TERMINAL_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef void (cmd_func)(char *params);

typedef struct {
  const char *txt;
  cmd_func * func;
  const char *help_txt;
} command_t;

extern int terminal_add_cmd(command_t *cmd);
extern int terminal_delete_cmd(command_t *cmd);
extern void terminal_init(int (*putch)(char), int (*getch)(void), int(*kbhit)(void));
extern void terminal_process(void);
extern int skipp_space(char *cmd_line, int start);
extern int find_word(char *cmd_line, int start);
extern int cmp_str(char *a, char *b);
extern void print(char *s);

#ifdef __cplusplus
}
#endif

#endif

/****************************** END OF FILE **********************************/
