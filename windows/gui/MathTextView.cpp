
// MathTextView.cpp : implementation of the CMathTextView class
//

#include "common-includes.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MathText.h"
#endif

#include "MathTextDoc.h"
#include "MathTextView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMathTextView

IMPLEMENT_DYNCREATE(CMathTextView, CView)

BEGIN_MESSAGE_MAP(CMathTextView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CMathTextView construction/destruction

CMathTextView::CMathTextView()
{
	EnableActiveAccessibility();
	// TODO: add construction code here

}

CMathTextView::~CMathTextView()
{
}

BOOL CMathTextView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CMathTextView drawing

void CMathTextView::OnDraw(CDC* /*pDC*/)
{
	CMathTextDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

void CMathTextView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMathTextView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMathTextView diagnostics

#ifdef _DEBUG
void CMathTextView::AssertValid() const
{
	CView::AssertValid();
}

void CMathTextView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMathTextDoc* CMathTextView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMathTextDoc)));
	return (CMathTextDoc*)m_pDocument;
}
#endif //_DEBUG


// CMathTextView message handlers
