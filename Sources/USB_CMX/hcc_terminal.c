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
#include "hcc_types.h"
#include "hcc_terminal.h"
#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Macro definitions
 *****************************************************************************/
#define MAX_CMDS    10
/*****************************************************************************
 * Local types.
 *****************************************************************************/
/* none */

/*****************************************************************************
 * External references.
 *****************************************************************************/
/* none */

/*****************************************************************************
 * Function predefinitions.
 *****************************************************************************/
static void print_greeting(void);
static void cmd_help(char *param);
static void print_prompt(void);
static int find_command(char *name);

/*****************************************************************************
 * Module variables.
 *****************************************************************************/
static const command_t help_cmd = {
  "help", cmd_help, "Prints help about commands. "
};

static char cmd_line[0x20];
static hcc_u8 cmd_line_ndx;

static hcc_u8 n_cmd;
static command_t * cmds[MAX_CMDS];

static int (*putch)(char);
static int (*getch)(void);
static int (*kbhit)(void);
/*****************************************************************************
 * Name:
 *    print
 * In:
 *    s: string
 * Out:
 *    n/a
 *
 * Description:
 *    Print the specified string.
 * Assumptions:
 *
 *****************************************************************************/
void print(char *s)
{
  while(*s)
  {
    while(*s != (char)putch(*s))
      ;
    s++;
  }
}

/*****************************************************************************
 * Name:
 *    skipp_space
 * In:
 *    cmd_line: string to parse
 *    start: start at offset
 * Out:
 *    index of first non space character
 *
 * Description:
 *
 * Assumptions:
 *
 *****************************************************************************/
int skipp_space(char *cmd_line, int start)
{
  /* Skip leading whitespace. */
  while(cmd_line[start] == ' ' || cmd_line[start] == '\t')
  {
    start++;
  }
  return(start);
}

/*****************************************************************************
 * Name:
 *    find_word
 * In:
 *    cmd_line - pointer to string to be processed
 *    start    - start offset of word
 *
 * Out:
 *    Index of end of word.
 *
 * Description:
 *    Will find the end of a word (first space, tab or end of line).
 *
 * Assumptions:
 *    --
 *****************************************************************************/
int find_word(char *cmd_line, int start)
{
  /* Find end of this word. */
  while(cmd_line[start] != ' ' && cmd_line[start] != '\t'
        && cmd_line[start] != '\n' && cmd_line[start] != '\0')
  {
    start++;
  }

  return(start);
}

/*****************************************************************************
 * Name:
 *    cmp_str
 * In:
 *    a - pointer to string one
 *    b - pointer to string two
 * Out:
 *    0 - strings differ
 *    1 - strings are the same
 * Description:
 *    Compare two strings.
 *
 * Assumptions:
 *    --
 *****************************************************************************/
int cmp_str(char *a, char *b)
{
  int x=0;
  do
  {
    if (a[x] != b[x])
    { 
      return(0);
    }
    x++;
  } while(a[x] != '\0' && b[x] != '\0');

  return(a[x]==b[x] ? 1 : 0);
}

/*****************************************************************************
* Name:
*    cmd_help
* In:
*    param - pointer to string containing parameters
*
* Out:
*    N/A
*
* Description:
*    List supported commands.
*
* Assumptions:
*    --
*****************************************************************************/
static void cmd_help(char *param)
{
  int x;

  param++;
  print("I understand the following commands:\r\n");

  for(x=0; x < n_cmd; x++)
  {
    print("  ");
    print((char *)cmds[x]->txt);
    print(":\t");
    print((char *)cmds[x]->help_txt);
    print("\r\n");
  }
  print("\r\n");
}


/*****************************************************************************
* Name:
*    print_prompt
* In:
*    N/A
*
* Out:
*    N/A
*
* Description:
*    Prints the prompt string.
*
* Assumptions:
*    --
*****************************************************************************/
static void print_prompt(void)
{
  print("\r\n>");
}

/*****************************************************************************
* Name:
*    print_greeting
* In:
*    N/A
*
* Out:
*    N/A
*
* Description:
*    --
*
* Assumptions:
*    --
*****************************************************************************/
static void print_greeting(void)
{
  print("This is the simple terminal version 1.0\r\n");
}

/*****************************************************************************
* Name:
*    find_command
* In:
*    name - pointer to command name string
*
* Out:
*    number - Index of command in "commands" array.
*    -1     - Command not found.
*
* Description:
*    Find a command by its name.
*
* Assumptions:
*    --
*****************************************************************************/
static int find_command(char *name)
{
  int x;
  for(x=0; x < n_cmd; x++)
  { /* If command found, execute it. */
    if (cmp_str(name, (char *) cmds[x]->txt))
    {
      return(x);
    }
  }
  return(-1);
}

/*****************************************************************************
* Name:
*    terminal_init
* In:
*    N/A
*
* Out:
*    N/A
*
* Description:
*    Inicialise the terminal. Set local varaibles to a default value, print
*    greeting message and prompt.
*
* Assumptions:
*    --
*****************************************************************************/
void terminal_init(int (*putch_)(char), int (*getch_)(void), int(*kbhit_)(void))
{
  cmd_line[sizeof(cmd_line)-1]='\0';
  cmd_line_ndx=0;

  cmds[0]=(void *)&help_cmd;
  n_cmd=1;
  
  putch=putch_;
  getch=getch_;
  kbhit=kbhit_;

  print_greeting();
  print_prompt();
}

/*****************************************************************************
* Name:
*    terminal_proces
* In:
*    N/A
*
* Out:
*    N/A
*
* Description:
*    Main loop of terminal application. gathers input, and executes commands.
*
* Assumptions:
*    --
*****************************************************************************/
void terminal_process(void)
{
  while((*kbhit)())
  {
    char c;
    c=(char)(*getch)();
    while(c!=(char)(*putch)(c))
      ;
    if (c=='\r')
    {
      while('\n'!=(char)(*putch)('\n'))
        ;
    }
    /* Execute command if enter is received, or cmd_line is full. */
    if ((c=='\r') || (cmd_line_ndx == sizeof(cmd_line)-2))
    {
      int start=skipp_space(cmd_line, 0);
      int end=find_word(cmd_line, start);
      int x;

      /* Separate command string from parameters, and close
         parameters string. */
      cmd_line[end]=cmd_line[cmd_line_ndx]='\0';

      /* Identify command. */
      x=find_command(cmd_line+start);
      /* Command not found. */
      if (x == -1)
      {
        print("Unknown command!\r\n");
      }
      else
      {
        (*cmds[x]->func)(cmd_line+end+1);
      }
      cmd_line_ndx=0;
      print_prompt();
    }
    else
    { /* Put character to cmd_line. */
      if (c=='\b')
      {
        if (cmd_line_ndx > 0)
        {
          cmd_line[cmd_line_ndx]='\0';
          cmd_line_ndx--;
        }
      }
      else
      {
        cmd_line[cmd_line_ndx++]=c;
      }
    }
  }
}

/*****************************************************************************
* Name:
*    terminal_add_cmd
* In:
*    N/A
*
* Out:
*    N/A
*
* Description:
*    Main loop of terminal application. gathers input, and executes commands.
*
* Assumptions:
*    --
*****************************************************************************/
int terminal_add_cmd(command_t *cmd)
{
  if (n_cmd >= MAX_CMDS)
  {
    return(1);
  }

  cmds[n_cmd]=cmd;
  n_cmd++;
  return(0);
}

/*****************************************************************************
* Name:
*    terminal_delete_cmd
* In:
*    N/A
*
* Out:
*    N/A
*
* Description:
*    Main loop of terminal application. gathers input, and executes commands.
*
* Assumptions:
*    --
*****************************************************************************/
int terminal_delete_cmd(command_t *cmd)
{
  int x;

  for(x=0; x<n_cmd; x++)
  {
    if (cmds[x] == cmd)
    {
      while(x<n_cmd-1)
      {
        cmds[x]=cmds[x+1];
        x++;
      }

      cmds[x]=0;
      n_cmd--;
      
      return(0);
    }
  }
  return(1);
}

#ifdef __cplusplus
}
#endif
/****************************** END OF FILE **********************************/
