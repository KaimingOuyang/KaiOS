#include <asmapi.h>

static char logo[] =
    "..........*****.....*****...**********...******........******..*****............"
    "............***.....***.........**.........**............**.....**.............."
    "............***.....***.........**.........**............**.....*..............."
    "............***.....***.........**.........**............**....*................"
    "............*.**...*.**.........**.........**............**...**................"
    "............*.**...*.**.........**.........**............**..**................."
    "............*.**...*.**.........**.........**............**..*.................."
    "............*.**...*.**.........**.........**............**.***................."
    "............*.***.*..**.........**.........**............***.**................."
    "............*..**.*..**.........**.........**............**..***................"
    "............*..**.*..**.........**.........**............**...**................"
    "............*..****..**.........**.........**............**....**..............."
    "............*..***...**.........**.........**............**....***.............."
    "............*...**...**.........**.........**........*...**.....**.............."
    "............*...**...**.........**.........**........*...**.....***............."
    "............*...**...**.........**.........**......**....**......**............."
    "..........*****.*..******...**********...************..******...*****...........";


void main(){
    for(int index_1=0;index_1<80*17;index_1++){
        if(logo[index_1] == '.')
            putchar(' ');
        else
            putchar(0x7);
    }
    //while(1);
    return;
}