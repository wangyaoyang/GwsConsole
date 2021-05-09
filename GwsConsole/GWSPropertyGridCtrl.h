#pragma once


// CGWSPropertyGridCtrl
class CGWSPropertyGridCtrl : public CMFCPropertyGridCtrl
{
	DECLARE_DYNAMIC(CGWSPropertyGridCtrl)

public:
	CGWSPropertyGridCtrl();
	virtual ~CGWSPropertyGridCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnPropertyChanged(CMFCPropertyGridProperty* pProp) const;
public:
	void	m_InitList();
};


#define	PROPERTY_BACKGROUND_COLOR		_T("Window Color")
#define	PROPERTY_TEXT_FONT				_T("Font")
#define	PROPERTY_TARGET_IP				_T("Device IP")
#define	PROPERTY_GROUP_SCRIPTS			_T("Scripts")
#define	PROPERTY_EXPECT					_T("Expected")
#define	PROPERTY_EXPECT_COMMON			_T("mifi>")
#define	PROPERTY_EXPECT_PROMPT			_T("6H mifi>")
#define	PROPERTY_EXPECT_TFTP			_T("TFTP transfer complete")
#define	PROPERTY_EXPECT_ERASED			_T("Erased")
#define	PROPERTY_EXPECT_DONE			_T("done")
