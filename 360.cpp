// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include "Stabctrl2.h"
#include "SAnimImg.h"
#include "MainDlg.h"

#include "core/SSkin.h"
#include "control/souictrls.h"
#include "layout/SouiLayout.h"

#define RES_TYPE 1
//#define RES_TYPE 0   //从文件中加载资源
//#define RES_TYPE 1   //从PE资源中加载UI资源


#ifdef _DEBUG
#define SYS_NAMED_RESOURCE _T("soui-sys-resourced.dll")
#else
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")
#endif

//注册系统控件，只注册当前APP里需要的控件，可以减小静态链接生成的APP的体积。
class CSystemObjectRegister : public TObjRefImpl<ISystemObjectRegister>
{
public:
	void RegisterWindows(SObjectFactoryMgr *objFactory)
	{
		objFactory->TplRegisterFactory<SWindow>();
		objFactory->TplRegisterFactory<SPanel>();
		objFactory->TplRegisterFactory<SStatic>();
		objFactory->TplRegisterFactory<SButton>();
		objFactory->TplRegisterFactory<SImageWnd>();
		objFactory->TplRegisterFactory<SProgress>();
		objFactory->TplRegisterFactory<SImageButton>();
		objFactory->TplRegisterFactory<SCheckBox>();
		objFactory->TplRegisterFactory<SIconWnd>();
		objFactory->TplRegisterFactory<SRadioBox>();
		objFactory->TplRegisterFactory<SLink>();
		objFactory->TplRegisterFactory<SGroup>();
		objFactory->TplRegisterFactory<SAnimateImgWnd>();
		objFactory->TplRegisterFactory<SScrollView>();
		objFactory->TplRegisterFactory<SToggle>();
		objFactory->TplRegisterFactory<SCaption>();
		objFactory->TplRegisterFactory<STabCtrl>();
		objFactory->TplRegisterFactory<STabPage>();
		objFactory->TplRegisterFactory<SHeaderCtrl>();
		objFactory->TplRegisterFactory<SListCtrl>();
	}

	void RegisterSkins(SObjectFactoryMgr *objFactory)
	{
		objFactory->TplRegisterFactory<SSkinImgList>();
		objFactory->TplRegisterFactory<SSkinImgFrame>();
		objFactory->TplRegisterFactory<SSkinButton>();
		objFactory->TplRegisterFactory<SSkinScrollbar>();
	}

	void RegisterLayouts(SObjectFactoryMgr *objFactory)
	{
		objFactory->TplRegisterFactory<SouiLayout>();
	}
};

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpstrCmdLine*/, int /*nCmdShow*/)
{
    HRESULT hRes = OleInitialize(NULL);
    SASSERT(SUCCEEDED(hRes));
    
    int nRet = 0; 
    
    SComMgr * pComMgr = new SComMgr;
    //将程序的运行路径修改到项目所在目录所在的目录
    TCHAR szCurrentDir[MAX_PATH]={0};
    GetModuleFileName( NULL, szCurrentDir, sizeof(szCurrentDir) );
    LPTSTR lpInsertPos = _tcsrchr( szCurrentDir, _T('\\') );
    _tcscpy(lpInsertPos+1,_T("\\..\\360"));
    SetCurrentDirectory(szCurrentDir);
        
    {

        CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
        CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
        pComMgr->CreateRender_GDI((IObjRef**)&pRenderFactory);
        pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);

        pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);

	ISystemObjectRegister *pRegister = new CSystemObjectRegister();
        SApplication *theApp=new SApplication(pRenderFactory,hInstance,L"360demo",new CSystemObjectRegister());
        pRegister->Release();
        
        theApp->RegisterWindowClass<STabPage2>();//注册STabPage2
        theApp->RegisterWindowClass<STabCtrl2>();//注册STabCtrl2
        theApp->RegisterWindowClass<SAnimImg>();//注册SAnimImg

        CAutoRefPtr<ITranslatorMgr> trans;
        pComMgr->CreateTranslator((IObjRef**)&trans);
        if(trans)
        {
            theApp->SetTranslator(trans);
            pugi::xml_document xmlLang;
            if(xmlLang.load_file(L"Translator/lang_cn.xml"))
            {
                CAutoRefPtr<ITranslator> langCN;
                trans->CreateTranslator(&langCN);
                langCN->Load(&xmlLang.child(L"language"),1);//1=LD_XML
                trans->InstallTranslator(langCN);
            }
        }
        	
        CAutoRefPtr<IResProvider>   pResProvider;
#if (RES_TYPE == 0)
        CreateResProvider(RES_FILE,(IObjRef**)&pResProvider);
        if(!pResProvider->Init((LPARAM)_T("uires"),0))
        {
            SASSERT(0);
            return 1;
        }
#else 
        CreateResProvider(RES_PE,(IObjRef**)&pResProvider);
        pResProvider->Init((WPARAM)hInstance,0);
#endif

        theApp->AddResProvider(pResProvider);
        
        //加载系统资源
        HMODULE hSysResource=LoadLibrary(SYS_NAMED_RESOURCE);
        if(hSysResource)
        {
            CAutoRefPtr<IResProvider> sysSesProvider;
            CreateResProvider(RES_PE,(IObjRef**)&sysSesProvider);
            sysSesProvider->Init((WPARAM)hSysResource,0);
            theApp->LoadSystemNamedResource(sysSesProvider);
        }
        // BLOCK: Run application
        {
            CMainDlg dlgMain;  
            dlgMain.Create(GetActiveWindow(),0,0,0,0);
            dlgMain.SendMessage(WM_INITDIALOG);
            dlgMain.CenterWindow(dlgMain.m_hWnd);
            dlgMain.ShowWindow(SW_SHOWNORMAL);
            nRet=theApp->Run(dlgMain.m_hWnd);
        }

        delete theApp;
    }


    delete pComMgr;
    OleUninitialize();
    return nRet;
}
