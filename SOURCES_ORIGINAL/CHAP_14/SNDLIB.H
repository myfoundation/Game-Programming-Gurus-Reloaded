
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

Voc_Set_Port(unsigned port);

Voc_Set_Speaker(unsigned on);

int Voc_Play_Sound(unsigned char far *addr,unsigned char header_length);

int Voc_Stop_Sound(void);

int Voc_Pause_Sound(void);

int Voc_Continue_Sound(void);

int Voc_Break_Sound(void);

Voc_Set_DMA(unsigned dma);

Voc_Set_Status_Addr(char __far *status);

Voc_Load_Driver();

char far *Voc_Load_Sound(char *filename, unsigned char *header_length);

void Voc_Unload_Sound(char far *sound_ptr);

