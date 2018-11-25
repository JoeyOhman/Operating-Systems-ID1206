#define COLUMNS 80
#define ROWS 25

char *name = "Joey's OS is running hahahohohehe";

typedef struct vga_char {
  char character;
  char colors;
} vga_char;

void myos_main(void) {
  vga_char *vga = (vga_char*)0xb8000;

  for(int i = 0; i < COLUMNS*ROWS; i++) {
    vga[i].character = ' ';
    vga[i].colors = 0x6f;
  }

  for(int i = 0; name[i] != '\0'; i++) {
    vga[600+i].character = name[i];
    vga[600+i].colors = 0x0c;
  }

  return;
}
