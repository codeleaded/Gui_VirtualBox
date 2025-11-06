#if defined(__linux__)
#include "/home/codeleaded/System/Static/Library/VWindow.h"
#include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
#define SYSTEM_PATH "/home/codeleaded/System/"
#elif defined(_WINE)
#include "/home/codeleaded/System/Static/Library/VWindow.h"
#include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
#define SYSTEM_PATH "/home/codeleaded/System/"
#elif defined(_WIN32)
#include "F:/home/codeleaded/System/Static/Library/VWindow.h"
#include "F:/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
#define SYSTEM_PATH "F:/home/codeleaded/System/"
#elif defined(__APPLE__)
#error "Apple not supported!"
#else
#error "Platform not supported!"
#endif

PVector VProcesses;
PVector VWindows;
Session VSession;

VWindow* Focused = NULL;
TextBox SearchBar;
char Picked = FALSE;
char Stretch = 0;
Vec2 Offset = { 0.0f,0.0f };

AlxFont TitleAlxFont;
AlxFont Icons;
AlxFont BigIcons;

void* GameTest(void* lpParam) {
    VWindow* w = (VWindow*)lpParam;
    
    Vec2 p1 = {   0.0f,  0.0f };
    Vec2 d1 = {   0.05f, 0.2f };
    Vec2 v1 = {   0.0f,  0.0f };

    Vec2 p2 = {   0.95f, 0.0f };
    Vec2 d2 = {   0.05f, 0.2f };
    Vec2 v2 = {   0.0f,  0.0f };

    int Points1 = 0;
    int Points2 = 0;

    Vec2 p3 = {   0.5f,  0.5f };
    float r = 0.02f;
    Vec2 v3 = {   0.0f,  0.0f };

    p3.x = 0.5f;
    p3.y = 0.5f;
    float a = (float)Random_f64_MinMax(0.0f,2 * F32_PI);
    if(a>F32_PI/4 && a<F32_PI/4*3)      a = F32_PI/4;
    if(a>F32_PI/4*5 && a<F32_PI/4*7)    a = F32_PI/4*5;
    v3 = Vec2_Mulf(Vec2_OfAngle(a),0.01f);
    
    while(TRUE){
        if(VWindow_Stroke(w,ALX_KEY_W).DOWN)            v1.y = -0.03f;
        else if(VWindow_Stroke(w,ALX_KEY_S).DOWN)       v1.y =  0.03f;
        else                                            v1.y =  0.0f;

        if(VWindow_Stroke(w,ALX_KEY_UP).DOWN)           v2.y = -0.03f;
        else if(VWindow_Stroke(w,ALX_KEY_DOWN).DOWN)    v2.y =  0.03f;
        else                                            v2.y =  0.0f;

        p1.y += v1.y;
        p2.y += v2.y;
        
        p3.x += v3.x;
        p3.y += v3.y;

        if(p1.y<0.0f){
            p1.y = 0.0f;
        }
        if(p1.y>1.0f-d1.y){
            p1.y = 1.0f-d1.y;
        }
        if(p2.y<0.0f){
            p2.y = 0.0f;
        }
        if(p2.y>1.0f-d2.y){
            p2.y = 1.0f-d2.y;
        }

        if(Overlap_Rect_Rect((Rect){p1,d1},(Rect){p3,(Vec2){2.0f*r,2.0f*r}})){
            v3.x *= -1;
            v3.x += 0.005f * (v3.x>0?1:-1);
        }
        if(Overlap_Rect_Rect((Rect){p2,d2},(Rect){p3,(Vec2){2.0f*r,2.0f*r}})){
            v3.x *= -1;
            v3.x += 0.005f * (v3.x>0?1:-1);
        }

        if(p3.x<-r*2.0f){
            Points2++;
            p3.x = 0.5f;
            p3.y = 0.5f;
            float a = (float)Random_f64_MinMax(0.0f,2 * F32_PI);
            if(a>F32_PI/4 && a<F32_PI/4*3)      a = F32_PI/4;
            if(a>F32_PI/4*5 && a<F32_PI/4*7)    a = F32_PI/4*5;
            v3 = Vec2_Mulf(Vec2_OfAngle(a),0.01f);
        }
        if(p3.x>1.0f){
            Points1++;
            p3.x = 0.5f;
            p3.y = 0.5f;
            float a = (float)Random_f64_MinMax(0.0f,2 * F32_PI);
            if(a>F32_PI/4 && a<F32_PI/4*3)      a = F32_PI/4;
            if(a>F32_PI/4*5 && a<F32_PI/4*7)    a = F32_PI/4*5;
            v3 = Vec2_Mulf(Vec2_OfAngle(a),0.01f);
        }
        if(p3.y<0.0f){
            v3.y *= -1;
        }
        if(p3.y>1.0f - (r * 2.0f)){
            v3.y *= -1;
        }

        Mutex_Lock(&w->hMutex);

        Rect_RenderXX(w->SwapBuffer,w->Context.w,w->Context.h,0.0f,0.0f,w->Context.w,w->Context.h,BLACK);
        
        Rect_RenderXX(w->SwapBuffer,w->Context.w,w->Context.h,p1.x * w->Context.w,p1.y * w->Context.h,d1.x * w->Context.w,d1.y * w->Context.h,BLUE);
        Rect_RenderXX(w->SwapBuffer,w->Context.w,w->Context.h,p2.x * w->Context.w,p2.y * w->Context.h,d2.x * w->Context.w,d2.y * w->Context.h,RED);
        Circle_RenderX(w->SwapBuffer,w->Context.w,w->Context.h,((Vec2){(p3.x + r) * w->Context.w,(p3.y + r) * w->Context.h}),r * w->Context.w,WHITE);

        CStr fs = CStr_Format("%d : %d",Points1,Points2);
        CStr_RenderAlxFont(w->SwapBuffer,w->Context.w,w->Context.h,&TitleAlxFont,fs,w->Context.w*0.4f,w->Context.h*0.05f,WHITE);
        CStr_Free(&fs);

        VWindow_Swap(w);

        Mutex_Unlock(&w->hMutex);
        
        Thread_Sleep_M(10);
    }
    return NULL;
}
void* Commander(void* lpParam) {
    VWindow* w = (VWindow*)lpParam;
    Cmd cmd = Cmd_Make(&VSession);
    
    char* Name = NULL;
    if(VSession.LogedIn) Name = VSession.LogedIn->Name;
    Stream_Printf(&cmd.Stdout,"User[%s]: %s $ ",Name,cmd.DirInStr);

    Thread hThread = Thread_Null();
    
    while(cmd.Running){
        cmd.Stdin.Enabled = w->Focus;
        hThread = Session_Update(&VSession,&cmd,hThread);

        Mutex_Lock(&w->hMutex);
        
        Graphics_Clear(w->SwapBuffer,w->Context.w,w->Context.h,BLACK);

        int Line = 0;
        int Last = 0;
        float SizeY = (TitleAlxFont.CharSizeY * 1.2f);
        char* cstr = String_CStr(&cmd.Stdout.Buffer);
        float Scroll = w->Context.h - (float)(CStr_CountOf(cstr,'\n')+4) * SizeY;
        if(Scroll>0) Scroll = 0;
        if(Scroll/w->Context.h<-4.0f){
            Cmd_Clear(&VSession,&cmd);
        }
        free(cstr);
        
        for(int i = 0;i<cmd.Stdout.Buffer.size;i++){
            if(String_Get(&cmd.Stdout.Buffer,i)=='\n'){
                int Size = i - Last;
                Graphics_RenderCStrSizeAlxFont(w->SwapBuffer,w->Context.w,w->Context.h,&TitleAlxFont,(char*)cmd.Stdout.Buffer.Memory+Last,Size,0.0f,Scroll + Line * SizeY,WHITE);
                Last = i+1;
                Line++;
            }
            if(i==cmd.Stdout.Buffer.size-1){
                int Size = i - Last;
                Graphics_RenderCStrSizeAlxFont(w->SwapBuffer,w->Context.w,w->Context.h,&TitleAlxFont,(char*)cmd.Stdout.Buffer.Memory+Last,Size,0.0f,Scroll + Line * SizeY,WHITE);
            }
        }

        Graphics_RenderCStrSizeAlxFont(w->SwapBuffer,w->Context.w,w->Context.h,&TitleAlxFont,(char*)cmd.Stdin.Buffer.Memory,cmd.Stdin.Buffer.size,(cmd.Stdout.Buffer.size-Last) * TitleAlxFont.CharSizeX,Scroll + Line * SizeY,WHITE);
        Graphics_RenderCharAlxFont(w->SwapBuffer,w->Context.w,w->Context.h,&TitleAlxFont,'_',(cmd.Stdin.Curser+(cmd.Stdout.Buffer.size-Last)) * TitleAlxFont.CharSizeX,Scroll + Line * SizeY,RED);

        VWindow_Swap(w);

        Mutex_Unlock(&w->hMutex);
        
        Thread_Sleep_M(10);
    }

    Cmd_Free(&cmd);

    return NULL;
}
void* Explorer(void* lpParam) {
    VWindow* w = (VWindow*)lpParam;
    
    TextBox tb = TextBox_New(Input_New(100,1),(Rect){0.0f,0.0f,w->Context.w,30.0f},ALXFONT_PATHS_BLOCKY,w->Context.h / 15,w->Context.h / 15,GRAY);
    
    Input_SetText(&tb.In,"Disk/");
    char* CStr = String_CStr(&tb.In.Buffer);
    Node* DirIn = FSystem_FindByPath(&VSession.fs,CStr);
    free(CStr);

    float Scroll = 0.0f;
    Rect Scroller = { { (w->Context.w * 0.96f),40.0f },{ (w->Context.w * 0.02f),(w->Context.h - 50.0f) } };

    while(TRUE){
        if(!w->Focus) tb.In.Enabled = FALSE;

        Scroller = (Rect){ { (w->Context.w * 0.96f),40.0f },{ (w->Context.w * 0.02f),(w->Context.h - 50.0f) } };

        Vector StuffIn = FSystem_DirIn(&VSession.fs,DirIn);
        float MaxScroll = -((float)(StuffIn.size+1) * (BigIcons.CharSizeY * 1.2f) - (w->Context.h - 30.0f));
        if(MaxScroll>0) MaxScroll = 0;

        if(VWindow_Stroke(w,ALX_MOUSE_L).DOWN){
            if(VWindow_Mouse(w).y<30.0f){
                tb.In.Enabled = TRUE;
            }else{
                tb.In.Enabled = FALSE;
                if(Overlap_Rect_Point(Scroller,VWindow_Mouse(w))){
                    float d = (VWindow_Mouse(w).y - 20.0f - Scroller.p.y) / (Scroller.d.y - 30.0f);
                    d = F32_Clamp(d,0.0f,1.0f);
                    Scroll = d * MaxScroll;
                    if(Scroll>0) Scroll = 0;
                }
            }
        }
        if(VWindow_Stroke(w,ALX_MOUSE_L).PRESSED){
            if(VWindow_Mouse(w).y<30.0f){
                char* str = String_CStr(&tb.In.Buffer);
                Node* Dir = FSystem_Path(&VSession.fs,DirIn,str);
                free(str);
                char* CStr = FSystem_PathByNode(&VSession.fs,Dir);
                Node* n = FSystem_FindByPath(&VSession.fs,CStr);
                free(CStr);
                DirIn = n;
                char* DirInStr = FSystem_PathByNode(&VSession.fs,n);
                String_Clear(&tb.In.Buffer);
                String_Append(&tb.In.Buffer,DirInStr);
                free(DirInStr);
                Scroll = 0.0f;
            }else if(VWindow_Mouse(w).x<(w->Context.w * 0.95f)){
                float y = (VWindow_Mouse(w).y - Scroll - 30.0f) / (BigIcons.CharSizeY * 1.2f);
                int Id = (int)y - 1;
                if(Id<0){
                    if(((File*)DirIn->Memory)->Parent) DirIn = ((File*)DirIn->Memory)->Parent;
                    char* DirInStr = FSystem_PathByNode(&VSession.fs,DirIn);
                    String_Clear(&tb.In.Buffer);
                    String_Append(&tb.In.Buffer,DirInStr);
                    free(DirInStr);
                }else{
                    if(Id>=0 && Id<StuffIn.size){
                        DirIn = *(Node**)Vector_Get(&StuffIn,Id);
                        char* DirInStr = FSystem_PathByNode(&VSession.fs,DirIn);
                        String_Clear(&tb.In.Buffer);
                        String_Append(&tb.In.Buffer,DirInStr);
                        free(DirInStr);
                    }
                }
                Scroll = 0.0f;
            }
        }
        
        TextBox_Update(&tb,window.Strokes,VWindow_Mouse(w));

        if(tb.r.d.x!=w->Context.w){
            tb.r.d.x = w->Context.w;
        }

        Mutex_Lock(&w->hMutex);

        Graphics_Clear(w->SwapBuffer,w->Context.w,w->Context.h,WHITE);

        if(DirIn){
            Graphics_RenderSubSpriteAlpha(w->SwapBuffer,w->Context.w,w->Context.h,&BigIcons.Atlas,0.0f,tb.r.d.y + Scroll,(4 % BigIcons.Columns) * BigIcons.CharSizeX,(4 / BigIcons.Columns) * BigIcons.CharSizeY,BigIcons.CharSizeX,BigIcons.CharSizeY);
            Graphics_RenderCStrAlxFont(w->SwapBuffer,w->Context.w,w->Context.h,&TitleAlxFont,"..",BigIcons.CharSizeX,tb.r.d.y + Scroll,BLACK);

            for(int i = 0;i<StuffIn.size;i++){
                Node* n = *(Node**)Vector_Get(&StuffIn,i);
                File* file = (File*)n->Memory;
                int Icon = 4;//file->IsDir?4:7
                Graphics_RenderSubSpriteAlpha(w->SwapBuffer,w->Context.w,w->Context.h,&BigIcons.Atlas,0.0f,tb.r.d.y + (i+1) * (BigIcons.CharSizeY * 1.2f) + Scroll,(Icon % BigIcons.Columns) * BigIcons.CharSizeX,(Icon / BigIcons.Columns) * BigIcons.CharSizeY,BigIcons.CharSizeX,BigIcons.CharSizeY);
                Graphics_RenderCStrAlxFont(w->SwapBuffer,w->Context.w,w->Context.h,&TitleAlxFont,file->FileName,BigIcons.CharSizeX,tb.r.d.y + (i+1) * (BigIcons.CharSizeY * 1.2f) + Scroll,BLACK);
                Graphics_RenderCStrAlxFont(w->SwapBuffer,w->Context.w,w->Context.h,&TitleAlxFont,(file->IsDir?"Directory":"File"),BigIcons.CharSizeX + TitleAlxFont.CharSizeX * 20,tb.r.d.y + (i+1) * (BigIcons.CharSizeY * 1.2f) + Scroll,BLACK);
            }
            
            Graphics_RenderRect(w->SwapBuffer,w->Context.w,w->Context.h,Scroller.p.x,Scroller.p.y,Scroller.d.x,Scroller.d.y,GRAY);
            Graphics_RenderRect(w->SwapBuffer,w->Context.w,w->Context.h,Scroller.p.x,Scroller.p.y + (Scroll / MaxScroll * (Scroller.d.y - 30.0f)),Scroller.d.x,30.0f,DARK_GRAY);
        }
        Vector_Free(&StuffIn);
        
        Graphics_RenderTextBox(w->SwapBuffer,w->Context.w,w->Context.h,&tb);
        
        VWindow_Swap(w);

        Mutex_Unlock(&w->hMutex);
        
        Thread_Sleep_M(10);
    }

    TextBox_Free(&tb);

    return NULL;
}

void ChooseFile(TextBox* SavingPath){
    FileChooser fc = FileChooser_New("File Chooser",".",(NameTypePair[]){
        NameTypePair_New("All Files","*.*"),
        NameTypePair_New("Text Files","*.TXT"),
        NameTypePair_Null()
    });
    if(fc) Input_SetText(&SavingPath->In,fc);
    CStr_Free(&fc);
}
void ChooseFileHigh(String* HighLighter){
    FileChooser fc = FileChooser_New("File Chooser",".",(NameTypePair[]){
        NameTypePair_New("All Files","*.*"),
        NameTypePair_New("ALXON Files","*.alxon"),
        NameTypePair_Null()
    });
    if(fc){
        String_Clear(HighLighter);
        String_Append(HighLighter,fc);
    }
    CStr_Free(&fc);
}

void Button_Save(Button* b,ButtonEvent* be){
    /*if(be->e.EventType==EVENT_PRESSED){
        char* Path = String_CStr(&SavingPath.In.Buffer);
        char* Data = String_CStr(&Field.In.Buffer);

        FILE* f = fopen(Path,"w");
        if(f){
            fprintf(f,"%s",Data);
            fclose(f);
        }

        free(Path);
        free(Data);
    }*/
}
void Button_Load(Button* b,ButtonEvent* be){
    /*if(be->e.EventType==EVENT_PRESSED){
        char* Path = String_CStr(&SavingPath.In.Buffer);
        
        FILE* f = fopen(Path,"r");
        if(f){
            fseek(f,0,SEEK_END);
            int Length = ftell(f);
            fseek(f,0,SEEK_SET);

            char* BufferIn = (char*)malloc(Length+1);
            fread(BufferIn,1,Length,f);
            BufferIn[Length] = '\0';

            String_Clear(&Field.In.Buffer);
            String_Append(&Field.In.Buffer,BufferIn);
            free(BufferIn);

            Field.In.Enabled = FALSE;
            Field.In.LastKey = 0;
            Field.In.LastChar = 0;
            Field.In.CharBefore = 0;
            Field.In.KeyBefore = 0;
            Field.In.Curser = 0;
            Field.In.CurserEnd = -1;
            Field.In.Again = FALSE;
            Field.In.FirstPress = 0;
            Field.In.LastTime = 0;

            Field.ScrollX = 0;
            Field.ScrollY = 0;

            fclose(f);
        }
        free(Path);
    }*/
}
void Button_Search(Button* b,ButtonEvent* be){
    /*if(be->e.EventType==EVENT_PRESSED){
        ChooseFile();
    }*/
}
void Button_SearchHigh(Button* b,ButtonEvent* be){
    /*if(be->e.EventType==EVENT_PRESSED){
        ChooseFileHigh();

        char* cstr = String_CStr(&HighLighter);
        TextBox_SetSyntax(&Field,cstr);
        free(cstr);
    }*/
}

void* Editor(void* lpParam) {
    VWindow* w = (VWindow*)lpParam;
    
    int OffsetX = 100;
    int OffsetY = 100;
    int Size = 16;
    int PathSize = 25;

    char* FilePath = NULL;
    if(!FilePath)
        FilePath = CStr_Cpy("./data/Saved.h");

    String HighLighter = String_New();

    TextBox SavingPath = TextBox_New(Input_New(INPUT_MAXLENGTH,1),(Rect){ { OffsetX * 4.1f,OffsetY*0.5f-Size*0.5f },{ w->Context.w-OffsetX * 3.1f,OffsetY*0.5f-Size*0.5f } },ALXFONT_PATHS_YANIS,PathSize,PathSize,WHITE);
    Input_SetText(&SavingPath.In,FilePath);
    TextBox_SetSyntax(&SavingPath,SYSTEM_PATH "SyntaxFiles/Path_Syntax.alxon");

    TextBox Field = TextBox_New(Input_New(INPUT_MAXLENGTH,INPUT_MAXLENGTH),(Rect){ { OffsetX,OffsetY },{ w->Context.w-OffsetX,w->Context.h-OffsetY } },ALXFONT_PATHS_YANIS,Size,Size,WHITE);
    TextBox_SetSyntax(&Field,SYSTEM_PATH "SyntaxFiles/C_Syntax.alxon");

    Scene scene = Scene_New();
    Scene_Add(&scene,(Button[]){ Button_NewStd(NULL,"Save",Button_Save,(Vec2){19.0f,19.0f},(Rect){ {0.0f,0.0f},{OffsetX,OffsetY} },RED,BLACK) },sizeof(Button));
    Scene_Add(&scene,(Button[]){ Button_NewStd(NULL,"Load",Button_Load,(Vec2){19.0f,19.0f},(Rect){ {OffsetX,0.0f},{OffsetX,OffsetY} },GREEN,BLACK) },sizeof(Button));
    Scene_Add(&scene,(Button[]){ Button_NewStd(NULL,"Find",Button_Search,(Vec2){19.0f,19.0f},(Rect){ {OffsetX*2,0.0f},{OffsetX,OffsetY} },BLUE,BLACK) },sizeof(Button));
    Scene_Add(&scene,(Button[]){ Button_NewStd(NULL,"Color",Button_SearchHigh,(Vec2){19.0f,19.0f},(Rect){ {OffsetX*3,0.0f},{OffsetX,OffsetY} },YELLOW,BLACK) },sizeof(Button));

    while(TRUE){
        if(Field.In.Again && VWindow_Stroke(w,ALX_KEY_PLUS).DOWN && VWindow_Stroke(w,ALX_KEY_CTRL).DOWN){
            Size += 3;
            TextBox_AlxFontResize(&Field,Size,Size);
        }else if(Field.In.Again && VWindow_Stroke(w,ALX_KEY_DASH).DOWN && VWindow_Stroke(w,ALX_KEY_CTRL).DOWN){
            Size -= 3;
            TextBox_AlxFontResize(&Field,Size,Size);
        }else if(Field.In.Again && VWindow_Stroke(w,ALX_KEY_UP).DOWN && VWindow_Stroke(w,ALX_KEY_CTRL).DOWN){
            Field.ScrollY += 1;
            if(Field.ScrollY>0) Field.ScrollY = 0;
        }else if(Field.In.Again && VWindow_Stroke(w,ALX_KEY_DOWN).DOWN && VWindow_Stroke(w,ALX_KEY_CTRL).DOWN){
            Field.ScrollY -= 1;
            char* cstr = String_CStr(&Field.In.Buffer);
            int Max = (CStr_CountOf(cstr,'\n')+4) - ((w->Context.h-OffsetY) / (Field.AlxFont.CharSizeY * INPUT_GAP_FAKTOR));
            free(cstr);
            if(Field.ScrollY<-Max)  Field.ScrollY = -Max;
            if(Field.ScrollY>0)     Field.ScrollY = 0;
        }else{
            Input_DefaultReact(&Field.In,NULL);
        }

        Scene_Update(&scene,window.Strokes,VWindow_Mouse(w),VWindow_Mouse(w));

        TextBox_Update(&SavingPath,window.Strokes,VWindow_Mouse(w));
        TextBox_Update(&Field,window.Strokes,VWindow_Mouse(w));

        Graphics_Clear(w->SwapBuffer,w->Context.w,w->Context.h,BLACK);

        Rect_RenderXX(w->SwapBuffer,w->Context.w,w->Context.h,0.0f,0.0f,OffsetX,w->Context.h,GRAY);

        TextBox_Render(w->SwapBuffer,w->Context.w,w->Context.h,&Field);

        int Chop = 0;
        int Lines = 0;
        int Chars = 0;
        int CharsCurser = 0;
        int LinesCurser = 0;

        if(Field.ScrollY!=0){
            for(int i = Chop;i<Field.In.Buffer.size;i++){
                char c = String_Get(&Field.In.Buffer,i);
                if(c=='\n' || i==Field.In.Buffer.size-1){
                    Chars = i - Chop;
                    if(Field.In.Curser>=Chop && Field.In.Curser<=i+1){
                        CharsCurser = Field.In.Curser-Chop;
                        LinesCurser = Lines;
                    }
                    Chop = i+1;
                    Lines++;
                    if(Lines>=-Field.ScrollY) break;
                }
                if(c=='\n' && i==Field.In.Buffer.size-1){
                    CharsCurser = 0;
                    LinesCurser = Lines;
                    Lines++;
                    Chars = -1;
                }
            }
        }
        for(int i = Chop;i<Field.In.Buffer.size;i++){
            char c = String_Get(&Field.In.Buffer,i);
            if(c=='\n' || i==Field.In.Buffer.size-1){
                char Buff[20];
                sprintf(Buff,"%d",Lines);
                CStr_RenderAlxFont(WINDOW_STD_ARGS,&Field.AlxFont,Buff,0,OffsetY + (Field.ScrollY + Lines) * (Field.AlxFont.CharSizeY * INPUT_GAP_FAKTOR),BLACK);

                Chars = i - Chop;
                if(Field.In.Curser>=Chop && Field.In.Curser<=i+1){
                    CharsCurser = Field.In.Curser-Chop;
                    LinesCurser = Lines;
                }
                Chop = i+1;
                Lines++;
            }
            if(c=='\n' && i==Field.In.Buffer.size-1){
                CharsCurser = 0;
                LinesCurser = Lines;
                Lines++;
                Chars = -1;
            }
        }

        Rect_RenderXX(w->SwapBuffer,w->Context.w,w->Context.h,0.0f,0.0f,w->Context.w,OffsetY,DARK_GRAY);
        Scene_Render(w->SwapBuffer,w->Context.w,w->Context.h,&scene);
        TextBox_Render(w->SwapBuffer,w->Context.w,w->Context.h,&SavingPath);

        VWindow_Swap(w);
        
        Mutex_Unlock(&w->hMutex);
        
        Thread_Sleep_M(10);
    }

    String_Free(&HighLighter);
    TextBox_Free(&SavingPath);
    TextBox_Free(&Field);
    free(FilePath);
    Scene_Free(&scene);

    return NULL;
}

int QCompare(const void* p1,const void* p2){
	VWindow* t1 = *(VWindow**)p1;
	VWindow* t2 = *(VWindow**)p2;
	return t1->TimeLifted > t2->TimeLifted;
}

void Setup(AlxWindow* w){
    TitleAlxFont = AlxFont_Make(ALXFONT_BLOCKY,GetHeight() / 55,GetHeight() / 55);

    Icons = AlxFont_Make(Sprite_Load(SYSTEM_PATH "Icons/VOS.png"),16,16,16,16,GetHeight() / 42,GetHeight() / 42);

    BigIcons = AlxFont_Make(Sprite_Load(SYSTEM_PATH "Icons/VOS.png"),16,16,16,16,GetHeight() / 12,GetHeight() / 12);

    VProcesses = PVector_New();
    VWindows = PVector_New();
    VSession = Session_Load(NULL,"./data/FileSystem.alxon");
    
    SearchBar = TextBox_New(Input_New(INPUT_MAXLENGTH,1),(Rect){ { 0.1f * GetWidth(),0.9f * GetHeight() },{ 0.3f * GetWidth(),0.1f * GetHeight() } },ALXFONT_PATHS_YANIS,0.05f * GetHeight(),0.05f * GetHeight(),BLACK);
}
void Update(AlxWindow* w){
    VWindow_HandleInput(&VProcesses,&VWindows,&Focused,&Picked,&Offset,Stroke(ALX_MOUSE_L),w->Width,w->Height,GetMouse());

    Clear(BLUE);
    RenderRect(0.0f * GetWidth(),0.9f * GetHeight(),0.1f * GetWidth(),0.1f * GetHeight(),GRAY);
    RenderRect(0.3f * GetWidth(),0.9f * GetHeight(),0.9f * GetWidth(),0.1f * GetHeight(),GRAY);

    TextBox_Update(&SearchBar,w->Strokes,GetMouse());

    CStr str = String_CStr(&SearchBar.In.Buffer);
    if(SearchBar.In.Strokes[ALX_KEY_ENTER].PRESSED){
        if(CStr_Cmp(str,"Cmd")){
            char* Title = str;
            PVector_Push(&VWindows,(VWindow[]){ VWindow_New(Title,(Vec2){GetHeight() / 10,GetHeight() / 10},(Vec2){(float)GetWidth() * 0.7,(float)GetHeight() * 0.5}) },sizeof(VWindow));
            VWindow* VWindowFirst = (VWindow*)PVector_Get(&VWindows,VWindows.size-1);
            VProcess_Make(VWindowFirst,&VProcesses,Title,Commander);
        }else if(CStr_Cmp(str,"Explorer")){
            char* Title = str;
            PVector_Push(&VWindows,(VWindow[]){ VWindow_New(Title,(Vec2){GetHeight() / 10,GetHeight() / 10},(Vec2){(float)GetWidth() * 0.7,(float)GetHeight() * 0.5}) },sizeof(VWindow));
            VWindow* VWindowFirst = (VWindow*)PVector_Get(&VWindows,VWindows.size-1);
            VProcess_Make(VWindowFirst,&VProcesses,Title,Explorer);
        }else if(CStr_Cmp(str,"Pong")){
            char* Title = str;
            PVector_Push(&VWindows,(VWindow[]){ VWindow_New(Title,(Vec2){GetHeight() / 10,GetHeight() / 10},(Vec2){(float)GetWidth() * 0.7,(float)GetHeight() * 0.5}) },sizeof(VWindow));
            VWindow* VWindowFirst = (VWindow*)PVector_Get(&VWindows,VWindows.size-1);
            VProcess_Make(VWindowFirst,&VProcesses,Title,GameTest);
        }else if(CStr_Cmp(str,"Editor")){
            char* Title = str;
            PVector_Push(&VWindows,(VWindow[]){ VWindow_New(Title,(Vec2){GetHeight() / 10,GetHeight() / 10},(Vec2){(float)GetWidth() * 0.7,(float)GetHeight() * 0.5}) },sizeof(VWindow));
            VWindow* VWindowFirst = (VWindow*)PVector_Get(&VWindows,VWindows.size-1);
            VProcess_Make(VWindowFirst,&VProcesses,Title,Editor);
        }
        String_Clear(&SearchBar.In.Buffer);
    }
    CStr_Free(&str);
    
    TextBox_Render(WINDOW_STD_ARGS,&SearchBar);
    
    qsort(VWindows.Memory,VWindows.size,sizeof(void*),QCompare);

    for(int i = 0;i<VWindows.size;i++){
        VWindow* w = (VWindow*)PVector_Get(&VWindows,i);
        VWindow_UDKB(w,window.Strokes,GetMouse());
        
        if(Overlap_Rect_Point((Rect){0.425f * GetWidth() + 0.05f * GetWidth() * i,0.905f * GetHeight(),0.05f * GetHeight(),0.05f * GetHeight()},(Vec2){GetMouse().x,GetMouse().y})){
            if(Stroke(ALX_MOUSE_L).PRESSED) w->Min = !w->Min;
        }
        
        //RenderRect(0.125f * GetWidth() + 0.05f * GetWidth() * i,0.905f * GetHeight(),0.05f * GetHeight(),0.05f * GetHeight(),BLACK);
        RenderSubSpriteAlpha(&BigIcons.Atlas,0.414f * GetWidth() + 0.05f * GetWidth() * i,0.893f * GetHeight(),(w->Icon % BigIcons.Columns) * BigIcons.CharSizeX,(w->Icon / BigIcons.Columns) * BigIcons.CharSizeY,BigIcons.CharSizeX,BigIcons.CharSizeY);
        VWindow_Render(WINDOW_STD_ARGS,w,&TitleAlxFont,&Icons);
    }
}
void Delete(AlxWindow* w){
    for(int i = 0;i<VProcesses.size;i++){
        VProcess* w = (VProcess*)PVector_Get(&VProcesses,i);
        VProcess_Free(w);
    }
    PVector_Free(&VProcesses);
    
    for(int i = 0;i<VWindows.size;i++){
        VWindow* w = (VWindow*)PVector_Get(&VWindows,i);
        VWindow_Free(w);
    }
    PVector_Free(&VWindows);

    AlxFont_Free(&TitleAlxFont);
    AlxFont_Free(&Icons);
    AlxFont_Free(&BigIcons);

    TextBox_Free(&SearchBar);
    
    Session_Free(&VSession);
}

int main(int argc,const char *argv[]){
    if(Create("Virtual Box",1800,1000,1,1,Setup,Update,Delete)){
        Start();
    }
    return 0;
}