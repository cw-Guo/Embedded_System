#include <stdio.h> 
#include <stdlib.h> 
#include <assert.h> 
 
#define BITMAPFILEHEADERLENGTH 14 
#define BM 19778 
 
void bmpfiletest(FILE *fpbmp); 
void bmpheaderpartlength(FILE *fpbmp); 
void bmpwidthheight(FILE *fpbmp); 
void bmpdatapart(FILE *fpbmp); 
void bmpoutput(FILE *fpout); 
 
unsigned int offset=0; 
long width; 
long height; 
unsigned char r[2000][2000],output_r[2000][2000]; 
unsigned char g[2000][2000],output_g[2000][2000]; 
unsigned char b[2000][2000],output_b[2000][2000]; 
 
int main() {  
    unsigned char *fp_temp;  
    FILE *fpbmp;  
    FILE *fpout; 
 
    fpbmp=fopen("1.bmp","rb");  
    if(fpbmp==NULL)  {   
    printf("Open bmp failed!\n");   
    return -1;  
     } 
     
    fpout=fopen("out.bmp","wb+");  
    if(fpout==NULL)  
    {   
         printf("Open out.bmp failed!\n");   
         return -1;  
    }  
    bmpfiletest(fpbmp);  
    bmpheaderpartlength(fpbmp);  
    bmpwidthheight(fpbmp); 
    
    fseek(fpbmp,0L,SEEK_SET);  
    fseek(fpout,0L,SEEK_SET);  
    fp_temp=(unsigned char*)malloc(offset);  
    fread(fp_temp,1,offset,fpbmp);  
    fwrite(fp_temp,1,offset,fpout);  
    bmpdatapart(fpbmp); 
    
    bmpoutput(fpout);  
    fclose(fpbmp);  
    fclose(fpout);  return 0; 
    
} 
 
void bmpoutput(FILE *fpout) { 
    int i=0,j=0;  
    int stride;  
    unsigned char *pixout=NULL;  
    stride=(24*width+31)/8;  
    stride=stride/16;  
    pixout=(unsigned char*)malloc(stride);  
    fseek(fpout,offset,SEEK_SET); 
 
    for(j=0;j<height;j++){   
        for(i=0;i<width;i++){    
            pixout[i*3+2]=output_r[height-j-1][i];    
            pixout[i*3+1]=output_g[height-j-1][i];    
            pixout[i*3+0]=output_b[height-j-1][i];   
        }   
        fwrite(pixout,1,stride,fpout);  
    } 
} 

void bmpdatapart(FILE *fpbmp) { 
    int i=0,j=0; 
    int stride; 
    unsigned char *pix=NULL; 
    FILE *fpr;     
    FILE *fpg;     
    FILE *fpb; 
 
    if((fpr=fopen("bmpr.txt","w+"))==NULL) { 
        printf("Failed to construct file bmpr.txt!\n");       
        assert(-1); 
    }     

    if((fpg=fopen("bmpg.txt","w+"))==NULL) {       
        printf("Failed to construct file bmpg.txt!\n");       
        assert(-1);     
    }     
    
    if((fpb=fopen("bmpb.txt","w+"))==NULL) {       
        printf("Failed to construct file bmpb.txt!\n");       
        assert(-1);  
    } 
 
    fseek(fpbmp,offset,SEEK_SET);     
    stride=(24*width+31)/8; 
    stride=stride/16; 
    pix=(unsigned char*)malloc(stride); 
    for(j=0;j<height;j++) {       
        fread(pix,1,stride,fpbmp);       
        for(i=0;i<width;i++){         
            r[height-j-1][i]=pix[i*3+2];         
            g[height-j-1][i]=pix[i*3+1];         
            b[height-j-1][i]=pix[i*3+0];         
            output_r[height-j-1][i]=pix[i*3+2];         
            output_g[height-j-1][i]=pix[i*3+1];         
            output_b[height-j-1][i]=pix[i*3+0];       
        } 
    } 
    
    for(i=0;i<height;i++) {       
        for(j=0;j<width-1;j++){         
            fprintf(fpb,"%4d",b[i][j]);         
            fprintf(fpg,"%4d",g[i][j]);         
            fprintf(fpr,"%4d",r[i][j]);       
        }       
        fprintf(fpb,"%4d\n",b[i][j]);       
        fprintf(fpg,"%4d\n",g[i][j]);       
        fprintf(fpr,"%4d\n",r[i][j]); 
    }    
    fclose(fpr);  
    fclose(fpg);   
    fclose(fpb); 
} 
 
void bmpfiletest(FILE *fpbmp) { 
    unsigned short bftype=0;     
    fseek(fpbmp,0L,SEEK_SET);     
    fread(&bftype,sizeof(char),2,fpbmp);     
    if(BM!=bftype){        
        printf("Thie file is not bmp file!\n");        
        assert(-1);      
    } 
} 
 
void bmpheaderpartlength(FILE *fpbmp) { 
    fseek(fpbmp,10L,SEEK_SET);     
    fread(&offset,sizeof(char),4,fpbmp);     
    printf("The Header part is of length %d.\n",offset); 
} 
 
void bmpwidthheight(FILE *fpbmp) { 
    fseek(fpbmp,18L,SEEK_SET);     
    fread(&width,sizeof(char),4,fpbmp);     
    fseek(fpbmp,22L,SEEK_SET);     
    fread(&height,sizeof(char),4,fpbmp); 
    printf("The width of the bmp file is %ld.\n",width); 
    printf("The height of the bmp file is %ld.\n",height);

}