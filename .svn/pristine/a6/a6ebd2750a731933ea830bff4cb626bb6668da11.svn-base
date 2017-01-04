// Machine generated IDispatch wrapper class(es) created with Add Class from Typelib Wizard

// CPlaybackDialogCallback wrapper class

class CPlaybackDialogCallback : public COleDispatchDriver
{
public:
   CPlaybackDialogCallback(){} // Calls COleDispatchDriver default constructor
   CPlaybackDialogCallback(LPDISPATCH pDispatch, BOOL bAutoRelease = FALSE) : COleDispatchDriver(pDispatch, bAutoRelease) {}
   CPlaybackDialogCallback(const CPlaybackDialogCallback& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

   // Attributes
public:

   // Operations
public:


   // IPlaybackDialogCallback methods
public:
   void OnPlaybackStarted()
   {
      InvokeHelper(GetDispId(L"OnPlaybackStarted"), DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
   }
   void OnPlaybackStopped()
   {
      InvokeHelper(GetDispId(L"OnPlaybackStopped"), DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
   }
   void OnPlaybackTimeChanged(DATE dateCurrentTime)
   {
      static BYTE parms[] = VTS_DATE ;
      InvokeHelper(GetDispId(L"OnPlaybackTimeChanged"), DISPATCH_METHOD, VT_EMPTY, NULL, parms, dateCurrentTime);
   }
   void OnPlaybackRateChanged(long lNewPlaybackRate)
   {
      static BYTE parms[] = VTS_I4 ;
      InvokeHelper(GetDispId(L"OnPlaybackRateChanged"), DISPATCH_METHOD, VT_EMPTY, NULL, parms, lNewPlaybackRate);
   }

protected:
   DISPID GetDispId(BSTR szMember)
   {
      DISPID dispid = 0;
      HRESULT sc = m_lpDispatch->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_USER_DEFAULT, &dispid);
      if (FAILED(sc))
      {
         COleException* pException = new COleException;
         pException->m_sc = sc;
         THROW(pException);
      }
      return dispid;
   }

};
