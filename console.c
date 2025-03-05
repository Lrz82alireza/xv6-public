// Console input and output.
// Input is from the keyboard or serial port.
// Output is written to the screen and serial port.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "traps.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"

const char EXCLAMATION_CHAR = '!';

static void consputc(int);

static int panicked = 0;

static struct {
  struct spinlock lock;
  int locking;
} cons;

// KEY DRIVER CODE
#define KBD_BACKSAPCE 0x08
#define KBD_CTRL_H 0x19

// History buffer
#define INPUT_BUF 128
#define HISTORY_SIZE 10
#define HISTORY_LIMIT 5
typedef struct {
  char buf[HISTORY_SIZE][INPUT_BUF];
  int index;
  int size;
} History;

static History history = {.index = 0, .size = 0};
static History cmd_history = {.index = 0 , .size = 0};

// size_t definition
typedef unsigned int size_t;

// Null declaration
#define NULL ((char*)0)

// Additional functions
char* find_prefix_match();
void clean_console(); //todo


static void
printint(int xx, int base, int sign)
{
  static char digits[] = "0123456789abcdef";
  char buf[16];
  int i;
  uint x;

  if(sign && (sign = xx < 0))
    x = -xx;
  else
    x = xx;

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);

  if(sign)
    buf[i++] = '-';

  while(--i >= 0)
    consputc(buf[i]);
}
//PAGEBREAK: 50

// Print to the console. only understands %d, %x, %p, %s.
void
cprintf(char *fmt, ...)
{
  int i, c, locking;
  uint *argp;
  char *s;

  locking = cons.locking;
  if(locking)
    acquire(&cons.lock);

  if (fmt == 0)
  {
    consputc('N');
    consputc('\n');
    consputc('N');
    panic("null fmt");
  }

  argp = (uint*)(void*)(&fmt + 1);
  for(i = 0; (c = fmt[i] & 0xff) != 0; i++){
    if(c != '%'){
      consputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if(c == 0)
      break;
    switch(c){
    case 'd':
      printint(*argp++, 10, 1);
      break;
    case 'x':
    case 'p':
      printint(*argp++, 16, 0);
      break;
    case 's':
      if((s = (char*)*argp++) == 0)
        s = "(null)";
      for(; *s; s++)
        consputc(*s);
      break;
    case '%':
      consputc('%');
      break;
    default:
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c);
      break;
    }
  }

  if(locking)
    release(&cons.lock);
}

void
panic(char *s)
{
  int i;
  uint pcs[10];

  cli();
  cons.locking = 0;
  // use lapiccpunum so that we can call panic from mycpu()
  cprintf("lapicid %d: panic: ", lapicid());
  cprintf(s);
  cprintf("\n");
  getcallerpcs(&s, pcs);
  for(i=0; i<10; i++)
    cprintf(" %p", pcs[i]);
  panicked = 1; // freeze other CPU
  for(;;)
    ;
}

//PAGEBREAK: 50
#define BACKSPACE 0x100
#define CRTPORT 0x3d4
static ushort *crt = (ushort*)P2V(0xb8000);  // CGA memory

static void
cgaputc(int c)
{
  int pos;

  // Cursor position: col + 80*row.
  outb(CRTPORT, 14);
  pos = inb(CRTPORT+1) << 8;
  outb(CRTPORT, 15);
  pos |= inb(CRTPORT+1);

  if(c == '\n')
    pos += 80 - pos%80;
  else if(c == BACKSPACE){
    if(pos > 0) --pos;
  } else
    crt[pos++] = (c&0xff) | 0x0700;  // black on white

  // Check if pos is out of the screen.
  if(pos < 0 || pos > 25*80)
    panic("pos under/overflow");

  if((pos/80) >= 24){  // Scroll up.
    memmove(crt, crt+80, sizeof(crt[0])*23*80);
    pos -= 80;
    memset(crt+pos, 0, sizeof(crt[0])*(24*80 - pos));
  }

  outb(CRTPORT, 14);
  outb(CRTPORT+1, pos>>8);
  outb(CRTPORT, 15);
  outb(CRTPORT+1, pos);
  crt[pos] = ' ' | 0x0700;
}

void
consputc(int c)
{
  if(panicked){
    cli();
    for(;;)
      ;
  }

  if(c == BACKSPACE){
    uartputc('\b'); uartputc(' '); uartputc('\b');
  } else
    uartputc(c);
  cgaputc(c);
}

struct {
  char buf[INPUT_BUF];
  uint r;  // Read index
  uint w;  // Write index
  uint e;  // Edit index
} input;

#define C(x)  ((x)-'@')  // Control-x

void
consoleintr(int (*getc)(void))
{
  int c, doprocdump = 0;

  acquire(&cons.lock);
  while((c = getc()) >= 0){
    
    switch(c){
    case C('P'):  // Process listing. CTRL+P
      // procdump() locks cons.lock indirectly; invoke later
      doprocdump = 1;
      break;
    case C('U'):  // Kill line. CTRL+U
      while(input.e != input.w &&
            input.buf[(input.e-1) % INPUT_BUF] != '\n'){
        input.e--;
        consputc(BACKSPACE);
      }
      break;
      case KBD_BACKSAPCE: case '\x7f':  // Backspace
      if(input.e != input.w){
        input.e--;
        consputc(BACKSPACE);
      }
      break;
    case KBD_CTRL_H: // CTRL + H. History
      showHistory();
      break;

    case '\t': // Tab
      {
        release(&cons.lock);
        char *result = find_prefix_match();
        
        // Some clear function can go here
        if (result) {
          for (int i = input.e; i > input.w; i--) {
              consputc(BACKSPACE);
          }
          strncpy(input.buf + input.w, result, INPUT_BUF - input.w);
          input.e = input.w + strlen(result);
        //////////////////////////////////////
          cprintf("%s", result);
      }
      }
      acquire(&cons.lock);
      break;


    default:
      if(c != 0 && input.e-input.r < INPUT_BUF){
        c = (c == '\r') ? '\n' : c;
        input.buf[input.e++ % INPUT_BUF] = c;

        consputc(c);

        if(c == '\n' || c == C('D') || input.e == input.r+INPUT_BUF){
          input.w = input.e;
          wakeup(&input.r);
        }
        
        // If the command is finished, save it in history
        if (c == '\n')
        {
          release(&cons.lock);
          saveLastInHistory();
          saveLastInCmdHistory();
          acquire(&cons.lock);
        }
      }
      break;
    }
  }

  release(&cons.lock);
  if(doprocdump) {
    procdump();  // now call procdump() wo. cons.lock held
  }
}

int
consoleread(struct inode *ip, char *dst, int n)
{
  uint target;
  int c;

  iunlock(ip);
  target = n;
  acquire(&cons.lock);
  while(n > 0){
    while(input.r == input.w){
      if(myproc()->killed){
        release(&cons.lock);
        ilock(ip);
        return -1;
      }
      sleep(&input.r, &cons.lock);
    }
    c = input.buf[input.r++ % INPUT_BUF];
    if(c == C('D')){  // EOF
      if(n < target){
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        input.r--;
      }
      break;
    }
    *dst++ = c;
    --n;
    if(c == '\n')
      break;
  }

  release(&cons.lock);
  ilock(ip);

  return target - n;
}

int
consolewrite(struct inode *ip, char *buf, int n)
{
  int i;

  iunlock(ip);
  acquire(&cons.lock);
  for(i = 0; i < n; i++)
    consputc(buf[i] & 0xff);
  release(&cons.lock);
  ilock(ip);

  return n;
}

void
consoleinit(void)
{
  initlock(&cons.lock, "console");

  devsw[CONSOLE].write = consolewrite;
  devsw[CONSOLE].read = consoleread;
  cons.locking = 1;

  ioapicenable(IRQ_KBD, 0);
}

void saveLastInHistory()
{
  // Save the command in history  
  strSplit(history.buf[history.index], input.buf, input.r, input.w - 1);
  history.index = (history.index + 1) % HISTORY_SIZE;

  if (history.size < HISTORY_SIZE)
  {
    history.size++;
  } 

}

void saveLastInCmdHistory()
{
  if (input.buf[input.r] != EXCLAMATION_CHAR)
  {
    strSplit(cmd_history.buf[cmd_history.index], input.buf, input.r, input.w - 1);
    cmd_history.index = (cmd_history.index + 1) % HISTORY_SIZE;

    if (cmd_history.size < HISTORY_SIZE)
    {
      cmd_history.size++;
    } 
  }
}

void showHistory()
{
  release(&cons.lock);
  cprintf("--History BEGIN--\n\n");
  int start = history.size < HISTORY_LIMIT ? 0 : history.size - HISTORY_LIMIT;
  for (int i = history.size - 1; i >= start; i--)
  {
    cprintf("%s\n", history.buf[i]);
  }
  // // show cmd_history
  // cprintf("command history :\n");
  // for (int i = cmd_history.size - 1; i >= 0; i--)
  // {
  //   cprintf("%s\n", cmd_history.buf[i]);
  // }
  // //////////////////////////////////
  cprintf("\n--History END----\n");
  cprintf("$ ");
  acquire(&cons.lock);
}

void strSplit(char *dst, char *src, int start, int end)
{
  int i = 0;
  for (int j = start; j < end; j++)
  {
    dst[i++] = src[j];
  }
  dst[i] = '\0';
}

char* find_prefix_match() {
  char temp[INPUT_BUF];
  strSplit(temp, input.buf, input.w, input.e); 
  int temp_len = strlen(temp);

  // cprintf("Input prefix: %s\n", temp);

  for (int i = cmd_history.size - 1; i >= 0; i--) {
    char* candidate = cmd_history.buf[i];

    // cprintf("Checking candidate: %s\n", candidate);

    if (strncmp(temp, candidate, temp_len) == 0) {
      // cprintf("Match found: %s\n", candidate);
      return candidate;
    }
  }
  return NULL;
}

void clean_console()
{
  //todo
}


