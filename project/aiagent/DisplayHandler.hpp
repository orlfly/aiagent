#ifndef DISPLAYHANDLER_HPP
#define DISPLAYHANDLER_HPP

#include <iostream>

#include "include/cef_display_handler.h"

class DisplayHandler: public CefDisplayHandler
{
public:

  DisplayHandler();

  //! \brief
  ~DisplayHandler();

  virtual void OnLoadingProgressChange( CefRefPtr< CefBrowser > browser, double progress ) override;
private:
  double m_progress;
  //! \brief CefBase interface
  IMPLEMENT_REFCOUNTING(DisplayHandler);

};

#endif //LOADHANDLER_HPP
