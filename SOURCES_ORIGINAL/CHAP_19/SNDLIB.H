
// SNDLIB.H - Header file //////////////////////////////////////////////////

// G L O B A L S  E X T E R N A L  D E C L A R A T I O N S////////////////////

extern char __far *driver_ptr;
extern unsigned version;
extern char __huge *data_ptr;
extern unsigned ct_voice_status;

// P R O T O T Y P E S  /////////////////////////////////////////////////////

void Voc_Get_Version(void);

int Voc_Init_Driver();

int Voc_Terminate_Driver();

void Voc_Set_Port(unsigned int port);

void Voc_Set_Speaker(unsigned int on);

int Voc_Play_Sound(unsigned char far *addr,unsigned char header_length);

int Voc_Stop_Sound(void);

int Voc_Pause_Sound(void);

int Voc_Continue_Sound(void);

int Voc_Break_Sound(void);

void Voc_Set_DMA(unsigned int dma);

void Voc_Set_Status_Addr(char far *status);

void Voc_Load_Driver(void);

char far *Voc_Load_Sound(char *filename, unsigned char *header_length);

void Voc_Unload_Sound(char far *sound_ptr);

