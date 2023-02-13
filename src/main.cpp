#include <Windows.h>
#include <gdiplus.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "headerres.h"

using namespace Gdiplus;

typedef int INT;

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdiplus.lib")

#define TREE_HEIGHT_MULTIPLIER 5

#define EXIT_IF(cond, lpszMsg, lpszCaption) \
    if ((cond))                             \
    {                                       \
        MessageBox((HWND)NULL,              \
                   TEXT(lpszMsg),           \
                   TEXT(lpszCaption),       \
                   MB_ICONERROR);           \
        ExitProcess(EXIT_FAILURE);          \
    }

#define SEARCHING_TIMER      1501

#define MAX_HEIGHT           5
#define RADIUS               30
#define ROW_HEIGHT           100

#define TRUE                 1
#define FALSE                0

#define SUCCESS              1 
#define FAILURE              0

// STACK ERRORS
#define STACK_EMPTY          2
#define STACK_OVERFLOW       3
#define STACK_UNDERFLOW      4

// BINARY SEARCH TREE ERRORS
#define TREE_EMPTY           2
#define TREE_DATA_NOT_FOUND  3
#define TREE_NO_SUCCESSOR    4
#define TREE_NO_PREDECESSOR  5

// Node Colors
#define NO_COLOR             0
#define DATA_FOUND_COLOR     1
#define DATA_HIGHLIGHT_COLOR 2

//BST Traversal
#define CASEPREORDER            1
#define CASEINORDER             2
#define CASEPOSTORDER           3
#define CASEINSUCC              4
#define CASEINPRE               5

struct stack
{
    int *data;
    long int top, max;
};
struct bst_node
{
    int data;
    short color; 
    struct bst_node *left;
    struct bst_node *right;
    struct bst_node *parent;
};

struct bst
{
    struct bst_node *p_root;
    size_t nr_elements;
};

typedef struct stack stack_t;

typedef struct bst_node bst_node_t;
typedef struct bst bst_t;

// Functions for Stack Operations
stack_t *CreateStack(size_t max);

int StackIsFull(stack_t *p_stack);
int StackIsEmpty(stack_t *p_stack);

int StackPush(stack_t *p_stack, int new_data);
int StackPop(stack_t *p_stack, int *old_data);

int StackPeek(stack_t *p_stack, int *data);
void StackDestroy(stack_t **pp_stack);

// Functions for BST Operations
struct bst *CreateBST(void);
int bst_insert(struct bst *p_bst, int new_element);

void preorder_r(struct bst *p_bst);
void inorder_r(struct bst *p_bst);
void postorder_r(struct bst *p_bst);
int bst_max(struct bst *p_bst, int *p_max_data);
int bst_min(struct bst *p_bst, int *p_min_data);
int bst_inorder_successor(PAINTSTRUCT ps, HWND hWnd, HDC hdc, struct bst *p_bst, int ext_data, int *p_succ_data);
int bst_inorder_predecessor(PAINTSTRUCT ps, HWND hWnd, HDC hdc, struct bst *p_bst, int ext_data, int *p_pred_data);
int BSTSearch(struct bst *p_bst, int search_data);
int RemoveData(struct bst *p_nst, int r_data);
void BSTDestroy(struct bst **pp_bst);

struct bst_node *get_max_node(struct bst_node *p_node);
struct bst_node *get_min_node(struct bst_node *p_node);
struct bst_node *inorder_successor(PAINTSTRUCT ps, HDC hdc, HWND hWnd, struct bst_node *p_node);
struct bst_node *inorder_predecessor(PAINTSTRUCT ps, HDC hdc, HWND hWnd, struct bst_node *p_node);
struct bst_node *SearchNode(struct bst *p_bst, int search_data);
void preorder_node(PAINTSTRUCT ps, HDC hdc, HWND hWnd, struct bst_node *p_root);
void inorder_node(PAINTSTRUCT ps, HDC hdc, HWND hWnd, struct bst_node *p_root);
void postorder_node(PAINTSTRUCT ps, HDC hdc, HWND hWnd, struct bst_node *p_root);
void DestroyNode(struct bst_node *p_root);
struct bst_node *GetBSTNode(int new_element);

// Win32
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcDel(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcSearch(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcInPre(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcInSucc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);
void addMenuBar(HWND hwnd);
void MyDrawText2(HDC hdc, HWND hWnd, PAINTSTRUCT ps, WCHAR *STRING,
                 INT X, INT Y, INT R, INT G, INT B, INT SIZE);
void SetBackgroundColor(HWND hWnd, HDC hdc);

int RandRange(int Start, int End);

// Functions to display the BST
void DisplayBSTNode(PAINTSTRUCT ps, HDC hdc, HWND hWnd, bst_node_t *p_node, int level, int isLeft);
void DisplayBST(PAINTSTRUCT ps, HDC hdc, HWND hWnd, bst_t *p_bst);
void DrawNode(PAINTSTRUCT ps,HDC hdc, HWND hWnd, int x, int y, bst_node_t* p_node);
void DrawBSTArrayNodeBox(HDC hdc, HWND hWnd, PAINTSTRUCT ps, int xStart, int yStart, int Lenght, int Height, int data);
VOID StartWindow(HDC hdc, HWND hWnd, PAINTSTRUCT ps);

// Global variables
// Graphical Tree Variables
LONG NodeYLenght = 5;

BOOL Flag = 1;
BOOL NotToAddFlag = 0;
BOOL NotToDelFlag = 0;
BOOL NotToSearchFlag = 0;

// Size Variables
INT cxsize;
INT cysize;
INT key = 0;
int NUMBER;
int NUMBER_DEL;
int NUMBER_SEARCH;
BOOL isStartWindow = TRUE;
INT ColorChangeR = 255;
INT ColorChangeG = 255;
INT ColorChangeB = 255;

//-------------------
bst_t *p_bst = NULL;
stack_t *prev_positions = NULL;
bst_node_t *p_curr_find_node = NULL;
char BUFFER[64];
int OFFSETS[MAX_HEIGHT + 1];
int STATUS;
int isRootNodeFlag = 0;
bst_node_t *MinMaxNode = NULL;

int BSTTraversalCase = 0;
int OrderArrayY;
int OrderArrayX;

int InInorderPreData = 0;
int OpInorderPreData = 0;
int InInorderSuccData = 0;
int OpInorderSuccData = 0;

BOOL bLocalRet = 0;
INT StopSleep = 0;

//---------------------

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    static TCHAR szAppName[] = TEXT("Tree Visualizer");
    static TCHAR szWindowClassName[] = TEXT("Binary Search Tree Visualizer");

    HBRUSH hBrush = NULL;
    HCURSOR hCursor = NULL;
    HICON hIcon = NULL;
    HICON hIconSm = NULL;
    HWND hWnd = NULL;

    ATOM bClassRegistrationStatus = 0;

    WNDCLASSEX wnd;
    MSG msg;

    ZeroMemory((void *)&wnd, sizeof(WNDCLASSEX));
    ZeroMemory((void *)&msg, sizeof(MSG));

    hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
    EXIT_IF(hBrush == NULL, "Error in loading brush", "GetStockObject");

    hCursor = LoadCursor((HINSTANCE)NULL, IDC_ARROW);
    EXIT_IF(hCursor == NULL, "Error in Loading cursor", "LoadCursor");

    hIcon = LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);
    EXIT_IF(hIcon == NULL, "Error in loading icon", "LoadIcon");

    hIconSm = LoadIcon((HINSTANCE)NULL, IDI_APPLICATION);
    EXIT_IF(hIconSm == NULL, "Error in loading icon", "LoadIcon");

    wnd.cbSize = sizeof(WNDCLASSEX);
    wnd.cbClsExtra = 0;
    wnd.cbWndExtra = 0;
    wnd.hbrBackground = hBrush;
    wnd.hCursor = hCursor;
    wnd.hIcon = hIcon;
    wnd.hInstance = hInstance;
    wnd.hIconSm = hIconSm;
    wnd.lpfnWndProc = WndProc;
    wnd.lpszClassName = szWindowClassName;
    wnd.lpszMenuName = NULL;
    wnd.style = CS_HREDRAW | CS_VREDRAW;

    bClassRegistrationStatus = RegisterClassEx(&wnd);
    EXIT_IF(bClassRegistrationStatus == 0, "Failed to register a window class",
            "RegisterClassEx");

    hWnd = CreateWindowEx(WS_EX_APPWINDOW, szWindowClassName, szAppName, WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                          (HWND)NULL, (HMENU)NULL, hInstance, NULL);
    EXIT_IF(hWnd == NULL, "Failed to create a window in memory", "CreateWindowEx");

    ShowWindow(hWnd, SW_MAXIMIZE);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, (HWND)NULL, 0U, 0U))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Gdiplus::GdiplusShutdown(gdiplusToken);

    return (msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    static int cxChar, cyChar, cxCaps;
    TEXTMETRIC tm;
    int array[] = {25, 20, 10, 5, 12, 22, 36, 30, 28, 40, 38, 48};
    int i;

    switch (uMsg)
    {
    case WM_TIMER:
        switch(wParam)
        {
            case SEARCHING_TIMER:
                KillTimer(hwnd, SEARCHING_TIMER);
                if(isRootNodeFlag == 1)
                {
                    p_curr_find_node -> color = DATA_HIGHLIGHT_COLOR;
                    SetTimer(hwnd, SEARCHING_TIMER, 1000, NULL);
                    InvalidateRect(hwnd, NULL, TRUE);
                    isRootNodeFlag = 0;
                    break;
                }
                if(p_curr_find_node -> data == NUMBER_SEARCH)
                {
                    KillTimer(hwnd, SEARCHING_TIMER);
                    p_curr_find_node -> color = DATA_FOUND_COLOR;
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                }
                else if(NUMBER_SEARCH > p_curr_find_node -> data)
                {
                    if(p_curr_find_node -> right == NULL)
                    {
                        KillTimer(hwnd, SEARCHING_TIMER);
                        p_curr_find_node -> color = NO_COLOR;
                        MessageBox(hwnd, TEXT("Data Not Found"), TEXT("BST"), MB_ICONERROR);
                        MessageBeep(MB_ICONERROR);
                        break;
                        InvalidateRect(hwnd, NULL, TRUE);
                        InvalidateRect(hwnd, NULL, TRUE);
                        break;
                    }
                    p_curr_find_node = p_curr_find_node -> right;
                    p_curr_find_node -> parent -> color = NO_COLOR;
                    p_curr_find_node -> color = DATA_HIGHLIGHT_COLOR;
                    SetTimer(hwnd, SEARCHING_TIMER, 1000, NULL);
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                else if(NUMBER_SEARCH < p_curr_find_node -> data)
                {
                    if(p_curr_find_node -> left == NULL)
                    {
                        KillTimer(hwnd, SEARCHING_TIMER);
                        p_curr_find_node -> color = NO_COLOR;
                        MessageBox(hwnd, TEXT("Data Not Found"), TEXT("BST"), MB_ICONERROR);
                        MessageBeep(MB_ICONERROR);
                        break;
                        InvalidateRect(hwnd, NULL, TRUE);
                        InvalidateRect(hwnd, NULL, TRUE);
                        break;
                    }
                    p_curr_find_node = p_curr_find_node -> left;
                    p_curr_find_node -> parent -> color = NO_COLOR;
                    p_curr_find_node -> color = DATA_HIGHLIGHT_COLOR;
                    SetTimer(hwnd, SEARCHING_TIMER, 1000, NULL);
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                break;

                 
        }
        break;

    case WM_CREATE:
        hdc = GetDC(hwnd);

        addMenuBar(hwnd);
        isStartWindow = 1;

        p_bst = CreateBST();
        prev_positions = CreateStack(MAX_HEIGHT + 2);

        GetTextMetrics(hdc, &tm);
        cxChar = tm.tmAveCharWidth;
        cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
        cyChar = tm.tmHeight;

        ReleaseDC(hwnd, hdc);
        hdc = NULL;
        break;

    case WM_SIZE:
        cxsize = LOWORD(lParam);
        cysize = HIWORD(lParam);
        OrderArrayX = 0 + 20;
        OrderArrayY = cysize - 70;
        break;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        SetBackgroundColor(hwnd, hdc);
        if(BSTTraversalCase != 0)
        {
            switch(BSTTraversalCase)
            {
                case CASEPREORDER:
                    preorder_node(ps, hdc, hwnd, p_bst -> p_root);
                    break;

                case CASEINORDER:
                    inorder_node(ps, hdc, hwnd, p_bst -> p_root);
                    break;

                case CASEPOSTORDER:
                    postorder_node(ps, hdc, hwnd, p_bst -> p_root);
                    break;

                case CASEINSUCC:
                    StopSleep = 1;
                    inorder_node(ps, hdc, hwnd, p_bst -> p_root);
                    bLocalRet = bst_inorder_successor(ps, hwnd, hdc, p_bst, InInorderSuccData, &OpInorderSuccData);

                    switch(bLocalRet)
                    {
                        case TREE_DATA_NOT_FOUND:
                            sprintf(BUFFER, "%d data not found", InInorderSuccData);
                            MessageBox(hwnd, BUFFER, TEXT("BST"), MB_OK);
                            break;
                        case TREE_NO_SUCCESSOR:
                            sprintf(BUFFER, "No Inorder Successor for %d", InInorderSuccData);
                            MessageBox(hwnd, BUFFER, TEXT("BST"), MB_OK);
                            break;
                        default:
                            sprintf(BUFFER, "Inorder Successor of %d : %d", InInorderSuccData, OpInorderSuccData);
                            MessageBox(hwnd, BUFFER, TEXT("BST"), MB_OK);
                    }

                    StopSleep = 0;
                    break;

                case CASEINPRE:
                    StopSleep = 1;
                    inorder_node(ps, hdc, hwnd, p_bst -> p_root);
                    bLocalRet = bst_inorder_predecessor(ps, hwnd, hdc, p_bst, InInorderPreData, &OpInorderPreData);

                    switch(bLocalRet)
                    {
                        case TREE_DATA_NOT_FOUND:
                            sprintf(BUFFER, "%d data not found", InInorderPreData);
                            MessageBox(hwnd, BUFFER, TEXT("BST"), MB_OK);
                            break;
                        case TREE_NO_PREDECESSOR:
                            sprintf(BUFFER, "No Inorder Predeccessor for %d", InInorderPreData);
                            MessageBox(hwnd, BUFFER, TEXT("BST"), MB_OK);
                            break;
                        default:
                            sprintf(BUFFER, "Inorder Predeccessor of %d : %d", InInorderPreData, OpInorderPreData);
                            MessageBox(hwnd, BUFFER, TEXT("BST"), MB_OK);
                    }

                    StopSleep = 0;
                    break;
            }

            OrderArrayX = 20;
            BSTTraversalCase = 0;
            break;
        }
        if (isStartWindow == 1)
        {
            // MyDrawText2(hdc, hwnd, ps, L"Binary Tree Visualiser",
            //             cxsize / 2 - 400, cysize / 2, 0, 0, 0, 60);
            StartWindow(hdc, hwnd, ps);
        }
        else
        {
            DisplayBST(ps, hdc, hwnd, p_bst);
        }

        EndPaint(hwnd, &ps);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_ABOUT:
            MessageBox(hwnd, TEXT("Binary Tree Visualiser\nLearn Binary Search Tree\n\nMade by Yash Thakare"), 
                        TEXT("About"), MB_OK);
            break;

        case ID_EXIT:
            PostQuitMessage(0);
            break;

        case ID_ADD:
            DialogBox(NULL, MAKEINTRESOURCE(DATAENTRY), hwnd, (DLGPROC)DlgProc);
            isStartWindow = 0;
            if (NotToAddFlag != 1)
            {
                bst_insert(p_bst, NUMBER);
                MessageBeep(MB_OK);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            // else
            // {
            //     MessageBox(hwnd, TEXT("Only Limited Nodes can be inserted"), TEXT("Error"), MB_ICONERROR);
            // }
            break;

        case ID_DEL:
            DialogBox(NULL, MAKEINTRESOURCE(DELETE_NODE_DLG), hwnd, (DLGPROC)DlgProcDel);
            if(isStartWindow)
            {
                MessageBox(hwnd, TEXT("Add some node to delete"), TEXT("Error"), MB_ICONERROR);
                MessageBeep(MB_ICONERROR);
                break;
            }
            if (NotToDelFlag != 1)
            {
                STATUS = RemoveData(p_bst, NUMBER_DEL);
                
                switch (STATUS) 
                {
                case SUCCESS:
                    MessageBeep(MB_OK);
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;
                case TREE_EMPTY:
                    MessageBox(hwnd, TEXT("Tree is Empty"), TEXT("Error"), MB_ICONERROR);
                    MessageBeep(MB_ICONERROR);
                    break;
                case TREE_DATA_NOT_FOUND:
                    MessageBox(hwnd, TEXT("Data Not Found"), TEXT("BST"), MB_ICONERROR);
                    MessageBeep(MB_ICONERROR);
                    break;
                }
            }
            break;
        
        case ID_SEARCH:
            DialogBox(NULL, MAKEINTRESOURCE(SEARCH_NODE_DLG), 
                        hwnd, (DLGPROC)DlgProcSearch);
            if(p_curr_find_node != NULL)
                p_curr_find_node -> color = NO_COLOR;

            if(isStartWindow)
            {
                MessageBox(hwnd, TEXT("Add some node to Search"), 
                            TEXT("Error"), MB_ICONERROR);
                MessageBeep(MB_ICONERROR);
                break;
            }
            if(NotToSearchFlag == 0)
            {
                p_curr_find_node = p_bst -> p_root;
                isRootNodeFlag = 1;
                SetTimer(hwnd, SEARCHING_TIMER, 1000, NULL);
                InvalidateRect(hwnd, NULL, TRUE);
            }
            p_curr_find_node -> color = NO_COLOR;
            break;

        case ID_DELETE_TREE:
            BSTDestroy(&p_bst);
            p_bst = CreateBST();
            isStartWindow = 1;
            ColorChangeR = 255;
            ColorChangeG = 255;
            ColorChangeB = 255;
            isStartWindow = 1;
            InvalidateRect(hwnd, NULL, TRUE);
            break;

        case ID_CHANGE_COLOR:
            ColorChangeR = RandRange(0, 255);
            ColorChangeG = RandRange(0, 255);
            ColorChangeB = RandRange(0, 255);
            InvalidateRect(hwnd, NULL, TRUE);
            break;

        case ID_DARK_MODE:
            ColorChangeR = 10;
            ColorChangeG = 10;
            ColorChangeB = 10;
            InvalidateRect(hwnd, NULL, TRUE);
            break;

        case ID_LIGHT_MODE:
            ColorChangeR = 255;
            ColorChangeG = 255;
            ColorChangeB = 255;
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        
        case ID_MIN_NODE:
            MinMaxNode = get_min_node(p_bst -> p_root);
            if(MinMaxNode == NULL)
            {
                MessageBox(hwnd, TEXT("Add some node to BST!"), TEXT("BST EMPTY"), MB_ICONERROR);
                MessageBeep(MB_ICONERROR);
                break;
            }
            sprintf(BUFFER, "Minimum Node Data : %d", MinMaxNode -> data);
            MessageBox(hwnd, BUFFER, TEXT("BST"), MB_OK);
            break;
        
        case ID_MAX_NODE:
            MinMaxNode = get_max_node(p_bst -> p_root);
            if(MinMaxNode == NULL)
            {
                MessageBox(hwnd, TEXT("Add some node to BST!"), TEXT("BST EMPTY"), MB_ICONERROR);
                MessageBeep(MB_ICONERROR);
                break;
            }
            sprintf(BUFFER, "Maximum Node Data : %d", MinMaxNode -> data);
            MessageBox(hwnd, BUFFER, TEXT("BST"), MB_OK);
            break;
        
        case ID_INORDER:
            if(isStartWindow == 1 || p_bst -> p_root == NULL)
            {
                MessageBox(hwnd, TEXT("Add some node to BST!"), TEXT("BST EMPTY"), MB_ICONERROR);
                MessageBeep(MB_ICONERROR);
                break;
            }
            BSTTraversalCase = CASEINORDER;
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        
        case ID_PREORDER:
            if(isStartWindow == 1 || p_bst -> p_root == NULL)
            {
                MessageBox(hwnd, TEXT("Add some node to BST!"), TEXT("BST EMPTY"), MB_ICONERROR);
                MessageBeep(MB_ICONERROR);
                break;
            }
            BSTTraversalCase = CASEPREORDER;
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        
        case ID_POSTORDER:
            if(isStartWindow == 1 || p_bst -> p_root == NULL)
            {
                MessageBox(hwnd, TEXT("Add some node to BST!"), TEXT("BST EMPTY"), MB_ICONERROR);
                MessageBeep(MB_ICONERROR);
                break;
            }
            BSTTraversalCase = CASEPOSTORDER;
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        
        case ID_INORDER_SUCC:
            // bst_inorder_successor(ps, hwnd, hdc, p_bst, 300, &OpInorderSuccData);
            DialogBox(NULL, MAKEINTRESOURCE(INORDER_SUCC_DLG), hwnd, (DLGPROC)DlgProcInSucc);
            if(p_bst -> p_root == NULL)
            {
                MessageBox(hwnd, TEXT("Add some node to BST!"), TEXT("BST EMPTY"), MB_ICONERROR);
                MessageBeep(MB_ICONERROR);
                break;
            }
            BSTTraversalCase = CASEINSUCC;
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        
        case ID_INORDER_PRE:
            DialogBox(NULL, MAKEINTRESOURCE(INORDER_PRE_DLG), hwnd, (DLGPROC)DlgProcInPre);

            if(p_bst -> p_root == NULL)
            {
                MessageBox(hwnd, TEXT("Add some node to BST!"), TEXT("BST EMPTY"), MB_ICONERROR);
                MessageBeep(MB_ICONERROR);
                break;
            }

            BSTTraversalCase = CASEINPRE;
            InvalidateRect(hwnd, NULL, TRUE);
            break;
        }
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
            case VK_ESCAPE:
                PostQuitMessage(0);
                break;
            case VK_F1:
                DialogBox(NULL, MAKEINTRESOURCE(DATAENTRY), hwnd, (DLGPROC)DlgProc);
                isStartWindow = 0;

                if (NotToAddFlag != 1)
                {
                    bst_insert(p_bst, NUMBER);
                    MessageBeep(MB_OK);
                    InvalidateRect(hwnd, NULL, TRUE);
                } 

                break;
            case VK_F2:
                DialogBox(NULL, MAKEINTRESOURCE(DELETE_NODE_DLG), hwnd, (DLGPROC)DlgProcDel);

                if(isStartWindow)
                {
                    MessageBox(hwnd, TEXT("Add some node to delete"), TEXT("Error"), MB_ICONERROR);
                    MessageBeep(MB_ICONERROR);
                    break;
                    InvalidateRect(hwnd, NULL, TRUE);
                }

                if (NotToDelFlag != 1)
                {
                    STATUS = RemoveData(p_bst, NUMBER_DEL);
                    
                    switch (STATUS) 
                    {
                        case SUCCESS:
                            MessageBeep(MB_OK);
                            InvalidateRect(hwnd, NULL, TRUE);
                            break;
                        case TREE_EMPTY:
                            MessageBox(hwnd, TEXT("Tree is Empty"), TEXT("Error"), MB_ICONERROR);
                            MessageBeep(MB_ICONERROR);
                            break;
                            InvalidateRect(hwnd, NULL, TRUE);
                            break;
                        case TREE_DATA_NOT_FOUND:
                            MessageBox(hwnd, TEXT("Data Not Found"), TEXT("BST"), MB_ICONERROR);
                            MessageBeep(MB_ICONERROR);
                            break;
                            InvalidateRect(hwnd, NULL, TRUE);
                            break;
                    }
                }
                break;
            case VK_F3:
                DialogBox(NULL, MAKEINTRESOURCE(SEARCH_NODE_DLG), hwnd, (DLGPROC)DlgProcSearch);
                if(p_curr_find_node != NULL)
                    p_curr_find_node -> color = NO_COLOR;

                if(isStartWindow)
                {
                    MessageBox(hwnd, TEXT("Add some node to Search"), TEXT("Error"), MB_ICONERROR);
                    MessageBeep(MB_ICONERROR);
                    break;
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                if(NotToSearchFlag == 0)
                {
                    p_curr_find_node = p_bst -> p_root;
                    isRootNodeFlag = 1;
                    SetTimer(hwnd, SEARCHING_TIMER, 1000, NULL);
                    InvalidateRect(hwnd, NULL, TRUE);
                }
                p_curr_find_node -> color = NO_COLOR;
                break;
            case VK_DELETE:
                BSTDestroy(&p_bst);
                p_bst = CreateBST();
                isStartWindow = 1;
                ColorChangeR = 255;
                ColorChangeG = 255;
                ColorChangeB = 255;
                isStartWindow = 1;
                InvalidateRect(hwnd, NULL, TRUE);
                break;

        }
        break;

        case WM_DESTROY:
            StackDestroy(&prev_positions);
            BSTDestroy(&p_bst);
            PostQuitMessage(0);
            break;
    }

    return (DefWindowProc(hwnd, uMsg, wParam, lParam));
}

VOID StartWindow(HDC hdc, HWND hwnd, PAINTSTRUCT ps)
{
    Graphics graphics(hdc);

    Color pixelColor;
    COLORREF pixelColorRef;

    Bitmap BSTBitMap(L"BST_2_LOGO.jpg");
    Image image(L"BST_2_LOGO.jpg");

    ColorChangeR = 0;
    ColorChangeG = 0;
    ColorChangeB = 0;

    SetBackgroundColor(hwnd, hdc);

    graphics.DrawImage(&image, PointF((float)(cxsize/2 - BSTBitMap.GetWidth()/2), 
                        (float)(cysize/2 - BSTBitMap.GetHeight()/2)));

    MyDrawText2(hdc, hwnd, ps, L"Binary Tree Visualiser", 
                cxsize / 2, cysize / 2 + BSTBitMap.GetHeight()/2 + 25, 
                255, 255, 255, 60);
}


void SetBackgroundColor(HWND hWnd, HDC hdc)
{
    RECT rect;
    GetClientRect(hWnd, &rect);

    SetDCBrushColor(hdc, RGB(ColorChangeR, ColorChangeG, ColorChangeB));
    HBRUSH brush = (HBRUSH)GetStockObject(DC_BRUSH);

    FillRect(hdc, &rect, brush);
}

void MyDrawText2(HDC hdc, HWND hWnd, PAINTSTRUCT ps,
                 WCHAR *STRING, INT X, INT Y, 
                 INT R, INT G, INT B, INT SIZE)
{
    Graphics graphics(hdc);

    StringFormat stringFormat;
    Font myFont(L"Times New Roman", SIZE);
    PointF origin((float)X, (float)Y);
    SolidBrush myBrush(Color(255, R, G, B));

    stringFormat.SetAlignment(StringAlignmentCenter);

    graphics.DrawString(STRING, -1, &myFont, origin, &stringFormat, &myBrush);
}

void addMenuBar(HWND hwnd)
{
    HMENU hMenuBar;
    HMENU hMenu;
    HMENU hMenu2;

    hMenuBar = CreateMenu();
    hMenu = CreateMenu();
    hMenu2 = CreateMenu();
    HMENU hMenu3 = CreateMenu();
    HMENU hMenu4 = CreateMenu();
    HMENU hMenu5 = CreateMenu();
    HMENU hMenu6 = CreateMenu();

    AppendMenuW(hMenu, MF_STRING, ID_ABOUT, L"About");
    AppendMenuW(hMenu, MF_STRING, ID_EXIT, L"Exit");
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hMenu, L"Application");
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hMenu2, L"Edit BST");
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hMenu3, L"Color Modes");
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hMenu4, L"BST Info");
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hMenu5, L"BST Traversal");
    AppendMenuW(hMenuBar, MF_POPUP, (UINT_PTR)hMenu6, L"Inorder");
    AppendMenuW(hMenu3, MF_STRING, ID_CHANGE_COLOR, L"Change Color");
    AppendMenuW(hMenu3, MF_STRING, ID_DARK_MODE, L"Dark Mode");
    AppendMenuW(hMenu3, MF_STRING, ID_LIGHT_MODE, L"Light Mode");
    AppendMenuW(hMenu2, MF_STRING, ID_ADD, L"Add Node (F1)");
    AppendMenuW(hMenu2, MF_STRING, ID_DEL, L"Delete Node (F2)");
    AppendMenuW(hMenu2, MF_STRING, ID_SEARCH, L"Search Node (F3)");
    AppendMenuW(hMenu2, MF_STRING, ID_DELETE_TREE, L"Delete Tree (Delete)");
    AppendMenuW(hMenu4, MF_STRING, ID_MIN_NODE, L"Minimum Node");
    AppendMenuW(hMenu4, MF_STRING, ID_MAX_NODE, L"Maximum Node");
    AppendMenuW(hMenu5, MF_STRING, ID_PREORDER, L"Preorder Traversal");
    AppendMenuW(hMenu5, MF_STRING, ID_INORDER, L"Inorder Traversal");
    AppendMenuW(hMenu5, MF_STRING, ID_POSTORDER, L"Postorder Traversal");
    AppendMenuW(hMenu6, MF_STRING, ID_INORDER_SUCC, L"Inorder Successor");
    AppendMenuW(hMenu6, MF_STRING, ID_INORDER_PRE, L"Inorder Predeccessor");



    SetMenu(hwnd, hMenuBar);
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    CHAR BUFFER[3];
    switch (iMsg)
    {
    case WM_INITDIALOG:
        SetFocus(GetDlgItem(hDlg, ID_USERINPUT));
        SendMessage(GetDlgItem(hDlg, ID_USERINPUT), EM_SETLIMITTEXT, 3, 0);
        SetDlgItemText(hDlg, ID_USERINPUT, TEXT("Enter 3 Digit Number"));
        return (TRUE);

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
            case IDOK:
                NotToAddFlag = 0;
                NUMBER = GetDlgItemInt(hDlg, ID_USERINPUT, NULL, TRUE);
                EndDialog(hDlg, 0);
                // sprintf(BUFFER, "%d", NUMBER);
                // MessageBox(hDlg, BUFFER, BUFFER, MB_OK);
                break;

            case IDCANCEL:
                NotToAddFlag = 1;
                EndDialog(hDlg, 0);
                break;
            }
            return TRUE;
        }
        return FALSE;
    }

    BOOL CALLBACK DlgProcDel(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
    {
        CHAR BUFFER[3];
        switch (iMsg)
        {
            case WM_INITDIALOG:
                SetFocus(GetDlgItem(hDlg, ID_USERINPUT));
                SendMessage(GetDlgItem(hDlg, ID_USERINPUT), EM_SETLIMITTEXT, 3, 0);
                SetDlgItemText(hDlg, ID_USERINPUT, TEXT("Enter 3 Digit Number"));
                return (TRUE);

            case WM_COMMAND:
                switch (LOWORD(wParam))
                {
                    case ID_DLG_DELETE:
                        NotToDelFlag = 0;
                        MessageBeep(MB_OK);
                        NUMBER_DEL = GetDlgItemInt(hDlg, ID_USERINPUT, NULL, TRUE);
                        EndDialog(hDlg, 0);
                        break;

                    case IDCANCEL:
                        NotToDelFlag = 1;
                        EndDialog(hDlg, 0);
                        break;
        }
        return TRUE;
    }
    return FALSE;
}

BOOL CALLBACK DlgProcSearch(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    CHAR BUFFER[3];
    switch (iMsg)
    {
        case WM_INITDIALOG:
            SetFocus(GetDlgItem(hDlg, ID_USERINPUT));
            SendMessage(GetDlgItem(hDlg, ID_USERINPUT), EM_SETLIMITTEXT, 3, 0);
            SetDlgItemText(hDlg, ID_USERINPUT, TEXT("Enter 3 Digit Number"));
            return (TRUE);

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case ID_DLG_SEARCH:
                    NotToSearchFlag = 0;
                    NUMBER_SEARCH = GetDlgItemInt(hDlg, ID_USERINPUT, NULL, TRUE);
                    EndDialog(hDlg, 0);
                    break;

                case IDCANCEL:
                    NotToSearchFlag = 1;
                    EndDialog(hDlg, 0);
                    break;
            }
            return TRUE;
    }
    return FALSE;
}

BOOL CALLBACK DlgProcInSucc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    CHAR BUFFER[3];
    switch (iMsg)
    {
    case WM_INITDIALOG:
        SetFocus(GetDlgItem(hDlg, ID_USERINPUT));
        SendMessage(GetDlgItem(hDlg, ID_USERINPUT), EM_SETLIMITTEXT, 3, 0);
        SetDlgItemText(hDlg, ID_USERINPUT, TEXT("Enter 3 Digit Number"));
        return (TRUE);

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            NotToAddFlag = 0;
            InInorderSuccData = GetDlgItemInt(hDlg, ID_USERINPUT, NULL, TRUE);
            EndDialog(hDlg, 0);
            break;

        case IDCANCEL:
            NotToAddFlag = 1;
            EndDialog(hDlg, 0);
            break;
        }
        return TRUE;
    }
    return FALSE;
}

BOOL CALLBACK DlgProcInPre(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    CHAR BUFFER[3];
    switch (iMsg)
    {
    case WM_INITDIALOG:
        SetFocus(GetDlgItem(hDlg, ID_USERINPUT));
        SendMessage(GetDlgItem(hDlg, ID_USERINPUT), EM_SETLIMITTEXT, 3, 0);
        SetDlgItemText(hDlg, ID_USERINPUT, TEXT("Enter 3 Digit Number"));
        return (TRUE);

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            NotToAddFlag = 0;
            InInorderPreData = GetDlgItemInt(hDlg, ID_USERINPUT, NULL, TRUE);
            EndDialog(hDlg, 0);
            break;

        case IDCANCEL:
            NotToAddFlag = 1;
            EndDialog(hDlg, 0);
            break;
        }
        return TRUE;
    }
    return FALSE;
}

int RandRange(int Start, int End)
{
    int num;
    num = rand() % End + Start;
    return (num);
}

// A function to draw the node on the window
void DrawNode(PAINTSTRUCT ps,HDC hdc, HWND hWnd, 
                int x, int y, bst_node_t* p_node)
{
    DWORD pen_style = PS_SOLID;
    COLORREF col;

    if(ColorChangeR == 0 && 
        ColorChangeG == 0 &&
        ColorChangeB == 0)
    {
        col = RGB(255, 255, 255);
    } 
    else 
    {   
        col = RGB(0, 0, 0);
    }

    HPEN hPen2 = CreatePen(pen_style, 5, col);
    HPEN holdPen2 = (HPEN)SelectObject(hdc, hPen2);

    switch(p_node -> color)
    {
        case NO_COLOR:
            SetDCBrushColor(hdc, RGB(255, 255, 255));
            SelectObject(ps.hdc, GetStockObject(DC_BRUSH));
            break;

        case DATA_HIGHLIGHT_COLOR:
            SetDCBrushColor(hdc, RGB(245, 195, 65));
            SelectObject(ps.hdc, GetStockObject(DC_BRUSH));
            col = RGB(0, 0, 255);
            break;

        case DATA_FOUND_COLOR:
            SetDCBrushColor(hdc, RGB(0, 255, 0));
            SelectObject(ps.hdc, GetStockObject(DC_BRUSH));
            col = RGB(255, 0, 0);
            break;
    }

    hPen2 = CreatePen(pen_style, 5, col);
    holdPen2 = (HPEN)SelectObject(hdc, hPen2);

    Ellipse(hdc, x - RADIUS, y - RADIUS, x + RADIUS, y + RADIUS);

    SetTextAlign(hdc, TA_CENTER);
    sprintf(BUFFER, "%d", p_node->data);
    TextOut(hdc, x, y, BUFFER, strlen(BUFFER));
    SetTextAlign(hdc, TA_CENTER);

    DeleteObject(hPen2);
    DeleteObject(holdPen2);
}

void DisplayBSTNode(PAINTSTRUCT ps, HDC hdc, HWND hWnd, 
                    bst_node_t *p_node, int level, int isLeft)
{
    Graphics graphics(hdc);

    DWORD pen_style = PS_SOLID;
    COLORREF col;
    if(ColorChangeR == 0 && 
        ColorChangeG == 0 &&
        ColorChangeB == 0)
    {
        col = RGB(255, 255, 255);
    } 
    else 
    {   
        col = RGB(0, 0, 0);
    }

    HPEN hPen2 = CreatePen(pen_style, 5, col);
    HPEN holdPen2 = (HPEN)SelectObject(hdc, hPen2);

    if (p_node == NULL) // Guard to return if node is NULL
        return;

    POINT pt[2];
    int x, y, prev_x, curr_offset;

    assert(StackPeek(prev_positions, &prev_x) == SUCCESS);

    // Calculate X, Y
    y = level * ROW_HEIGHT;
    curr_offset = ((isLeft == TRUE) ? (-1 * OFFSETS[level - 1]) : (OFFSETS[level - 1]));
    x = prev_x + curr_offset;

    pt[0] = {prev_x, (level - 1) * ROW_HEIGHT + RADIUS};
    pt[1] = {x, y};
    Polyline(hdc, pt, 2);

    DrawNode(ps, hdc, hWnd, x, y, p_node);

    StackPush(prev_positions, x);

    DisplayBSTNode(ps, hdc, hWnd, p_node->left, level + 1, TRUE);
    DisplayBSTNode(ps, hdc, hWnd, p_node->right, level + 1, FALSE);

    StackPop(prev_positions, &prev_x);

    DeleteObject(hPen2);
    DeleteObject(holdPen2);
}

void DisplayBST(PAINTSTRUCT ps, HDC hdc, HWND hWnd, bst_t *p_bst)
{
    int x = cxsize / 2;
    int y = 1 * ROW_HEIGHT;
    int tmp, i;

    // Handle BST with no elements
    if (p_bst->p_root == NULL)
    {
        isStartWindow = TRUE;
        InvalidateRect(hWnd, NULL, TRUE);
        return;
    }

    // Calculate the offset for node in each level using the size of window
    OFFSETS[0] = 0;
    for (i = 1; i < MAX_HEIGHT + 1; i++)
    {
        OFFSETS[i] = cxsize / pow(2, i + 1) - 20;
    }

    DrawNode(ps, hdc, hWnd, x, y, p_bst->p_root);

    assert(StackPush(prev_positions, x) == SUCCESS);

    DisplayBSTNode(ps, hdc, hWnd, p_bst->p_root->left, 2, TRUE);
    DisplayBSTNode(ps, hdc, hWnd, p_bst->p_root->right, 2, FALSE);

    StackPop(prev_positions, &tmp);
}

//**********Functions to represent BST in Array Form*************
void DrawBSTArrayNodeBox(HDC hdc, HWND hWnd, PAINTSTRUCT ps, 
                        int xStart, int yStart, int Lenght, 
                        int Height, int data)
{
    Graphics graphics(hdc);

    DWORD pen_style = PS_SOLID;
    COLORREF col;

    if(ColorChangeR == 0 && 
        ColorChangeG == 0 &&
        ColorChangeB == 0)
    {
        col = RGB(255, 255, 255);
    } 
    else 
    {   
        col = RGB(0, 0, 0);
    }

    HPEN hPen2 = CreatePen(pen_style, 5, col);
    HPEN holdPen2 = (HPEN)SelectObject(hdc, hPen2);
    HBRUSH hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);

    POINT pt[] = {{xStart, yStart}, {xStart + Lenght,yStart}, 
                    {xStart + Lenght, yStart + Height}, 
                    {xStart, yStart + Height}, {xStart, yStart}};

    RECT rc = {xStart, yStart, xStart + Lenght, yStart + Height};

    FillRect(hdc, &rc, hBrush);

    Polyline(hdc, pt, 5);

    sprintf(BUFFER, "%d", data);
    SetTextAlign(hdc, TA_CENTER);
    TextOut(hdc, xStart + Lenght/2, 
            yStart + Height/2 - 10, 
            BUFFER, strlen(BUFFER));

    DeleteObject(holdPen2);
    DeleteObject(hPen2);
    DeleteObject(hBrush);
}

// ******************** STACK ********************
stack_t *CreateStack(size_t max)
{
    stack_t *p_stack = NULL;

    p_stack = (stack_t *)malloc(sizeof(stack_t));
    assert(p_stack != NULL);

    p_stack->max = max;
    p_stack->top = -1;
    p_stack->data = NULL;

    p_stack->data = (int *)calloc(max, sizeof(int));
    assert(p_stack->data != NULL);

    return p_stack;
}

int StackIsFull(stack_t *p_stack)
{
    return (p_stack->top >= p_stack->max - 1);
}

int StackIsEmpty(stack_t *p_stack)
{
    return (p_stack->top <= -1);
}

int StackPush(stack_t *p_stack, int new_data)
{
    if (StackIsFull(p_stack))
    {
        return STACK_OVERFLOW;
    }

    p_stack->top++;
    p_stack->data[p_stack->top] = new_data;

    return SUCCESS;
}

int StackPop(stack_t *p_stack, int *old_data)
{
    if (StackIsEmpty(p_stack))
    {
        return STACK_UNDERFLOW;
    }

    *old_data = p_stack->data[p_stack->top];
    p_stack->top--;

    return SUCCESS;
}

int StackPeek(stack_t *p_stack, int *data)
{
    if (StackIsEmpty(p_stack))
    {
        return STACK_EMPTY;
    }

    *data = p_stack->data[p_stack->top];

    return SUCCESS;
}

void StackDestroy(stack_t **pp_stack)
{
    stack_t *p_stack = *pp_stack;

    free(p_stack->data);
    free(p_stack);

    *pp_stack = NULL;
}

// ******************** BST ********************
struct bst *CreateBST(void)
{
    struct bst *p_bst = NULL;

    p_bst = (struct bst *)malloc(sizeof(struct bst));
    if (p_bst == NULL)
    {
        fprintf(stderr, "malloc:fatal:out of virtual memory\n");
        exit(EXIT_FAILURE);
    }

    p_bst->p_root = NULL;
    p_bst->nr_elements = 0;

    return (p_bst);
}

int bst_insert(struct bst *p_bst, int new_element)
{
    struct bst_node *p_new_node = NULL;
    struct bst_node *p_run = NULL;

    p_new_node = GetBSTNode(new_element);
    p_run = p_bst->p_root;

    if (p_run == NULL)
    {
        p_bst->p_root = p_new_node;
        p_bst->nr_elements += 1;
        return (SUCCESS);
    }

    while (1)
    {
        if (new_element <= p_run->data)
        {
            if (p_run->left == NULL)
            {
                p_run->left = p_new_node;
                p_new_node->parent = p_run;
                p_bst->nr_elements += 1;
                break;
            }
            else
            {
                p_run = p_run->left;
            }
        }
        else
        {
            if (p_run->right == NULL)
            {
                p_run->right = p_new_node;
                p_new_node->parent = p_run;
                p_bst->nr_elements += 1;
                break;
            }
            else
            {
                p_run = p_run->right;
            }
        }
    }

    return (SUCCESS);
}


int bst_max(struct bst *p_bst, int *p_max_data)
{
    struct bst_node *p_max_node = NULL;

    if (p_bst->p_root == NULL)
        return (TREE_EMPTY);

    p_max_node = get_max_node(p_bst->p_root);
    *p_max_data = p_max_node->data;

    return (SUCCESS);
}

int bst_min(struct bst *p_bst, int *p_min_data)
{
    struct bst_node *p_min_node = NULL;

    if (p_bst->p_root == NULL)
        return (TREE_EMPTY);

    p_min_node = get_min_node(p_bst->p_root);
    *p_min_data = p_min_node->data;

    return (SUCCESS);
}

int bst_inorder_successor(PAINTSTRUCT ps, HWND hWnd, HDC hdc, 
                        struct bst *p_bst, int ext_data, int *p_succ_data)
{
    struct bst_node *p_ext_node = NULL;
    struct bst_node *p_succ_node = NULL;

    p_ext_node = SearchNode(p_bst, ext_data);
    if (p_ext_node == NULL)
        return (TREE_DATA_NOT_FOUND);

    p_succ_node = inorder_successor(ps, hdc, hWnd, p_ext_node);
    if (p_succ_node == NULL)
        return (TREE_NO_SUCCESSOR);

    *p_succ_data = p_succ_node->data;
    return (SUCCESS);
}

int bst_inorder_predecessor(PAINTSTRUCT ps, HWND hWnd, HDC hdc, 
                            struct bst *p_bst, int ext_data, int *p_pred_data)
{
    struct bst_node *p_ext_node = NULL;
    struct bst_node *p_pred_node = NULL;

    p_ext_node = SearchNode(p_bst, ext_data);
    if (p_ext_node == NULL)
        return (TREE_DATA_NOT_FOUND);

    p_pred_node = inorder_predecessor(ps, hdc, hWnd, p_ext_node);
    if (p_pred_node == NULL)
        return (TREE_NO_PREDECESSOR);

    *p_pred_data = p_pred_node->data;
    return (SUCCESS);
}

int BSTSearch(struct bst *p_bst, int search_data)
{
    return (SearchNode(p_bst, search_data) != NULL);
}

int RemoveData(struct bst *p_bst, int r_data)
{
    struct bst_node *z = NULL; /* pointer to node to be deleted */
    struct bst_node *w = NULL; /* pointer to node to be deleted */

    z = SearchNode(p_bst, r_data);
    if (z == NULL)
        return (TREE_DATA_NOT_FOUND);

    if (z->left == NULL)
    {
        if (z->parent == NULL)
            p_bst->p_root = z->right;

        else if (z == z->parent->left)
            z->parent->left = z->right;

        else
            z->parent->right = z->right;

        if (z->right != NULL)
            z->right->parent = z->parent;

    }

    else if (z->right == NULL)
    {
        if (z->parent == NULL)
            p_bst->p_root = z->left;

        else if (z == z->parent->left)
            z->parent->left = z->left;

        else
            z->parent->right = z->left;

        if (z->left != NULL)
            z->left->parent = z->parent;
    }

    else
    {
        w = z->right;
        while (w->left != NULL)
            w = w->left;

        if (z->right != w)
        {
            w->parent->left = w->right;
            if (w->right != NULL)
                w->right->parent = w->parent;

            w->right = z->right;
            w->right->parent = w;
        }

        if (z->parent == NULL)
            p_bst->p_root = w;

        else if (z == z->parent->left)
            z->parent->left = w;

        else
            z->parent->right = w;

        w->parent = z->parent;

        w->left = z->left;
        w->left->parent = w;
    }

    free(z);
    z = NULL;

    return (SUCCESS);
}

void BSTDestroy(struct bst **pp_bst)
{
    DestroyNode((*pp_bst)->p_root);
    free(*pp_bst);
    *pp_bst = NULL;
}

// TODO: Implement Traversal using Win32
void preorder_node(PAINTSTRUCT ps, HDC hdc, HWND hWnd, 
                    struct bst_node *p_root)
{
    if (p_root != NULL)
    {
        p_root -> color = DATA_HIGHLIGHT_COLOR;
        DisplayBST(ps, hdc, hWnd, p_bst);
        DrawBSTArrayNodeBox(hdc, hWnd, ps, OrderArrayX, OrderArrayY, 50, 30, p_root -> data);
        OrderArrayX += 50;
        Sleep(1000);
        p_root -> color = NO_COLOR;

        preorder_node(ps, hdc, hWnd,  p_root->left);
        preorder_node(ps, hdc, hWnd, p_root->right);
    }
}

void inorder_node(PAINTSTRUCT ps, HDC hdc, HWND hWnd, 
                    struct bst_node *p_root)
{
    static unsigned long long int i = 0;
    if (p_root != NULL)
    {
        inorder_node(ps, hdc, hWnd, p_root->left);
        if(StopSleep == 0)
        {
            p_root -> color = DATA_HIGHLIGHT_COLOR;
            DisplayBST(ps, hdc, hWnd, p_bst);
        }

        DrawBSTArrayNodeBox(hdc, hWnd, ps, OrderArrayX, OrderArrayY, 50, 30, p_root -> data);

        OrderArrayX += 50;
        if(StopSleep == 0)
        {
            Sleep(1000);
            p_root -> color = NO_COLOR;
        }
        inorder_node(ps, hdc, hWnd, p_root->right);
    }
}

void postorder_node(PAINTSTRUCT ps, HDC hdc, HWND hWnd, 
                    struct bst_node *p_root)
{
    if (p_root != NULL)
    {
        postorder_node(ps, hdc, hWnd, p_root->left);
        postorder_node(ps, hdc, hWnd, p_root->right);

        p_root -> color = DATA_HIGHLIGHT_COLOR;
        DisplayBST(ps, hdc, hWnd, p_bst);
        DrawBSTArrayNodeBox(hdc, hWnd, ps, OrderArrayX, OrderArrayY, 50, 30, p_root -> data);
        OrderArrayX += 50;
        Sleep(1000);

        p_root -> color = NO_COLOR;
    }
}

void DestroyNode(struct bst_node *p_root)
{
    if (p_root != NULL)
    {
        DestroyNode(p_root->left);
        DestroyNode(p_root->right);
        free(p_root);
    }
}

struct bst_node *get_max_node(struct bst_node *p_node)
{
    struct bst_node *p_run = NULL;

    if(p_node == NULL)
    {
        return(NULL);
    }

    p_run = p_node;

    while (p_run->right != NULL)
        p_run = p_run->right;

    return (p_run);
}

struct bst_node *get_min_node(struct bst_node *p_node)
{
    struct bst_node *p_run = NULL;

    if(p_node == NULL)
    {
        return(NULL);
    }

    p_run = p_node;

    while (p_run->left != NULL)
        p_run = p_run->left;

    return (p_run);
}

struct bst_node *inorder_successor(PAINTSTRUCT ps, HDC hdc, 
                                    HWND hWnd, struct bst_node *p_node)
{
    struct bst_node *p_run = NULL;

    struct bst_node *x = NULL, *y = NULL;

    p_run = p_node;
    if (p_run->right != NULL)
    {
        p_run = p_run->right;
        while (p_run->left != NULL)
        {
            if(p_run != NULL)
            {
                p_run -> color = DATA_HIGHLIGHT_COLOR;
                DisplayBST(ps, hdc, hWnd, p_bst);
                Sleep(1000);
                p_run -> color = NO_COLOR;
            }

            p_run = p_run->left;
        }
        
        if(p_run != NULL)
        {
            p_run -> color = DATA_HIGHLIGHT_COLOR;
            DisplayBST(ps, hdc, hWnd, p_bst);
            p_run -> color = NO_COLOR;
        }
        return (p_run);
    }
    else
    {
        x = p_run;
        y = x->parent;
        while (y != NULL && y->right == x)
        {
            if(y != NULL)
            {

                y -> color = DATA_HIGHLIGHT_COLOR;
                DisplayBST(ps, hdc, hWnd, p_bst);
                Sleep(1000);
                y -> color = NO_COLOR;
                
            }
            x = y;
            y = y->parent;
            
        }
        if(y != NULL)
        {
            y -> color = DATA_FOUND_COLOR;
            DisplayBST(ps, hdc, hWnd, p_bst);
            y -> color = NO_COLOR;
        }
        return (y);
    }
}

struct bst_node *inorder_predecessor(PAINTSTRUCT ps, HDC hdc, 
                                    HWND hWnd, struct bst_node *p_node)
{
    struct bst_node *p_run = NULL;
    struct bst_node *x = NULL, *y = NULL;

    p_run = p_node;
    if (p_run->left != NULL)
    {
        p_run = p_run->left;
        while (p_run->right != NULL)
        {
            if(p_run != NULL)
            {
                p_run -> color = DATA_HIGHLIGHT_COLOR;
                DisplayBST(ps, hdc, hWnd, p_bst);
                Sleep(1000);
                p_run -> color = NO_COLOR;
            }
            p_run = p_run->right;
        }
        if(p_run != NULL)
        {
            p_run -> color = DATA_HIGHLIGHT_COLOR;
            DisplayBST(ps, hdc, hWnd, p_bst);
            p_run -> color = NO_COLOR;
        }
        return (p_run);
    }
    else
    {
        x = p_run;
        y = x->parent;
        while (y != NULL && y->left == x)
        {   
            if(y != NULL)
            {
                y -> color = DATA_HIGHLIGHT_COLOR;
                DisplayBST(ps, hdc, hWnd, p_bst);
                Sleep(1000);
                y -> color = NO_COLOR;
            }
            x = y;
            y = y->parent;
        }
        if(y != NULL)
        {
            y -> color = DATA_FOUND_COLOR;
            DisplayBST(ps, hdc, hWnd, p_bst);
            y -> color = NO_COLOR;
        }
        return (y);
    }
}

struct bst_node *SearchNode(struct bst *p_bst, int search_data)
{
    struct bst_node *p_run = NULL;

    p_run = p_bst->p_root;

    while (p_run != NULL)
    {
        if (search_data == p_run->data)
            break;
        else if (search_data < p_run->data)
            p_run = p_run->left;
        else
            p_run = p_run->right;
    }

    return (p_run);
}

struct bst_node *GetBSTNode(int new_element)
{
    struct bst_node *p_new_node = NULL;

    p_new_node = (struct bst_node *)malloc(sizeof(struct bst_node));
    if (p_new_node == NULL)
    {
        fprintf(stderr, "malloc:fatal:out of virtual memory\n");
        exit(EXIT_FAILURE);
    }

    p_new_node->data = new_element;
    p_new_node->left = NULL;
    p_new_node->right = NULL;
    p_new_node->parent = NULL;
    p_new_node->color = NO_COLOR;

    return (p_new_node);
}
