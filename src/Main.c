#include "C:/Wichtig/System/Static/Library/Math.h"
#include "C:/Wichtig/System/Static/Library/Random.h"
#include "C:/Wichtig/System/Static/Container/PVector.h"
#include "C:/Wichtig/System/Static/Library/WindowEngine1.0.h"

#define VWINDOW_NONE        0
#define VWINDOW_GRAPPED     1
#define VWINDOW_TOP         2
#define VWINDOW_BOTTOM      3
#define VWINDOW_LEFT        4
#define VWINDOW_RIGHT       5

typedef struct VWindow {
    char* Title;
    Vec2 p;
    Pixel* SwapBuffer;
    Sprite Context;

    Vec2 pBefore;
    Vec2 dBefore;
    unsigned char Icon;
    long long TimeLifted;

    struct {
        char Min    : 1;
        char Max    : 1;
        char Focus  : 1;
    };
} VWindow;

VWindow VNew(char* Title,Vec2 p,Vec2 d){
    VWindow w;
    w.Title = CStr_Cpy(Title);
    w.p = p;
    w.Context = Sprite_Init(d.x,d.y);
    w.SwapBuffer = (Pixel*)calloc((int)(d.x * d.y),sizeof(Pixel));
    w.pBefore = (Vec2){ 0.0f,0.0f };
    w.dBefore = (Vec2){ 0.0f,0.0f };
    w.TimeLifted = 0LL;
    w.Min = FALSE;
    w.Max = FALSE;
    w.Focus = TRUE;
    w.Icon = 5;
    return w;
}

void VResize(VWindow* w,Vec2 d){
    w->Context.w = d.x;
    w->Context.h = d.y;
    if(w->Context.img) free(w->Context.img);
    w->Context.img = (Pixel*)calloc((int)(d.x * d.y),sizeof(Pixel));
    w->SwapBuffer = (Pixel*)calloc((int)(d.x * d.y),sizeof(Pixel));
}

void VSetTitle(VWindow* w,char* Title){
    if(w->Title) free(w->Title);
    w->Title = CStr_Cpy(Title);
}

void VSwap(VWindow* w){
    memcpy(w->Context.img,w->SwapBuffer,w->Context.w * w->Context.h);
}

void VRender(VWindow* w,Font* f,Font* I){
    if(!w->Min){
        RenderRect(w->p.x,w->p.y,w->Context.w,0.02f * GetHeight(),LIGHT_GRAY);
        RenderCircle(((Vec2){w->p.x + w->Context.w - 0.05f * GetHeight(),w->p.y + 0.01f * GetHeight()}),0.01f * GetHeight(),GREEN);
        RenderCircle(((Vec2){w->p.x + w->Context.w - 0.03f * GetHeight(),w->p.y + 0.01f * GetHeight()}),0.01f * GetHeight(),YELLOW);
        RenderCircle(((Vec2){w->p.x + w->Context.w - 0.01f * GetHeight(),w->p.y + 0.01f * GetHeight()}),0.01f * GetHeight(),RED);
        RenderSprite(&w->Context,w->p.x,w->p.y + 0.02f * GetHeight());

        RenderCStrFont(f,w->Title,w->p.x+I->CharSizeX,w->p.y,BLACK);
        RenderSubSpriteAlpha(&I->Atlas,w->p.x,w->p.y,(w->Icon % I->Columns) * I->CharSizeX,(w->Icon / I->Columns) * I->CharSizeY,I->CharSizeX,I->CharSizeY);
    }
}

void VFree(VWindow* w){
    if(w->Title)    free(w->Title);
    Sprite_Free(&w->Context);
    free(w->SwapBuffer);
}

void VHandleInput(PVector* VWindows,VWindow** Focused,char* Picked,Vec2* Offset,States Input){
    if(Input.PRESSED){
        if((*Focused)) (*Focused)->Focus = FALSE;
        (*Focused) = NULL;
        *Picked = VWINDOW_NONE;
        *Offset = (Vec2){0.0f,0.0f};
        
        Vec2 Mouse = (Vec2){GetMouse().x,GetMouse().y};
        for(int i = VWindows->size-1;i>=0;i--){
            VWindow* w = (VWindow*)PVector_Get(VWindows,i);
            if(Overlap_Rect_Point((Rect){Vec2_Add(w->p,(Vec2){0.0f,-0.01f * GetHeight()}),(Vec2){w->Context.w,w->Context.h + 0.03f * GetHeight()}},Mouse)){
                (*Focused) = w;
                (*Focused)->Focus = TRUE;
                (*Focused)->TimeLifted = Time_Nano();
                
                if(Overlap_Rect_Point((Rect){Vec2_Add(w->p,(Vec2){w->Context.w - 0.06f * GetHeight(),0.0f}),(Vec2){0.02f * GetHeight(),0.02f * GetHeight()}},Mouse)){
                    (*Focused)->Min = TRUE;
                }else if(Overlap_Rect_Point((Rect){Vec2_Add(w->p,(Vec2){w->Context.w - 0.04f * GetHeight(),0.0f}),(Vec2){0.02f * GetHeight(),0.02f * GetHeight()}},Mouse)){
                    (*Focused)->Max = !(*Focused)->Max;
                    if(!(*Focused)->Max){
                        w->p = w->pBefore;
                        VResize(w,w->dBefore);
                        w->pBefore = (Vec2){0.0f,0.0f};
                        w->dBefore = (Vec2){0.0f,0.0f};
                    }else{
                        w->pBefore = w->p;
                        w->dBefore = (Vec2){w->Context.w,w->Context.h};
                        w->p = (Vec2){0.0f,0.0f};
                        VResize(w,(Vec2){GetWidth(),GetHeight()-0.02f * GetHeight()});
                    }
                }else if(Overlap_Rect_Point((Rect){Vec2_Add(w->p,(Vec2){w->Context.w - 0.02f * GetHeight(),0.0f}),(Vec2){0.02f * GetHeight(),0.02f * GetHeight()}},Mouse)){
                    VFree(w);
                    PVector_Remove(VWindows,i);
                    
                    (*Focused) = NULL;
                    *Picked = VWINDOW_NONE;
                    *Offset = (Vec2){0.0f,0.0f};
                    break;
                }else if(Overlap_Rect_Point((Rect){w->p,(Vec2){w->Context.w,0.02f * GetHeight()}},Mouse)){
                    *Picked = VWINDOW_GRAPPED;
                    *Offset = Vec2_Sub(w->p,Mouse);
                }else if(Overlap_Rect_Point((Rect){Vec2_Add(w->p,(Vec2){0.0f,-0.01f * GetHeight()}),(Vec2){w->Context.w,0.01f * GetHeight()}},Mouse)){
                    *Picked = VWINDOW_TOP;
                }else if(Overlap_Rect_Point((Rect){Vec2_Add(w->p,(Vec2){0.0f,w->Context.h + 0.01f * GetHeight()}),(Vec2){w->Context.w,0.01f * GetHeight()}},Mouse)){
                    *Picked = VWINDOW_BOTTOM;
                }else if(Overlap_Rect_Point((Rect){Vec2_Add(w->p,(Vec2){0.0f,0.02f * GetHeight()}),(Vec2){0.01f * GetWidth(),w->Context.h}},Mouse)){
                    *Picked = VWINDOW_LEFT;
                }else if(Overlap_Rect_Point((Rect){Vec2_Add(w->p,(Vec2){w->Context.w-0.01f * GetWidth(),0.02f * GetHeight()}),(Vec2){0.01f * GetWidth(),w->Context.h}},Mouse)){
                    *Picked = VWINDOW_RIGHT;
                }
                break;
            }
        }
    }
    if(Input.DOWN){
        if((*Focused) && *Picked>0 && !(*Focused)->Min && !(*Focused)->Max) 
            if(*Picked==VWINDOW_GRAPPED){
                (*Focused)->p = Vec2_Add((Vec2){GetMouse().x,GetMouse().y},*Offset);
            }else if(*Picked==VWINDOW_TOP){
                VResize((*Focused),(Vec2){(*Focused)->Context.w,(*Focused)->Context.h + ((*Focused)->p.y - GetMouse().y)});
                (*Focused)->p.y = GetMouse().y;
            }else if(*Picked==VWINDOW_BOTTOM){
                VResize((*Focused),(Vec2){(*Focused)->Context.w,(*Focused)->Context.h + (GetMouse().y - ((*Focused)->p.y + (*Focused)->Context.h))});
            }else if(*Picked==VWINDOW_LEFT){
                VResize((*Focused),(Vec2){(*Focused)->Context.w + ((*Focused)->p.x - GetMouse().x),(*Focused)->Context.h});
                (*Focused)->p.x = GetMouse().x;
            }else if(*Picked==VWINDOW_RIGHT){
                VResize((*Focused),(Vec2){(*Focused)->Context.w + (GetMouse().x - ((*Focused)->p.x + (*Focused)->Context.w)),(*Focused)->Context.h});
            }
    }
    if(Input.RELEASED){
        *Picked = VWINDOW_NONE;
        (*Focused) = NULL;
        *Offset = (Vec2){0.0f,0.0f};
    }
}

Font TitleFont;
PVector VWindows;
VWindow* Focused = NULL;
char Picked = FALSE;
char Stretch = 0;
Vec2 Offset = { 0.0f,0.0f };
Font Icons;
Font BigIcons;

int QCompare(const void* p1,const void* p2){
	VWindow* t1 = *(VWindow**)p1;
	VWindow* t2 = *(VWindow**)p2;
	return t1->TimeLifted > t2->TimeLifted;
}

void Setup(AlxWindow* w){
    TitleFont = Font_New(Sprite_Load("C:/Wichtig/System/Fonts/Alx_Font_Blocky.png"),16,16,16,16);
    Font_Resize(&TitleFont,GetHeight() / 55,GetHeight() / 55);

    Icons = Font_New(Sprite_Load("C:/Wichtig/System/Icons/VOS.png"),16,16,16,16);
    Font_Resize(&Icons,GetHeight() / 42,GetHeight() / 42);

    BigIcons = Font_New(Sprite_Load("C:/Wichtig/System/Icons/VOS.png"),16,16,16,16);
    Font_Resize(&BigIcons,GetHeight() / 12,GetHeight() / 12);

    VWindows = PVector_New(sizeof(VWindow));
    PVector_Push(&VWindows,(VWindow[]){ VNew("WindowTest",(Vec2){100.0f,100.0f},(Vec2){300.0f,200.0f}) },sizeof(VWindow));
}
void Update(AlxWindow* w){
    VHandleInput(&VWindows,&Focused,&Picked,&Offset,Stroke(VK_LBUTTON));

    if(Stroke('W').PRESSED){
        PVector_Push(&VWindows,(VWindow[]){ VNew("WindowTest",(Vec2){100.0f,100.0f},(Vec2){300.0f,200.0f}) },sizeof(VWindow));
    }

    Clear(BLUE);
    RenderRect(0.0f * GetWidth(),0.9f * GetHeight(),0.1f * GetWidth(),0.1f * GetHeight(),WHITE);
    RenderRect(0.1f * GetWidth(),0.9f * GetHeight(),0.9f * GetWidth(),0.1f * GetHeight(),GRAY);

    qsort(VWindows.Memory,VWindows.size,sizeof(void*),QCompare);

    for(int i = 0;i<VWindows.size;i++){
        VWindow* w = (VWindow*)PVector_Get(&VWindows,i);
        
        if(Overlap_Rect_Point((Rect){0.125f * GetWidth() + 0.05f * GetWidth() * i,0.905f * GetHeight(),0.05f * GetHeight(),0.05f * GetHeight()},(Vec2){GetMouse().x,GetMouse().y})){
            if(Stroke(VK_LBUTTON).PRESSED) w->Min = !w->Min;
        }
        
        //RenderRect(0.125f * GetWidth() + 0.05f * GetWidth() * i,0.905f * GetHeight(),0.05f * GetHeight(),0.05f * GetHeight(),BLACK);
        RenderSubSpriteAlpha(&BigIcons.Atlas,0.114f * GetWidth() + 0.05f * GetWidth() * i,0.893f * GetHeight(),(w->Icon % BigIcons.Columns) * BigIcons.CharSizeX,(w->Icon / BigIcons.Columns) * BigIcons.CharSizeY,BigIcons.CharSizeX,BigIcons.CharSizeY);
        VRender(w,&TitleFont,&Icons);
    }
}
void Delete(AlxWindow* w){
    for(int i = 0;i<VWindows.size;i++){
        VWindow* w = (VWindow*)PVector_Get(&VWindows,i);
        VFree(w);
    }
    PVector_Free(&VWindows);
    Font_Free(&TitleFont);
}

int main(int argc,const char *argv[]){
    if(Create("Virtual Box",900,500,2,2,Setup,Update,Delete)){
        Start();
    }
    return 0;
}