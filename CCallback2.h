// Machine generated IDispatch wrapper class(es) created with Add Class from Typelib Wizard

// CCallback2 wrapper class

class CCallback2 : public COleDispatchDriver
{
public:
   CCallback2(){} // Calls COleDispatchDriver default constructor
   CCallback2(LPDISPATCH pDispatch, BOOL bAutoRelease = FALSE) : COleDispatchDriver(pDispatch, bAutoRelease) {}
   CCallback2(const CCallback2& dispatchSrc) : COleDispatchDriver(dispatchSrc) {}

   // Attributes
public:

   // Operations
public:


   // ICallback2 methods
public:
   void GetMenuItemsEx(double lat, double lon, long layer_handle, long object_handle, long menu_id, BSTR * menu_text)
   {
      static BYTE parms[] = VTS_R8 VTS_R8 VTS_I4 VTS_I4 VTS_I4 VTS_PBSTR ;
      InvokeHelper(GetDispId(L"GetMenuItemsEx"), DISPATCH_METHOD, VT_EMPTY, NULL, parms, lat, lon, layer_handle, object_handle, menu_id, menu_text);
   }
   void MaskComplete(LPCTSTR mask_file, double lat, double lon)
   {
      static BYTE parms[] = VTS_BSTR VTS_R8 VTS_R8 ;
      InvokeHelper(GetDispId(L"MaskComplete"), DISPATCH_METHOD, VT_EMPTY, NULL, parms, mask_file, lat, lon);
   }
   void OnOverlayOpened(long overlay_handle)
   {
      static BYTE parms[] = VTS_I4 ;
      InvokeHelper(GetDispId(L"OnOverlayOpened"), DISPATCH_METHOD, VT_EMPTY, NULL, parms, overlay_handle);
   }
   void OnOverlaySaved(long overlay_handle)
   {
      static BYTE parms[] = VTS_I4 ;
      InvokeHelper(GetDispId(L"OnOverlaySaved"), DISPATCH_METHOD, VT_EMPTY, NULL, parms, overlay_handle);
   }
   void OnCoverageChanged()
   {
      InvokeHelper(GetDispId(L"OnCoverageChanged"), DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
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
