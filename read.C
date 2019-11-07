//=============================================================//
//
// root macro for reading DRSOsc binary files
//
// Stefan Ritt, 2014
// Michal Dragowski, 2018
//
// run with: root -l "read.C(\" path_to_binary_file \")"
// example:  root -l "read.C(\"/media/backup/run111.dat\")"
//
//=============================================================//

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

typedef struct {
   char           tag[3];
   char           version;
} FHEADER;

typedef struct {
   char           time_header[4];
} THEADER;

typedef struct {
   char           bn[2];
   unsigned short board_serial_number;
} BHEADER;

typedef struct {
   char           event_header[4];
   unsigned int   event_serial_number;
   unsigned short year;
   unsigned short month;
   unsigned short day;
   unsigned short hour;
   unsigned short minute;
   unsigned short second;
   unsigned short millisecond;
   unsigned short range;
} EHEADER;

typedef struct {
   char           tc[2];
   unsigned short trigger_cell;
} TCHEADER;

typedef struct {
   char           c[1];
   char           cn[3];
} CHEADER;

//=============================================================//

FHEADER  fh;
THEADER  th;
BHEADER  bh;
EHEADER  eh;
TCHEADER tch;
CHEADER  ch;

unsigned int scaler;
unsigned short voltage[1024];
float bin_width[4][1024];
double waveform[4][1024], timeV[4][1024];
int i, j, b, chn, n, chn_index;

double t1, t2, dt;
int ndt;

int read_headers(FILE* f){
    // file header
    fread(&fh, sizeof(fh), 1, f);
    if (fh.tag[0] != 'D' || fh.tag[1] != 'R' || fh.tag[2] != 'S') {
        printf("Found invalid file header, aborting.\n");
        return 1;
    }
    if (fh.version != '2') {
        printf("Found invalid file version \'%c\', should be \'2\', aborting.\n", fh.version);
        return 1;
    }
    
    // time header
    fread(&th, sizeof(th), 1, f);
    if (memcmp(th.time_header, "TIME", 4) != 0) {
        printf("Invalid time header, aborting.\n");
        return 1;
    }
    
    // board header
    fread(&bh, sizeof(bh), 1, f);
    if (memcmp(bh.bn, "B#", 2) != 0) {
        // probably event header found
        fseek(f, -4, SEEK_CUR);
    }
    printf("Found data for board #%d\n", bh.board_serial_number);
    
    // time bin widths
    for (chn=0 ; chn<5 ; chn++) {
        fread(&ch, sizeof(ch), 1, f);
        if (ch.c[0] != 'C') {
            // event header found
            fseek(f, -4, SEEK_CUR);
            break;
        }
        i = ch.cn[2] - '0' - 1;
        printf("Found timing calibration for channel #%d\n", i+1);
        fread(&bin_width[i][0], sizeof(float), 1024, f);
    }
    
    return 0;
}

int read_event(FILE* f){
    if(n%100000==0)
        std::cout << "Event " << n << "\n";
    
    // read event header
    i = (int)fread(&eh, sizeof(eh), 1, f);
    if (i < 1)
        return 2;
    //printf("Found event #%d %d %d\n", eh.event_serial_number, eh.second, eh.millisecond);
    
    // read board header
    fread(&bh, sizeof(bh), 1, f);
    if (memcmp(bh.bn, "B#", 2) != 0) {
        printf("Invalid board header, aborting.\n");
        return 1;
    }
    
    // read trigger cell
    fread(&tch, sizeof(tch), 1, f);
    if (memcmp(tch.tc, "T#", 2) != 0) {
        printf("Invalid trigger cell header, aborting.\n");
        return 1;
    }
    
    // read channel data
    for (chn=0 ; chn<4 ; chn++) {
        // read channel header
        fread(&ch, sizeof(ch), 1, f);
        if (ch.c[0] != 'C') {
            // event header found
            fseek(f, -4, SEEK_CUR);
            break;
        }
        chn_index = ch.cn[2] - '0' - 1;
        fread(&scaler, sizeof(int), 1, f);
        fread(voltage, sizeof(short), 1024, f);

        waveform[chn_index][0] = (voltage[0] / 65536. + eh.range/1000.0 - 0.5);
        timeV[chn_index][0]=0;
        for (i=1 ; i<1024 ; i++) {
            // convert data to volts
            waveform[chn_index][i] = (voltage[i] / 65536. + eh.range/1000.0 - 0.5);
            // calculate time for this cell
            timeV[chn_index][i] = timeV[chn_index][i-1] + bin_width[chn_index][(i-1+tch.trigger_cell) % 1024];
        }
    }
    
    // align cell #0 of all channels
    t1 = timeV[0][(1024-tch.trigger_cell) % 1024];
    for (chn=1 ; chn<4 ; chn++) {
        t2 = timeV[chn][(1024-tch.trigger_cell) % 1024];
        dt = t1 - t2;
        for (i=0 ; i<1024 ; i++)
            timeV[chn][i] += dt;
    }
    
    return 0;
}

//=============================================================//

int read(string filestring){
    // open the binary file
    const char* filename = filestring.c_str();
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        printf("Cannot find file \'%s\'\n", filename);
        return 1;
    }
    
    if(read_headers(f)==1)
        return 1;
    
    // event loop
    for (n=0 ; ; n++) {
        int status = read_event(f);
        if(status==1)
            return 1;
        else if(status==2)
            break;
        
        //===================================
        //
        // put your analysis code here
        //
        // waveform Y(t) is stored in
        // arrays timeV[][] and waveform[][]
        //
        //===================================
        
 
        
    }
    
    return 0;
}
