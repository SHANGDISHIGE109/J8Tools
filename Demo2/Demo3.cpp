#include <windows.h>
#include <ctime>
#include <thread>
#include <iostream>
#include <tchar.h>
#include <WinUser.h>
#include <string>
#include <windowsx.h>
#include <fstream>
#include <regex>
//#include <afxwin.h>
using namespace std;
//gcc/g++ demo3.cpp -o Demo3 -mwindows

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif


/*
GetDlgItem //通过标识符(HMENU)获取句柄
GetDlgCtrlID	//通过句柄获取标识符(HMENU)
*/




#define HotKey_ID 0x1643

#define ISKEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

#define FWIDTH 525
#define FHEIGHT 500

//默认宏可设置数量
#define MacroKeyNum 8

//默认宏的控件的宽度
#define MacroKey_w 50

//默认宏的控件的高度
#define MacroKey_h 20

#define IDC_GeneralText 10/*普通文本HMENU ID*/
#define IDC_HotKey 11/*热键HMENU　ID*/
//Response_/Trigger_ (HMENU)ID
#define IDC_RT 12/*延迟与时长HMENU　ID*/
//MacroKey_(HMENU)ID
#define IDC_MacroKey 13/*宏HMENU　ID*/
#define IDC_TriKey 14/*触发键HMENU　ID*/
#define IDC_UpBTN 15/*更新按钮HMENU　ID*/
#define IDC_StatusText 16/*状态文本HMENU ID*/
#define IDC_TABLETITLE 17/*表头文本HMENU ID*/
#define IDC_MODELEFTBORDER 18/*模式切换左选项边框HMENU ID*/
#define IDC_MODERIGHTBORDER 19/*模式切换右选项边框HMENU ID*/
#define IDC_LOCKSYNC 20/*同步HMENU ID*/
#define IDC_LOCKASYNC 21/*异步HMENU ID*/
#define IDC_SAVEFILEBTN 22/*保存数据到本地 按钮HMENU ID*/
#define IDC_LOADFILEBTN 23/*打开本地数据载入程序 按钮HMENU ID*/
#define IDC_ASYNCORSYNCHINT 24/*同步异步状态提示文本HMENU ID*/
#define IDC_PREPREBTN 900/*预载宏控件起始ID*/


//默认宏的起始ID
#define MacroKey_Def_ID 300

//宏可设置的最大数量
#define MacroKeyMaxNum 128

/*预载宏按钮控件id组*/
struct IDC_PREBTN
{
	int id[MacroKeyMaxNum] = { 900,901,902 };
	char name[20][MacroKeyMaxNum] = { "赌场转盘","小岛侦察","左键连点" };
	int last = 2;
}IDC_PREBTN;

/*预载宏按钮控件信息*/
struct PreBtnRect
{
	int width = 67;/*预载宏按钮控件宽度*/
	int height = 30;/*预载宏按钮控件高度*/
	int def_x = 289;/*每行第一个预载宏按钮控件与窗口左边框的x轴距离*/
	int x = def_x;/*下一个预载宏按钮控件与窗口左边框的x轴距离*/
	int y = 278;/*当前行预载宏按钮控件与窗口顶边框的y轴距离*/
	int gap = 6;/*控件间距*/
	int curlinenum = 0;/*当前行预载宏按钮控件数量，每行至多3个预载宏按钮控件*/
}prebtnrect;

/*宏的控件组*/
struct Res_MK_Tr_ID
{
	HWND Response_ID;/*响应延迟的控件ID*/
	HWND MacroKey_ID;/*响应键的控件ID*/
	HWND Trigger_ID;/*响应键触发时长的控件ID*/
};


/*宏的控件ID数组*/
struct MacroKey_Arr_ID
{
	Res_MK_Tr_ID ArrID[MacroKeyMaxNum] = { 0 };
	int last = -1;
}MKA;

/*鼠标移动控件组*/
struct ForDxDyPing_ID
{
	HWND FOR_ID;
	HWND DX_ID;
	HWND DY_ID;
	HWND PING_ID;
};

/*鼠标移动控件ID数组*/
struct ForDxDyPing_Arr_ID
{
	ForDxDyPing_ID ArrID[MacroKeyMaxNum] = { 0 };
	int last = -1;
}FXYPA;

/*时间-宏-时间*/
struct T_MacroKey_T
{
	int Response_Time;/*响应延迟*/
	int MacroKey;/*响应键*/
	int Trigger_Time;/*触发时长*/
};

/*宏逻辑执行数组*/
struct Macro_Logic_arr
{
	T_MacroKey_T Logic_Group[MacroKeyMaxNum] = { 0 };
}KeyLogArr;


struct KeyArr
{
	int vk_code[5] = { 0 };
	int last = -1;
};

/*触发键数组*/
KeyArr T_k;

/*控件ID数组*/
struct BoxIDArr
{
	HWND IDC[MacroKeyNum] = {0};
	int last = -1;
}RadioBox;

/*鼠标移动数据组*/
struct F_DxDy_P
{
	int forTime;
	int dx;
	int dy;
	int ping;
};
/*逻辑执行组*/
struct Move_Logic_arr
{
	F_DxDy_P Logic_Group[MacroKeyMaxNum] = { 0 };
}MovLogArr;


/*用户载入宏逻辑组*/
struct Load_Logic_arr
{																										//赌场转盘												小岛侦察											左键连点
	Macro_Logic_arr keylist[MacroKeyMaxNum] = { {{ {0, VK_RETURN, 10}, {7700, 'S', 10} }},							{{0}},							{{ {5,VK_LBUTTON,15} }} };/*按钮组的按键宏逻辑组*/
	Move_Logic_arr movelist[MacroKeyMaxNum] = {								{{ 0 }},						{{ {50,3000,0,5},{50,-3000,0,5} }},						{{ 0 }} };/*按钮组的移动宏逻辑组*/
	int syncorasync[MacroKeyMaxNum] = { IDC_LOCKASYNC, IDC_LOCKASYNC, IDC_LOCKASYNC };/*按钮组存放的同步或异步按钮控件的标识符*/
	int last = 2;
}LoadLogArr;
///*预载宏-start*/
////赌场转盘
//T_MacroKey_T preload_gta5_dc_car_vk[MacroKeyMaxNum] = { {0, VK_RETURN, 10},  {7700, 'S', 10} };/*宏运行的逻辑组*/
//F_DxDy_P preload_gta5_dc_car_xy[MacroKeyMaxNum] = { 0 };
//
////小岛侦察
//T_MacroKey_T preload_gat5_perico_reconnoitre_vk[MacroKeyMaxNum] = { 0 };
//F_DxDy_P preload_gat5_perico_reconnoitre_xy[MacroKeyMaxNum] = { {50,3000,0,5},{50,-3000,0,5} };
//
////左键连点
//T_MacroKey_T preload_lbutton_ghostspeed_vk[MacroKeyMaxNum] = { {5,VK_LBUTTON,15} };
//F_DxDy_P preload_lbutton_ghostspeed_xy[MacroKeyMaxNum] = { 0 };
///*预载宏-end*/


HWND hWnd/*窗口句柄*/,
hWndHot/*热键切换句柄*/,
hWndtrigger/*触发键，即触发宏的按键*/,
hWndHintText/*提示文本*/,
hWndUpBTN/*更新按钮句柄*/;

WNDPROC MacroKey_EDITPROC;
WNDPROC Radio_RadioProc;
WNDPROC MouseMove_EDITPROC;

//宏的控件的x轴y轴
int MacroKey_x = 10, MacroKey_y = 33;

//宏开关       线程开关
int power = 0, se = 1;

//模式状态-通过单击单选框改变
int mode = 0;/*按住(0)-切换(1)*/

//锁模式-同步-异步
int lock = 0;/*异步(0)-同步(1)*/

/*切换模式 判断是否运行*/
//int g = 0;/*停止(0)-运行(1)*/

/*同步值 单行运行中(0)-单行结束(1)*/
int keyCompare = 1, mouseCompare = 1;

int Edit_focus = 0;//触发键框焦点

int TriKey_VK;/*触发键的虚拟键码*/

int HotKey_VK;/*热键的虚拟键码*/

HFONT EngFont, ChFont;/*字体*/

/*预载宏按钮idc数组*/
int idc_preloadarr[10] = { 0 };

OPENFILENAMEA op;

char strInitialDir[MAX_PATH],//初始保存位置
strFile[MAX_PATH] = { 0 },// 保存获取文件绝对路径+名称的缓冲区
strFileTitle[MAX_PATH] = { 0 };// 保存获取文件名+扩展名的缓冲区

//HWND 窗口处理函数
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//宏-按钮窗口处理函数
LRESULT CALLBACK KeyEditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/*鼠标移动的dx，dy控件处理函数*/
LRESULT CALLBACK MouseMoveEditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


//获取按下的键名
int Pressed_Key_Name(int VK_CODE, char* getname);

//通过键名获取虚拟键码
int Pressed_Key_VK(char* key_name);

//获取热键
UINT HotKey_MOD(int vk_code);

/*初始化菜单*/
void CreateKeyEdit(HWND hWnd_, LPARAM lParam);

/*鼠标按下键名*/
void MacroDown(HWND hWnd, char* str_KeyName);

//字符串切割真意 返回值为切割二维数组    判断 例：str[i]
char** str_qie(char* str, char* CutSymbol);

/*用户设置新的宏时 若没设置时间 默认设置->响应延迟为5 触发时长为10*/
void Auto_MK_Time(HWND hWnd_, MacroKey_Arr_ID* MKA_, int Response_Time, int Trigger_Time);

//延迟函数
void  Delay(int time);

/*键-宏运行*/
void macrorun(int i);

/*模式选择*/
int KeyModeCheck();

/*移动-宏运行*/
void moverun(int i);

/*鼠标移动模式切换*/
int MoveModeCheck();


int charat(char* str, char key);

/*清扬-我无懈可击-无需隐藏*/
void ClearAllEdit(HWND hWnd_);

/*模式切换控件边框绘画*/
void ModeBorderPaint(HWND hWnd_, HDC hdc, COLORREF rColor);
//void ModeBorderPaint(HWND hWnd_, COLORREF rColor);

/*获取程序窗口内坐标*/
void GetProgramPos(HWND hWnd_, LPRECT* rect);

/*OPENFILENAME初始化*/
void opfileinit(HWND hWnd_, LPOPENFILENAMEA lpofn);

/*获取宏数据*/
int getmacrodata();

/*保存数据至本地*/
int savedatatolocal();

/*载入本地数据*/
int loadlocaldata();


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, LPSTR lpCmdLine, int nCmdshow)
{
	//1.设计窗口类 Spy++
	TCHAR szAppClassName[] = TEXT("Demo");   //TCHAR自适应字符集
	WNDCLASS wndClass = { 0 };
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//加载白色画刷背景
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);//加载系统默认光标
	wndClass.hInstance = hInstance;//当前应用程序实例句柄
	wndClass.lpfnWndProc = WindowProc;//窗口处理函数
	wndClass.lpszClassName = szAppClassName;//窗口类型名
	wndClass.style = CS_HREDRAW | CS_VREDRAW;//窗口类的风格 

	//2.注册窗口类
	RegisterClass(&wndClass);

	//3.创建窗口
	hWnd = CreateWindowEx(0, szAppClassName, szAppClassName, /*WS_BORDER | WS_CAPTION |*/
		WS_SYSMENU | WS_MINIMIZEBOX, 500, 200, FWIDTH, FHEIGHT, NULL, NULL, hInstance, NULL);

	//4.显示和更新窗口
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	/*为宏设置线程 以此不影响主线程的运行*/
	thread mymacrostar(KeyModeCheck);

	/*为鼠标移动设置线程*/
	thread mymovestar(MoveModeCheck);

	//5.消息循环
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))//获取WM_QUIT消息，返回0，结束循环
	{
		//将虚拟键消息转换为字符消息
		TranslateMessage(&msg);
		//将消息分发给窗口处理函数
		DispatchMessage(&msg);
	}
	return 0;
}

//表头
void TableTitle(HWND hWnd_, LPARAM lParam)
{
	//按键宏表头/*左边区域总长8+54+3+104+3+54=226*/
	int t_w = 226,
		res_tri_w = 55,
		key_w = 107,
		height = MacroKey_h + 5;
	int x = MacroKey_x - 2, y = MacroKey_y - height - 3;
	CreateWindowEx(0, _T("STATIC"), _T("延迟"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_CENTER, x, y, res_tri_w, height, hWnd_, (HMENU)(IDC_TABLETITLE), ((LPCREATESTRUCT)lParam)->hInstance, NULL);
	x += res_tri_w;
	CreateWindowEx(0, _T("STATIC"), _T("键"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_CENTER, x, y, key_w, height, hWnd_, (HMENU)(IDC_TABLETITLE), ((LPCREATESTRUCT)lParam)->hInstance, NULL);
	x += key_w;																																				/*+1是下边控件边框*/
	CreateWindowEx(0, _T("STATIC"), _T("时长"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_CENTER, x, y, res_tri_w + 1, height, hWnd_, (HMENU)(IDC_TABLETITLE), ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	//鼠标移动表头/*我这边长225，嘿嘿*/
	x = t_w + 50;
	CreateWindowEx(0, _T("STATIC"), _T("循环"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_CENTER, x, y, res_tri_w + 1, height, hWnd_, (HMENU)(IDC_TABLETITLE), ((LPCREATESTRUCT)lParam)->hInstance, NULL);
	x += res_tri_w + 1;
	CreateWindowEx(0, _T("STATIC"), _T("dx"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_CENTER, x, y, res_tri_w + 1, height, hWnd_, (HMENU)(IDC_TABLETITLE), ((LPCREATESTRUCT)lParam)->hInstance, NULL);
	x += res_tri_w + 1;
	CreateWindowEx(0, _T("STATIC"), _T("dy"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_CENTER, x, y, res_tri_w + 1, height, hWnd_, (HMENU)(IDC_TABLETITLE), ((LPCREATESTRUCT)lParam)->hInstance, NULL);
	x += res_tri_w + 1;
	CreateWindowEx(0, _T("STATIC"), _T("延迟"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_CENTER, x, y, res_tri_w + 2, height, hWnd_, (HMENU)(IDC_TABLETITLE), ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	//热键表头
	x = 8, y = 250;//520;
	int width = 270;
	CreateWindowEx(0, _T("STATIC"), _T(" 热键"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE, x, y, width, height, hWnd_, (HMENU)(IDC_TABLETITLE), ((LPCREATESTRUCT)lParam)->hInstance, NULL);

	//预载表头
	x = 290, width = 211;
	CreateWindowEx(0, _T("STATIC"), _T(" 预载宏"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE, x, y, width, height, hWnd_, (HMENU)(IDC_TABLETITLE), ((LPCREATESTRUCT)lParam)->hInstance, NULL);


}


//创建宏的控件
void CreateKeyEdit(HWND hWnd_, LPARAM lParam)
{
	TableTitle(hWnd_, lParam);
	int x = MacroKey_x - 2, y = MacroKey_y - 2;
	int in_out_gap = 4,/*内外间隙*/
		outbox_gap = 3,/*控件间距*/
		inbox_gap = 5;
	int outside_w = MacroKey_w + in_out_gap, outside_h = MacroKey_h + in_out_gap;
	int Key_w = MacroKey_w + 50, out_Key_w = Key_w + 4;

	for (int i = 1;i <= (int)MacroKeyNum;i++)/*两组一行 响应延迟 宏 触发时长*/
	{
		MKA.last++;

		//宏响应延迟控件/				左边区域总长8+54+3+104+3+54=226
		CreateWindowEx(0, _T("static"), NULL, WS_BORDER | WS_CHILD | WS_VISIBLE, x, y, outside_w, outside_h, hWnd_, (HMENU)IDC_GeneralText, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		MKA.ArrID[MKA.last].Response_ID = CreateWindowEx(0, _T("Edit"), NULL, WS_CHILD | WS_VISIBLE | ES_NUMBER | SS_CENTERIMAGE | SS_CENTER | ES_MULTILINE, x + in_out_gap / 2, y + in_out_gap / 2, MacroKey_w, MacroKey_h, hWnd_, (HMENU)IDC_RT, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		x += (outside_w + outbox_gap);

		//宏 即按键     ES_READONLY 禁止用户输入
		CreateWindowEx(0, _T("static"), NULL, WS_BORDER | WS_CHILD | WS_VISIBLE, x, y, out_Key_w, outside_h, hWnd_, (HMENU)IDC_GeneralText, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		MKA.ArrID[MKA.last].MacroKey_ID = CreateWindowEx(0, _T("Edit"), NULL, WS_CHILD | WS_VISIBLE | ES_READONLY | SS_CENTERIMAGE | SS_CENTER | ES_MULTILINE, x + in_out_gap / 2, y + in_out_gap / 2, Key_w, MacroKey_h, hWnd_, (HMENU)IDC_MacroKey, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		/*设置新的处理函数*/
		MacroKey_EDITPROC = (WNDPROC)SetWindowLongPtr(MKA.ArrID[MKA.last].MacroKey_ID, GWLP_WNDPROC, (LONG64)KeyEditProc);
		x += (out_Key_w + outbox_gap);

		//宏触发时长控件
		CreateWindowEx(0, _T("static"), NULL, WS_BORDER | WS_CHILD | WS_VISIBLE, x, y, outside_w, outside_h, hWnd_, (HMENU)IDC_GeneralText, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		MKA.ArrID[MKA.last].Trigger_ID = CreateWindowEx(0, _T("Edit"), NULL, WS_CHILD | WS_VISIBLE | ES_NUMBER | SS_CENTERIMAGE | SS_CENTER | ES_MULTILINE, x + in_out_gap / 2, y + in_out_gap / 2, MacroKey_w, MacroKey_h, hWnd_, (HMENU)IDC_RT, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

		x = MacroKey_x - 2;
		y += (outside_h + outbox_gap);

	}
}

/*同步异步按钮*/
void CreateSync(HWND hWnd_, LPARAM lParam)
{
	int y1 = 5, h = MacroKey_h + y1;
	int x = 226 + 10/*有50的可用宽度 间隔为10*/, y = MacroKey_y, width = 30, height = 100;
	//锁显示文本
	CreateWindowEx(0, _T("STATIC"), _T("-异步-"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE | SS_CENTER, x - 5, y1, width + 10, h, hWnd_, (HMENU)(IDC_ASYNCORSYNCHINT), ((LPCREATESTRUCT)lParam)->hInstance, NULL);
	CreateWindowEx(0, _T("BUTTON"), _T("同步"), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x, y, width, height, hWnd_, (HMENU)(IDC_LOCKSYNC), ((LPCREATESTRUCT)lParam)->hInstance, NULL);
	y += (height + 10);
	CreateWindowEx(0, _T("BUTTON"), _T("异步"), WS_CHILD | WS_VISIBLE | BS_OWNERDRAW, x, y, width, height, hWnd_, (HMENU)(IDC_LOCKASYNC), ((LPCREATESTRUCT)lParam)->hInstance, NULL);

}

/*创建鼠标移动控件*/
void CreateMouseEdit(HWND hWnd_, LPARAM lParam)
{
	int def_x = 226 + 50, x = def_x, y = MacroKey_y - 2;
	int in_out_gap = 4,/*内外间隙*/
		outbox_gap = 3,/*控件间距*/
		inbox_gap = 5;
	int outside_w = MacroKey_w + in_out_gap, outside_h = MacroKey_h + in_out_gap;
	for (int i = 0;i < (int)MacroKeyNum;i++)
	{
		FXYPA.last++;

		CreateWindowEx(0, _T("static"), NULL, WS_BORDER | WS_CHILD | WS_VISIBLE, x, y, outside_w, outside_h, hWnd_, (HMENU)IDC_GeneralText, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		FXYPA.ArrID[i].FOR_ID = CreateWindowEx(0, _T("Edit"), NULL, WS_CHILD | WS_VISIBLE | ES_NUMBER | SS_CENTERIMAGE | SS_CENTER | ES_MULTILINE, x + in_out_gap / 2, y + in_out_gap / 2, MacroKey_w, MacroKey_h, hWnd_, (HMENU)IDC_RT, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		x += (outside_w + outbox_gap);

		CreateWindowEx(0, _T("static"), NULL, WS_BORDER | WS_CHILD | WS_VISIBLE, x, y, outside_w, outside_h, hWnd_, (HMENU)IDC_GeneralText, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		FXYPA.ArrID[i].DX_ID = CreateWindowEx(0, _T("Edit"), NULL, WS_CHILD | WS_VISIBLE | ES_READONLY | SS_CENTERIMAGE | SS_CENTER | ES_MULTILINE, x + in_out_gap / 2, y + in_out_gap / 2, MacroKey_w, MacroKey_h, hWnd_, (HMENU)IDC_MacroKey, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		MouseMove_EDITPROC = (WNDPROC)SetWindowLongPtr(FXYPA.ArrID[i].DX_ID, GWLP_WNDPROC, (LONG64)MouseMoveEditProc);
		x += (outside_w + outbox_gap);

		CreateWindowEx(0, _T("static"), NULL, WS_BORDER | WS_CHILD | WS_VISIBLE, x, y, outside_w, outside_h, hWnd_, (HMENU)IDC_GeneralText, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		FXYPA.ArrID[i].DY_ID = CreateWindowEx(0, _T("Edit"), NULL, WS_CHILD | WS_VISIBLE | ES_READONLY | SS_CENTERIMAGE | SS_CENTER | ES_MULTILINE, x + in_out_gap / 2, y + in_out_gap / 2, MacroKey_w, MacroKey_h, hWnd_, (HMENU)IDC_MacroKey, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		MouseMove_EDITPROC = (WNDPROC)SetWindowLongPtr(FXYPA.ArrID[i].DY_ID, GWLP_WNDPROC, (LONG64)MouseMoveEditProc);
		x += (outside_w + outbox_gap);

		CreateWindowEx(0, _T("static"), NULL, WS_BORDER | WS_CHILD | WS_VISIBLE, x, y, outside_w, outside_h, hWnd_, (HMENU)IDC_GeneralText, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		FXYPA.ArrID[i].PING_ID = CreateWindowEx(0, _T("Edit"), NULL, WS_CHILD | WS_VISIBLE | ES_NUMBER | SS_CENTERIMAGE | SS_CENTER | ES_MULTILINE, x + in_out_gap / 2, y + in_out_gap / 2, MacroKey_w, MacroKey_h, hWnd_, (HMENU)IDC_RT, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

		x = def_x;
		y += (outside_h + outbox_gap);

	}
}

/*创建预载宏按钮控件*/
void CreatePreloadBTN(HWND hWnd_, int i)
{
	//创建一个预载或用户载入按钮控件
	HWND h_tmp=CreateWindowA("BUTTON", IDC_PREBTN.name[i], WS_CHILD | WS_VISIBLE, prebtnrect.x, prebtnrect.y, prebtnrect.width, prebtnrect.height, hWnd_, (HMENU)IDC_PREBTN.id[i], NULL, NULL);

	SendMessage(h_tmp, WM_SETFONT, (WPARAM)ChFont, NULL);/*设置字体*/

	prebtnrect.x += (prebtnrect.width + prebtnrect.gap);/*设置下一个按钮的x*/
	prebtnrect.curlinenum++;//当前行按钮数量+1
	if (prebtnrect.curlinenum % 3 == 0)/*如果当前行满3个按钮 则到下一行*/
	{
		prebtnrect.curlinenum = 0;//重置行的按钮数
		prebtnrect.x = prebtnrect.def_x;//重置x
		prebtnrect.y += (prebtnrect.height + prebtnrect.gap);//y增加到下一行
	}
}

/*创建预载宏控件*/
void CreatePreloadMacro(HWND hWnd_, LPARAM lParam)
{
	//int def_x = 290, x = def_x, y = 278, gap = 5, width = 67, height = 30;
	for (int i = 0;i <= IDC_PREBTN.last;i++)
		CreatePreloadBTN(hWnd_, i);

	int x = prebtnrect.def_x, y = 398, gap = 7, width = 102, height = 30;
	CreateWindowEx(0, _T("BUTTON"), _T("保存"), WS_CHILD | WS_VISIBLE, x, y, width, height, hWnd_, (HMENU)IDC_SAVEFILEBTN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
	x += (width + gap);
	CreateWindowEx(0, _T("BUTTON"), _T("载入"), WS_CHILD | WS_VISIBLE, x, y, width, height, hWnd_, (HMENU)IDC_LOADFILEBTN, ((LPCREATESTRUCT)lParam)->hInstance, NULL);

}


/*间隔符*/
#define gapsymbol " + "

/*组合键数组最后一键下标*/
int TeamKey_Index = -1;

// char str_KeyName[30] = { 0 };//按键名数组
char* TeamKey[5] = { 0 };//组合键数组
LRESULT CALLBACK KeyEditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	char str_KeyName[30] = { 0 };//按键名数组


	int size = 20;
	switch (uMsg)
	{
	case WM_SETFOCUS:
		Edit_focus = 1;
		break;
	case WM_KILLFOCUS:
		Edit_focus = 0;
		break;
	case WM_KEYDOWN:
	{
		/**
		* 采取Unicode编码时，实际调用的是 MessageBoxW，该函数接受的参数是LPCWSTR
		* 采取多字节编码时，实际调用的是 MessageBoxA，该函数接受的参数是LPCSTR
		*/
		// MessageBoxA(hWnd,str,NULL,0);


		//Pressed_Key_Name返回-1，表示用户所按按键不在程序设计之内，则不改变文本内容

		/*按下为删除键*/
		if (wParam == VK_DELETE)
		{
			TeamKey_Index = -1;
			SetWindowTextA(hWnd, NULL);
			break;
		}

		/*存放按键名*/
		char str_get[20] = { 0 };

		int x = Pressed_Key_Name((int)toascii(wParam), str_get);/*获取按键名 并返回按键类型值*/

		/*判断按下按键是否在程序设计之外*/
		if (x == -1)
		{
			TeamKey_Index = -1;
			SetWindowTextA(hWnd, NULL);
			break;
		}

		/*判断当前按键是否与上一按键相同 即处于长按状态*/
		if (TeamKey_Index >= 0 && strcmp(TeamKey[TeamKey_Index], str_get) == 0)
			break;
		else
		{
			/*判断之前按键是否还处于按下状态*/
			int j = -1;
			for (int i = 0;i <= TeamKey_Index;i++)
				j += ISKEYDOWN(Pressed_Key_VK(TeamKey[i]));


			if (j != TeamKey_Index)/*不相等表示有按键处于弹起状态*/
				TeamKey_Index = -1;
			/*把当前按键存入组合按键数组中*/
			TeamKey[++TeamKey_Index] = new char[20];
			strcpy_s(TeamKey[TeamKey_Index], size, str_get);
		}

		/*把当前正在按下的按键拼接*/
		strcat_s(str_KeyName, size, TeamKey[0]);
		for (int i = 1;i <= TeamKey_Index;i++)
		{
			strcat_s(str_KeyName, size, gapsymbol);
			strcat_s(str_KeyName, size, TeamKey[i]);
		}

		/*判断按下是否为非特殊按键*/
		if (x == 0)
			TeamKey_Index = -1;


		MacroDown(hWnd, str_KeyName);

		break;
	}
	case WM_LBUTTONDOWN:
	{
		if (Edit_focus)
		{
			strcpy_s(str_KeyName, size, "LBUTTON");
			MacroDown(hWnd, str_KeyName);
		}
		break;
	}
	case WM_MBUTTONDOWN:
	{
		if (Edit_focus)
		{
			strcpy_s(str_KeyName, size, "MBUTTON");
			MacroDown(hWnd, str_KeyName);
		}
		break;
	}
	case WM_RBUTTONDOWN:
	{
		if (Edit_focus)
		{
			strcpy_s(str_KeyName, size, "RBUTTON");
			MacroDown(hWnd, str_KeyName);
		}
		//返回1能屏蔽掉系统右键菜单
		return 1;
	}
	case WM_XBUTTONDOWN:
	{
		if (Edit_focus)
		{
			switch (LOWORD(wParam))
			{
			case MK_XBUTTON1:/*侧键1*/
			{
				strcpy_s(str_KeyName, size, "XBUTTON1");
				MacroDown(hWnd, str_KeyName);
				break;
			}
			case MK_XBUTTON2:/*侧键2*/
			{
				strcpy_s(str_KeyName, size, "XBUTTON2");
				MacroDown(hWnd, str_KeyName);
				break;
			}
			}
		}
		break;
	}
	}
	return CallWindowProc(MacroKey_EDITPROC, hWnd, uMsg, wParam, lParam);
}

void PreNextstr(int n, int m, string& s, string& prestr, string& nextstr, int nCounts)
{
	if (n <= m)
	{
		prestr = s.substr(0, n),/*获取插入光标之前的字符串*/
			nextstr = s.substr(m, nCounts);/*获取插入光标之后的字符串*/
	}
	else//n>m
	{
		prestr = s.substr(0, m),/*获取插入光标之前的字符串*/
			nextstr = s.substr(n, nCounts);/*获取插入光标之后的字符串*/
	}
}

/*鼠标移动的dx，dy控件处理函数*/
LRESULT CALLBACK MouseMoveEditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
	{
		char key[20] = { '-',0 };
		unsigned __int64 n = 0, m = 0;
		int k = SendMessage(hWnd, EM_GETSEL, (WPARAM)&n, (LPARAM)&m);/*获取插入符在字符串中的位置*/
		string prestr, nextstr;/*保存插入位置之前的字符串 和 插入位置之后的字符串*/
		char str[8];
		GetWindowTextA(hWnd, str, sizeof(str));/*获取edit控件text*/
		string s = str;

		/*按下为删除键*/
		if (wParam == VK_DELETE)
		{
			SetWindowTextA(hWnd, NULL);
		}
		/*按下为退格键*/
		else if (wParam == VK_BACK)
		{
			/*判断所选文本是从前往后选 还是从后往前选*/
			if (n < m)
			{
				prestr = s.substr(0, n),/*获取插入光标之前的字符串*/
					nextstr = s.substr(m, strlen(str));/*获取插入光标之后的字符串*/
			}
			else if (n > m)
			{
				prestr = s.substr(0, m),/*获取插入光标之前的字符串*/
					nextstr = s.substr(n, strlen(str));/*获取插入光标之后的字符串*/
			}
			else if (n == m && n != 0)
			{
				prestr = s.substr(0, n - 1),/*获取插入光标之前的字符串*/
					nextstr = s.substr(m, strlen(str));/*获取插入光标之后的字符串*/
			}
			else/*如果插入符在最前面并没有选文本 则跳过*/
			{
				break;
			}

			prestr.append(nextstr);/*拼接字符串*/
			const char* str2 = prestr.c_str();
			SetWindowTextA(hWnd, str2);
			SendMessage(hWnd, EM_SETSEL, (WPARAM)n - 1, (LPARAM)m - 1);/*设置插入符位置不变*/
		}

		if (strlen(str) == 7)break;/*达到输入上限*/

		/*输入为‘-’号*/
		else if (wParam == VK_SUBTRACT/*小键盘-*/ || (wParam == VK_OEM_MINUS && ISKEYDOWN(VK_SHIFT) == 0/*退格键旁边-*/))
		{

			if (str[0] == '\0')/*无数字 无‘-’号*/
				SetWindowTextA(hWnd, key);
			else if (charat(str, key[0]) < 0)/*有数字 无‘-’号*/
				if (n == 0)/*光标在最前面*/
				{
					strcat_s(key, sizeof(key), str);
					SetWindowTextA(hWnd, key);
				}
			//MAKELPARAM(999, -5);//创建一个值，用作消息中的 lParam参数
			SendMessage(hWnd, EM_SETSEL, (WPARAM)99, (LPARAM)99);/*设置插入符在最后一位*/
		}
		/*输入为数字*/
		else if ((wParam >= '0' && wParam <= '9') || (wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9))
		{

			/*判断有无‘-’号*/
			//if (charat(str1, key) >= 0)/*存在‘-’号*/
			int flag = 0;
			int index = s.find(key[0]);
			if (index >= 0)/*存在‘-’号*/
			{
				if (n >= 1 && m >= 1)/*插入符在‘-’之后*/
				{
					PreNextstr(n, m, s, prestr, nextstr, strlen(str));
					flag = 1;
				}
				else if (n >= 1 || m >= 1)/*光标所选文本包含‘-’号*/
				{
					PreNextstr(n, m, s, prestr, nextstr, strlen(str));
					flag = 1;
				}
			}
			else /*不存在‘-’号*/
			{
				PreNextstr(n, m, s, prestr, nextstr, strlen(str));
				flag = 1;
			}

			if (flag)
			{
				/*判断数小键盘数字还是那个单排数字*/
				if (wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9)/*96-105*/
				{
					int mid = wParam - 96;
					prestr.append(to_string(mid));
				}
				else/*非小键盘数字*/
				{
					char mid[2] = { (char)toascii(wParam), 0 };
					prestr.append(mid);
				}

				/*拼接字符串 并输出到控件*/
				prestr.append(nextstr);
				const char* str2 = prestr.c_str();
				SetWindowTextA(hWnd, str2);

			}


			/*自动填充 for-ping*/
			for (int i = 0;i <= FXYPA.last;i++)
				if (FXYPA.ArrID[i].DX_ID == hWnd || FXYPA.ArrID[i].DY_ID == hWnd)
				{
					char str1[10], str2[10];
					GetWindowTextA(FXYPA.ArrID[i].FOR_ID, str1, sizeof(str1));
					GetWindowTextA(FXYPA.ArrID[i].PING_ID, str2, sizeof(str2));
					if (str1[0] == '\0' && str2[0] == '\0')
					{
						_itoa_s(1, str1, sizeof(str1), 10);
						SetWindowTextA(FXYPA.ArrID[i].FOR_ID, str1);
						_itoa_s(10, str1, sizeof(str1), 10);
						SetWindowTextA(FXYPA.ArrID[i].PING_ID, str1);
					}
				}
			SendMessage(hWnd, EM_SETSEL, (WPARAM)99, (LPARAM)99);/*设置插入符在最后一位*/
		}

		break;
	}
	}
	return CallWindowProc(MouseMove_EDITPROC, hWnd, uMsg, wParam, lParam);
}

/*设置透明度*/
void SetWindowsAlpha(HWND hWnd_)
{
	//LONG t = GetWindowLong(hWnd_, GWL_EXSTYLE);
	//t |= WS_EX_LAYERED;
	SetWindowLong(hWnd_, GWL_EXSTYLE, WS_EX_LAYERED);

	SetLayeredWindowAttributes(hWnd_, RGB(0, 0, 0), 200, LWA_COLORKEY/*黑色透明*/);
}

/*设置字体*/
void ChildTeamSetFont(HWND hWnd_)
{
	HWND Child, NextChild;
	HFONT hFont;

	//SetWindowsAlpha(hWnd_);//设置透明度

	Child = GetWindow(hWnd_, GW_CHILD);/*返回Tab序顶端的子窗口的句柄*/
	NextChild = Child;

	while (NextChild != NULL)
	{
		int id = GetDlgCtrlID(NextChild);
		RECT rect;
		int flag = 0;

		if (id == IDC_RT || id == IDC_MacroKey || id == IDC_HotKey || id == IDC_TriKey)
		{
			hFont = EngFont;
			flag = 1;
			GetClientRect(NextChild, &rect);
			OffsetRect(&rect, 0, (rect.bottom - rect.top) / 2 - 8);
		}
		else
			hFont = ChFont;

		SendMessage(NextChild, WM_SETFONT, (WPARAM)hFont, NULL);/*设置字体*/

		if (flag)
			SendMessage(NextChild, EM_SETRECT, 0, (LPARAM)&rect);/*垂直居中要在设置字体之后才生效*/

		Child = NextChild;
		NextChild = GetNextWindow(Child, GW_HWNDNEXT);
	}
}

/*画图*/
VOID DrawFrame(HWND hWnd, HWND PrehWnd) {
	RECT rect;
	GetWindowRect(hWnd, &rect);/*函数返回指定窗口的边框矩形的大小*/
	//OffsetRect(/*将指定的矩形移动到指定的位置*/
	//	&rect, // 矩形
	//	-rect.left, // 横向偏移
	//	-rect.top);// 纵向偏移

	HDC hdc = GetWindowDC(PrehWnd);/*返回hWnd参数所指定的窗口的设备环境*/
	HPEN hPen = CreatePen(
		PS_SOLID/*画笔画出的是实线*/,
		1, /*画笔的宽度*/
		RGB(255, 0, 0));/*指定的样式、宽度和颜色创建画笔*/
	SelectObject(hdc, hPen);/*选择hPen到指定的设备上下文环境中，该新对象替换先前的相同类型的对象*/
	SelectObject(hdc, GetStockObject(BLACK_BRUSH));/*用空画刷替换之前的画刷*/
	Rectangle(/*画一个矩形，可以用当前的画笔画矩形轮廓，用当前画刷进行填充*/
		hdc,
		rect.left - 10, /*指定矩形左上角的逻辑X坐标*/
		rect.top - 10, /*指定矩形左上角的逻辑Y坐标*/
		rect.right + 10, /*指定矩形右下角的逻辑X坐标*/
		rect.bottom + 10);/*指定矩形右下角的逻辑Y坐标*/
	DeleteObject(hPen);/*删除一个逻辑笔*/
	ReleaseDC(PrehWnd, hdc);/*释放设备上下文环境（DC）供其他应用程序使用*/
}


/*程序窗口处理函数*/
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE://窗口创建消息
	{
		EngFont = CreateFontA(
			-13/*高度*/, -5/*宽度*/, 0/*不用管*/, 0/*不用管*/,
			FW_SEMIBOLD /*一般这个值设为400表示标准体*/,
			FALSE/*不带斜体*/, FALSE/*不带下划线*/, FALSE/*不带删除线*/,
			DEFAULT_CHARSET,  //这里我们使用默认字符集，还有其他以 _CHARSET 结尾的常量可用
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,  //这行参数不用管
			DEFAULT_QUALITY,  //默认输出质量
			FF_DONTCARE,  //不指定字体族*/
			"Arial"  //字体名
		);
		ChFont = CreateFontA(
			-15/*高度*/, -7/*宽度*/, 0/*不用管*/, 0/*不用管*/,
			FW_MEDIUM /*一般这个值设为400表示标准体*/,
			FALSE/*不带斜体*/, FALSE/*不带下划线*/, FALSE/*不带删除线*/,
			DEFAULT_CHARSET,  //这里我们使用默认字符集，还有其他以 _CHARSET 结尾的常量可用
			OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS,  //这行参数不用管
			DEFAULT_QUALITY,  //默认输出质量
			FF_DONTCARE,  //不指定字体族*/
			"黑体"  //字体名
		);
		// if(!InstallKeyboardHook())
			// MessageBox(hWnd,TEXT("FAIL"),TEXT(""),MB_OK);
		//int nm = MOD_CONTROL;
		//nm |= MOD_SHIFT;
		RegisterHotKey(hWnd, HotKey_ID, MOD_CONTROL, VK_F1);//注册 Ctrl+F1 的热键，注意，不要冲突了
			//第二个参数 HotKey_ID 用于定义热键的标识符

		////初始化宏ID数组长度
		//MKA.last = -1;

		//初始化控件虚拟键码
		HotKey_VK = VK_CONTROL + VK_F1;
		TriKey_VK = VK_RBUTTON;

		int def_x = 8,
			interval_x = 50, /*x轴间隔*/
			interval_y = 10;/*y轴间隔*/
		int AvailableWidth = 270;/*右边区域剩余可用宽度*/
		int x = def_x, y = 278;//MacroKey_y - 2;
		int w = 100, h = 30;
		int in_out_gap = 4;
		int edit_out_w = 120, edit_out_h = 30, edit_w = edit_out_w - in_out_gap, edit_h = edit_out_h - in_out_gap;

		//静态文本  参数1-控件类型  参数2-控件文字  参数3-控件样式-WS_CHILD-子窗口  参数4-7-x,y,width,height    参数8-父窗口    参数9-控件ID-UINT即int型    参数10-
		CreateWindowEx(0, _T("STATIC"), _T("启动切换热键"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE, x, y, w, h, hWnd, (HMENU)(IDC_GeneralText), ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		x += (w + interval_x);
		//热键框
		CreateWindowEx(0, _T("static"), NULL, WS_BORDER | WS_CHILD | WS_VISIBLE, x, y, edit_out_w, edit_out_h, hWnd, (HMENU)(IDC_GeneralText), ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hWndHot = CreateWindowEx(0, _T("Edit"), _T("Ctrl + F1"), WS_CHILD | WS_VISIBLE | ES_READONLY | ES_CENTER | ES_MULTILINE, x + in_out_gap / 2, y + in_out_gap / 2, edit_w, edit_h, hWnd, (HMENU)(IDC_HotKey), ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		// 为热键设置新的消息处理函数
		MacroKey_EDITPROC = (WNDPROC)SetWindowLongPtr(hWndHot, GWLP_WNDPROC, (LONG64)KeyEditProc);


		x = def_x;
		y += (h + interval_y);
		//触发键框
		CreateWindowEx(0, _T("STATIC"), _T("触发键"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE, x, y, w, h, hWnd, (HMENU)(IDC_GeneralText), ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		x += (w + interval_x);
		CreateWindowEx(0, _T("static"), NULL, WS_BORDER | WS_CHILD | WS_VISIBLE, x, y, edit_out_w, edit_out_h, hWnd, (HMENU)(IDC_GeneralText), ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		hWndtrigger = CreateWindowEx(0, _T("Edit"), _T("RBUTTON"), WS_CHILD | WS_VISIBLE | ES_READONLY | ES_CENTER | ES_MULTILINE, x + in_out_gap / 2, y + in_out_gap / 2, edit_w, edit_h, hWnd, (HMENU)(IDC_TriKey), ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		T_k.vk_code[++T_k.last] = VK_RBUTTON;
		// 为触发键设置新的消息处理函数
		MacroKey_EDITPROC = (WNDPROC)SetWindowLongPtr(hWndtrigger, GWLP_WNDPROC, (LONG64)KeyEditProc);


		x = def_x;
		y += (h + interval_y);
		//单选框
		HWND hl = CreateWindowEx(0, _T("static"), NULL, WS_CHILD, x, y - 2, AvailableWidth / 2, edit_out_h + 4, hWnd, (HMENU)(IDC_MODELEFTBORDER), ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		HWND hr = CreateWindowEx(0, _T("static"), NULL, WS_BORDER | WS_CHILD, x + AvailableWidth / 2, y - 2, AvailableWidth / 2, edit_out_h + 4, hWnd, (HMENU)(IDC_MODERIGHTBORDER), ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		CreateWindowEx(0, L"STATIC", L"模\n\n式", WS_CHILD | WS_VISIBLE | ES_CENTER, (AvailableWidth - 20) / 2 + def_x, y - 10, 20, 60, hWnd, (HMENU)IDC_GeneralText, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		x += 2;
		RadioBox.IDC[++RadioBox.last] = CreateWindowEx(0, L"BUTTON", L"按住运行", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_GROUP, x, y, w, h, hWnd, (HMENU)1007, ((LPCREATESTRUCT)lParam)->hInstance, NULL);
		x += (AvailableWidth - w - 4);
		RadioBox.IDC[++RadioBox.last] = CreateWindowEx(0, L"BUTTON", L"切换运行", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, x, y, w, h, hWnd, (HMENU)1008, ((LPCREATESTRUCT)lParam)->hInstance, NULL);


		x = def_x;
		y += (h + interval_y);
		//更新按钮
		hWndUpBTN = CreateWindowEx(0, _T("BUTTON"), _T("更\n新"), WS_CHILD | WS_VISIBLE /*| BS_OWNERDRAW*/, x - 1/*这里是上面控件有边框*/, y, AvailableWidth + 2, h, hWnd, (HMENU)(IDC_UpBTN), ((LPCREATESTRUCT)lParam)->hInstance, NULL);


		int x1 = 0;
		int y1 = FHEIGHT - 64;
		//提示文本
		hWndHintText = CreateWindowEx(0, _T("STATIC"), _T(" 关闭中"), WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE, x1, y1, FWIDTH, 25, hWnd, (HMENU)(IDC_StatusText), ((LPCREATESTRUCT)lParam)->hInstance, NULL);

		/*初始化宏的控件*/
		CreateKeyEdit(hWnd, lParam);
		/*初始化鼠标移动控件*/
		CreateMouseEdit(hWnd, lParam);
		/*初始化预载宏控件*/
		//Init_pgpreload();
		CreatePreloadMacro(hWnd, lParam);
		/*初始化同步异步控件*/
		CreateSync(hWnd, lParam);


		/*初始化单选框check状态*/
		//SendMessageA(RadioBox.IDC[0], BM_SETCHECK, true, 0);
						/*窗口句柄 第一个标识符 最后一个标识符 想要选中的标识符*/
		CheckRadioButton(hWnd, 1007, 1008, 1007);

		/*设置字体*/
		ChildTeamSetFont(hWnd);

		/*初始化op*/
		opfileinit(hWnd, &op);
		//SendMessage(hWndHintText, WM_SETFONT, (WPARAM)hFont, NULL);
		//DrawFrame(hbox);
		//SendMessage(hbox, WM_NCPAINT, wParam, lParam);
		break;
	}

	case WM_NCPAINT:
	{
		//DrawFrame(hWnd);
		break;
	}

	/*控件将要绘制时,使用指定的设备上下文句柄来设置静态控件的文本前景色和背景色*/
	case WM_CTLCOLORSTATIC:
	{
		COLORREF rColor;
		//创建红色画刷
		HBRUSH hbrush;
		//该函数可获取指定窗口的有关信息,GWL_ID获取窗口ID
		int id = GetWindowLongPtr((HWND)lParam, GWL_ID);/*通过控件句柄获取标识符ID*/
		//MFC中的设备上下文句柄
		HDC hdc = (HDC)wParam;
		switch (id)
		{
		case IDC_StatusText:
		{
			SetTextColor(hdc, RGB(255, 255, 255));
			if (power)
			{
				rColor = RGB(202, 81, 0);
				hbrush = CreateSolidBrush(rColor);
			}
			else
			{
				rColor = RGB(0, 122, 204);
				hbrush = CreateSolidBrush(rColor);
			}
			SetBkColor(hdc, rColor);
			return (LONG)hbrush;
		}
		case IDC_TABLETITLE:
		{
			SetTextColor(hdc, RGB(245, 245, 245));
			rColor = RGB(105, 105, 105);
			hbrush = CreateSolidBrush(rColor);
			SetBkColor(hdc, rColor);
			return (LONG)hbrush;
		}
		case IDC_MacroKey:/*禁止输入的edit*/
		{
			SetTextColor(hdc, RGB(255, 246, 230));
			rColor = RGB(65, 80, 90);
			hbrush = CreateSolidBrush(rColor);
			SetBkColor(hdc, rColor);
			return (LONG)hbrush;
		}
		}
		return 0;
	}

	case WM_CTLCOLOREDIT:
	{
		COLORREF rColor;
		//创建红色画刷
		HBRUSH hbrush;
		//该函数可获取指定窗口的有关信息,GWL_ID获取窗口ID
		int id = GetWindowLongPtr((HWND)lParam, GWL_ID);/*通过控件句柄获取标识符ID*/
		//MFC中的设备上下文句柄
		HDC hdc = (HDC)wParam;
		switch (id)
		{
		case IDC_RT:
		{
			//SetTextColor(hdc, RGB(240, 240, 240));
			rColor = RGB(204, 204, 204);
			hbrush = CreateSolidBrush(rColor);
			SetBkColor(hdc, rColor);
			return (LONG)hbrush;
		}
		case IDC_MacroKey:/*允许输入的edit*/
		{
			SetTextColor(hdc, RGB(255, 246, 230));
			rColor = RGB(65, 80, 90);
			hbrush = CreateSolidBrush(rColor);
			SetBkColor(hdc, rColor);
			return (LONG)hbrush;
		}
		}
		return 0;
	}

	case WM_CTLCOLORBTN:
	{
		COLORREF rColor;
		//创建红色画刷
		HBRUSH hbrush;
		//该函数可获取指定窗口的有关信息,GWL_ID获取窗口ID
		int id = GetWindowLongPtr((HWND)lParam, GWL_ID);/*通过控件句柄获取标识符ID*/
		//MFC中的设备上下文句柄
		HDC hdc = (HDC)wParam;
		switch (id)
		{
			//case IDC_UpBTN:
			//{
			//	rColor = RGB(255, 0, 0);
			//	hbrush = CreateSolidBrush(rColor);
			//	SetBkColor(hdc, rColor);
			//	char mmp[] = "更新";
			//	SelectObject(hdc, ChFont);
			//	TextOutA(hdc, 10, 10, mmp, strlen(mmp));
			//	return (LONG)hbrush;
			//	break;
			//}
		case IDC_LOCKSYNC://1
		{
			//HWND hWnd_bro = GetDlgItem(hWnd, IDC_LOCKASYNC);
			//HDC hdc_bro = GetDC(hWnd_bro);
			rColor = RGB(67, 205, 128);//RGB(248, 248, 255);
			//SetBkColor(hdc_bro, rColor);
			//ReleaseDC(hWnd_bro, hdc_bro);

			//rColor = RGB(248, 0, 0);
			SetBkColor(hdc, rColor);
			hbrush = CreateSolidBrush(rColor);

			char mmp[] = "同\n\n步";
			SelectObject(hdc, ChFont);

			RECT r, r2;
			GetClientRect(GetDlgItem(hWnd, IDC_LOCKSYNC), &r);/*获取控件的大小 控件的左上角为0，0*/

			r2 = r;
			//GetTextMetrics(hdc, );
			//SIZE size;
			//GetTextExtentPoint32A(hdc, mmp, sizeof(mmp), &size);
			//TextOutA(hdc, (r.right - size.cx) / 2, (r.bottom - size.cy) / 2, mmp, strlen(mmp));
			DrawTextA(hdc, mmp, strlen(mmp), &r, DT_CENTER | DT_CALCRECT | DT_WORDBREAK);//DT_CALCRECT 计算字符串所占空间并赋值给r

			int h_gap = r2.bottom - r.bottom,//y轴 字符串与矩形内间距
				w_gap = r2.right - r.right;//x轴 字符串与矩形内间距

			if (h_gap > 0 || w_gap)//判断高度和宽度是否超出范围，以免出现负数情况
			{
				r.top += h_gap / 2;
				r.bottom += r.top;
				r.left += w_gap / 2;
				r.right += r.left;
			}
			DrawTextA(hdc, mmp, strlen(mmp), &r, DT_CENTER);

			//ReleaseDC(hWnd, hdc);
			return (LONG)hbrush;
			break;
		}
		case IDC_LOCKASYNC://0
		{
			//HWND hWnd_bro = GetDlgItem(hWnd, IDC_LOCKSYNC);
			//HDC hdc_bro = GetDC(hWnd_bro);
			rColor = RGB(67, 205, 128);//RGB(248, 248, 255);
			//SetBkColor(hdc_bro, rColor);
			//ReleaseDC(hWnd_bro, hdc_bro);


			SetBkColor(hdc, rColor);
			//rColor = RGB(248, 0, 0);
			hbrush = CreateSolidBrush(rColor);

			char mmp[] = "异\n\n步";
			SelectObject(hdc, ChFont);

			RECT r, r2;
			GetClientRect(GetDlgItem(hWnd, IDC_LOCKSYNC), &r);

			r2 = r;
			DrawTextA(hdc, mmp, strlen(mmp), &r, DT_CENTER | DT_CALCRECT | DT_WORDBREAK);//DT_CALCRECT 计算字符串所占空间并赋值给r

			int h_gap = r2.bottom - r.bottom,//y轴 字符串与矩形内间距
				w_gap = r2.right - r.right;//x轴 字符串与矩形内间距

			if (h_gap > 0 || w_gap)//判断高度和宽度是否超出范围，以免出现负数情况
			{
				r.top += h_gap / 2;
				r.bottom += r.top;
				r.left += w_gap / 2;
				r.right += r.left;
			}
			DrawTextA(hdc, mmp, strlen(mmp), &r, DT_CENTER);

			//ReleaseDC(hWnd, hdc);
			return (LONG)hbrush;
			break;
		}
		}
		break;
	}

	case WM_MOUSEMOVE:
	{
		//使用下面的代码获取的水平和垂直位置 头文件<windowsx.h>
		//int xPos = GET_X_LPARAM(lParam),
		//	yPos = GET_Y_LPARAM(lParam);

		POINT p;
		GetCursorPos(&p);/*获取鼠标坐标 以屏幕为坐标系*/

		char xy[20] = { 0 }, y[8] = { 0 };
		_itoa_s(p.x, xy, sizeof(xy), 10);
		strcat_s(xy, sizeof(xy), " , ");
		_itoa_s(p.y, y, sizeof(y), 10);
		strcat_s(xy, sizeof(xy), y);

		SetWindowTextA(hWnd, xy);
		break;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		HDC hdc = BeginPaint(hWnd, &ps);

		//if (mode == 0)
		//{
		//	/*绘制模式切换的边框*/
		//	ModeBorderPaint(GetDlgItem(hWnd, IDC_MODELEFTBORDER), hdc, RGB(0, 122, 204));
		ModeBorderPaint(GetDlgItem(hWnd, IDC_MODERIGHTBORDER), hdc, RGB(105, 105, 105));
		//}
		//else
		//{
		ModeBorderPaint(GetDlgItem(hWnd, IDC_MODELEFTBORDER), hdc, RGB(105, 105, 105));
		//ModeBorderPaint(GetDlgItem(hWnd, IDC_MODERIGHTBORDER), hdc, RGB(0, 122, 204));
	//}

	//if (lock == 0)
	//{
	//	COLORREF rColor = RGB(248, 248, 255);
	//	HBRUSH hbrush = CreateSolidBrush(rColor);
	//	LPRECT rec;

	//	SelectObject(hdc, hbrush);
	//	GetProgramPos(GetDlgItem(hWnd, IDC_LOCKSYNC), &rec);

	//	Rectangle(hdc, rec->left, rec->top, rec->right, rec->bottom);

	//	char mmp[] = "同\n\n步";
	//	SelectObject(hdc, ChFont);

	//	RECT r, r2;
	//	GetClientRect(GetDlgItem(hWnd, IDC_LOCKSYNC), &r);/*获取控件的大小 控件的左上角为0，0*/

	//	r2 = r;
	//	DrawTextA(hdc, mmp, strlen(mmp), &r, DT_CENTER | DT_CALCRECT | DT_WORDBREAK);//DT_CALCRECT 计算字符串所占空间并赋值给r

	//	int h_gap = r2.bottom - r.bottom,//y轴 字符串与矩形内间距
	//		w_gap = r2.right - r.right;//x轴 字符串与矩形内间距

	//	if (h_gap > 0 || w_gap)//判断高度和宽度是否超出范围，以免出现负数情况
	//	{
	//		r.top += h_gap / 2;
	//		r.bottom += r.top;
	//		r.left += w_gap / 2;
	//		r.right += r.left;
	//	}
	//	DrawTextA(hdc, mmp, strlen(mmp), &r, DT_CENTER);
	//	DeleteObject(hbrush);
	//	DeleteObject(ChFont);
	//}

	/*绘制模式切换的边框*/
	//ModeBorderPaint(GetDlgItem(hWnd, IDC_MODELEFTBORDER), RGB(0, 122, 204));
	//ModeBorderPaint(GetDlgItem(hWnd, IDC_MODERIGHTBORDER), RGB(105, 105, 105));

		ReleaseDC(hWnd, hdc);

		EndPaint(hWnd, &ps);

		break;
	}

	//case WM_KEYDOWN:
	//{
	//	HWND tmp = GetFocus();
	//	for (int i = 0;i <= FXYPA.last;i++)
	//		if (FXYPA.ArrID[i].DX_ID == tmp || FXYPA.ArrID[i].DY_ID == tmp)
	//			MessageBoxA(hWnd, "yes", NULL, 0);
	//	MessageBoxA(hWnd, "no", NULL, 0);
	//	break;
	//}

	case WM_DRAWITEM:
	{
		//int id = (UINT)wParam;
		//char a[20];
		//_itoa_s(id,a,10);
		//SetWindowTextA(hWnd, a);
		//HDC hdc = GetDC(hWnd);
		//LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT)lParam;
		//RECT itemRect = lpDIS->rcItem;  // 按钮的矩形区域
		//if (id == IDC_LOCKSYNC)
		//{
		//	//HDC hdc = GetDC(GetDlgItem(hWnd, IDC_LOCKASYNC));
		//	COLORREF rColor = RGB(248, 248, 0);
		//	HBRUSH hbrush = CreateSolidBrush(rColor);
		//	SetBkColor(hdc, rColor);
		//	//return (LONG)hbrush;
		//}
		//else if (id == IDC_LOCKASYNC)
		//{
		//	//HDC hdc = GetDC(GetDlgItem(hWnd, IDC_LOCKSYNC));
		//	COLORREF rColor = RGB(248, 248, 0);
		//	HBRUSH hbrush = CreateSolidBrush(rColor);
		//	SetBkColor(hdc, rColor);

		//	//return (LONG)hbrush;
		//}
		break;
	}
	/*按钮子窗口的WM_COMMAND消息
	wParam参数的低位是子窗口ID，高位是通知码， lParam参数是子窗口句柄。*/
	case WM_COMMAND:
	{
		COLORREF rColor = RGB(248, 248, 255);
		int low = LOWORD(wParam);

		/*预载与载入 按钮控件 start*/
		for (int i = 0;i <= IDC_PREBTN.last;i++)
		{
			if (low == IDC_PREBTN.id[i])
			{
				/*清除edit控件显示数据*/
				ClearAllEdit(hWnd);

				/*把逻辑数据写入逻辑运行组*/
				memcpy_s(KeyLogArr.Logic_Group, sizeof(KeyLogArr.Logic_Group), LoadLogArr.keylist[i].Logic_Group, sizeof(LoadLogArr.keylist[i].Logic_Group));
				memcpy_s(MovLogArr.Logic_Group, sizeof(MovLogArr.Logic_Group), LoadLogArr.movelist[i].Logic_Group, sizeof(LoadLogArr.movelist[i].Logic_Group));


				/*数据输出到edit控件*/
				char str_p[20] = { 0 };
				for (int j = 0;j <= MKA.last;j++)
				{
					/*如果当前行按键宏与移动宏均为空 则跳过*/
					if (KeyLogArr.Logic_Group[j].MacroKey != '\0')
					{
						/*设置当前行按键宏*/
						Pressed_Key_Name(KeyLogArr.Logic_Group[j].MacroKey, str_p);/*获取按键名*/
						SetWindowTextA(MKA.ArrID[j].MacroKey_ID, str_p);/*显示按键名*/

						_itoa_s(KeyLogArr.Logic_Group[j].Response_Time, str_p, sizeof(str_p), 10);/* int转char* */
						SetWindowTextA(MKA.ArrID[j].Response_ID, str_p);/*显示延迟*/


						_itoa_s(KeyLogArr.Logic_Group[j].Trigger_Time, str_p, sizeof(str_p), 10);
						SetWindowTextA(MKA.ArrID[j].Trigger_ID, str_p);/*显示按下时长*/

					}



					if (MovLogArr.Logic_Group[j].forTime != '\0')
					{
						/*设置当前行移动宏*/
						//循环
						_itoa_s(MovLogArr.Logic_Group[j].forTime, str_p, sizeof(str_p), 10);
						SetWindowTextA(FXYPA.ArrID[j].FOR_ID, str_p);
						//dx
						_itoa_s(MovLogArr.Logic_Group[j].dx, str_p, sizeof(str_p), 10);
						SetWindowTextA(FXYPA.ArrID[j].DX_ID, str_p);
						//dy
						_itoa_s(MovLogArr.Logic_Group[j].dy, str_p, sizeof(str_p), 10);
						SetWindowTextA(FXYPA.ArrID[j].DY_ID, str_p);
						//延迟
						_itoa_s(MovLogArr.Logic_Group[j].ping, str_p, sizeof(str_p), 10);
						SetWindowTextA(FXYPA.ArrID[j].PING_ID, str_p);

					}
				}
				/*发送同步或异步按钮单击消息 用户或预载按钮组设置的同步或异步*/
				SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(LoadLogArr.syncorasync[i], BN_CLICKED), lParam);
				break;
			}
		}
		/*预载与载入 按钮控件 end*/

		switch (low)
		{
		case IDC_UpBTN:
		{


			POINT p;
			GetCursorPos(&p);/*获取鼠标坐标*/

			char xy[20] = { 0 }, y[8] = { 0 };
			_itoa_s(p.x, xy, sizeof(xy), 10);
			strcat_s(xy, sizeof(xy), " , ");
			_itoa_s(p.y, y, sizeof(y), 10);
			strcat_s(xy, sizeof(xy), y);

			SetWindowTextA(hWnd, xy);


			/*获取宏数据*/
			getmacrodata();


			/*获取全局热键*/
			char str[20];
			GetWindowTextA(hWndHot, str, sizeof(str)); /*获取热键框按键组合*/
			UnregisterHotKey(hWnd, HotKey_ID);          /*注销原有热键*/
			char edg[] = " + ";
			char** hotlist = str_qie(str, edg);                /*组合键字符串拆分*/

			UINT hotnum, hotnum2, hotnum3;
			hotnum3 = Pressed_Key_VK(hotlist[0]);
			hotnum = HotKey_MOD(hotnum3);
			hotnum2 = 0;
			for (int i = 1;hotlist[i];i++)//按键为单个键时不执行
			{
				hotnum2 = Pressed_Key_VK(hotlist[i]);
				UINT hotnum_tmp = HotKey_MOD(hotnum2);
				if (hotnum_tmp == 0)
					break;
				else hotnum |= hotnum_tmp;
			}
			if (hotnum == 0 && hotnum2 == 0 && hotnum3 == -1)//按键在程序设计之外
				RegisterHotKey(hWnd, HotKey_ID, NULL, NULL);
			else if (hotnum == 0)													//按键为单个普通按键
				RegisterHotKey(hWnd, HotKey_ID, NULL, hotnum3);
			else if (hotnum2 == 0)												//按键为单个特殊按键
				RegisterHotKey(hWnd, HotKey_ID, hotnum, NULL);
			else																			//按键为特殊键+普通键的组合键
				RegisterHotKey(hWnd, HotKey_ID, hotnum, hotnum2);



			/*获取触发键*/
			GetWindowTextA(hWndtrigger, str, sizeof(str));
			hotlist = str_qie(str, edg);                    /*组合键字符串拆分*/
			T_k.last = -1;                                      /*初始化组合键数组下标*/
			for (int i = 0;hotlist[i];i++)                  /*获取组合键虚拟键码*/
				T_k.vk_code[++T_k.last] = Pressed_Key_VK(hotlist[i]);

			MessageBox(hWnd, _T("更新成功"), _T("提示"), MB_OK);

			break;
		}

		case IDC_LOCKSYNC:/*同步*/
		{
			lock = 1;

			SetWindowTextA(GetDlgItem(hWnd, IDC_ASYNCORSYNCHINT), "-同步-");
			break;
		}

		case IDC_LOCKASYNC:/*异步*/
		{
			lock = 0;

			SetWindowTextA(GetDlgItem(hWnd, IDC_ASYNCORSYNCHINT), "-异步-");
			//RECT rc;
			//HWND hbox = GetDlgItem(hWnd, IDC_LOCKSYNC);
			//GetClientRect(hbox, &rc);
			//InvalidateRect(hWnd, &rc, true);
			//UpdateWindow(hWnd);
			break;
		}

		case IDC_SAVEFILEBTN:/*保存*/
		{
			int flag = GetSaveFileNameA(&op);
			if (flag)//保存(1)-取消(0)
			{
				if (savedatatolocal())
					MessageBox(hWnd, _T("保存成功"), _T("提示"), 0);
				else MessageBox(hWnd, _T("没有数据需要保存"), _T("错误"), 0);

				//ofstream fout(op.lpstrFile);
				//fout << "123" << endl;
				//fout << "456" << endl;
				//fout.close();
				//SetWindowText(hWnd, _T("123"));
			}
			else
			{

				//SetWindowText(hWnd, _T("456"));
			}
			break;
		}
		case IDC_LOADFILEBTN:
		{
			if (GetOpenFileNameA(&op))
			{
				if (loadlocaldata())
					MessageBox(hWnd, _T("载入成功"), _T("提示"), 0);
				else MessageBox(hWnd, _T("文件打开失败"), _T("错误"), 0);
			}
			break;
		}

		case 1007:/*单选框“按住运行”被单击时*/
		{
			mode = 0;


			//RECT rc;
			//HWND hbox = GetDlgItem(hWnd, IDC_MODELEFTBORDER);
			//GetClientRect(hbox, &rc);
			//InvalidateRect(hbox, &rc, true);
			//UpdateWindow(hbox);

			break;
		}
		case 1008:/*单选框“切换运行”被单击时*/
		{
			mode = 1;

			RECT rc;
			HWND hbox = GetDlgItem(hWnd, IDC_MODERIGHTBORDER);
			GetClientRect(hbox, &rc);
			//InvalidateRect(hWnd, &rc, true);
			//UpdateWindow(hWnd);
			//RedrawWindow(hWnd, &rc, nullptr, RDW_INTERNALPAINT | RDW_INVALIDATE | RDW_UPDATENOW);




			break;
		}
		}
		break;
	}

	//case WM_SETFOCUS:
	//{
	//    // MessageBox(hWnd,TEXT("in"),NULL,MB_OK);
	//    HWND nowhWnd = GetFocus();
	//    SetWindowText(nowhWnd, _T("123"));
	//    break;
	//}
	//case BM_GETCHECK:
	//	MessageBox(hWnd, TEXT("in"), NULL, MB_OK);
	//	break;


	/*热键消息*/
	case WM_HOTKEY:
	{
		switch (wParam)
		{
		case HotKey_ID:
		{
			(++power) %= 2;
			if (power)
			{
				SetWindowText(hWndHintText, _T(" 开启中"));
				SendMessage(hWnd, WM_CTLCOLORSTATIC, wParam, lParam);
				//InvalidateRect(hWndHintText, NULL, false);/*重画时不擦除背景*/
			}
			else
			{
				SetWindowText(hWndHintText, _T(" 关闭中"));
				//InvalidateRect(hWndHintText, NULL, false);/*重画时不擦除背景*/
				SendMessage(hWnd, WM_CTLCOLORSTATIC, wParam, lParam);
			}
			break;
		}
		}
		break;
	}
	case WM_CLOSE://窗口关闭消息
	{
		se = 0;//关闭线程
		Sleep(500);/*等线程关闭*/
		DestroyWindow(hWnd);//销毁窗口
		break;
	}
	case WM_DESTROY://窗口销毁消息
		PostQuitMessage(0);//发送WM_QUIT消息，退出
		break;

	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//当前按下的最后一个按键
//char getname[10];

/**
 * 获取按下的键名
 * 返回-1为不识别按键
 * 返回0为普通按键
 * 返回1为ctrl、shift、alt、win
 * */
int Pressed_Key_Name(int VK_CODE, char* getname)
{
	int x, size = 20;

	if ((VK_CODE >= '0' && VK_CODE <= '9') || (VK_CODE >= 'A' && VK_CODE <= 'Z'))
	{
		sprintf_s(getname, size, "%c", VK_CODE);
		return 0;
	}

	if (VK_CODE >= VK_F1 && VK_CODE <= VK_F12)/*112-123*/
	{
		x = VK_CODE - 111;
		sprintf_s(getname, size, "F%d", x);
		return 0;
	}

	if (VK_CODE >= VK_NUMPAD0 && VK_CODE <= VK_NUMPAD9)/*96-105*/
	{
		x = VK_CODE - 96;
		sprintf_s(getname, size, "NUMPAD%d", x);
		return 0;
	}
	if (VK_CODE == VK_LBUTTON)
	{
		sprintf_s(getname, size, "LBUTTON");
		return 0;
	}
	if (VK_CODE == VK_MBUTTON)
	{
		sprintf_s(getname, size, "MBUTTON");
		return 0;
	}
	if (VK_CODE == VK_RBUTTON)
	{
		sprintf_s(getname, size, "RBUTTON");
		return 0;
	}

	/*非自福建(Non-character keys)*/
	switch (VK_CODE)
	{
	case VK_BACK:			//0x08
		sprintf_s(getname, size, "%s", "Backspace");
		return 0;
	case VK_TAB:            //0x09
		sprintf_s(getname, size, "%s", "Tab");
		return 0;
	case VK_RETURN:      //0x0D
		sprintf_s(getname, size, "%s", "Enter");
		return 0;
	case VK_SPACE:        //0x20
		sprintf_s(getname, size, "%s", "Space");
		return 0;
	case VK_LEFT:           //0x25
		sprintf_s(getname, size, "%s", "Left");
		return 0;
	case VK_UP:              //0x26
		sprintf_s(getname, size, "%s", "Up");
		return 0;
	case VK_RIGHT:         //0x27
		sprintf_s(getname, size, "%s", "Right");
		return 0;
	case VK_DOWN:         //0x28
		sprintf_s(getname, size, "%s", "Down");
		return 0;
	default:
		break;
	}

	//PorKey
	switch (VK_CODE)
	{
	case VK_CONTROL:
	{
		sprintf_s(getname, size, "%s", "Ctrl");
		return 1;
	}
	case VK_SHIFT:
	{
		sprintf_s(getname, size, "%s", "Shift");
		return 1;
	}
	case VK_MENU:
	{
		sprintf_s(getname, size, "%s", "Alt");
		return 1;
	}
	case VK_LWIN:
	{
		sprintf_s(getname, size, "%s", "WIN");
		return 1;
	}
	}
	return -1;
}

//通过键名获取虚拟键码
int Pressed_Key_VK(char* key_name)
{
	//PorKey
	if (strcmp(key_name, "Ctrl") == 0)
		return VK_CONTROL;
	if (strcmp(key_name, "Shift") == 0)
		return VK_SHIFT;
	if (strcmp(key_name, "Alt") == 0)
		return VK_MENU;
	if (strcmp(key_name, "WIN") == 0)
		return VK_LWIN;
	if (strcmp(key_name, "LBUTTON") == 0)
		return VK_LBUTTON;
	if (strcmp(key_name, "MBUTTON") == 0)
		return VK_MBUTTON;
	if (strcmp(key_name, "RBUTTON") == 0)
		return VK_RBUTTON;
	if (strcmp(key_name, "XBUTTON1") == 0)
		return VK_XBUTTON1;
	if (strcmp(key_name, "XBUTTON2") == 0)
		return VK_XBUTTON1;

	/*非自福建(Non-character keys)*/
	if (strcmp(key_name, "Tab") == 0)
		return VK_TAB;
	if (strcmp(key_name, "Enter") == 0)
		return VK_RETURN;
	if (strcmp(key_name, "Space") == 0)
		return VK_SPACE;
	if (strcmp(key_name, "Left") == 0)
		return VK_LEFT;
	if (strcmp(key_name, "Up") == 0)
		return VK_UP;
	if (strcmp(key_name, "Right") == 0)
		return VK_RIGHT;
	if (strcmp(key_name, "Down") == 0)
		return VK_DOWN;


	/*0-9&A-Z*/
	int x = key_name[0];
	if (strlen(key_name) == 1 && ((x >= '0' && x <= '9') || (x >= 'A' && x <= 'Z')))
		return x;

	/*F1-F12*/
	x = atoi(&key_name[1]) + 111;
	if ((strlen(key_name) == 2 || strlen(key_name) == 3) && x >= VK_F1 && x <= VK_F12)
		return x;

	/*NUMPAD0-NUMPAD0*/
	x = atoi(&key_name[6]) + 96;
	if (strlen(key_name) == 7 && x >= VK_NUMPAD0 && x <= VK_NUMPAD9)
		return x;



	return -1;
}


/*获取热键*/
UINT HotKey_MOD(int vk_code)
{
	switch (vk_code)
	{
	case VK_CONTROL:
		return MOD_CONTROL;
	case VK_SHIFT:
		return MOD_SHIFT;
	case VK_LWIN:
		return MOD_WIN;
	case VK_MENU:
		return MOD_ALT;
	}
	return 0;
}

void MacroDown(HWND hWnd, char* str_KeyName)
{
	//strcpy_s(str_KeyName, size, "XBUTTON1");
	SetWindowTextA(hWnd, str_KeyName);

	/*若未设置延迟和时长 程序将按默认值填充*/
	Auto_MK_Time(hWnd, &MKA, 5, 10);
}


//字符串切割真意 返回值为切割二维数组    判断 例：str[i]
char** str_qie(char* str, char* CutSymbol)/*参数1-待切字符串  参数2-分割符*/
{
	int len = 10;
	char** strlist = new char* [5];

	int i;//下标

	for (i = 0;i < 5;i++)
		strlist[i] = new char[len];
	i = 0;
	char* buf = { 0 };//用来在strtok_s内部保存切分时的上下文，以应对连续调用分解相同源字符串

	//分割出的子字符串赋给substr
	char* substr = strtok_s(str, CutSymbol, &buf);/*利用现成的分割函数,substr为分割出来的子字符串*/
	while (substr != NULL)
	{
		strcpy_s(strlist[i], len, substr);
		i++;
		substr = strtok_s(NULL, CutSymbol, &buf);
	}
	strlist[i] = { 0 };
	return strlist;
}

/*用户设置新宏时 若未设置时间 默认设置->响应延迟为5 触发时长为10*/
void Auto_MK_Time(HWND hWnd_, MacroKey_Arr_ID* MKA_, int Response_Time, int Trigger_Time)
{

	for (int i = 0;i <= MKA_->last;i++)
	{
		if (MKA_->ArrID[i].MacroKey_ID == hWnd_)
		{
			char str1[10], str2[10];
			GetWindowTextA(MKA_->ArrID[i].Response_ID, str1, sizeof(str1));
			GetWindowTextA(MKA_->ArrID[i].Trigger_ID, str2, sizeof(str2));
			if (str1[0] == '\0' && str2[0] == '\0')
			{
				_itoa_s(Response_Time, str1, sizeof(str1), 10);
				SetWindowTextA(MKA_->ArrID[i].Response_ID, str1);
				_itoa_s(Trigger_Time, str1, sizeof(str1), 10);
				SetWindowTextA(MKA_->ArrID[i].Trigger_ID, str1);
			}

		}
	}
}

/*判断触发组合键都处于按下状态否*/
int KeyDownForWhat(KeyArr* T)
{
	for (int i = 0;i <= T->last;i++)
		if (!ISKEYDOWN(T->vk_code[i]))
			return 0;
	return 1;
}

/*宏响应为 键盘或鼠标*/
int* KeyOrMouse(int vk_code)
{
	int* vk = new int[3];
	vk[0] = 1;
	switch (vk_code)
	{
	case VK_LBUTTON:
	{
		vk[0] = 0;
		vk[1] = MOUSEEVENTF_LEFTDOWN;
		vk[2] = MOUSEEVENTF_LEFTUP;
		return vk;
	}
	case VK_MBUTTON:
	{
		vk[0] = 0;
		vk[1] = MOUSEEVENTF_MIDDLEDOWN;
		vk[2] = MOUSEEVENTF_MIDDLEUP;
		return vk;
	}
	case VK_RBUTTON:
	{
		vk[0] = 0;
		vk[1] = MOUSEEVENTF_RIGHTDOWN;
		vk[2] = MOUSEEVENTF_RIGHTUP;
		return vk;
	}
	case VK_XBUTTON1:
	{
		vk[0] = 0;
		vk[1] = MOUSEEVENTF_XDOWN;
		vk[2] = MOUSEEVENTF_XUP;
	}
	case VK_XBUTTON2:
	{
		vk[0] = 0;
		vk[1] = MOUSEEVENTF_XDOWN;
		vk[2] = MOUSEEVENTF_XUP;
	}
	default:return vk;
	}
}

//延迟函数
void  Delay(int time)//time为毫秒数
{
	clock_t now = clock();

	while (clock() - now < time && se);
}

/*宏运行*/
void macrorun(int i)
{
	/* 跳过没有设定宏的编辑框*/
	if (KeyLogArr.Logic_Group[i].MacroKey == -1 || KeyLogArr.Logic_Group[i].MacroKey == '\0')
		return;

	//宏响应延迟
	Delay(KeyLogArr.Logic_Group[i].Response_Time);
	// mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
	int* km = KeyOrMouse(KeyLogArr.Logic_Group[i].MacroKey);
	if (km[0] == 1)/*设定为键盘按下*/
	{
		//按下设定的宏按键
		keybd_event(KeyLogArr.Logic_Group[i].MacroKey, MapVirtualKey(KeyLogArr.Logic_Group[i].MacroKey, 0), 0, 0);
		//宏的按下时长
		Delay(KeyLogArr.Logic_Group[i].Trigger_Time);
		//弹起设定的宏按键
		keybd_event(KeyLogArr.Logic_Group[i].MacroKey, MapVirtualKey(KeyLogArr.Logic_Group[i].MacroKey, 0), KEYEVENTF_KEYUP, 0);
	}
	else//鼠标按下
	{
		mouse_event(km[1], 0, 0, 0, 0);
		Delay(KeyLogArr.Logic_Group[i].Trigger_Time);
		mouse_event(km[2], 0, 0, 0, 0);
		//mouse_event(MOUSEEVENTF_MIDDLEDOWN | MOUSEEVENTF_MIDDLEUP, 0, 0, 0, 0);
		//mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
	}
}

/*按键宏模式选择*/
int KeyModeCheck()
{
	int p = 1;/*判断是否处于长按状态*/
	while (se)/*线程开关*/
	{
		while (mode == 1 && se)/*切换模式の开关*/
		{
			int g = 0, i = 0;
			/*p==1并且触发键处于按下状态*/
			if (p == 1 && KeyDownForWhat(&T_k) == 1)
			{
				++g %= 2;
				p = 0;/*代表触发键处于按下状态*/
			}
			/*p==0并且触发键从按下状态变为弹起状态*/
			if (p == 0 && KeyDownForWhat(&T_k) == 0)
				p = 1;/*代表触发键处于弹起状态*/
			while (power && g && mode == 1 && se)
			{
				if (p == 0 && KeyDownForWhat(&T_k) == 0)
					p = 1;
				/*如果触发键一直处于按下状态时,p是等于0的,不执行g的改变*/
				if (p == 1 && KeyDownForWhat(&T_k) == 1)
				{
					++g %= 2;
					p = 0;
				}
				keyCompare = 0;
				macrorun(i);

				keyCompare = 1;
				while (lock && se && mode == 1 && power && keyCompare != mouseCompare)
				{
					if (p == 0 && KeyDownForWhat(&T_k) == 0)
						p = 1;
					/*如果触发键一直处于按下状态时,p是等于0的,不执行g的改变*/
					if (p == 1 && KeyDownForWhat(&T_k) == 1)
					{
						++g %= 2;
						p = 0;
					}
				}
				(++i) %= (MKA.last + 1);
			}
		}
		while (((mode + 1) % 2) == 1 && se)/*按住模式の开关*/
		{
			int i = 0;
			/*power 热键功能是否启动 */
			while (power && KeyDownForWhat(&T_k) == 1 && se)/*判断触发组合键是否按下*/
			{
				keyCompare = 0;
				macrorun(i);
				keyCompare = 1;
				while (lock && se && power && ((mode + 1) % 2) == 1 && KeyDownForWhat(&T_k) == 1 && mouseCompare != keyCompare);
				//int movtime = MovLogArr.Logic_Group[i].forTime * MovLogArr.Logic_Group[i].ping,
				//	keytime = KeyLogArr.Logic_Group[i].Response_Time + KeyLogArr.Logic_Group[i].Trigger_Time;
				//if (lock && se && power && (keytime - movtime) < 0)/*判断同步否*/
				//	Delay(movtime - keytime);
				(++i) %= (MKA.last + 1);
			}
		}
	}
	return 0;
}

/*宏运行*/
void moverun(int i)
{
	if (MovLogArr.Logic_Group[i].forTime == '\0' /*|| MovLogArr.Logic_Group[i].ping == '\0'*/)
		return;
	mouse_event(MOUSEEVENTF_MOVE, MovLogArr.Logic_Group[i].dx, MovLogArr.Logic_Group[i].dy, 0, 0);
	Delay(MovLogArr.Logic_Group[i].ping);
}

/*鼠标移动模式切换*/
int MoveModeCheck()
{
	while (se)/*线程开关*/
	{
		int p = 1;/*判断是否处于长按状态*/
		while (mode == 1 && se)/*切换模式の开关*/
		{
			int g = 0, i = 0;
			/*p==1并且触发键处于按下状态*/
			if (p == 1 && KeyDownForWhat(&T_k) == 1)
			{
				++g %= 2;
				p = 0;/*代表触发键处于按下状态*/
			}
			/*p==0并且触发键从按下状态变为弹起状态*/
			if (p == 0 && KeyDownForWhat(&T_k) == 0)
				p = 1;/*代表触发键处于弹起状态*/
			while (power && g && mode == 1 && se)
			{
				if (p == 0 && KeyDownForWhat(&T_k) == 0)
					p = 1;
				/*如果触发键一直处于按下状态时,p是等于0的,不执行g的改变*/
				if (p == 1 && KeyDownForWhat(&T_k) == 1)
				{
					++g %= 2;
					p = 0;
				}
				int j, k = MovLogArr.Logic_Group[i].forTime;
				for (j = 0;j < k && se == 1;j++)
				{
					if (!(power && g && mode == 1))
						break;
					if (p == 0 && KeyDownForWhat(&T_k) == 0)
					{
						p = 1;
						break;
					}
					/*如果触发键一直处于按下状态时,p是等于0的,不执行g的改变*/
					if (p == 1 && KeyDownForWhat(&T_k) == 1)
					{
						++g %= 2;
						p = 0;
						break;
					}
					mouseCompare = 0;
					moverun(i);
				}
				mouseCompare = 1;
				/*mouseCompare != keyCompare 表示按键宏处于运行中 鼠标宏需要等待*/
				while (lock && se && mode == 1 && power && mouseCompare != keyCompare)
				{
					if (p == 0 && KeyDownForWhat(&T_k) == 0)
					{
						p = 1;
						break;
					}
					/*如果触发键一直处于按下状态时,p是等于0的,不执行g的改变*/
					if (p == 1 && KeyDownForWhat(&T_k) == 1)
					{
						++g %= 2;
						p = 0;
						break;
					}
				}
				(++i) %= (FXYPA.last + 1);
			}
		}
		while (((mode + 1) % 2) == 1 && se)/*按住模式の开关*/
		{
			int i = 0;
			/*power 热键功能是否启动 */
			while (power && KeyDownForWhat(&T_k) == 1 && se)/*判断触发组合键是否按下*/
			{
				int j, k = MovLogArr.Logic_Group[i].forTime;
				for (j = 0;j < k && se == 1;j++)
				{
					if (!(power && KeyDownForWhat(&T_k) == 1) || ((mode + 1) % 2) == 0)
						break;
					mouseCompare = 0;
					moverun(i);
				}
				mouseCompare = 1;
				while (lock && se && power && ((mode + 1) % 2) == 1 && KeyDownForWhat(&T_k) == 1 && mouseCompare != keyCompare);
				//int movtime = MovLogArr.Logic_Group[i].forTime * MovLogArr.Logic_Group[i].ping,
				//	keytime = KeyLogArr.Logic_Group[i].Response_Time + KeyLogArr.Logic_Group[i].Trigger_Time;
				//if (lock && se && power && (movtime - keytime) < 0)/*判断同步否*/
				//	Delay(keytime - movtime);
				(++i) %= (FXYPA.last + 1);
			}
		}
	}
	return 0;
}



int charat(char* str, char key)
{
	for (int i = 0;str[i];i++)
		if (str[i] == key)
			return i;
	return -1;
}


/*清扬-我无懈可击-无需隐藏*/
void ClearAllEdit(HWND hWnd_)
{
	HWND Child, NextChild;
	Child = GetWindow(hWnd_, GW_CHILD);/*返回Tab序顶端的子窗口的句柄*/
	NextChild = Child;
	while (NextChild != NULL)
	{
		int id = GetDlgCtrlID(NextChild);
		if (id == IDC_RT || id == IDC_MacroKey)
			SetWindowText(NextChild, NULL);
		Child = NextChild;
		NextChild = GetNextWindow(Child, GW_HWNDNEXT);
	}
}

/*获取控件以程序窗口为坐标系的坐标*/
void GetProgramPos(HWND hWnd_, LPRECT* rect)
{
	RECT prerc, childrc;

	GetWindowRect(hWnd, &prerc);
	GetWindowRect(hWnd_, &childrc);

	int left = childrc.left - prerc.left - 8,
		top = childrc.top - prerc.top - 31,
		right = childrc.right - childrc.left + left,
		bottom = childrc.bottom - prerc.top - 31;
	RECT r{ left, top, right, bottom };

	*rect = &r;
}

/*模式切换控件边框绘画*/
void ModeBorderPaint(HWND hWnd_, HDC hdc, COLORREF rColor)
//void ModeBorderPaint(HWND hWnd_, COLORREF rColor)
{

	//RECT prerc, childrc;

	/*设置画笔*/
	HPEN hPen = CreatePen(
		PS_SOLID/*画笔画出的是实线*/,
		1, /*画笔的宽度*/
		rColor);/*指定的样式、宽度和颜色创建画笔*/
	SelectObject(hdc, hPen);/*选择hPen到指定的设备上下文环境中，该新对象替换先前的相同类型的对象*/


	LPRECT r = NULL;
	GetProgramPos(hWnd_, &r);

	Rectangle(hdc, r->left, r->top, r->right, r->bottom);

	DeleteObject(hPen);/*删除一个逻辑笔*/

}


/*OPENFILENAME初始化*/
void opfileinit(HWND hWnd_, LPOPENFILENAMEA lpofn)
{

	GetModuleFileNameA(NULL, strInitialDir, MAX_PATH);
	string s_tmp = strInitialDir;
	regex r("(.*)(?:\\\\.*)");
	s_tmp = regex_replace(s_tmp, r, "$1");//获取捕获分组1 当前应用程序所在路径
	sprintf_s(strInitialDir, sizeof(strInitialDir), s_tmp.data());

	lpofn->lStructSize = sizeof(OPENFILENAME);
	lpofn->hwndOwner = hWnd_;// 指定它的父窗口
	lpofn->hInstance = NULL;
	lpofn->lpstrFilter = "Text File(.txt)\0*.txt\0All File(.*)\0*.*\0\0";//设置过滤器
	lpofn->lpstrCustomFilter = NULL;
	lpofn->nMaxCustFilter = NULL;/*上面缓冲区大小*/
	lpofn->nFilterIndex = 0;
	lpofn->lpstrFile = strFile;//初始化文件名编辑控件的文件名,包含驱动器标识符，路径，文件名和所选文件的扩展名
	lpofn->nMaxFile = MAX_PATH;//指向的缓冲区的大小（以字符为单位）,缓冲区必须足够大以存储路径和文件名字符串，包括终止NULL字符
	lpofn->lpstrFileTitle = strFileTitle;//所选文件的文件名和扩展名（无路径信息）
	lpofn->nMaxFileTitle = MAX_PATH;/*上面缓冲区大小*/
	lpofn->lpstrInitialDir = strInitialDir;//初始目录 NULL为“我的文档”
	lpofn->lpstrTitle = NULL;
	lpofn->Flags = 0;
	lpofn->nFileOffset = 0;
	lpofn->nFileExtension = 0;
	lpofn->lpstrDefExt = "txt";//保存的时候忘记加.txt时自动加上
	lpofn->lCustData = 0;
	lpofn->lpfnHook = NULL;
	lpofn->lpTemplateName = NULL;//定制对话框不能为NULL
}

/*获取宏数据*/
int getmacrodata()
{
	int f1 = 0, f2 = f1;
	/*获取宏的设置*/
	for (int i = 0;i < (int)MacroKeyMaxNum;i++)
	{
		//if (MKA.ArrID[i].MacroKey_ID == NULL) break;

		char strText1[20], strText2[20], strText3[20], strText4[20];

		/*获取按键宏*/
		//获取响应延迟
		GetWindowTextA(MKA.ArrID[i].Response_ID, strText1, sizeof(strText1));
		KeyLogArr.Logic_Group[i].Response_Time = atoi(strText1);

		//获取响应键
		GetWindowTextA(MKA.ArrID[i].MacroKey_ID, strText2, sizeof(strText2));
		KeyLogArr.Logic_Group[i].MacroKey = Pressed_Key_VK(strText2);

		//获取触发时长
		GetWindowTextA(MKA.ArrID[i].Trigger_ID, strText3, sizeof(strText3));
		KeyLogArr.Logic_Group[i].Trigger_Time = atoi(strText3);

		/*若设置了宏 但未设置延迟或时长 将默认视为0*/
		if (strText2[0] != '\0')
		{
			f1 = 1;//有键就算
			if (strText1[0] == '\0')
				SetWindowTextA(MKA.ArrID[i].Response_ID, "0");
			if (strText3[0] == '\0')
				SetWindowTextA(MKA.ArrID[i].Trigger_ID, "0");
		}


		/*获取移动宏*/
		//获取轮回次数
		GetWindowTextA(FXYPA.ArrID[i].FOR_ID, strText1, sizeof(strText1));
		MovLogArr.Logic_Group[i].forTime = atoi(strText1);

		//获取dx
		GetWindowTextA(FXYPA.ArrID[i].DX_ID, strText2, sizeof(strText2));
		MovLogArr.Logic_Group[i].dx = atoi(strText2);

		//获取dy
		GetWindowTextA(FXYPA.ArrID[i].DY_ID, strText3, sizeof(strText3));
		MovLogArr.Logic_Group[i].dy = atoi(strText3);

		//获取ping
		GetWindowTextA(FXYPA.ArrID[i].PING_ID, strText4, sizeof(strText4));
		MovLogArr.Logic_Group[i].ping = atoi(strText4);

		if (strText1[0] != '\0') f2 = 2;//有循环次数就算 x,y为0没问题

		if (strText2[0] != '\0' && strText3[0] != '\0')
		{
			if (strText1[0] == '\0')
				SetWindowTextA(FXYPA.ArrID[i].FOR_ID, "0");
			if (strText4[0] == '\0')
				SetWindowTextA(FXYPA.ArrID[i].PING_ID, "0");
		}
		else if (strText2[0] != '\0')
		{
			SetWindowTextA(FXYPA.ArrID[i].DY_ID, "0");
			if (strText1[0] == '\0')
				SetWindowTextA(FXYPA.ArrID[i].FOR_ID, "0");
			if (strText4[0] == '\0')
				SetWindowTextA(FXYPA.ArrID[i].PING_ID, "0");
		}
		else if (strText3[0] != '\0')
		{
			SetWindowTextA(FXYPA.ArrID[i].DX_ID, "0");
			if (strText1[0] == '\0')
				SetWindowTextA(FXYPA.ArrID[i].FOR_ID, "0");
			if (strText4[0] == '\0')
				SetWindowTextA(FXYPA.ArrID[i].PING_ID, "0");
		}
	}
	return f1 + f2;
}

/*保存数据至本地*/
int savedatatolocal()
{
	char gap[] = ",";
	int f = getmacrodata();
	if (f == 0) return 0;
	ofstream fout(op.lpstrFile);//创建一个文件输出流对象fout来打开文件 
	if (lock == 0)/*0为异步*/
		fout << "#ASYNC" << endl;
	else fout << "#SYNC" << endl;

	fout /*<< "#Start\n"*/ << "#Key-S" << endl;
	for (int i = 0;MKA.ArrID[i].MacroKey_ID;i++)
	{
		if (f != 1 && f != 3) break;//1表示只有按键宏设有键 3表示按键宏与移动宏都设有键
		//if (KeyLogArr.Logic_Group[i].MacroKey == -1 || KeyLogArr.Logic_Group[i].MacroKey == '\0')
		//	continue;
		fout << KeyLogArr.Logic_Group[i].Response_Time << gap
			<< KeyLogArr.Logic_Group[i].MacroKey << gap
			<< KeyLogArr.Logic_Group[i].Trigger_Time << endl;
	}
	fout << "#Key-E\n#Mouse-S" << endl;
	for (int i = 0;MKA.ArrID[i].MacroKey_ID;i++)
	{
		if (f != 2 && f != 3) break;//2表示只有移动宏设有键 3表示按键宏与移动宏都设有键
		//if (MovLogArr.Logic_Group[i].forTime == '\0')
		//	continue;
		fout << MovLogArr.Logic_Group[i].forTime << gap
			<< MovLogArr.Logic_Group[i].dx << gap
			<< MovLogArr.Logic_Group[i].dy << gap
			<< MovLogArr.Logic_Group[i].ping << endl;
	}
	fout << "#Mouse-E\n" <</* "#End" <<*/ endl;
	fout.close();//关闭文件输出流
	return 1;
}

/*载入本地数据*/
int loadlocaldata()
{
	char gap[] = ",";
	ifstream fin(op.lpstrFile);//创建一个文件输入流对象
	if (fin.fail())
	{
		fin.close();
		return 0;
	}

	char btnname[50];
	sprintf_s(btnname, sizeof(btnname), op.lpstrFileTitle);/*把文件名+扩展名写入缓冲区*/
	string b_tmp = btnname;
	regex r("(.*)(?:\\..*)");
	b_tmp = regex_replace(b_tmp, r, "$1");/*获取捕获分组1 文件名*/
	//IDC_PREBTN.name[++IDC_PREBTN.last] = new char[8];/*设置按钮文本 并让预载宏控件id组长度+1*/
	sprintf_s(IDC_PREBTN.name[++IDC_PREBTN.last], sizeof(IDC_PREBTN.name[IDC_PREBTN.last]), b_tmp.data());
	//设置新增的按钮控件的标识符为上一个按钮控件的标识符+1
	IDC_PREBTN.id[IDC_PREBTN.last] = IDC_PREBTN.id[IDC_PREBTN.last - 1] + 1;

	char strline[50] = { 0 };
	int flag = 0, i = 0;
	fin.getline(strline, sizeof(strline));
	while (1)
	{
		//if (strcmp(strline, "#Start")) flag = 0;//1为获取按键宏数据
		if (flag == 0 && !strcmp(strline, "#Key-S"))
		{
			flag = 1;
			fin.getline(strline, sizeof(strline));
			//continue;
		}
		if (flag == 1 && !strcmp(strline, "#Key-E"))
		{
			flag = 0;
			i = 0;
			fin.getline(strline, sizeof(strline));
			//continue;
		}
		if (flag == 0 && !strcmp(strline, "#Mouse-S"))
		{
			flag = 2;
			fin.getline(strline, sizeof(strline));
			//continue;
		}
		if (flag == 2 && !strcmp(strline, "#Mouse-E"))
		{
			flag = 0;
			i = 0;
			CreatePreloadBTN(::hWnd, IDC_PREBTN.last);
			break;
		}
		if (!strcmp(strline, "#ASYNC"))
			LoadLogArr.syncorasync[++LoadLogArr.last] = IDC_LOCKASYNC;
		else if (!strcmp(strline, "#SYNC"))
			LoadLogArr.syncorasync[++LoadLogArr.last] = IDC_LOCKSYNC;

		if (flag == 1)//载入按键宏
		{
			/*拆分数据*/
			char** macro = str_qie(strline, gap);
			int mk = atoi(macro[1]);
			if (mk != '\0' && mk != -1)
			{
				//载入响应时间
				LoadLogArr.keylist[LoadLogArr.last].Logic_Group[i].Response_Time = atoi(macro[0]);
				//SetWindowTextA(MKA.ArrID[i].Response_ID, macro[0]);//输出到edit控件

				//载入按键宏
				LoadLogArr.keylist[LoadLogArr.last].Logic_Group[i].MacroKey = atoi(macro[1]);
				Pressed_Key_Name(atoi(macro[1]), strline);
				//SetWindowTextA(MKA.ArrID[i].MacroKey_ID, strline);//输出按键名到edit控件

				//载入触发时长
				LoadLogArr.keylist[LoadLogArr.last].Logic_Group[i].Trigger_Time = atoi(macro[2]);
				//SetWindowTextA(MKA.ArrID[i].Trigger_ID, macro[2]);//输出触发时长到edit控件

			}


			i++;
		}
		else if (flag == 2)//载入鼠标移动宏
		{
			char** move = str_qie(strline, gap);
			int ft = atoi(move[0]);
			if (ft != '\0' && ft != 0)
			{
				//载入循环次数
				LoadLogArr.movelist[LoadLogArr.last].Logic_Group[i].forTime = ft;
				//SetWindowTextA(FXYPA.ArrID[i].FOR_ID, move[0]);//输出循环次数到edit控件

				//载入dx x轴位移量
				LoadLogArr.movelist[LoadLogArr.last].Logic_Group[i].dx = atoi(move[1]);
				//SetWindowTextA(FXYPA.ArrID[i].DX_ID, move[1]);//输出dy到edit控件

				//载入dy y轴位移量
				LoadLogArr.movelist[LoadLogArr.last].Logic_Group[i].dy = atoi(move[2]);
				//SetWindowTextA(FXYPA.ArrID[i].DY_ID, move[2]);//输出dy到edit控件

				////载入延迟ping
				LoadLogArr.movelist[LoadLogArr.last].Logic_Group[i].ping = atoi(move[3]);
				//SetWindowTextA(FXYPA.ArrID[i].PING_ID, move[3]);//输出延迟到edit控件

			}


			i++;
		}
		fin.getline(strline, sizeof(strline));
	}
	fin.close();
	return 1;
}

